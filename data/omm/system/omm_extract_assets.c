#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#if defined(OMM_EXT_ASSETS)
#if !OMM_CODE_DYNOS && !OMM_CODE_DEV
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#endif

typedef struct {
    char filename[SYS_MAX_PATH];
    s32 width;
    s32 height;
    s32 size;
    s32 address;
    s32 offset;
    bool mio0;
} AssetData;

OMM_INLINE s32 __str_to_int(const char *begin, const char *end) {
    char buffer[32] = { 0 };
    mem_cpy(buffer, begin, min_s(31, end - begin));
    s32 x = 0;
    sscanf(buffer, "%d", &x);
    return x;
}

static void __mkdirs(const char *name, const char *skip) {
    if (!fs_sys_dir_exists(name)) {
        char path[SYS_MAX_PATH];
        fs_convert_path(path, SYS_MAX_PATH, name);
        char *sep = path + strlen(skip);
        while ((sep = strchr(sep + 1, '/')) != NULL) {
            *sep = 0;
            fs_sys_mkdir(path);
            *sep = '/';
        }
    }
}

//
// Assets
//

static OmmArray omm_load_assets(const char *pathAssets) {
    FILE *fileAssets = fopen(pathAssets, "r");
    if (fileAssets) {
        OmmArray assets = omm_array_zero;
        char buffer[1024];
        while (fgets(buffer, 1024, fileAssets)) {

            // Skip stuff
            if (strstr(buffer, "@")) continue; // audio
            if (strstr(buffer, ".ia1.")) continue; // unused format
            if (!strstr(buffer, ".png")) continue; // not a texture

            // C strings...
            const char *sepFilenameBegin = strchr(buffer, '"');
            if (!sepFilenameBegin) continue;
            const char *sepFilenameEnd = strchr(sepFilenameBegin + 1, '"');
            if (!sepFilenameEnd) continue;
            const char *sepDataWidth = strchr(sepFilenameEnd + 1, '[');
            if (!sepDataWidth) continue;
            const char *sepDataHeight = strchr(sepDataWidth + 1, ',');
            if (!sepDataHeight) continue;
            const char *sepDataSize = strchr(sepDataHeight + 1, ',');
            if (!sepDataSize) continue;
            const char *sepDataSizeEnd = strchr(sepDataSize + 1, ',');
            if (!sepDataSizeEnd) continue;
            const char *sepDataBegin = strchr(sepFilenameEnd + 1, '{');
            if (!sepDataBegin) continue;
            const char *sepDataUs = strstr(sepDataBegin + 1, "\"us\"");
            if (!sepDataUs) continue;
            const char *sepDataAddress = strchr(sepDataUs + 1, '[');
            if (!sepDataAddress) continue;
            const char *sepDataEnd = strchr(sepDataAddress + 1, ']');
            if (!sepDataEnd) continue;
            const char *sepDataOffset = strchr(sepDataAddress + 1, ',');

            // Fill data
            AssetData asset = { 0 };
            mem_cpy(asset.filename, sepFilenameBegin + 1, min_s(sepFilenameEnd - sepFilenameBegin - 1, SYS_MAX_PATH));
            if (sepDataBegin > sepDataSizeEnd) {
                asset.width = __str_to_int(sepDataWidth + 1, sepDataHeight);
                asset.height = __str_to_int(sepDataHeight + 1, sepDataSize);
                asset.size = __str_to_int(sepDataSize + 1, sepDataSizeEnd);
            } else {
                asset.size = __str_to_int(sepDataWidth + 1, sepDataHeight);
            }
            if (sepDataOffset && sepDataOffset > sepDataAddress && sepDataOffset < sepDataEnd) {
                asset.address = __str_to_int(sepDataAddress + 1, sepDataOffset);
                asset.offset = __str_to_int(sepDataOffset + 1, sepDataEnd);
                asset.mio0 = true;
            } else {
                asset.address = __str_to_int(sepDataAddress + 1, sepDataEnd);
                asset.mio0 = false;
            }

            // Add to assets
            omm_array_add(assets, ptr, mem_dup(&asset, sizeof(asset)));
        }
        fclose(fileAssets);
        return assets;
    }
    sys_fatal("omm_load_assets: Cannot load assets from file '%s'.", pathAssets);
}

