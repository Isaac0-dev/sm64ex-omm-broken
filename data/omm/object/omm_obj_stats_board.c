#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#include "behavior_commands.h"

static Scroll sOmmStatsBoardPageScroll = {0};
static Scroll sOmmStatsBoardSubScroll = {0};
InterpData sOmmStatsBoard[1];

#define OMM_STATS_BOARD_FADE                    (10)
#define OMM_STATS_BOARD_WIDTH                   (240)
#define OMM_STATS_BOARD_HEIGHT                  (152)

#define OMM_STATS_BOARD_PAGE_COLLECTIBLES       (0)
#define OMM_STATS_BOARD_PAGE_ACTIONS            (1)
#define OMM_STATS_BOARD_PAGE_DISTANCE           (2)
#define OMM_STATS_BOARD_PAGE_TIME               (3)
#define OMM_STATS_BOARD_PAGE_CAPTURES           (4)
#define OMM_STATS_BOARD_PAGE_REWARDS            (5)
#define OMM_STATS_BOARD_PAGES                   (6)

static s32 bhv_omm_stats_board_get_sub_page_count(s32 page) {
    switch (page) {
        case OMM_STATS_BOARD_PAGE_CAPTURES: return (omm_level_get_all_available_captures_count(OMM_GAME_MODE) + 15) / 16;
        case OMM_STATS_BOARD_PAGE_REWARDS: return omm_rewards_get_count();
    }
    return 1;
}

static void bhv_omm_stats_board_update_scrolls() {

    // Page
    s32 prevPage = sOmmStatsBoardPageScroll.idx;
    omm_render_update_scroll(&sOmmStatsBoardPageScroll, OMM_STATS_BOARD_PAGES, -gPlayer1Controller->stickY);
    if (sOmmStatsBoardPageScroll.idx != prevPage) {
        sOmmStatsBoardSubScroll.idx = 0;
    }

    // Sub-page
    s32 subPageCount = bhv_omm_stats_board_get_sub_page_count(sOmmStatsBoardPageScroll.idx);
    if (subPageCount > 1) {
        omm_render_update_scroll(&sOmmStatsBoardSubScroll, subPageCount, gPlayer1Controller->stickX);
    }
}

//
// Captures
//

#define FLAG_NAME(x) { OMM_CAPTURE_##x, OMM_TEXT_CAPTURE_##x }
static struct { u64 flag; const char *name; } OMM_STATS_BOARD_CAPTURES[] = {

    // Page 1
    FLAG_NAME(GOOMBA),
    FLAG_NAME(KOOPA),
    FLAG_NAME(KOOPA_SHELL),
    FLAG_NAME(KOOPA_SHELL_WATER),
    FLAG_NAME(BOBOMB),
    FLAG_NAME(BOBOMB_BUDDY),
    FLAG_NAME(BOBOMB_BUDDY_CANNON),
    FLAG_NAME(CHAIN_CHOMP),
    FLAG_NAME(HOOT),
    FLAG_NAME(WHOMP),
    FLAG_NAME(THWOMP),
    FLAG_NAME(BULLET_BILL),
    FLAG_NAME(SPINDRIFT),
    FLAG_NAME(SMALL_PENGUIN),
    FLAG_NAME(SNOWMAN),
    FLAG_NAME(MR_BLIZZARD),

    // Page 2
    FLAG_NAME(BOO),
    FLAG_NAME(BIG_BOO),
    FLAG_NAME(SCUTTLEBUG),
    FLAG_NAME(MAD_PIANO),
    FLAG_NAME(CRAZY_BOX),
    FLAG_NAME(MR_I),
    FLAG_NAME(SWOOP),
    FLAG_NAME(SNUFIT),
    FLAG_NAME(MONTY_MOLE),
    FLAG_NAME(DORRIE),
    FLAG_NAME(BULLY),
    FLAG_NAME(FLY_GUY),
    FLAG_NAME(POKEY),
    FLAG_NAME(AMP),
    FLAG_NAME(GRINDEL),
    FLAG_NAME(FIRE_SPITTER),

