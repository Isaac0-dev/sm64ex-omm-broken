#ifndef OMM_PATCHES_H
#define OMM_PATCHES_H

//
// Patches
//

#if defined(DYNOS) || defined(R96X)
#define OMM_CODE_DYNOS 1
#else
#define OMM_CODE_DYNOS 0
#endif

#if defined(TIME_TRIALS)
#ifdef OMM_ALL_HEADERS
#include "game/time_trials.h"
#endif
#define OMM_CODE_TIME_TRIALS 1
#else
#define OMM_CODE_TIME_TRIALS 0
#endif

//
// Some preprocessor magic
// That's a really awful way of coding, but it avoids patching files for a couple diffs :)
//

// Prevent the game from playing the demo and inhibiting X and Y buttons
#define run_demo_inputs_0 ;
#define run_demo_inputs_1 run_demo_inputs_unused()
#define run_demo_inputs(...) CAT(run_demo_inputs_, N_ARGS(__VA_ARGS__))

// Prevent the game from applying the old 60 FPS interpolation
#define patch_interpolations_0 ;
#define patch_interpolations_1 patch_interpolations()
#define patch_interpolations(...) CAT(patch_interpolations_, N_ARGS(__VA_ARGS__))

// render_hud is replaced by omm_render_hud
#define render_hud_0 omm_render_hud()
#define render_hud_1 render_hud()
#define render_hud(...) CAT(render_hud_, N_ARGS(__VA_ARGS__))

// render_pause_courses_and_castle is replaced by omm_render_pause
#define render_pause_courses_and_castle_0 omm_render_pause()
#define render_pause_courses_and_castle_1 render_pause_courses_and_castle()
#define render_pause_courses_and_castle(...) CAT(render_pause_courses_and_castle_, N_ARGS(__VA_ARGS__))
#define render_pause_screen_0 omm_render_pause()
#define render_pause_screen_1 render_pause_screen()
#define render_pause_screen(...) CAT(render_pause_screen_, N_ARGS(__VA_ARGS__))

// render_course_complete_screen is replaced by omm_render_course_complete
#define render_course_complete_screen_0 omm_render_course_complete()
#define render_course_complete_screen_1 render_course_complete_screen()
#define render_course_complete_screen(...) CAT(render_course_complete_screen_, N_ARGS(__VA_ARGS__))

// bhv_mario_update uses OMM Mario update
#define bhv_mario_update_0 bhv_mario_update()
#define bhv_mario_update_1 bhv_mario_update_unused(void)
#define bhv_mario_update(...) CAT(bhv_mario_update_, N_ARGS(__VA_ARGS__))

// cur_obj_within_12k_bounds always returns TRUE
#define cur_obj_within_12k_bounds_0 TRUE
#define cur_obj_within_12k_bounds_1 cur_obj_within_12k_bounds_unused()
#define cur_obj_within_12k_bounds(...) CAT(cur_obj_within_12k_bounds_, N_ARGS(__VA_ARGS__))

// cur_obj_is_mario_ground_pounding_platform now works with any type of ground pound
#define cur_obj_is_mario_ground_pounding_platform_0 (gMarioObject->platform == o && omm_mario_is_ground_pound_landing(gMarioState))
#define cur_obj_is_mario_ground_pounding_platform_1 cur_obj_is_mario_ground_pounding_platform(void)
#define cur_obj_is_mario_ground_pounding_platform(...) CAT(cur_obj_is_mario_ground_pounding_platform_, N_ARGS(__VA_ARGS__))

// Star Road: Don't print the Hard Mode strings
#if OMM_GAME_IS_SMSR
#define print_hard_mode_strings_0 
#define print_hard_mode_strings_1 print_hard_mode_strings(void)
#define print_hard_mode_strings(...) CAT(print_hard_mode_strings_, N_ARGS(__VA_ARGS__))
#endif

// DynOS init makes a call to omm_opt_init before initializing itself
#define DynOS_Init \
DynOS_Init_Startup_() { extern void DynOS_Init_Omm_(); DynOS_Init_Omm_(); } \
extern "C" { extern void omm_opt_init(); } \
void DynOS_Init_Omm_() { extern void DynOS_Init_(); omm_opt_init(); DynOS_Init_(); } \
void DynOS_Init_