static void omm_clear_assets(OmmArray assets) {
    omm_array_for_each(assets, p) {
        mem_del(p->as_ptr);
    }
    omm_array_delete(assets);
}

//
// MIO0 decompression
//

#define MIO0_GET_BIT(buf, bit) ((buf)[(bit) / 8] & (1 << (7 - ((bit) % 8))))
#define MIO0_U32_BE(buf) (s32) (((buf)[0] << 24) + ((buf)[1] << 16) + ((buf)[2] << 8) + ((buf)[3] << 0))

static u8 *decompress_mio0(const u8 *data, s32 *size) {

    // Extract header
    if (!mem_eq(data, "MIO0", 4)) {
        sys_fatal("decompress_mio0: Input data is not MIO0-compressed!");
    }
    s32 destSize = MIO0_U32_BE(data + 4);
    s32 compOffset = MIO0_U32_BE(data + 8);
    s32 uncompOffset = MIO0_U32_BE(data + 12);

    // Decompress data
    u8 *output = mem_new(u8, destSize);
    for (s32 bitIdx = 0, bytesWritten = 0, compIdx = 0, uncompIdx = 0; bytesWritten < destSize;) {
        if (MIO0_GET_BIT(data + 16, bitIdx)) {
            output[bytesWritten] = data[uncompOffset + uncompIdx];
            bytesWritten++;
            uncompIdx++;
        } else {
            const u8 *vals = &data[compOffset + compIdx];
            compIdx += 2;
            s32 length = ((vals[0] & 0xF0) >> 4) + 3;
            s32 idx = ((vals[0] & 0x0F) << 8) + vals[1] + 1;
            for (s32 i = 0; i < length; i++) {
                output[bytesWritten] = output[bytesWritten - idx];
                bytesWritten++;
            }
        }
        bitIdx++;
    }

    // Return data
    *size = min_s(*size, destSize);
    return output;
}

//
// Texture conversion
//

#define SCALE_5_8(VAL_) (((VAL_) * 0xFF) / 0x1F)
#define SCALE_4_8(VAL_) ((VAL_) * 0x11)
#define SCALE_3_8(VAL_) ((VAL_) * 0x24)

static u8 *convert_rgba16(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 2);
    u8 *buf = img;
    for (s32 i = 0; i != size; i += 2) {
        u16 c = (data[i + 0] << 8) | data[i + 1];
        u8 r = (c >> 11) & 0x1F;
        u8 g = (c >>  6) & 0x1F;
        u8 b = (c >>  1) & 0x1F;
        u8 a = (c >>  0) & 0x01;
        *(buf++) = (SCALE_5_8(r));
        *(buf++) = (SCALE_5_8(g));
        *(buf++) = (SCALE_5_8(b));
        *(buf++) = (0xFF  *  (a));
    }
    return img;
}

static u8 *convert_rgba32(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 1);
    mem_cpy(img, data, size);
    return img;
}

static u8 *convert_ia4(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 8);
    u8 *buf = img;
    for (s32 i = 0; i != size; ++i) {
        u8 h0 = (data[i] >> 4) & 0xF;
        u8 h1 = (data[i] >> 0) & 0xF;
        *(buf++) = (SCALE_3_8(h0 >> 1));
        *(buf++) = (SCALE_3_8(h0 >> 1));
        *(buf++) = (SCALE_3_8(h0 >> 1));
        *(buf++) = (0xFF  *  (h0 &  1));
        *(buf++) = (SCALE_3_8(h1 >> 1));
        *(buf++) = (SCALE_3_8(h1 >> 1));
        *(buf++) = (SCALE_3_8(h1 >> 1));
        *(buf++) = (0xFF  *  (h1 &  1));
    }
    return img;
}

