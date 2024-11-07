#ifndef OMM_MODELS_H
#define OMM_MODELS_H

#include "types.h"
#include "data/omm/system/omm_memory.h"

//
// Data types
//

#define OMM_MODELS_DATA_TYPE_NONE               (0x00)
#define OMM_MODELS_DATA_TYPE_LIGHT              (0x01)
#define OMM_MODELS_DATA_TYPE_TEXTURE            (0x02)
#define OMM_MODELS_DATA_TYPE_VERTEX             (0x03)
#define OMM_MODELS_DATA_TYPE_DISPLAY_LIST       (0x04)
#define OMM_MODELS_DATA_TYPE_GEO_LAYOUT         (0x05)
#define OMM_MODELS_DATA_TYPE_ANIMATION_VALUE    (0x06)
#define OMM_MODELS_DATA_TYPE_ANIMATION_INDEX    (0x07)
#define OMM_MODELS_DATA_TYPE_ANIMATION          (0x08)
#define OMM_MODELS_DATA_TYPE_ANIMATION_TABLE    (0x09)
#define OMM_MODELS_DATA_TYPE_PRIORITY           (0xFF)

#define OMM_MODELS_CACHING_IGNORE               (-1)
#define OMM_MODELS_CACHING_DISABLED             (0)
#define OMM_MODELS_CACHING_ENABLED              (1)
#define OMM_MODELS_CACHING_CACHED               (2)

//
// File buffer
//

typedef struct {
    u8 *data;
    const u8 *head;
    const u8 *tail;
} OmmFileBuffer;

void        fb_init(OmmFileBuffer *fb, u8 *buffer, s32 length);
void        fb_free(OmmFileBuffer *fb);
bool        fb_eof(OmmFileBuffer *fb);
s32         fb_tell(OmmFileBuffer *fb);
void        fb_seek(OmmFileBuffer *fb, s32 pos);
const void *fb_read_bytes(OmmFileBuffer *fb, s32 size);
const void *fb_read_buffer(OmmFileBuffer *fb, s32 size, s32 *length);
const char *fb_read_str(OmmFileBuffer *fb, str_t dst);
#define     fb_read(fb, type) *((type *) fb_read_bytes(fb, sizeof(type)))

//
// Structs
//

typedef struct {
    const u8 *png_data;
    const u8 *raw_data;
    s32 raw_width;
    s32 raw_height;
    bool uploaded;
} OmmTexData;

typedef struct {
    s16 flags;
    s16 start_frame;
    s16 loop_start;
    s16 loop_end;
    s16 bone_count;
    const s16 *values;
    const u16 *index;
    s32 length;
} OmmAnimData;

#define OmmDataNode_(type) \
typedef struct { \
    str_t name; \
    type *data; \
    s32 size; \
} OmmDataNode_##type

OmmDataNode_(Lights1);
OmmDataNode_(OmmTexData);
OmmDataNode_(Vtx);
OmmDataNode_(Gfx);
OmmDataNode_(GeoLayout);
OmmDataNode_(OmmAnimData);

typedef struct {
    OmmArray_(OmmDataNode_Lights1 *) lights;
    OmmArray_(OmmDataNode_OmmTexData *) textures;
    OmmArray_(OmmDataNode_Vtx *) vertices;
    OmmArray_(OmmDataNode_Gfx *) display_lists;
    OmmArray_(OmmDataNode_GeoLayout *) geo_layouts;
    OmmArray_(OmmDataNode_OmmAnimData *) animations;
    OmmArray_(OmmAnimData *) animation_table;
    bool disable_billboard;
    u8 priority;
} OmmGfxData;

typedef struct {
    OmmGfxData *gfx_data;
    struct GraphNode *graph_node;
    s32 pack_index;
} OmmActorGfx;

typedef struct {
    sys_path_t name;
    sys_path_t path;
    bool exists;
    bool enabled;
    s32 caching;
} OmmPackData;

//
// Functions
//

#define omm_data_nodes_for_each(arr, item, type) \
__OmmNoP *p_##item = (arr).p; OmmDataNode_##type *item = (OmmDataNode_##type *) (p_##item ? p_##item->as_ptr : NULL); \
for (s32 i_##item = 0, c_##item = omm_array_count(arr); i_##item != c_##item; ++i_##item, item = (OmmDataNode_##type *) (++p_##item)->as_ptr)

extern OmmArray_(OmmActorGfx *) gOmmActorList;
extern OmmArray_(OmmPackData *) gOmmPackList;

s32          omm_models_get_actor_count();
const char **omm_models_get_actor_names(s32 index);
const void  *omm_models_get_actor_layout(s32 index);
s32          omm_models_get_actor_index(const void *geo_layout);
void        *omm_models_get_func_pointer(s32 index);
s32          omm_models_get_model_pack_index(const void *geo_layout);
s32          omm_models_get_mario_model_pack_index();
s32         *omm_models_get_caching_state(bool *toggle);

const void **omm_models_init();
OmmGfxData  *omm_models_load_from_binary(const char *pack_folder, const char **actor_names, u64 *loaded_bytes);
void         omm_models_load_all(u64 *loaded_bytes);
bool         omm_models_read_config(const char *name, const char *value);
void         omm_models_write_config(FILE *file);
void         omm_models_update();
void         omm_models_update_object(struct Object *obj);
void         omm_models_swap_animations(void *ptr);

#endif