    // Page 3
    FLAG_NAME(CHEEP_CHEEP),
    FLAG_NAME(SUSHI_SHARK),
    FLAG_NAME(MONEY_BAG),
    FLAG_NAME(SKEETER),
    FLAG_NAME(HEAVE_HO),
    FLAG_NAME(CHUCKYA),
    FLAG_NAME(HAT_UKIKI),
    FLAG_NAME(CAGE_UKIKI),
    FLAG_NAME(GOOMBA_HUGE),
    FLAG_NAME(GOOMBA_TINY),
    FLAG_NAME(LAKITU),
    FLAG_NAME(SPINY),
    FLAG_NAME(TOAD),
    FLAG_NAME(MIPS),
    FLAG_NAME(FLAMING_BOBOMB),
    FLAG_NAME(YOSHI),
};

//
// Render
//

static void bhv_omm_stats_board_render_time(f32 t, s16 bgx, s16 bgw, s16 dx, s16 y, u64 value) {
    u64 ms = (1000llu * value) / 30llu;
    u64 s = (ms / 1000llu) % 60;
    u64 m = (ms / 60000llu) % 60;
    u64 h = (ms / 3600000llu);
    str_t valueStr;
    str_fmt(valueStr, sizeof(valueStr), "%llu:%02llu:%02llu", h, m, s);
    u8 *valueStr64 = omm_text_convert(valueStr, false);
    s16 x = bgx + bgw - (8 + dx + omm_render_get_string_width(valueStr64)) * t;
    omm_render_string_sized(x, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, valueStr64, false);
    for (u8 *c = valueStr64; *c != 0xFF; ++c) {
        if (*c >= 1 && *c <= 9) {
            *c = 0xFF;
            break;
        }
    }
    omm_render_string_sized(x, y, 8 * t, 8 * t, 0x40, 0x40, 0x40, 0xFF * t, valueStr64, false);
}