static u8 *convert_ia8(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 4);
    u8 *buf = img;
    for (s32 i = 0; i != size; ++i) {
        u8 c = (data[i] >> 4) & 0xF;
        u8 a = (data[i] >> 0) & 0xF;
        *(buf++) = (SCALE_4_8(c));
        *(buf++) = (SCALE_4_8(c));
        *(buf++) = (SCALE_4_8(c));
        *(buf++) = (SCALE_4_8(a));
    }
    return img;
}

static u8 *convert_ia16(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 2);
    u8 *buf = img;
    for (s32 i = 0; i != size; i += 2) {
        u8 c = data[i + 0];
        u8 a = data[i + 1];
        *(buf++) = (c);
        *(buf++) = (c);
        *(buf++) = (c);
        *(buf++) = (a);
    }
    return img;
}

static u8 *convert_i4(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 8);
    u8 *buf = img;
    for (s32 i = 0; i != size; ++i) {
        u8 h0 = (data[i] >> 4) & 0xF;
        u8 h1 = (data[i] >> 0) & 0xF;
        *(buf++) = (SCALE_4_8(h0));
        *(buf++) = (SCALE_4_8(h0));
        *(buf++) = (SCALE_4_8(h0));
        *(buf++) = (0xFF);
        *(buf++) = (SCALE_4_8(h1));
        *(buf++) = (SCALE_4_8(h1));
        *(buf++) = (SCALE_4_8(h1));
        *(buf++) = (0xFF);
    }
    return img;
}

static u8 *convert_i8(const u8 *data, s32 size) {
    u8 *img = mem_new(u8, size * 4);
    u8 *buf = img;
    for (s32 i = 0; i != size; ++i) {
        *(buf++) = (data[i]);
        *(buf++) = (data[i]);
        *(buf++) = (data[i]);
        *(buf++) = (255);
    }
    return img;
}

static u8 *convert_to_rgba32(const char *filename, const u8 *data, s32 size) {
    if (strstr(filename, ".rgba16.")) return convert_rgba16(data, size);
    if (strstr(filename, ".rgba32.")) return convert_rgba32(data, size);
    if (strstr(filename, ".ia4."))    return convert_ia4   (data, size);
    if (strstr(filename, ".ia8."))    return convert_ia8   (data, size);
    if (strstr(filename, ".ia16."))   return convert_ia16  (data, size);
    if (strstr(filename, ".i4."))     return convert_i4    (data, size);
    if (strstr(filename, ".i8."))     return convert_i8    (data, size);
    sys_fatal("convert_to_rgba32: File '%s': Unknown texture format.", filename);
}

//
// Tiles extraction
//

static void omm_extract_tiles(const char *pathResGfx, const char *filename, const u8 *data, s32 size, const char *prefix, s32 tileWidth, s32 tileHeight, s32 tableWidth, s32 tableHeight) {

    // Retrieve tiles name
    const char *tilesNameBegin = strstr(filename, prefix) + strlen(prefix);
    const char *tilesNameEnd = strstr(tilesNameBegin, ".png");
    char tilesName[SYS_MAX_PATH] = { 0 };
    mem_cpy(tilesName, tilesNameBegin, (tilesNameEnd - tilesNameBegin));

    // Extract tiles
    s32 tileSize = tileWidth * tileHeight * sizeof(u16);
    s32 tableSize = tableWidth * tableHeight * sizeof(u32);
    s32 numTiles = (size - tableSize) / tileSize;
    for (s32 i = 0; i != numTiles; ++i, data += tileSize) {
        str_fmt_sa(tileFilename, SYS_MAX_PATH, "%s/textures/skybox_tiles/%s.%d.rgba16.png", pathResGfx, tilesName, i);
        if (!fs_sys_file_exists(tileFilename)) {
            u8 *tile = convert_rgba16(data, tileSize);
            __mkdirs(tileFilename, pathResGfx);
            stbi_write_png(tileFilename, tileWidth, tileHeight, 4, tile, 0);
            mem_del(tile);
        }
    }
}