// Redefine the play_sequence function if it was replaced by the static seq_player_play_sequence
#if music_play_sequence_define
#define unused_8031E4F0 \
placeholder_func() {} \
static void seq_player_play_sequence(u8 player, u8 seqId, u16 arg2); \
void play_sequence(u8 player, u8 seqId, u16 arg2) { \
    seq_player_play_sequence(player, seqId, arg2); \
} void unused_8031E4F0
#endif

// If the boss sequence is missing, the level music is not going to restart...
// So, add the boss music in the queue to force the stop_background_music
// function to remove it from the queue and restart the level music.
#define stop_background_music_fix_boss_music() \
if ((seqId & 0xFF) == SEQ_EVENT_BOSS) { \
    for (u8 i = 0; i <= sBackgroundMusicQueueSize; ++i) { \
        if (i == sBackgroundMusicQueueSize) { \
            sBackgroundMusicQueueSize = min_s(sBackgroundMusicQueueSize + 1, MUSIC_QUEUE_MAX_SIZE); \
            omm_move(sBackgroundMusicQueue + 1, sBackgroundMusicQueue, sizeof(sBackgroundMusicQueue[0]) * (MUSIC_QUEUE_MAX_SIZE - 1)); \
            sBackgroundMusicQueue[0].seqId = SEQ_EVENT_BOSS; \
            sBackgroundMusicQueue[0].priority = 4; \
            break; \
        } else if (sBackgroundMusicQueue[i].seqId == SEQ_EVENT_BOSS) { \
            break; \
        } \
    } \
}

// Compute the lower bound before resetting gDialogScrollOffsetY to not break frame interpolation
#define fix_dialog_box_text_lower_bound() \
lowerBound = (gDialogScrollOffsetY / 16) + 1; \
if (gDialogScrollOffsetY >= dialog->linesPerBox * DIAG_VAL1) { \
    gDialogTextPos = gLastDialogPageStrPos; \
    gDialogBoxState = DIALOG_STATE_VERTICAL; \
    gDialogScrollOffsetY = 0; \
} \
break

// Fix annoying crashes that can occur in rooms with paintings
#define geo_painting_update_fix_floor() \
gLastPaintingUpdateCounter = gPaintingUpdateCounter; \
gPaintingUpdateCounter = gAreaUpdateCounter; \
extern struct MarioState *gMarioState; \
if (gMarioState->floor) { \
    surface = gMarioState->floor; \
} else { \
    find_floor(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ, &surface); \
} \
if (surface) { \
    gPaintingMarioFloorType = surface->type; \
    gPaintingMarioXPos = gMarioObject->oPosX; \
    gPaintingMarioYPos = gMarioObject->oPosY; \
    gPaintingMarioZPos = gMarioObject->oPosZ; \
} \
return NULL;

// Call omm_render_menu_update before drawing the cursor
#ifdef FILE_SELECT_H
#define get_str_x_pos_from_center(...) 10000
static s8 sSelectedButtonID;
static u8 sTextBaseAlpha;
static f32 sCursorPos[];

#define print_menu_cursor_0 print_menu_cursor()
#define print_menu_cursor_1 \
print_menu_cursor(void) { \
    extern void omm_render_menu_update(f32 *cursorPos, s8 selectedButtonId, u8 alpha); \
    omm_render_menu_update(sCursorPos, sSelectedButtonID, sTextBaseAlpha); \
    extern void print_menu_cursor_(void); \
    print_menu_cursor_(); \
} \
void print_menu_cursor_(void)
#define print_menu_cursor(...) CAT(print_menu_cursor_, N_ARGS(__VA_ARGS__))
#endif

// Call omm_camera_update and return if TRUE
#ifdef INCLUDED_FROM_CAMERA_C
#define update_camera \
update_camera(struct Camera *c) { \
    OMM_RETURN_IF_TRUE(omm_camera_update(c, gMarioState), , ); \
    extern void update_camera_(struct Camera *c); \
    update_camera_(c); \
} \
void update_camera_
#endif

#endif