Gfx *omm_render_stats_board(Gfx *pos, Gfx *gfx, f32 t) {
    gSPDisplayList(pos, pos + 3);
    gSPDisplayList(pos + 1, gfx);
    gSPEndDisplayList(pos + 2);
    pos += 3;

    // Render board
    Gfx *displayListHead = gDisplayListHead;
    gDisplayListHead = gfx;
    if (t > 0) {

        // Black background
        s16 bgw = OMM_STATS_BOARD_WIDTH * t;
        s16 bgh = OMM_STATS_BOARD_HEIGHT * t;
        s16 bgx = (SCREEN_WIDTH - bgw) / 2;
        s16 bgy = (SCREEN_HEIGHT - bgh) / 2;
        omm_render_texrect(bgx, bgy, bgw, bgh, 32, 32, 0x00, 0x00, 0x00, 0xC0 * t, OMM_TEXTURE_MISC_WHITE, false);

        // Vertical arrows
        s16 aw = 8 * t;
        s16 ah = 8 * t;
        s16 vaxl = (SCREEN_WIDTH) / 2 - aw;
        s16 vaxr = (SCREEN_WIDTH) / 2;
        s16 vayu = (SCREEN_HEIGHT + bgh) / 2 + 4 * t;
        s16 vayd = (SCREEN_HEIGHT - bgh) / 2 - 4 * t - ah;
        omm_render_texrect(vaxl, vayu, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_DOWN_RIGHT, false);
        omm_render_texrect(vaxr, vayu, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_DOWN_LEFT, false);
        omm_render_texrect(vaxl, vayd, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_UP_RIGHT, false);
        omm_render_texrect(vaxr, vayd, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_UP_LEFT, false);

        // Horizontal arrows
        if (bhv_omm_stats_board_get_sub_page_count(sOmmStatsBoardPageScroll.idx) > 1) {
            s16 haxl = (SCREEN_WIDTH - bgw) / 2 - 4 * t - aw;
            s16 haxr = (SCREEN_WIDTH + bgw) / 2 + 4 * t;
            s16 hayu = (SCREEN_HEIGHT / 2);
            s16 hayd = (SCREEN_HEIGHT / 2) - ah;
            omm_render_texrect(haxl, hayu, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_DOWN_RIGHT, false);
            omm_render_texrect(haxl, hayd, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_UP_RIGHT, false);
            omm_render_texrect(haxr, hayu, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_DOWN_LEFT, false);
            omm_render_texrect(haxr, hayd, aw, ah, 32, 32, 0xFF, 0xFF, 0xFF, 0xFF * t, OMM_TEXTURE_MISC_WHITE_UP_LEFT, false);
        }

        // Current page
        switch (sOmmStatsBoardPageScroll.idx) {
            case OMM_STATS_BOARD_PAGE_COLLECTIBLES:
            case OMM_STATS_BOARD_PAGE_ACTIONS: {
                const struct { const char *name; u64 value; } OMM_STATS_BOARD_SINGLE_PAGES[2][9] = {{
                    { OMM_TEXT_STATS_STARS, gOmmStats->starsCollected },
                    { OMM_TEXT_STATS_SPARKLY_STARS, gOmmStats->sparklyStarsCollected },
                    { OMM_TEXT_STATS_COINS, gOmmStats->coinsCollected },
                    { OMM_TEXT_STATS_CAPS, gOmmStats->capsCollected },
                    { OMM_TEXT_STATS_MUSHROOMS_1UP, gOmmStats->mushrooms1upCollected },
                    { OMM_TEXT_STATS_SECRETS, gOmmStats->secretsCollected },
                    { OMM_TEXT_STATS_EXCLAMATION_BOXES, gOmmStats->exclamationBoxesBroken },
                    { OMM_TEXT_STATS_ENEMIES, gOmmStats->enemiesDefeated },
                    { OMM_TEXT_STATS_BOWSERS, gOmmStats->bowsersDefeated },
                }, {
                    { OMM_TEXT_STATS_A_PRESSES, gOmmStats->aPresses },
                    { OMM_TEXT_STATS_JUMPS, gOmmStats->jumps },
                    { OMM_TEXT_STATS_ATTACKS, gOmmStats->attacks },
                    { OMM_TEXT_STATS_CAPPY_THROWS, gOmmStats->cappyThrows },
                    { OMM_TEXT_STATS_CAPPY_BOUNCES, gOmmStats->cappyBounces },
                    { OMM_TEXT_STATS_CAPTURES, gOmmStats->captures },
                    { OMM_TEXT_STATS_HITS_TAKEN, gOmmStats->hitsTaken },
                    { OMM_TEXT_STATS_RESTARTS, gOmmStats->restarts },
                    { OMM_TEXT_STATS_DEATHS, gOmmStats->deaths },
                }};
                for (s32 i = 0; i != 9; ++i) {
                    str_t valueStr;
                    str_fmt(valueStr, sizeof(valueStr), "%llu", OMM_STATS_BOARD_SINGLE_PAGES[sOmmStatsBoardPageScroll.idx][i].value);
                    const u8 *nameStr64 = omm_text_convert(OMM_STATS_BOARD_SINGLE_PAGES[sOmmStatsBoardPageScroll.idx][i].name, false);
                    const u8 *valueStr64 = omm_text_convert(valueStr, false);
                    s16 xl = bgx + 8 * t;
                    s16 xr = bgx + bgw - (8 + omm_render_get_string_width(valueStr64)) * t;
                    s16 y = bgy + bgh - (i + 1) * 16 * t;
                    omm_render_string_sized(xl, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, nameStr64, false);
                    omm_render_string_sized(xr, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, valueStr64, false);
                }
            } break;

            case OMM_STATS_BOARD_PAGE_DISTANCE:
            case OMM_STATS_BOARD_PAGE_TIME: {
                const struct { const char *name; u64 *values; } OMM_STATS_BOARD_DOUBLE_PAGES[2][7] = {{
                    { OMM_TEXT_STATS_TOTAL, gOmmStats->distanceTotal },
                    { OMM_TEXT_STATS_ON_GROUND, gOmmStats->distanceOnGround },
                    { OMM_TEXT_STATS_AIRBORNE, gOmmStats->distanceAirborne },
                    { OMM_TEXT_STATS_UNDERWATER, gOmmStats->distanceUnderwater },
                    { OMM_TEXT_STATS_WING_CAP, gOmmStats->distanceWingCap },
                    { OMM_TEXT_STATS_METAL_CAP, gOmmStats->distanceMetalCap },
                    { OMM_TEXT_STATS_VANISH_CAP, gOmmStats->distanceVanishCap },
                }, {
                    { OMM_TEXT_STATS_TOTAL, gOmmStats->timeTotal },
                    { OMM_TEXT_STATS_ON_GROUND, gOmmStats->timeOnGround },
                    { OMM_TEXT_STATS_AIRBORNE, gOmmStats->timeAirborne },
                    { OMM_TEXT_STATS_UNDERWATER, gOmmStats->timeUnderwater },
                    { OMM_TEXT_STATS_WING_CAP, gOmmStats->timeWingCap },
                    { OMM_TEXT_STATS_METAL_CAP, gOmmStats->timeMetalCap },
                    { OMM_TEXT_STATS_VANISH_CAP, gOmmStats->timeVanishCap },
                }};
                bool isTime = (sOmmStatsBoardPageScroll.idx == OMM_STATS_BOARD_PAGE_TIME);
                const u8 *titleStr64 = omm_text_convert(isTime ? OMM_TEXT_STATS_TIME : OMM_TEXT_STATS_DISTANCE, false);
                const u8 *column0Str64 = omm_text_convert(OMM_TEXT_STATS_MARIO, false);
                const u8 *column1Str64 = omm_text_convert(OMM_TEXT_STATS_CAPTURE, false);
                s16 xl = bgx + 8 * t;
                s16 xr0 = bgx + bgw - (8 + OMM_STATS_BOARD_WIDTH / 3 + omm_render_get_string_width(column0Str64)) * t;
                s16 xr1 = bgx + bgw - (8 + omm_render_get_string_width(column1Str64)) * t;
                s16 yt0 = bgy + bgh - 16 * t;
                s16 yt1 = bgy + bgh - 32 * t;
                omm_render_string_sized(xl, yt0, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, titleStr64, false);
                omm_render_string_sized(xr0, yt1, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, column0Str64, false);
                omm_render_string_sized(xr1, yt1, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, column1Str64, false);
                for (s32 i = 0; i != 7; ++i) {
                    const u8 *nameStr64 = omm_text_convert(OMM_STATS_BOARD_DOUBLE_PAGES[isTime][i].name, false);
                    s16 xl = bgx + 8 * t;
                    s16 y = bgy + bgh - (i + 3) * 16 * t;
                    omm_render_string_sized(xl, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, nameStr64, false);
                    u64 value0 = OMM_STATS_BOARD_DOUBLE_PAGES[isTime][i].values[0];
                    u64 value1 = OMM_STATS_BOARD_DOUBLE_PAGES[isTime][i].values[1];
                    if (isTime) {
                        bhv_omm_stats_board_render_time(t, bgx, bgw, OMM_STATS_BOARD_WIDTH / 3, y, value0);
                        if (i < 4) { // Don't display cap values for captures
                            bhv_omm_stats_board_render_time(t, bgx, bgw, 0, y, value1);
                        }
                    } else {
                        f64 toMeters = 1.55 / 120.0; // Mario's human height (155 cm) / Mario's model height (120 u)
                        str_t value0Str; str_fmt(value0Str, sizeof(value0Str), "%llu", (u64) (value0 * toMeters));
                        str_t value1Str; str_fmt(value1Str, sizeof(value1Str), "%llu", (u64) (value1 * toMeters));
                        const u8 *value0Str64 = omm_text_convert(value0Str, false);
                        const u8 *value1Str64 = omm_text_convert(value1Str, false);
                        s16 xr0 = bgx + bgw - (8 + OMM_STATS_BOARD_WIDTH / 3 + omm_render_get_string_width(value0Str64)) * t;
                        s16 xr1 = bgx + bgw - (8 + omm_render_get_string_width(value1Str64)) * t;
                        omm_render_string_sized(xr0, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, value0Str64, false);
                        if (i < 4) { // Don't display cap values for captures
                            omm_render_string_sized(xr1, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, value1Str64, false);
                        }
                    }
                }
            } break;

            case OMM_STATS_BOARD_PAGE_CAPTURES: {
                u64 availableCaptureFlags = omm_level_get_all_available_captures(OMM_GAME_MODE);
                s32 availableCaptureCount = omm_level_get_all_available_captures_count(OMM_GAME_MODE);
                u64 captureFlags = omm_save_file_get_capture_flags(gCurrSaveFileNum - 1, OMM_GAME_MODE);
                s32 captureCount = omm_save_file_get_capture_count(gCurrSaveFileNum - 1, OMM_GAME_MODE);
                str_t countStr; str_fmt(countStr, sizeof(countStr), "%d", captureCount);
                str_t totalStr; str_fmt(totalStr, sizeof(totalStr), " / %d", availableCaptureCount);
                const u8 *titleStr64 = omm_text_convert(OMM_TEXT_STATS_CAPTURES, false);
                const u8 *countStr64 = omm_text_convert(countStr, false);
                const u8 *totalStr64 = omm_text_convert(totalStr, false);
                s16 xl = bgx + 8 * t;
                s16 xr0 = bgx + bgw - (8 + omm_render_get_string_width(totalStr64)) * t;
                s16 xr1 = xr0 - omm_render_get_string_width(countStr64) * t;
                s16 y = bgy + bgh - 16 * t;
                omm_render_string_sized(xl, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, titleStr64, false);
                omm_render_string_sized(xr0, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, totalStr64, false);
                omm_render_string_sized(xr1, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, countStr64, false);
                for (s32 i = 0, j = 0, j0 = sOmmStatsBoardSubScroll.idx * 16, j1 = (sOmmStatsBoardSubScroll.idx + 1) * 16; i != OMM_MAX_CAPTURES && j != j1; ++i) {
                    u64 flag = OMM_STATS_BOARD_CAPTURES[i].flag;
                    if (flag & availableCaptureFlags) {
                        if (j >= j0) {
                            s16 row = (j - j0) % 8;
                            s16 col = (j - j0) / 8;
                            const u8 *nameStr64 = omm_text_convert(OMM_STATS_BOARD_CAPTURES[i].name, false);
                            s16 x = bgx + (col ? (bgw - (8 + omm_render_get_string_width(nameStr64)) * t) : (8 * t));
                            s16 y = bgy + bgh - (row + 2) * 16 * t;
                            if (flag & captureFlags) {
                                omm_render_string_sized(x, y, 8 * t, 8 * t, 0x00, 0xFF, 0xFF, 0xFF * t, nameStr64, false);
                            } else {
                                omm_render_string_sized(x, y, 8 * t, 8 * t, 0x80, 0x80, 0x80, 0xFF * t, nameStr64, false);
                            }
                        }
                        j++;
                    }
                }
            } break;

            case OMM_STATS_BOARD_PAGE_REWARDS: {
                str_t countStr; str_fmt(countStr, sizeof(countStr), "%u", omm_rewards_get_unlocked_count());
                str_t totalStr; str_fmt(totalStr, sizeof(totalStr), " / %u", omm_rewards_get_count());
                const u8 *titleStr64 = omm_text_convert(OMM_TEXT_STATS_REWARDS, false);
                const u8 *countStr64 = omm_text_convert(countStr, false);
                const u8 *totalStr64 = omm_text_convert(totalStr, false);
                s16 xl = bgx + 8 * t;
                s16 xr0 = bgx + bgw - (8 + omm_render_get_string_width(totalStr64)) * t;
                s16 xr1 = xr0 - omm_render_get_string_width(countStr64) * t;
                s16 y = bgy + bgh - 16 * t;
                omm_render_string_sized(xl, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, titleStr64, false);
                omm_render_string_sized(xr0, y, 8 * t, 8 * t, 0xFF, 0xFF, 0xFF, 0xFF * t, totalStr64, false);
                omm_render_string_sized(xr1, y, 8 * t, 8 * t, 0xFF, 0xFF, 0x80, 0xFF * t, countStr64, false);
                s32 index = sOmmStatsBoardSubScroll.idx;
                bool unlocked = omm_rewards_is_unlocked(index, false);
                const u8 *rewardTitleStr64 = omm_text_convert(unlocked ? omm_rewards_get_title(index) : OMM_TEXT_LEVEL_UNKNOWN, false);
                s16 rewardTitleY = bgy + bgh - 32 * t;
                if (unlocked) {
                    omm_render_string_sized(xl, rewardTitleY, 8 * t, 8 * t, 0x00, 0xFF, 0x00, 0xFF * t, rewardTitleStr64, false);
                } else {
                    omm_render_string_sized(xl, rewardTitleY, 8 * t, 8 * t, 0x80, 0x80, 0x80, 0xFF * t, rewardTitleStr64, false);
                }
                for (s32 line = 0; line != (unlocked ? 7 : 4); ++line) {
                    const u8 *rewardLineStr64 = omm_text_convert(omm_rewards_get_line(index, line, !unlocked), false);
                    s16 rewardLineY = bgy + bgh - (line + 3) * 16 * t;
                    omm_render_string_sized(xl, rewardLineY, 8 * t, 8 * t, 0xC0, 0xC0, 0xC0, 0xFF * t, rewardLineStr64, false);
                }
            } break;
        }
    }
    gSPEndDisplayList(gDisplayListHead++);
    gDisplayListHead = displayListHead;
    return pos;
}