//
// Sound extraction
//

static const u8 *omm_extract_sound_data(const char *pathRes, const u8 *data, const char *filename) {
    s32 size = *((s32 *) data);
    data += 4;
    if (!fs_sys_file_exists(filename)) {
        __mkdirs(filename, pathRes);
        FILE *file = fopen(filename, "wb");
        if (file) {
            while (size) {
                s32 writtenBytes = min_s(0x1000, size);
                fwrite(data, sizeof(u8), writtenBytes, file);
                data += writtenBytes;
                size -= writtenBytes;
            }
            fclose(file);
            return data;
        }
        sys_fatal("omm_extract_sound_data: Cannot extract sound data to '%s'.", filename);
    }
    data += size;
    return data;
}

//
// OMM assets extraction
//

static const u8 *omm_extract_omm_asset(const char *pathRes, const u8 *data) {
    const char *filename = ((const char *) (data + 4)) + sizeof("build/us_pc/" FS_BASEDIR "/") - 1;
    data += (5 + *((s32 *) data));
    s32 size = *((s32 *) data);
    data += 4;
    str_cat_paths_sa(filepath, SYS_MAX_PATH, pathRes, filename);
    if (!fs_sys_file_exists(filepath)) {
        omm_printf("Extracting %s\n",, filename);
        __mkdirs(filepath, pathRes);
        FILE *file = fopen(filepath, "wb");
        if (file) {
            while (size) {
                s32 writtenBytes = min_s(0x1000, size);
                fwrite(data, sizeof(u8), writtenBytes, file);
                data += writtenBytes;
                size -= writtenBytes;
            }
            fclose(file);
            return data;
        }
        sys_fatal("omm_extract_omm_asset: Cannot extract OMM asset data to '%s'.", filepath);
    }
    data += size;
    return data;
}

//
// Custom assets extraction
//

static const u8 *omm_extract_custom_asset(const char *pathRes, const u8 *data) {
    const char *filename = ((const char *) (data + 4)) + sizeof("build/us_pc/" FS_BASEDIR "/") - 1;
    data += (5 + *((s32 *) data));
    u8 is_custom = *((u8 *) data);
    data += 1;
    s32 size = *((s32 *) data);
    data += 4;
    str_cat_paths_sa(filepath, SYS_MAX_PATH, pathRes, filename);
    if (!fs_sys_file_exists(filepath)) {
        omm_printf("Extracting %s\n",, filename);
        __mkdirs(filepath, pathRes);
        if (is_custom) {
            FILE *file = fopen(filepath, "wb");
            if (file) {
                while (size) {
                    s32 writtenBytes = min_s(0x1000, size);
                    fwrite(data, sizeof(u8), writtenBytes, file);
                    data += writtenBytes;
                    size -= writtenBytes;
                }
                fclose(file);
                return data;
            }
            sys_fatal("omm_extract_custom_asset: Cannot extract custom asset data to '%s'.", filepath);
        } else {
            const char *vanilla_filename = ((const char *) data) + sizeof("build/us_pc/" FS_BASEDIR "/") - 1;
            str_cat_paths_sa(vanilla_filepath, SYS_MAX_PATH, pathRes, vanilla_filename);
            if (!fs_sys_copy_file(vanilla_filepath, filepath)) {
                sys_fatal("omm_extract_custom_asset: Cannot extract custom asset data to '%s'.", filepath);
            }
        }
    }
    data += size;
    return data;
}

//
// Assets extraction
//

