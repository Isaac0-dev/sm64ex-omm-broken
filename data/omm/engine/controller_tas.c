#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS

static FILE *fp;

void tas_shutdown(void) {
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
        printf("CLOSED\n");
    }
}

void tas_record(OSContPad *pad) {
    if (fp != NULL) {
        uint8_t bytes[5] = {0};
        bytes[0] = (pad->button >> 8);
        bytes[1] = pad->button;
        bytes[2] = pad->stick_x;
        bytes[3] = pad->stick_y;
        bytes[4] = gOmmMario->spin.pressed;

        fwrite(bytes, 1, 5, fp);
        printf("WROTE %d\n", bytes[0]);
    }
}

void tas_init(void) {
    fp = fopen("cont.m64", "rb");
    if (fp == NULL) {
        fp = fopen("write.m64", "wb");
        if (fp != NULL) {
            uint8_t buf[0x400];
            fwrite(buf, 1, sizeof(buf), fp);
        }
    } else {
        fclose(fp);
        fp = NULL;
    }
}
