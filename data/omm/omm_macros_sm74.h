#ifndef OMM_MACROS_SM74_H
#define OMM_MACROS_SM74_H

// Game macros
#define OMM_GAME_TYPE                                   OMM_GAME_SM74
#define OMM_GAME_SAVE                                   OMM_GAME_SM74
#define OMM_GAME_MODE                                   (gCurrAreaIndex - 1)
#define OMM_GAME_MENU                                   "sm74"
#define OMM_GAME_RF14                                   1

// SM74 stuff
#define OMM_SM74_MODE_NORMAL                            (SM74_MODE_NORMAL - 1)
#define OMM_SM74_MODE_EXTREME                           (SM74_MODE_EXTREME - 1)

// Better camera
#define BETTER_CAM_IS_PUPPY_CAM                         1
#define BETTER_CAM_IS_ENABLED                           gPuppyCam.enabled
#define BETTER_CAM_MODE                                 0
#define BETTER_CAM_YAW                                  0
#define BETTER_CAM_RAYCAST_ARGS                         __EXPAND(, UNUSED s32 flags)
#define BETTER_CAM_MOUSE_CAM                            (BETTER_CAM_IS_ENABLED && configCameraMouse)
#define mouse_x                                         gMouseDeltaX
#define mouse_y                                         gMouseDeltaY
#define gMouseXPos                                      gMouseDeltaX
#define gMouseYPos                                      gMouseDeltaY
#define gOldMouseXPos                                   gMouseDeltaX
#define gOldMouseYPos                                   gMouseDeltaY

// Animation
#define struct_AnimInfo                                 struct AnimInfo
#define mAreaIndex                                      areaIndex
#define mActiveAreaIndex                                activeAreaIndex
#define mModel                                          model
#define mAnimInfo                                       animInfo
#define mStartFrame                                     startFrame
#define mLoopStart                                      loopStart
#define mLoopEnd                                        loopEnd

// Mario animation
#define struct_MarioAnimations                          struct DmaHandlerList
#define struct_MarioAnimDmaTable                        struct DmaTable
#define gMarioAnimations                                gMarioState->animList
#define gMarioAnimDmaTable                              gMarioAnimations->dmaTable
#define gMarioCurrAnimAddr                              gMarioAnimations->currentAddr
#define gMarioTargetAnim                                gMarioAnimations->bufTarget
#define gMarioAnimsData                                 gMarioAnimsBuf

// Audio
#define gGlobalSoundArgs                                gGlobalSoundSource
#define gAcousticReachPerLevel                          sLevelAcousticReaches
#define audio_play_wing_cap_music()                     { play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP)); }
#define audio_play_metal_cap_music()                    { play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP)); }
#define audio_play_vanish_cap_music()                   { play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP)); }
#define audio_stop_cap_music()                          { stop_cap_music(); }
#define audio_play_shell_music()                        { play_shell_music(); }
#define audio_stop_shell_music()                        { stop_shell_music(); }
#define audio_play_course_clear()                       { play_course_clear(); }
#define audio_stop_course_clear()                       {}
#define audio_play_puzzle_jingle()                      { play_puzzle_jingle(); }
#define audio_play_toads_jingle()                       { play_toads_jingle(); }
#define audio_play_star_jingle()                        { play_power_star_jingle(1); }
#define audio_play_star_fanfare()                       { play_star_fanfare(); }

// Music
#define music_pause()                                   { gSequencePlayers[0].muted = 1; }
#define music_resume()                                  { gSequencePlayers[0].muted = 0; }
#define music_stop()                                    {}
#define audio_mute(...)                                 { set_audio_muted(__VA_ARGS__); }
#define music_fade_out(...)                             { seq_player_fade_out(__VA_ARGS__); }
#define music_lower_volume(...)                         { seq_player_lower_volume(__VA_ARGS__); }
#define music_unlower_volume(...)                       { seq_player_unlower_volume(__VA_ARGS__); }