OMM_AT_STARTUP static void omm_extract_assets() {

    // Paths
    str_cat_paths_sa(pathExe, SYS_MAX_PATH, sys_exe_path(), "");
    str_cat_paths_sa(pathBaserom, SYS_MAX_PATH, sys_exe_path(), "baserom.us.z64");
    str_cat_paths_sa(pathRes, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR);
    str_cat_paths_sa(pathResGfx, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/gfx");
    str_cat_paths_sa(pathAssets, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/assets.json");
    str_cat_paths_sa(pathAssetsOmm, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/omm_assets.bin");
    str_cat_paths_sa(pathAssetsCustom, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/custom_assets.bin");
#if OMM_GAME_IS_SMMS
    str_cat_paths_sa(pathRequiredSMMS, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/gfx/actors/Bee/Base.rgba32.png");
#elif OMM_GAME_IS_R96X
    str_cat_paths_sa(pathRequiredR96X, SYS_MAX_PATH, sys_exe_path(), "dynos/audio/music.txt");
#endif

    // Check mandatory files
    if (!fs_sys_file_exists(pathBaserom)) {
        sys_fatal("omm_extract_assets: File '%s' not found.", pathBaserom);
    }
    if (!fs_sys_file_exists(pathAssets)) {
        sys_fatal("omm_extract_assets: File '%s' not found.", pathAssets);
    }
    if (!fs_sys_file_exists(pathAssetsOmm)) {
        sys_fatal("omm_extract_assets: File '%s' not found.", pathAssetsOmm);
    }
    if (!fs_sys_file_exists(pathAssetsCustom)) {
        sys_fatal("omm_extract_assets: File '%s' not found.", pathAssetsCustom);
    }
#if OMM_GAME_IS_SMMS
    if (!fs_sys_file_exists(pathRequiredSMMS)) {
        sys_fatal("omm_extract_assets: Missing Super Mario 64 Moonshine assets.\nCopy the content of the Moonshine archive to the 'res/gfx' directory.");
    }
#elif OMM_GAME_IS_R96X
    if (!fs_sys_file_exists(pathRequiredR96X)) {
        sys_fatal("omm_extract_assets: Missing Render96 audio.\nCopy the 'dynos' directory of the Render96 repository to the executable directory.");
    }
#endif

    // ROM data
    FILE *fileBaserom = fopen(pathBaserom, "rb");
    if (!fileBaserom) {
        sys_fatal("omm_extract_assets: Cannot load ROM data from '%s'.", pathBaserom);
    }
    fseek(fileBaserom, 0, SEEK_END);
    s32 romSize = ftell(fileBaserom);
    fseek(fileBaserom, 0, SEEK_SET);
    u8 *rom = mem_new(u8, romSize);
    fread(rom, sizeof(u8), romSize, fileBaserom);
    fclose(fileBaserom);

    // Assets
    OmmArray assets = omm_load_assets(pathAssets);
    omm_array_for_each(assets, p) {
        AssetData *asset = (AssetData *) p->as_ptr;
        str_cat_paths_sa(filepath, SYS_MAX_PATH, pathResGfx, asset->filename);

        // Extract texture if not already done
        if (!fs_sys_file_exists(filepath)) {
            omm_printf("Extracting %s\n",, asset->filename);
            s32 size = asset->size;
            u8 *data = (asset->mio0 ? decompress_mio0(rom + asset->address, &size) + asset->offset : rom + asset->address);

            // Skybox
            if (strstr(asset->filename, "textures/skyboxes/") == asset->filename) {
                omm_extract_tiles(pathResGfx, asset->filename, data, size, "textures/skyboxes/", 32, 32, 10, 8);
            }

            // Cake picture
            else if (strstr(asset->filename, "levels/ending/cake") == asset->filename) {
                omm_extract_tiles(pathResGfx, asset->filename, data, size, "levels/ending/", 80, 20, 0, 0);
            }

            // Regular texture
            else {
                u8 *img = convert_to_rgba32(asset->filename, data, size);
                __mkdirs(filepath, pathExe);
                stbi_write_png(filepath, asset->width, asset->height, 4, img, 0);
                mem_del(img);
            }

            // Free decompressed MIO0 data
            if (asset->mio0) {
                data -= asset->offset;
                mem_del(data);
            }
        }
    }

    // Sound data
    FILE *fileSounds = fopen(*__argv, "rb");
    if (!fileSounds) {
        sys_fatal("omm_extract_assets: Cannot load sound data from '%s'.", pathBaserom);
    }
    fseek(fileSounds, -4, SEEK_END);
    s32 soundsSize = 0;
    fread(&soundsSize, 1, sizeof(s32), fileSounds);
    fseek(fileSounds, -(4 + soundsSize), SEEK_END);
    u8 *sounds = mem_new(u8, soundsSize);
    fread(sounds, sizeof(u8), soundsSize, fileSounds);
    fclose(fileSounds);

    // Extract and build sound data
    const u8 *soundData = sounds;
    str_cat_paths_sa(pathSoundBankSets, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/sound/bank_sets");
    str_cat_paths_sa(pathSoundSequences, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/sound/sequences.bin");
    str_cat_paths_sa(pathSoundSoundData, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/sound/sound_data.ctl");
    str_cat_paths_sa(pathSoundSoundDataTable, SYS_MAX_PATH, sys_exe_path(), FS_BASEDIR "/sound/sound_data.tbl");
    soundData = omm_extract_sound_data(pathRes, soundData, pathSoundBankSets);
    soundData = omm_extract_sound_data(pathRes, soundData, pathSoundSequences);
    soundData = omm_extract_sound_data(pathRes, soundData, pathSoundSoundData);
    soundData = omm_extract_sound_data(pathRes, soundData, pathSoundSoundDataTable);

    // Custom assets data
    FILE *fileAssetsCustom = fopen(pathAssetsCustom, "rb");
    if (!fileAssetsCustom) {
        sys_fatal("omm_extract_assets: Cannot load custom assets data from '%s'.", pathAssetsCustom);
    }
    fseek(fileAssetsCustom, 0, SEEK_END);
    s32 assetsCustomSize = ftell(fileAssetsCustom);
    fseek(fileAssetsCustom, 0, SEEK_SET);
    u8 *assetsCustom = mem_new(u8, assetsCustomSize + 1);
    fread(assetsCustom, sizeof(u8), assetsCustomSize, fileAssetsCustom);
    fclose(fileAssetsCustom);

    // Extract custom assets
    for (const u8 *head = assetsCustom; head < assetsCustom + assetsCustomSize;) {
        head = omm_extract_custom_asset(pathRes, head);
    }

    // OMM assets data
    FILE *fileAssetsOmm = fopen(pathAssetsOmm, "rb");
    if (!fileAssetsOmm) {
        sys_fatal("omm_extract_assets: Cannot load OMM assets data from '%s'.", pathAssetsOmm);
    }
    fseek(fileAssetsOmm, 0, SEEK_END);
    s32 assetsOmmSize = ftell(fileAssetsOmm);
    fseek(fileAssetsOmm, 0, SEEK_SET);
    u8 *assetsOmm = mem_new(u8, assetsOmmSize);
    fread(assetsOmm, sizeof(u8), assetsOmmSize, fileAssetsOmm);
    fclose(fileAssetsOmm);

    // Extract OMM assets
    for (const u8 *head = assetsOmm; head < assetsOmm + assetsOmmSize;) {
        head = omm_extract_omm_asset(pathRes, head);
    }

    // Free stuff
    mem_del(rom);
    mem_del(sounds);
    mem_del(assetsOmm);
    mem_del(assetsCustom);
    omm_clear_assets(assets);
}

#endif