OMM_ROUTINE_PRE_RENDER(bhv_omm_stats_board_render) {
    struct Object *activeBoard = obj_get_first_with_behavior_and_field_s32(bhvOmmStatsBoard, _FIELD(oAction), 1);
    if (activeBoard) {

        // Interpolate and render
        f32 t = sqr_f(relerp_0_1_f(abs_f(activeBoard->oSubAction), 0, OMM_STATS_BOARD_FADE, 0, 1));
        Gfx *gfx = omm_alloc_gfx(0x1000);
        interp_data_update(sOmmStatsBoard, t != 0, gDisplayListHead, gfx, NULL, 0, 0, 0, 0, 0, t);
        gDisplayListHead = omm_render_stats_board(gDisplayListHead, sOmmStatsBoard->gfx, sOmmStatsBoard->t0);

        // Inputs
        if (t == 1) {

            // Change the current page
            bhv_omm_stats_board_update_scrolls();

            // Close the board
            if (gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON)) {
                activeBoard->oSubAction = -OMM_STATS_BOARD_FADE;
                return;
            }
        }
        return;
    }
    interp_data_reset(sOmmStatsBoard);
}

//
// Behavior
//

static void bhv_omm_stats_board_update() {
    struct MarioState *m = gMarioState;
    struct Object *o = gCurrentObject;
    o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
    switch (o->oAction) {

        // Default
        case 0: {
            o->oIntangibleTimer = 0;
            obj_load_collision_model(o);
            if (!omm_mario_is_capture(m) && omm_mario_is_ground_pound_landing(m) && m->marioObj->platform == o) {
                omm_mario_set_action(m, ACT_READING_AUTOMATIC_DIALOG, OMM_DIALOG_STATS_BOARD_RESET, 0);
                o->oDialogResponse = 0;
                o->oAction = 2;
                m->usedObj = o;
            }
        } break;

        // Stats board open
        case 1: {
            if (++(o->oSubAction) == 0) {
                mem_zero(&sOmmStatsBoardPageScroll, sizeof(sOmmStatsBoardPageScroll));
                mem_zero(&sOmmStatsBoardSubScroll, sizeof(sOmmStatsBoardSubScroll));
                o->oAction = 0;
            }
        } break;

        // Wait for the dialog to end
        case 2: {
            obj_load_collision_model(o);
            if (m->action != ACT_READING_AUTOMATIC_DIALOG) {
                if (o->oDialogResponse == 2) {
                    mem_zero(gOmmStats, sizeof(gOmmStats));
                    omm_mario_set_action(m, ACT_HARD_BACKWARD_AIR_KB, 1, 0);
                    set_camera_shake_from_point(SHAKE_POS_SMALL, o->oPosX, o->oPosY, o->oPosZ);
                    obj_spawn_white_puff(o, SOUND_GENERAL_BREAK_BOX);
                    obj_spawn_triangle_break_particles(o, OBJ_SPAWN_TRI_BREAK_PRESET_TRIANGLES_30);
                    obj_scale(o, 0.f);
                    o->oAction = 3;
                } else {
                    o->oAction = 0;
                }
            }
        } break;

        // Grow a new signpost
        case 3: {
            if (o->oTimer > 90) {
                o->oAction = 0;
            } else if (o->oTimer > 75) {
                obj_scale_xyz(o, 1.f, relerp_0_1_f(o->oTimer, 75, 90, 0.f, 1.f), 1.f);
            }
        } break;
    }
}

