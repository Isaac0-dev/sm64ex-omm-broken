#include <stdio.h>
#include <ultra64.h>

#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS

#include "controller_api.h"

static FILE *fp;

static void tas_init(void) {
    fp = fopen("cont.m64", "rb");
    if (fp != NULL) {
        uint8_t buf[0x400];
        fread(buf, 1, sizeof(buf), fp);
    }
}

bool is_tas_reading(void) {
    if (fp != NULL) {
        return true;
    }
    return false;
}

FILE *tas_init_off_start(void) {
    fp = fopen("singleStar.m64", "rb");
    if (fp != NULL) {
        uint8_t buf[0x400];
        fread(buf, 1, sizeof(buf), fp);
    }
    return fp;
}

static void tas_shutdown(void) {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
}

static void tas_read(OSContPad *pad) {
    if (fp != NULL) {
        uint8_t bytes[5] = {0};
        fread(bytes, 1, 5, fp);
        pad->button = (bytes[0] << 8) | bytes[1];
        pad->stick_x = bytes[2];
        pad->stick_y = bytes[3];
        gOmmMario->spin.pressed = bytes[4];

        printf("READ %d\n", bytes[0]);
        if (feof(fp)) {
            printf("END OF FILE\n");
            tas_shutdown();
        }
    }
}

static u32 tas_rawkey(void) {
    return VK_INVALID;
}

struct ControllerAPI controller_recorded_tas = {
    VK_INVALID,
    tas_init,
    tas_read,
    tas_rawkey,
    NULL, // no rumble_play
    NULL, // no rumble_stop
    NULL, // no rebinding
    tas_shutdown
};
