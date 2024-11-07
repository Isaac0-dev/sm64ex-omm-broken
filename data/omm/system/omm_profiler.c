#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#define REFRESH_RATE (15)

#if OMM_CODE_DEBUG
extern void omm_gfx_profiler_update(u32 refreshRate);
extern const char *omm_gfx_profiler_get_display_info(u8 op, f64 *disp0, f64 *disp1);
#endif

static struct {
    struct {
        s64 curr;
        s64 sum;
        f64 disp;
    } pc[OMM_PRF_MAX];
    s64 timer;
    s64 counter;
    s64 disp;
    Gfx gfx[0x10000];
} sOmmProfiler[1];

void omm_profiler_start(s32 prf) {
#if OMM_CODE_DEBUG
    sOmmProfiler->pc[prf].curr = SDL_GetPerformanceCounter();
#endif
}

void omm_profiler_stop(s32 prf) {
#if OMM_CODE_DEBUG
    sOmmProfiler->pc[prf].sum += SDL_GetPerformanceCounter() - sOmmProfiler->pc[prf].curr;
#endif
}

void omm_profiler_frame_drawn() {
    sOmmProfiler->counter++;
}

OMM_ROUTINE_UPDATE(omm_profiler_update) {
    s64 curr = SDL_GetPerformanceCounter();
    sOmmProfiler->pc[OMM_PRF_FPS].sum += curr - sOmmProfiler->pc[OMM_PRF_FPS].curr;
    sOmmProfiler->pc[OMM_PRF_FPS].curr = curr;
    if (sOmmProfiler->timer++ % REFRESH_RATE == 0) {
        f64 freq = SDL_GetPerformanceFrequency();
        for (s32 i = 0; i != OMM_PRF_MAX; ++i) {
            sOmmProfiler->pc[i].disp = (f64) sOmmProfiler->pc[i].sum / (freq * REFRESH_RATE);
            sOmmProfiler->pc[i].sum = 0;
        }
        sOmmProfiler->disp = sOmmProfiler->counter;
        sOmmProfiler->counter = 0;
#if OMM_CODE_DEBUG
        omm_gfx_profiler_update(REFRESH_RATE);
#endif
    }
}

static void omm_profiler_print_text(Gfx **gfx, const char *str, s32 x, s32 y, s32 w, s32 h) {
    *gfx = gfx_font_init(*gfx, OMM_TEXTURE_FONT_DISPLAY, 16, 768, 480, 48, 80);
    for (char *c = (char *) str, d = 0, e = 0; *c; ++c, d = (d + (*c >= '1' && *c <= '9')) * (*c != ' '), e = ((*c >= '0' && *c <= '9') || *c == '.')) {
        s32 i = (*c == '0' && !d ? 0x10 : (e ? 0xFF : 0xAA));
        s32 j = (*c == '$' ? 2 : 1);
        *gfx = gfx_font_render_char(*gfx, *c, x, y, w / j, h, 0, -8, i, i, i, 0xE0);
        x += (w / j);
    }
    *gfx = gfx_font_end(*gfx);
}

void omm_profiler_display() {
    if (gOmmShowFPS && !omm_is_main_menu()) {
        Gfx *gfx = sOmmProfiler->gfx;
        gSPDisplayList(gDisplayListHead++, gfx);
#if OMM_CODE_DEBUG
        for (s32 i = 0x3F, j = 0; i >= 0; --i) {
            f64 disp0, disp1;
            const char *name = omm_gfx_profiler_get_display_info(i, &disp0, &disp1);
            if (name) {
                str_t buffer;
                str_fmt(buffer, sizeof(buffer), "$%-20s %06d us %08d %06d ns$", name, (s32) (disp0 * 1000000.0), (s32) (disp1), (s32) (disp0 * 1000000000.0 / max(1, disp1)));
                omm_profiler_print_text(&gfx, buffer, GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(50 * 3 + 1), 1 + 5 * j++, 3, 5);
            }
        }
        static const char *_prf[] = { "OMM", "LVL", "PRE", "GEO", "GFX", "RDR", "FRM" };
        for (s32 i = OMM_PRF_OMM; i <= OMM_PRF_FRM; ++i) {
            str_t buffer;
            str_fmt(buffer, sizeof(buffer), "$%s %06d$", _prf[i], (s32) (1000000.0 * sOmmProfiler->pc[i].disp));
            omm_profiler_print_text(&gfx, buffer, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(1), 1 + 7 * (OMM_PRF_MAX - (i + 1)), 4, 7);
        }
#endif
        {
            str_t buffer;
            str_fmt(buffer, sizeof(buffer), "$FPS %06.2f$", (f32) sOmmProfiler->disp / (sOmmProfiler->pc[OMM_PRF_FPS].disp * REFRESH_RATE));
            omm_profiler_print_text(&gfx, buffer, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(1), 1, 4, 7);
        }
        gSPEndDisplayList(gfx);
    }
}