const BehaviorScript bhvOmmStatsBoard[] = {
    OBJ_TYPE_SURFACE,
    BHV_OR_INT(oFlags, OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE),
    BHV_LOAD_COLLISION_DATA(wooden_signpost_seg3_collision_0302DD80),
    BHV_SET_INTERACT_TYPE(INTERACT_TEXT),
    BHV_SET_INT(oInteractionSubtype, INT_SUBTYPE_SIGN),
    BHV_DROP_TO_FLOOR(),
    BHV_SET_HITBOX(150, 80),
    BHV_BEGIN_LOOP(),
        BHV_SET_INT(oIntangibleTimer, -1),
        BHV_CALL_NATIVE(bhv_omm_stats_board_update),
        BHV_SET_INT(oInteractStatus, 0),
    BHV_END_LOOP(),
};

//
// Spawner
//

struct Object *omm_obj_spawn_stats_board(struct Object *o, f32 x, f32 y, f32 z, s16 yaw) {
    struct Object *board = obj_spawn_from_geo(o, wooden_signpost_geo, bhvOmmStatsBoard);
    board->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
    obj_set_xyz(board, x, y, z);
    obj_set_angle(board, 0, yaw, 0);
    obj_set_scale(board, 1, 1, 1);
    board->oAction = 0;
    board->oSubAction = 0;
    return board;
}