// Sound
#define gSfxMute                                        0
#define sound_stop(...)                                 { stop_sound(__VA_ARGS__); }
#define sound_stop_from_source(...)                     { stop_sounds_from_source(__VA_ARGS__); }
#define sound_stop_continuous()                         { stop_sounds_in_continuous_banks(); }
#define sound_set_moving_speed(...)                     { set_sound_moving_speed(__VA_ARGS__); }

// Object fields
#define oSnowmansBodyScale                              oSnowmansBottomScale
#define oSkeeterTurnAwayFromWall                        oSkeeterTurningAwayFromWall
#define oBitsPlatformBowserObject                       oBitsPlatformBowser
#define oBowserCameraState                              oBowserCamAct
#define oBowserFlags                                    oBowserStatus
#define oBowserActionTimer                              oBowserTimer
#define oBowserBitsJump                                 oBowserBitsJustJump
#define oBowserSpitFireNumFlames                        oBowserRandSplitFloor
#define oBowserGrabbedAction                            oBowserGrabbedStatus
#define oBowserActionSelected                           oBowserIsReacting
#define oBowserOpacityTarget                            oBowserTargetOpacity
#define oBowserBlinkTimer                               oBowserEyesTimer
#define oBowserRainbowLightEffect                       oBowserRainbowLight
#define oBowserShockwaveScale                           oBowserShockWaveScale

// Text
#define gCourseNameTable(mode)                          ((const u8 **) (mode ? seg2_course_name_table_EE : seg2_course_name_table))
#define gActNameTable(mode)                             ((const u8 **) (mode ? seg2_act_name_table_EE : seg2_act_name_table))
#define gDialogTable(mode)                              ((struct DialogEntry **) (mode ? seg2_dialog_table_EE : seg2_dialog_table))

// Level scripts
#define level_script_entry_point                        level_script_entry
#define level_script_splash_screen                      level_intro_splash_screen
#define level_script_goddard_regular                    level_intro_mario_head_regular
#define level_script_goddard_game_over                  level_intro_mario_head_dizzy
#define level_script_debug_level_select                 level_intro_entry_4
#define level_script_to_file_select                     script_intro_L1
#define level_script_to_debug_level_select              script_intro_L2
#define level_script_to_star_select_1                   script_intro_L3
#define level_script_to_star_select_2                   script_intro_L4
#define level_script_to_splash_screen                   script_intro_L5
#define level_script_file_select                        level_main_menu_entry_1
#define level_script_star_select                        level_main_menu_entry_2
#define level_script_cake_ending                        level_ending_entry

// OMM
#define OMM_MARIO_COLORS                                1
#define OMM_CAP_COLORS                                  1
#define OMM_STAR_COLORS                                 0, 1, 4, 3, 2, 5, 6, 10, 8, 14, 11, 9, 12, 7, 13, 15, 16, 17, 18, 19, \
                                                        0, 14, 2, 3, 5, 9, 6, 10, 1, 7, 4, 11, 12, 13, 8, 15, 16, 17, 18, 19,
#define OMM_STAR_COLOR_OFFSET(modeIndex)                (modeIndex * 20)
#define OMM_STAR_COLOR_COUNT                            40
#define OMM_LEVEL_ENTRY_WARP(levelNum)                  0x0A
#define OMM_LEVEL_EXIT_DISTANCE                         150
#define OMM_LEVEL_SLIDE                                 LEVEL_PSS
#define OMM_LEVEL_ENTRY_POINT                           LEVEL_CASTLE_COURTYARD
#define OMM_LEVEL_RETURN_TO_CASTLE                      LEVEL_CASTLE_COURTYARD, OMM_GAME_MODE + 1, 0x40, 0
#define OMM_LEVEL_YELLOW_COIN_BOO                       LEVEL_NONE
#define OMM_LEVEL_EXCLUDE_LIST                          LEVEL_NONE
#define OMM_LEVEL_HAS_BOWSER                            1
#define OMM_SEQ_MAIN_MENU                               SEQ_MENU_TITLE_SCREEN
#define OMM_SEQ_FILE_SELECT                             SEQ_MENU_FILE_SELECT
#define OMM_SEQ_STAR_SELECT                             SEQ_MENU_STAR_SELECT
#define OMM_SEQ_PALETTE_EDITOR                          SEQ_MENU_TITLE_SCREEN
#define OMM_STATS_BOARD_LEVEL                           (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? LEVEL_CASTLE_COURTYARD : LEVEL_CASTLE_COURTYARD)
#define OMM_STATS_BOARD_AREA                            (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? 1 : 2)
#define OMM_STATS_BOARD_X                               (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? 3000 : -2700)
#define OMM_STATS_BOARD_Y                               (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? -861 : -900)
#define OMM_STATS_BOARD_Z                               (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? 6000 : 7000)
#define OMM_STATS_BOARD_ANGLE                           (OMM_GAME_MODE == OMM_SM74_MODE_NORMAL ? 0xC000 : 0x4000)
#define OMM_CAMERA_LOOK_UP_WARP_STARS                   10
#define OMM_CAMERA_IS_BOWSER_FIGHT                      omm_camera_is_bowser_fight()
#define OMM_NUM_PLAYABLE_CHARACTERS                     2
#define OMM_NUM_SAVE_MODES                              OMM_GAME_SM74__MODES
#define OMM_NUM_STARS_MAX_PER_COURSE                    OMM_GAME_SM74__STARS
#define OMM_NUM_ACTS_MAX_PER_COURSE                     (OMM_NUM_STARS_MAX_PER_COURSE - 1)
#define OMM_NUM_MIPS_STARS                              2
#define OMM_TEXT_FORMAT(id, str)                        str
#define STAR                                            "STAR"
#define Star                                            "Star"

// Sparkly stars
#define OMM_SPARKLY_REQUIREMENT                         (OMM_GAME_MODE ? 157 : 151)
#define OMM_SPARKLY_BLOCK_LEVEL                         LEVEL_CASTLE_COURTYARD
#define OMM_SPARKLY_BLOCK_AREA                          (OMM_GAME_MODE + 1)
#define OMM_SPARKLY_BLOCK_COUNT                         1
#define OMM_SPARKLY_BLOCK_AVAILABLE                     0b1001
#define OMM_SPARKLY_BLOCK_X                             (OMM_GAME_MODE ? 6464 : -5665)
#define OMM_SPARKLY_BLOCK_Y                             (OMM_GAME_MODE ? -920 :  -860)
#define OMM_SPARKLY_BLOCK_Z                             (OMM_GAME_MODE ? 6300 :  5365)
#define OMM_SPARKLY_BLOCK_ANGLE                         0x0000

// Extra text
#define OMM_TEXT_FS_PLAY                                OMM_TEXT_FS_SELECT_FILE_74, OMM_TEXT_FS_SELECT_FILE_EE, NULL, NULL
#define OMM_TEXT_FS_COPY                                OMM_TEXT_FS_COPY_FILE_74, OMM_TEXT_FS_COPY_FILE_EE, NULL, NULL
#define OMM_TEXT_FS_ERASE                               OMM_TEXT_FS_ERASE_FILE_74, OMM_TEXT_FS_ERASE_FILE_EE, OMM_TEXT_FS_SPARKLY_STARS, NULL
#define OMM_TEXT_FS_SCORE                               OMM_TEXT_FS_SCORES_74, OMM_TEXT_FS_SCORES_EE, OMM_TEXT_FS_SPARKLY_STARS, NULL

// Files
#define FILE_BETTERCAM_H                                "extras/bettercamera.h"
#define FILE_OPTIONS_H                                  "extras/options_menu.h"
#define FILE_SOUNDS_H                                   "sounds.h"
#define FILE_CHEATS_H                                   "extras/cheats.h"
#define FILE_MARIO_CHEATS_H                             "extras/cheats.h"
#define FILE_TITLE_H                                    "menu/title_screen.h"
#define FILE_TXT_CONV_H                                 "types.h"
#define FILE_TEXT_LOADER_H                              "types.h"
#define FILE_R96_SYSTEM_H                               "types.h"

#endif