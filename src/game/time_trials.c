#include "time_trials.h"
#include "sm64.h"
#include "game_init.h"
#include "sound_init.h"
#include "geo_commands.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "object_list_processor.h"
#include "save_file.h"
#include "behavior_data.h"
#include "pc/configfile.h"
#include "pc/fs/fs.h"
#include "engine/math_util.h"
#include "audio/external.h"
#include "model_ids.h"
#include <stdio.h>
#define COURSE_IS_BOWSER_COURSE(cmd) (cmd >= COURSE_BITDW && cmd <= COURSE_BITS)

// Games
#if defined(SUPER_MARIO_74_AND_EXTREME_EDITION) // SM74
#define TIME_TRIALS_DIRECTORY (gCurrAreaIndex == 1 ? "time_trials_sm74" : "time_trials_sm74ee")
#define COURSE_NAME_TABLE (gCurrAreaIndex == 1 ? seg2_course_name_table : seg2_course_name_table_EE)
#define HAS_BOWSER_FIGHTS 1
#define IS_SM64 0
static const u8 AVAILABLE_STARS_PER_COURSE_74_EE[2][COURSE_COUNT] = { {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_JRB]   = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1111111,
    [COURSE_SSL]   = 0b1111111,
    [COURSE_DDD]   = 0b1111111,
    [COURSE_SL]    = 0b1111111,
    [COURSE_WDW]   = 0b1111111,
    [COURSE_TTM]   = 0b1111111,
    [COURSE_THI]   = 0b1111111,
    [COURSE_TTC]   = 0b1111111,
    [COURSE_RR]    = 0b1111111,
    [COURSE_BITDW] = 0b0001111,
    [COURSE_BITFS] = 0b0011111,
    [COURSE_BITS]  = 0b0111111,
    [COURSE_TOTWC] = 0b0000111,
    [COURSE_COTMC] = 0b0011111,
    [COURSE_VCUTM] = 0b0111111,
    [COURSE_PSS]   = 0b0001111,
    [COURSE_WMOTR] = 0b0111111,
    [COURSE_SA]    = 0b0000101,
}, {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_JRB]   = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1111111,
    [COURSE_SSL]   = 0b1111111,
    [COURSE_DDD]   = 0b1111111,
    [COURSE_SL]    = 0b1111111,
    [COURSE_WDW]   = 0b1111111,
    [COURSE_TTM]   = 0b1111111,
    [COURSE_THI]   = 0b1111111,
    [COURSE_TTC]   = 0b1111111,
    [COURSE_RR]    = 0b1111111,
    [COURSE_BITDW] = 0b0001111,
    [COURSE_BITFS] = 0b0011111,
    [COURSE_BITS]  = 0b0111111,
    [COURSE_TOTWC] = 0b0000111,
    [COURSE_COTMC] = 0b0111111,
    [COURSE_VCUTM] = 0b0111111,
    [COURSE_PSS]   = 0b0001111,
    [COURSE_WMOTR] = 0b0111111,
    [COURSE_SA]    = 0b1111111,
} };
#define AVAILABLE_STARS_PER_COURSE AVAILABLE_STARS_PER_COURSE_74_EE[gCurrAreaIndex - 1]
static const s32 AVAILABLE_COURSES[] = {
    COURSE_BOB,
    COURSE_WF,
    COURSE_JRB,
    COURSE_CCM,
    COURSE_BBH,
    COURSE_HMC,
    COURSE_LLL,
    COURSE_SSL,
    COURSE_DDD,
    COURSE_SL,
    COURSE_WDW,
    COURSE_TTM,
    COURSE_THI,
    COURSE_TTC,
    COURSE_RR,
    COURSE_BITDW,
    COURSE_BITFS,
    COURSE_BITS,
    COURSE_TOTWC,
    COURSE_COTMC,
    COURSE_VCUTM,
    COURSE_PSS,
    COURSE_WMOTR,
    COURSE_SA,
};
static const s32 NUM_AVAILABLE_COURSES = sizeof(AVAILABLE_COURSES) / sizeof(AVAILABLE_COURSES[0]);
#elif defined(SUPER_MARIO_STAR_ROAD) // Star Road
#define TIME_TRIALS_DIRECTORY "time_trials_smsr"
#define COURSE_NAME_TABLE seg2_course_name_table
#define HAS_BOWSER_FIGHTS 1
#define IS_SM64 0
static const u8 AVAILABLE_STARS_PER_COURSE[COURSE_COUNT] = {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_JRB]   = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1111111,
    [COURSE_SSL]   = 0b1111111,
    [COURSE_DDD]   = 0b1111111,
    [COURSE_SL]    = 0b1111111,
    [COURSE_WDW]   = 0b1111111,
    [COURSE_TTM]   = 0b1111111,
    [COURSE_THI]   = 0b1111111,
    [COURSE_TTC]   = 0b1111111,
    [COURSE_RR]    = 0b1111111,
    [COURSE_BITDW] = 0b0001001,
    [COURSE_BITFS] = 0b0010001,
    [COURSE_BITS]  = 0b0100001,
    [COURSE_TOTWC] = 0b0100001,
    [COURSE_COTMC] = 0b0010001,
    [COURSE_VCUTM] = 0b0100001,
    [COURSE_PSS]   = 0b0100011,
    [COURSE_WMOTR] = 0b0100001,
    [COURSE_SA]    = 0b0100011,
};
static const s32 AVAILABLE_COURSES[] = {
    COURSE_BOB,
    COURSE_WF,
    COURSE_JRB,
    COURSE_CCM,
    COURSE_BBH,
    COURSE_HMC,
    COURSE_LLL,
    COURSE_SSL,
    COURSE_DDD,
    COURSE_SL,
    COURSE_WDW,
    COURSE_TTM,
    COURSE_THI,
    COURSE_TTC,
    COURSE_RR,
    COURSE_BITDW,
    COURSE_BITFS,
    COURSE_BITS,
    COURSE_TOTWC,
    COURSE_COTMC,
    COURSE_VCUTM,
    COURSE_PSS,
    COURSE_WMOTR,
    COURSE_SA,
};
static const s32 NUM_AVAILABLE_COURSES = sizeof(AVAILABLE_COURSES) / sizeof(AVAILABLE_COURSES[0]);
#elif defined(SUPER_MARIO_64_THE_GREEN_STARS) // Green Stars
#define TIME_TRIALS_DIRECTORY "time_trials_smgs"
#define COURSE_NAME_TABLE seg2_course_name_table
#define HAS_BOWSER_FIGHTS 1
#define IS_SM64 0
static const u8 AVAILABLE_STARS_PER_COURSE[COURSE_COUNT] = {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_JRB]   = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1111111,
    [COURSE_SSL]   = 0b1111111,
    [COURSE_DDD]   = 0b1111111,
    [COURSE_SL]    = 0b1111111,
    [COURSE_WDW]   = 0b1111111,
    [COURSE_TTM]   = 0b1111111,
    [COURSE_THI]   = 0b1111111,
    [COURSE_TTC]   = 0b1111111,
    [COURSE_RR]    = 0b1111111,
    [COURSE_BITDW] = 0b0000111,
    [COURSE_BITFS] = 0b0001111,
    [COURSE_BITS]  = 0b0001111,
    [COURSE_TOTWC] = 0b0000011,
    [COURSE_COTMC] = 0b0000111,
    [COURSE_PSS]   = 0b0001111,
    [COURSE_SA]    = 0b0000011,
};
static const s32 AVAILABLE_COURSES[] = {
    COURSE_BOB,
    COURSE_WF,
    COURSE_JRB,
    COURSE_CCM,
    COURSE_BBH,
    COURSE_HMC,
    COURSE_LLL,
    COURSE_SSL,
    COURSE_DDD,
    COURSE_SL,
    COURSE_WDW,
    COURSE_TTM,
    COURSE_THI,
    COURSE_TTC,
    COURSE_RR,
    COURSE_BITDW,
    COURSE_BITFS,
    COURSE_BITS,
    COURSE_TOTWC,
    COURSE_COTMC,
    COURSE_PSS,
    COURSE_SA,
};
static const s32 NUM_AVAILABLE_COURSES = sizeof(AVAILABLE_COURSES) / sizeof(AVAILABLE_COURSES[0]);
#elif defined(MODEL_MARIO_SUB) // Moonshine
#define TIME_TRIALS_DIRECTORY "time_trials_smms"
#define COURSE_NAME_TABLE seg2_course_name_table
#define HAS_BOWSER_FIGHTS 0
#define IS_SM64 0
static const u8 AVAILABLE_STARS_PER_COURSE[COURSE_COUNT] = {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1000011,
    [COURSE_WDW]   = 0b1001111,
    [COURSE_BITFS] = 0b0000011,
    [COURSE_BITS]  = 0b0000011,
    [COURSE_PSS]   = 0b0000011,
};
static const s32 AVAILABLE_COURSES[] = {
    COURSE_BOB,
    COURSE_WF,
    COURSE_CCM,
    COURSE_BBH,
    COURSE_HMC,
    COURSE_LLL,
    COURSE_WDW,
    COURSE_BITFS,
    COURSE_BITS,
    COURSE_PSS,
};
static const s32 NUM_AVAILABLE_COURSES = sizeof(AVAILABLE_COURSES) / sizeof(AVAILABLE_COURSES[0]);
#else // SM64
#define TIME_TRIALS_DIRECTORY "time_trials_sm64"
#define COURSE_NAME_TABLE seg2_course_name_table
#define HAS_BOWSER_FIGHTS 1
#define IS_SM64 1
static const u8 AVAILABLE_STARS_PER_COURSE[COURSE_COUNT] = {
    [COURSE_BOB]   = 0b1111111,
    [COURSE_WF]    = 0b1111111,
    [COURSE_JRB]   = 0b1111111,
    [COURSE_CCM]   = 0b1111111,
    [COURSE_BBH]   = 0b1111111,
    [COURSE_HMC]   = 0b1111111,
    [COURSE_LLL]   = 0b1111111,
    [COURSE_SSL]   = 0b1111111,
    [COURSE_DDD]   = 0b1111111,
    [COURSE_SL]    = 0b1111111,
    [COURSE_WDW]   = 0b1111111,
    [COURSE_TTM]   = 0b1111111,
    [COURSE_THI]   = 0b1111111,
    [COURSE_TTC]   = 0b1111111,
    [COURSE_RR]    = 0b1111111,
    [COURSE_BITDW] = 0b0000001,
    [COURSE_BITFS] = 0b0000001,
    [COURSE_BITS]  = 0b0000001,
    [COURSE_TOTWC] = 0b0000001,
    [COURSE_COTMC] = 0b0000001,
    [COURSE_VCUTM] = 0b0000001,
    [COURSE_PSS]   = 0b0000011,
    [COURSE_WMOTR] = 0b0000001,
    [COURSE_SA]    = 0b0000001,
};
static const s32 AVAILABLE_COURSES[] = {
    COURSE_BOB,
    COURSE_WF,
    COURSE_JRB,
    COURSE_CCM,
    COURSE_BBH,
    COURSE_HMC,
    COURSE_LLL,
    COURSE_SSL,
    COURSE_DDD,
    COURSE_SL,
    COURSE_WDW,
    COURSE_TTM,
    COURSE_THI,
    COURSE_TTC,
    COURSE_RR,
    COURSE_BITDW,
    COURSE_BITFS,
    COURSE_BITS,
    COURSE_TOTWC,
    COURSE_COTMC,
    COURSE_VCUTM,
    COURSE_PSS,
    COURSE_WMOTR,
    COURSE_SA,
};
static const s32 NUM_AVAILABLE_COURSES = sizeof(AVAILABLE_COURSES) / sizeof(AVAILABLE_COURSES[0]);
#endif

// Versions
#if defined(MODEL_MILK_BOTTLE) // Render96
#define _DmaTable   MarioAnimDmaRelatedThing
#define _dmaTable   animDmaTable
#define _currAddr   currentAnimAddr
#define _animList   animation
#define _animInfo   curAnim
#define _loopEnd    unk08
#define _gTextAlpha gDialogTextAlpha
extern u8 **seg2_course_name_table;
#elif defined(MARIO_SPECIAL_CAPS) // ex-alo
#define _DmaTable   DmaTable
#define _dmaTable   dmaTable
#define _currAddr   currentAddr
#define _animList   animList
#define _animInfo   animInfo
#define _loopEnd    loopEnd
#if defined(SUPER_MARIO_74_AND_EXTREME_EDITION)
#define _gTextAlpha gDialogTextAlpha
#else
#define _gTextAlpha gMenuTextAlpha
#endif
#else // ex-nightly
#define _DmaTable   MarioAnimDmaRelatedThing
#define _dmaTable   animDmaTable
#define _currAddr   currentAnimAddr
#define _animList   animation
#define _animInfo   unk38
#define _loopEnd    unk08
#define _gTextAlpha gDialogTextAlpha
#endif

// OMM
#if defined(OMM_STARS_NON_STOP)
#define TIME_TRIALS_FILENAME_EXT ".omm.ttg"
#undef USE_SYSTEM_MALLOC
#else
#define TIME_TRIALS_FILENAME_EXT ".ttg"
#define OMM_STARS_NON_STOP 0
#define OMM_ALL_STARS 0
#endif

// Constants
#define TIME_TRIALS_MAX_ALLOWED_TIME ((10 * 60 * 30) - 1) // 9'59"99
#define TIME_TRIALS_UNDEFINED_TIME -1
#define TIME_TRIALS_100_COINS_STAR_NUM 7
#define TIME_TRIALS_BOWSER_KEY_STAR_NUM 7
#define TIME_TRIALS_NUM_SAVE_FILES NUM_SAVE_FILES
#define TIME_TRIALS_NUM_COURSES (COURSE_MAX - COURSE_MIN)
#define TIME_TRIALS_NUM_STARS_PER_COURSE 7
#define TIME_TRIALS_NUM_TIMES_PER_COURSE (TIME_TRIALS_NUM_STARS_PER_COURSE + 1) // All-stars

// Checks
#define GET_FI(fileNum)   s32 fi = (fileNum) - 1;
#define GET_CI(courseNum) s32 ci = (courseNum) - COURSE_MIN;
#define GET_SI(starNum)   s32 si = (starNum) - 1;
#define CHECK_FI(x) if (fi < 0 || fi >= TIME_TRIALS_NUM_SAVE_FILES) { return x; }
#define CHECK_CI(x) if (ci < 0 || ci >= TIME_TRIALS_NUM_COURSES) { return x; }
#define CHECK_SI(x) if (si < 0 || si >= TIME_TRIALS_NUM_TIMES_PER_COURSE) { return x; }

static bool sTimeTrialsEnabled = true;
bool time_trials_enabled() {
    return sTimeTrialsEnabled;
}

//
// Ghost behaviors
//

static const BehaviorScript bhvTimeTrialsGhost0[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost1[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost2[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost3[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost4[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost5[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost6[] = { 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhost7[] = { 0x08000000, 0x09000000 };
static const BehaviorScript *bhvTimeTrialsGhost[] = {
    bhvTimeTrialsGhost0,
    bhvTimeTrialsGhost1,
    bhvTimeTrialsGhost2,
    bhvTimeTrialsGhost3,
    bhvTimeTrialsGhost4,
    bhvTimeTrialsGhost5,
    bhvTimeTrialsGhost6,
    bhvTimeTrialsGhost7,
};

static const BehaviorScript bhvTimeTrialsGhostNumber0[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber1[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber2[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber3[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber4[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber5[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber6[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript bhvTimeTrialsGhostNumber7[] = { 0x21000000, 0x08000000, 0x09000000 };
static const BehaviorScript *bhvTimeTrialsGhostNumber[] = {
    bhvTimeTrialsGhostNumber0,
    bhvTimeTrialsGhostNumber1,
    bhvTimeTrialsGhostNumber2,
    bhvTimeTrialsGhostNumber3,
    bhvTimeTrialsGhostNumber4,
    bhvTimeTrialsGhostNumber5,
    bhvTimeTrialsGhostNumber6,
    bhvTimeTrialsGhostNumber7,
};

//
// Ghost models
//

#include "time_trials_ghost_geo.inl"

enum {
    GHOST_MARIO_DEFAULT,   // Blue
    GHOST_MARIO_RED_COINS, // Red
    GHOST_MARIO_PSS_2,     // Green
    GHOST_MARIO_100_COINS, // Yellow
    GHOST_MARIO_ALL_STARS, // Pink
};

static struct GraphNode *time_trials_get_ghost_mario_graph_node(s32 type) {
    static struct { const GeoLayout *geo; struct GraphNode *node; } sGhostMarios[] = {
        [GHOST_MARIO_DEFAULT]   = { ghost_mario_blue_geo,  NULL },
        [GHOST_MARIO_RED_COINS] = { ghost_mario_red_geo,   NULL },
        [GHOST_MARIO_PSS_2]     = { ghost_mario_green_geo, NULL },
        [GHOST_MARIO_100_COINS] = { ghost_mario_gold_geo,  NULL },
        [GHOST_MARIO_ALL_STARS] = { ghost_mario_pink_geo,  NULL },
    };
    if (!sGhostMarios[type].node) {
#if defined(USE_SYSTEM_MALLOC) // this is garbage
        struct AllocOnlyPool {
            void *lastBlock;
            u32 lastBlockSize;
            u32 lastBlockNextPos;
        };
        struct AllocOnlyPool *pool = (struct AllocOnlyPool *) main_pool_alloc(sizeof(struct AllocOnlyPool), NULL);
        pool->lastBlock = NULL;
        pool->lastBlockSize = 0;
        pool->lastBlockNextPos = 0;
#else
        struct AllocOnlyPool *pool = calloc(1, 0x100000);
        pool->totalSpace = 0x100000 - sizeof(struct AllocOnlyPool);
        pool->usedSpace = 0;
        pool->startPtr = ((u8 *) pool) + sizeof(struct AllocOnlyPool);
        pool->freePtr = ((u8 *) pool) + sizeof(struct AllocOnlyPool);
#endif
        sGhostMarios[type].node = process_geo_layout((void *) pool, (void *) sGhostMarios[type].geo);
    }
    return sGhostMarios[type].node;
}

//
// Files
//

static const char *__time_trials_get_dirname() {
    static char dirname[256];
    snprintf(dirname, 256, "%s/%s", sys_user_path(), TIME_TRIALS_DIRECTORY);
    return dirname;
}

static const char *__time_trials_get_filename(s32 fi, s32 ci, s32 si) {
    static char filename[256];
    const char *dirname = __time_trials_get_dirname();
    snprintf(filename, 256, "%s/%d.%02d.%d" TIME_TRIALS_FILENAME_EXT, dirname, fi, ci, si);
    return filename;
}

//
// Ghost Frame data
//

typedef struct {
    s16 posX, posY, posZ;
    s8 pitch, yaw, roll;
    u8 scaleX, scaleY, scaleZ;
    u8 animIndex, animFrame;
    u8 levelNum, areaIndex;
} TTGF;
typedef TTGF TTGD[TIME_TRIALS_MAX_ALLOWED_TIME + 1];
typedef TTGF *TTGP;
#define time_trials_is_end_frame(p, f) (p[f].levelNum == 0)

//
// Data
//

enum { TT_TIMER_DISABLED, TT_TIMER_RUNNING, TT_TIMER_STOPPED };
static s32  sTimeTrialsState = TT_TIMER_DISABLED;
static s32  sTimeTrialsTimer = 0;
static bool sTimeTrialsHiScore = false;
static TTGD sTimeTrialsGhostDataR[TIME_TRIALS_NUM_TIMES_PER_COURSE];
static TTGD sTimeTrialsGhostDataW;

//
// Read
//

static s32 __time_trials_read(s32 fi, s32 ci, s32 si, TTGP ghostData) {
    if (ghostData) {
        memset(ghostData, 0, sizeof(TTGD));
    }
    const char *filename = __time_trials_get_filename(fi, ci, si);
    FILE *f = fopen(filename, "rb");
    if (f) {
        s32 t = 0; fread(&t, sizeof(s32), 1, f);
        if (t && ghostData) {
            fread(ghostData, sizeof(TTGF), t, f);
        }
        fclose(f);
        return t;
    }
    return 0;
}

#if defined(SUPER_MARIO_74_AND_EXTREME_EDITION)
static s32 *__time_trials_time(s32 fi, s32 ci, s32 si) {
    static s32 sTimeTrialsTimes[2][TIME_TRIALS_NUM_SAVE_FILES][TIME_TRIALS_NUM_COURSES][TIME_TRIALS_NUM_TIMES_PER_COURSE];
    static bool inited = 0;
    if (!inited) {
        s32 currAreaIndex = gCurrAreaIndex;
        for (gCurrAreaIndex = 1; gCurrAreaIndex <= 2; ++gCurrAreaIndex)
        for (s32 i = 0; i != TIME_TRIALS_NUM_SAVE_FILES; ++i)
        for (s32 j = 0; j != TIME_TRIALS_NUM_COURSES; ++j)
        for (s32 k = 0; k != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++k) {
            s32 t = __time_trials_read(i, j, k, NULL);
            if (t <= 0 || t > TIME_TRIALS_MAX_ALLOWED_TIME) {
                t = TIME_TRIALS_UNDEFINED_TIME;
            }
            sTimeTrialsTimes[gCurrAreaIndex - 1][i][j][k] = t;
        }
        inited = true;
        gCurrAreaIndex = currAreaIndex;
    }
    return &sTimeTrialsTimes[gCurrAreaIndex - 1][fi][ci][si];
}
#else
static s32 *__time_trials_time(s32 fi, s32 ci, s32 si) {
    static s32 sTimeTrialsTimes[TIME_TRIALS_NUM_SAVE_FILES][TIME_TRIALS_NUM_COURSES][TIME_TRIALS_NUM_TIMES_PER_COURSE];
    static bool inited = 0;
    if (!inited) {
        for (s32 i = 0; i != TIME_TRIALS_NUM_SAVE_FILES; ++i)
        for (s32 j = 0; j != TIME_TRIALS_NUM_COURSES; ++j)
        for (s32 k = 0; k != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++k) {
            s32 t = __time_trials_read(i, j, k, NULL);
            if (t <= 0 || t > TIME_TRIALS_MAX_ALLOWED_TIME) {
                t = TIME_TRIALS_UNDEFINED_TIME;
            }
            sTimeTrialsTimes[i][j][k] = t;
        }
        inited = true;
    }
    return &sTimeTrialsTimes[fi][ci][si];
}
#endif

//
// Write
//

static void __time_trials_write(s32 fi, s32 ci, s32 si) {
    if (sTimeTrialsTimer > 0 && sTimeTrialsTimer < TIME_TRIALS_MAX_ALLOWED_TIME) {
        fs_sys_mkdir(__time_trials_get_dirname());
        const char *filename = __time_trials_get_filename(fi, ci, si);
        FILE *f = fopen(filename, "wb");
        if (f) {
            fwrite(&sTimeTrialsTimer, sizeof(s32), 1, f);
            fwrite(sTimeTrialsGhostDataW, sizeof(TTGF), sTimeTrialsTimer, f);
            fclose(f);
        }
    }
    *__time_trials_time(fi, ci, si) = sTimeTrialsTimer;
}

//
// Get
//

static s32 time_trials_get_time(s32 fileNum, s32 courseNum, s32 starNum) {
    GET_FI(fileNum);
    GET_CI(courseNum);
    GET_SI(starNum);
    CHECK_FI(TIME_TRIALS_UNDEFINED_TIME);
    CHECK_CI(TIME_TRIALS_UNDEFINED_TIME);
    CHECK_SI(TIME_TRIALS_UNDEFINED_TIME);
    return *__time_trials_time(fi, ci, si);
}

static s32 time_trials_get_course_time(s32 fileNum, s32 courseNum) {
    GET_FI(fileNum);
    GET_CI(courseNum);
    CHECK_FI(TIME_TRIALS_UNDEFINED_TIME);
    CHECK_CI(TIME_TRIALS_UNDEFINED_TIME);
    s32 courseTime = 0;
    u8 availableStars = AVAILABLE_STARS_PER_COURSE[courseNum];
    for (s32 si = 0; si != TIME_TRIALS_NUM_STARS_PER_COURSE; ++si) {
        if ((availableStars >> si) & 1) {
            s32 t = *__time_trials_time(fi, ci, si);
            if (t == TIME_TRIALS_UNDEFINED_TIME) {
                return TIME_TRIALS_UNDEFINED_TIME;
            }
            courseTime += t;
        }
    }
    return courseTime;
}

static s32 time_trials_get_total_time(s32 fileNum) {
    GET_FI(fileNum);
    CHECK_FI(TIME_TRIALS_UNDEFINED_TIME);
    s32 totalTime = 0;
    for (s32 i = 0; i != NUM_AVAILABLE_COURSES; ++i) {
        s32 courseTime = time_trials_get_course_time(fileNum, AVAILABLE_COURSES[i]);
        if (courseTime == TIME_TRIALS_UNDEFINED_TIME) {
            return TIME_TRIALS_UNDEFINED_TIME;
        }
        totalTime += courseTime;
    }
    return totalTime;
}

//
// Ghost
//

typedef struct {
    struct Animation _animList;
    u8 animationData[0x40000];
    s32 type;
    s32 prevValidAnimIndex;
} TTGA;
static TTGA sTimeTrialsGhostAnimData[TIME_TRIALS_NUM_TIMES_PER_COURSE];

static s32 time_trials_ghost_retrieve_current_animation_index(struct MarioState *m) {
    struct _DmaTable *_dmaTable = m->_animList->_dmaTable;
    void *a = m->_animList->_currAddr;
    for (s32 i = 0; i != (s32) _dmaTable->count; ++i) {
        void *b = _dmaTable->srcAddr + _dmaTable->anim[i].offset;
        if (a == b) {
            return i;
        }
    }
    return 0;
}

static s32 time_trials_ghost_load_animation(TTGA *animData, struct MarioState *m, s32 animIndex) {
    struct _DmaTable *_dmaTable = m->_animList->_dmaTable;
    if (animIndex >= (s32) _dmaTable->count) {
        return 0;
    }

    // Raw data
    u8 *addr = _dmaTable->srcAddr + _dmaTable->anim[animIndex].offset;
    u32 size = _dmaTable->anim[animIndex].size;        
    memcpy(animData->animationData, addr, size);

    // Seek index and values pointers
    struct Animation *a = (struct Animation *) animData->animationData;
    const u16 *indexStart = (const u16 *) ((uintptr_t) (animData->animationData) + (uintptr_t) (a->index));
    const s16 *valueStart = (const s16 *) ((uintptr_t) (animData->animationData) + (uintptr_t) (a->values));

    // Fill ghost _animList data
    memcpy(&animData->_animList, animData->animationData, sizeof(struct Animation));
    animData->_animList.index = indexStart;
    animData->_animList.values = valueStart;
    return TRUE;
}

static void time_trials_ghost_update_animation(TTGA *animData, struct Object *obj, struct MarioState *m, s32 animIndex, s32 animFrame) {
    if (!obj->header.gfx._animInfo.curAnim) {
        animData->prevValidAnimIndex = -1;
    }

    // Load & set _animList
    if (animIndex != animData->prevValidAnimIndex && time_trials_ghost_load_animation(animData, m, animIndex)) {
        obj->header.gfx._animInfo.curAnim = &animData->_animList;
        obj->header.gfx._animInfo.animAccel = 0;
        obj->header.gfx._animInfo.animYTrans = m->unkB0;
        animData->prevValidAnimIndex = animIndex;
    }

    // Set frame
    if (obj->header.gfx._animInfo.curAnim) {
        obj->header.gfx._animInfo.animFrame = MIN(animFrame, obj->header.gfx._animInfo.curAnim->_loopEnd - 1);
    }
}

static struct Object *time_trials_ghost_get_object(const BehaviorScript *behavior) {
    struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_DEFAULT];
    struct Object *next = (struct Object *) head->header.next;
    while (next != head) {
        if (next->behavior == behavior && next->activeFlags) {
            return next;
        }
        next = (struct Object *) next->header.next;
    }
    return NULL;
}

static void time_trials_ghost_load(s32 fileNum, s32 courseNum, s32 starNum, s32 type) {
    GET_FI(fileNum);
    GET_CI(courseNum);
    GET_SI(starNum);
    CHECK_FI();
    CHECK_CI();
    CHECK_SI();
    if (__time_trials_read(fi, ci, si, sTimeTrialsGhostDataR[si])) {
        memset(&sTimeTrialsGhostAnimData[si], 0, sizeof(sTimeTrialsGhostAnimData[si]));
        sTimeTrialsGhostAnimData[si].type = type;
        sTimeTrialsGhostAnimData[si].prevValidAnimIndex = -1;
    }
}

static void time_trials_ghost_update(struct MarioState *m, s32 starNum, s32 frame) {
    GET_SI(starNum);
    CHECK_SI();
    TTGP ghostData = sTimeTrialsGhostDataR[si];
    TTGA *animData = &sTimeTrialsGhostAnimData[si];
    const BehaviorScript *ghostBhv = bhvTimeTrialsGhost[si];
    const BehaviorScript *numberBhv = bhvTimeTrialsGhostNumber[si];
    struct Object *ghost = time_trials_ghost_get_object(ghostBhv);
    struct Object *number = time_trials_ghost_get_object(numberBhv);

    // If timer reached its max or frame data is ended, unload the ghost
    if (frame >= TIME_TRIALS_MAX_ALLOWED_TIME || time_trials_is_end_frame(ghostData, frame)) {
        if (ghost) obj_mark_for_deletion(ghost);
        if (number) obj_mark_for_deletion(number);
        return;
    }

    // Spawn ghost if not loaded
    if (!ghost) {
        ghost = spawn_object(m->marioObj, 0, ghostBhv);
        number = spawn_object(m->marioObj, MODEL_NUMBER, numberBhv);
    }

    // Hide ghost if disabled, on first frame or its level or area differs from Mario
    if (frame == 0 || ghostData[frame].levelNum != (u8) gCurrLevelNum || ghostData[frame].areaIndex != (u8) gCurrAreaIndex) {
        ghost->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        number->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        return;
    }

    // Update ghost
    ghost->hitboxRadius           = 0;
    ghost->hitboxHeight           = 0;
    ghost->oOpacity               = 0xA0;
    ghost->oIntangibleTimer       = -1;
    ghost->oPosX                  = (f32) ghostData[frame].posX;
    ghost->oPosY                  = (f32) ghostData[frame].posY;
    ghost->oPosZ                  = (f32) ghostData[frame].posZ;
    ghost->oFaceAnglePitch        = (s16) ghostData[frame].pitch  * 0x100;
    ghost->oFaceAngleYaw          = (s16) ghostData[frame].yaw    * 0x100;
    ghost->oFaceAngleRoll         = (s16) ghostData[frame].roll   * 0x100;
    ghost->oMoveAnglePitch        = (s16) ghostData[frame].pitch  * 0x100;
    ghost->oMoveAngleYaw          = (s16) ghostData[frame].yaw    * 0x100;
    ghost->oMoveAngleRoll         = (s16) ghostData[frame].roll   * 0x100;
    ghost->header.gfx.pos[0]      = (f32) ghostData[frame].posX;
    ghost->header.gfx.pos[1]      = (f32) ghostData[frame].posY;
    ghost->header.gfx.pos[2]      = (f32) ghostData[frame].posZ;
    ghost->header.gfx.angle[0]    = (s16) ghostData[frame].pitch  * 0x100;
    ghost->header.gfx.angle[1]    = (s16) ghostData[frame].yaw    * 0x100;
    ghost->header.gfx.angle[2]    = (s16) ghostData[frame].roll   * 0x100;
    ghost->header.gfx.scale[0]    = (f32) ghostData[frame].scaleX / 100.f;
    ghost->header.gfx.scale[1]    = (f32) ghostData[frame].scaleY / 100.f;
    ghost->header.gfx.scale[2]    = (f32) ghostData[frame].scaleZ / 100.f;
    ghost->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    ghost->header.gfx.sharedChild = time_trials_get_ghost_mario_graph_node(animData->type);
    time_trials_ghost_update_animation(animData, ghost, m, (u16) ghostData[frame].animIndex, (u16) ghostData[frame].animFrame);

    // Update number
    number->hitboxRadius           = 0;
    number->hitboxHeight           = 0;
    number->oIntangibleTimer       = -1;
    number->oAnimState             = starNum;
    number->oPosX                  = (f32) ghostData[frame].posX;
    number->oPosY                  = (f32) ghostData[frame].posY + 200;
    number->oPosZ                  = (f32) ghostData[frame].posZ;
    number->oFaceAnglePitch        = 0;
    number->oFaceAngleYaw          = 0;
    number->oFaceAngleRoll         = 0;
    number->oMoveAnglePitch        = 0;
    number->oMoveAngleYaw          = 0;
    number->oMoveAngleRoll         = 0;
    number->header.gfx.pos[0]      = (f32) ghostData[frame].posX;
    number->header.gfx.pos[1]      = (f32) ghostData[frame].posY + 200;
    number->header.gfx.pos[2]      = (f32) ghostData[frame].posZ;
    number->header.gfx.angle[0]    = 0;
    number->header.gfx.angle[1]    = 0;
    number->header.gfx.angle[2]    = 0;
    number->header.gfx.scale[0]    = 0.8f;
    number->header.gfx.scale[1]    = 0.8f;
    number->header.gfx.scale[2]    = 0.8f;

    // Hide number for 100 coins star ghost, Bowser key ghost or All-stars ghost
    if ((starNum == TIME_TRIALS_100_COINS_STAR_NUM && COURSE_IS_MAIN_COURSE(gCurrCourseNum)) ||
        (starNum == TIME_TRIALS_BOWSER_KEY_STAR_NUM && COURSE_IS_BOWSER_COURSE(gCurrCourseNum)) ||
        (starNum == TIME_TRIALS_NUM_TIMES_PER_COURSE)) {
        number->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
    } else {
        number->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    }
}

static void time_trials_ghost_record(struct MarioState *m, s32 frame) {
    if (frame >= 0 && frame < TIME_TRIALS_MAX_ALLOWED_TIME) {
        if (m->marioObj && m->marioObj->header.gfx._animInfo.curAnim) {
            sTimeTrialsGhostDataW[frame].posX      = (s16) m->marioObj->header.gfx.pos[0];
            sTimeTrialsGhostDataW[frame].posY      = (s16) m->marioObj->header.gfx.pos[1];
            sTimeTrialsGhostDataW[frame].posZ      = (s16) m->marioObj->header.gfx.pos[2];
            sTimeTrialsGhostDataW[frame].pitch     = (s8) (m->marioObj->header.gfx.angle[0] / 0x100);
            sTimeTrialsGhostDataW[frame].yaw       = (s8) (m->marioObj->header.gfx.angle[1] / 0x100);
            sTimeTrialsGhostDataW[frame].roll      = (s8) (m->marioObj->header.gfx.angle[2] / 0x100);
            sTimeTrialsGhostDataW[frame].scaleX    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[0] * 100.f));
            sTimeTrialsGhostDataW[frame].scaleY    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[1] * 100.f));
            sTimeTrialsGhostDataW[frame].scaleZ    = (u8) MAX(0, MIN(255, m->marioObj->header.gfx.scale[2] * 100.f));
            sTimeTrialsGhostDataW[frame].animIndex = (u8) time_trials_ghost_retrieve_current_animation_index(m);
            sTimeTrialsGhostDataW[frame].animFrame = (u8) MAX(0, MIN(255, MIN(m->marioObj->header.gfx._animInfo.animFrame, m->marioObj->header.gfx._animInfo.curAnim->_loopEnd - 1)));
            sTimeTrialsGhostDataW[frame].levelNum  = (u8) gCurrLevelNum;
            sTimeTrialsGhostDataW[frame].areaIndex = (u8) gCurrAreaIndex;
        } else if (frame > 0) {
            sTimeTrialsGhostDataW[frame]           = sTimeTrialsGhostDataW[frame - 1];
        }
    }
}

static void time_trials_ghost_unload_all() {
    for (s32 i = 0; i != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++i) {
        const BehaviorScript *ghostBhv = bhvTimeTrialsGhost[i];
        const BehaviorScript *numberBhv = bhvTimeTrialsGhostNumber[i];
        struct Object *ghost = time_trials_ghost_get_object(ghostBhv);
        struct Object *number = time_trials_ghost_get_object(numberBhv);
        if (ghost) obj_mark_for_deletion(ghost);
        if (number) obj_mark_for_deletion(number);
    }
}

//
// SM64 Strings
//

static const u8 *time_trials_to_sm64_string(const char *str) {
    static u8 sStringPool[256][256];
    static u8 sStringPoolIndex = 0;
    u8 *buffer = sStringPool[sStringPoolIndex++];
    for (s32 k = 0;; str++) {
        if (*str == 0) {                buffer[k++] = 0xFF; break; }
        if (*str >= '0' && *str <= '9') buffer[k++] = (u8) (*str - '0' + 0);
        if (*str >= 'A' && *str <= 'Z') buffer[k++] = (u8) (*str - 'A' + 10);
        if (*str >= 'a' && *str <= 'z') buffer[k++] = (u8) (*str - 'a' + 36);
        if (*str == '\'')               buffer[k++] = (u8) (62);
        if (*str == '.')                buffer[k++] = (u8) (63);
        if (*str == ':')                buffer[k++] = (u8) (230);
        if (*str == ',')                buffer[k++] = (u8) (111);
        if (*str == '-')                buffer[k++] = (u8) (159);
        if (*str == '"')                buffer[k++] = (u8) (246);
        if (*str == '@')                buffer[k++] = (u8) (249); // coin
        if (*str == '_')                buffer[k++] = (u8) (253); // empty star
        if (*str == '+')                buffer[k++] = (u8) (250); // star filled
        if (*str == '*')                buffer[k++] = (u8) (251); // x
        if (*str == ' ')                buffer[k++] = (u8) (158); // space
    }
    return buffer;
}

static const u8 *time_trials_int_to_sm64_string(s32 value, const char *format) {
    static u8 buffer[64];
    memset(buffer, 0, 64);
    if (!format || strlen(format) > 8) {
        return buffer;
    }
    s32 len = snprintf((char *) buffer, 64, format, value);
    for (s32 i = 0; i < len; ++i) {
        if (buffer[i] >= 'A' && buffer[i] <= 'F') {
            buffer[i] = buffer[i] - 'A';
        } else if (buffer[i] >= 'a' && buffer[i] <= 'f') {
            buffer[i] = buffer[i] - 'a';
        } else {
            buffer[i] = buffer[i] - '0';
        }
    }
    buffer[len] = 0xFF;
    return buffer;
}

//
// Int to String
//

static const u8 *time_trials_time_to_string(s32 time) {
    if (time == TIME_TRIALS_UNDEFINED_TIME) {
        return time_trials_to_sm64_string("-'--\"--");
    }
    char buffer[32];
    sprintf(buffer, "%01d'%02d\"%02d", (time / 1800) % 10, (time / 30) % 60, ((time * 10) / 3) % 100);
    return time_trials_to_sm64_string(buffer);
}

static const u8 *time_trials_course_time_to_string(s32 courseTime) {
    if (courseTime == TIME_TRIALS_UNDEFINED_TIME) {
        return time_trials_to_sm64_string("-'--\"--");
    }
    char buffer[32];
    sprintf(buffer, "%02d'%02d\"%02d", (courseTime / 1800) % 100, (courseTime / 30) % 60, ((courseTime * 10) / 3) % 100);
    return time_trials_to_sm64_string(buffer);
}

static const u8 *time_trials_total_time_to_string(s32 totalTime) {
    if (totalTime == TIME_TRIALS_UNDEFINED_TIME) {
        return time_trials_to_sm64_string("--:--:--.--");
    }
    char buffer[32];
    sprintf(buffer, "%02d:%02d:%02d.%02d", (totalTime / 1080000) % 100, (totalTime / 1800) % 60, (totalTime / 30) % 60, ((totalTime * 10) / 3) % 100);
    return time_trials_to_sm64_string(buffer);
}

//
// Render
//

extern u8 gDialogCharWidths[256];
static s32 time_trials_get_string_width(const u8 *str) {
    s32 length = 0;
    for (; *str != 0xFF; ++str) {
        length += gDialogCharWidths[*str];
    }
    return length;
}

enum { SCREEN_LEFT = 0, SCREEN_CENTER = 1, SCREEN_RIGHT = 2 };
enum { ALIGN_LEFT = 0, ALIGN_CENTER = 1, ALIGN_RIGHT = 2 };
enum { COLOR_WHITE = 0, COLOR_GRAY, COLOR_YELLOW };
static void time_trials_render_string(s32 dx, s32 y, const u8 *str, s32 origin, s32 alignment, s32 color) {

    // Compute x position from origin and alignment
    s32 x = 0;
    s32 l = time_trials_get_string_width(str);
    s32 mode = origin * 10 + alignment;
    switch (mode) {
        case 00: x = GFX_DIMENSIONS_FROM_LEFT_EDGE (dx);         break; // SCREEN_LEFT, ALIGN_LEFT
        case 01: x = GFX_DIMENSIONS_FROM_LEFT_EDGE (dx - l / 2); break; // SCREEN_LEFT, ALIGN_CENTER
        case 02: x = GFX_DIMENSIONS_FROM_LEFT_EDGE (dx - l);     break; // SCREEN_LEFT, ALIGN_RIGHT
        case 10: x = SCREEN_WIDTH / 2 +            (dx);         break; // SCREEN_CENTER, ALIGN_LEFT
        case 11: x = SCREEN_WIDTH / 2 +            (dx - l / 2); break; // SCREEN_CENTER, ALIGN_CENTER
        case 12: x = SCREEN_WIDTH / 2 +            (dx - l);     break; // SCREEN_CENTER, ALIGN_RIGHT
        case 20: x = GFX_DIMENSIONS_FROM_RIGHT_EDGE(dx);         break; // SCREEN_RIGHT, ALIGN_LEFT
        case 21: x = GFX_DIMENSIONS_FROM_RIGHT_EDGE(dx + l / 2); break; // SCREEN_RIGHT, ALIGN_CENTER
        case 22: x = GFX_DIMENSIONS_FROM_RIGHT_EDGE(dx + l);     break; // SCREEN_RIGHT, ALIGN_RIGHT
    }

    // Render characters
    static const u8 colors[][3] = {
        { 0xFF, 0xFF, 0xFF },
        { 0xA0, 0xA0, 0xA0 },
        { 0xFF, 0xFF, 0x00 },
    };
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, colors[color][0], colors[color][1], colors[color][2], _gTextAlpha);
    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
    for (; *str != 0xFF; str++) {
        if (*str != DIALOG_CHAR_SPACE) {
            gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, main_font_lut[*str]);
            gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
        }
        create_dl_translation_matrix(MENU_MTX_NOPUSH, gDialogCharWidths[*str], 0, 0);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

static void time_trials_render_centered_box(s32 y, s32 w, s32 h) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
    if (matrix) {
        create_dl_translation_matrix(MENU_MTX_PUSH, (SCREEN_WIDTH - w) / 2, y + h, 0);
        guScale(matrix, (f32) w / 130.f, (f32) h / 80.f, 1.f);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
        gDPSetEnvColor(gDisplayListHead++, 0x00, 0x00, 0x00, 0x69);
        gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
}

/*
Position:
Title
0 4
1 5
2 6
3 7
C T
*/

#define TIME_TRIALS_BOX_H_MARGIN    (22.5f * sqr(gfx_current_dimensions.aspect_ratio))
#define TIME_TRIALS_BOX_V_MARGIN    (40.f)
#define TIME_TRIALS_TEXT_H_MARGIN   (9.f * gfx_current_dimensions.aspect_ratio)
#define TIME_TRIALS_TEXT_V_MARGIN   (12.f)

typedef struct {
    const char *label;
    bool starGet;
    s32 time;
} RenderParams;

static void time_trials_render_pause_times(RenderParams *params, const u8 *courseName, s32 courseTime, s32 totalTime) {

    // Title
    time_trials_render_string(0, SCREEN_HEIGHT - TIME_TRIALS_BOX_V_MARGIN - TIME_TRIALS_TEXT_V_MARGIN - 14, courseName, SCREEN_CENTER, ALIGN_CENTER, COLOR_WHITE);

    // Stars
    for (s32 i = 0; i != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++i) {
        if (params[i].label == NULL) continue;
        s32 dx, y = (SCREEN_HEIGHT / 2) + ((TIME_TRIALS_TEXT_V_MARGIN / 2) + 10 + TIME_TRIALS_TEXT_V_MARGIN) - (10 + TIME_TRIALS_TEXT_V_MARGIN) * (i % 4) - 3;
        s32 color = ((strcmp(params[i].label, "ALL STARS") == 0) ? COLOR_YELLOW : COLOR_WHITE);

        // Label
        dx = TIME_TRIALS_TEXT_H_MARGIN + (i < 4 ? TIME_TRIALS_BOX_H_MARGIN : 0);
        time_trials_render_string(dx, y, time_trials_to_sm64_string(params[i].label), (i < 4 ? SCREEN_LEFT : SCREEN_CENTER), ALIGN_LEFT, color);

        // Time
        bool undefined = (params[i].time == TIME_TRIALS_UNDEFINED_TIME);
        dx = (TIME_TRIALS_TEXT_H_MARGIN + (i < 4 ? 0 : TIME_TRIALS_BOX_H_MARGIN)) * (i < 4 ? -1 : +1);
        time_trials_render_string(dx, y, time_trials_time_to_string(params[i].time), (i < 4 ? SCREEN_CENTER : SCREEN_RIGHT), ALIGN_RIGHT, undefined ? COLOR_GRAY : color);

        // Star icon
        bool collected = (params[i].starGet);
        dx = (TIME_TRIALS_TEXT_H_MARGIN + (i < 4 ? 0 : TIME_TRIALS_BOX_H_MARGIN) + 48) * (i < 4 ? -1 : +1);
        time_trials_render_string(dx, y, time_trials_to_sm64_string(params[i].starGet ? "+" : "_"), (i < 4 ? SCREEN_CENTER : SCREEN_RIGHT), ALIGN_RIGHT, collected ? color : COLOR_GRAY);
    }

    // Course time
    {
        bool undefined = (courseTime == TIME_TRIALS_UNDEFINED_TIME);
        time_trials_render_string(TIME_TRIALS_TEXT_H_MARGIN + TIME_TRIALS_BOX_H_MARGIN, TIME_TRIALS_BOX_V_MARGIN + TIME_TRIALS_TEXT_V_MARGIN - 3, time_trials_to_sm64_string("COURSE"), SCREEN_LEFT, ALIGN_LEFT, COLOR_WHITE);
        time_trials_render_string(-TIME_TRIALS_TEXT_H_MARGIN, TIME_TRIALS_BOX_V_MARGIN + TIME_TRIALS_TEXT_V_MARGIN - 3, time_trials_course_time_to_string(courseTime), SCREEN_CENTER, ALIGN_RIGHT, undefined ? COLOR_GRAY : COLOR_WHITE);
    }
    
    // Total time
    {
        bool undefined = (totalTime == TIME_TRIALS_UNDEFINED_TIME);
        time_trials_render_string(TIME_TRIALS_TEXT_H_MARGIN, TIME_TRIALS_BOX_V_MARGIN + TIME_TRIALS_TEXT_V_MARGIN - 3, time_trials_to_sm64_string("TOTAL"), SCREEN_CENTER, ALIGN_LEFT, COLOR_WHITE);
        time_trials_render_string(TIME_TRIALS_TEXT_H_MARGIN + TIME_TRIALS_BOX_H_MARGIN, TIME_TRIALS_BOX_V_MARGIN + TIME_TRIALS_TEXT_V_MARGIN - 3, time_trials_total_time_to_string(totalTime), SCREEN_RIGHT, ALIGN_RIGHT, undefined ? COLOR_GRAY : COLOR_WHITE);
    }
}

static void time_trials_render_pause_course_times(s32 fileNum, s32 courseNum) {
    bool isMainCourse   = COURSE_IS_MAIN_COURSE(courseNum);
    bool isBowserCourse = COURSE_IS_BOWSER_COURSE(courseNum);
    u8 availableStars   = AVAILABLE_STARS_PER_COURSE[courseNum];
    u8 collectedStars   = save_file_get_star_flags(fileNum - 1, courseNum - COURSE_MIN);

    // Params
    static const s32 PARAM_INDEX_TO_PARAM_POS[] = { 0, 4, 1, 5, 2, 6, 3, 7 };
    static const char *PARAM_LABELS[] = { "STAR 1", "STAR 2", "STAR 3", "STAR 4", "STAR 5", "STAR 6", "STAR 7", "STAR @", "KEY", "G. STAR", "ALL STARS" };
    RenderParams params[8] = { 0 };
    s32 paramIndex = 0;

    // Regular Stars
    for (s32 si = 0; si != TIME_TRIALS_NUM_STARS_PER_COURSE + (isMainCourse || isBowserCourse ? -1 : 0); ++si) {
        if ((availableStars >> si) & 1) {
            s32 i = PARAM_INDEX_TO_PARAM_POS[paramIndex];
            params[i].label = PARAM_LABELS[paramIndex];
            params[i].starGet = (collectedStars >> si) & 1;
            params[i].time = time_trials_get_time(fileNum, courseNum, si + 1);
            paramIndex++;
        }
    }

    // 100 Coins Star or Bowser Key (starNum 7)
    if (isMainCourse) {
        s32 i = PARAM_INDEX_TO_PARAM_POS[6];
        params[i].label = PARAM_LABELS[7];
        params[i].starGet = (collectedStars >> 6) & 1;
        params[i].time = time_trials_get_time(fileNum, courseNum, TIME_TRIALS_100_COINS_STAR_NUM);
    }
#if HAS_BOWSER_FIGHTS
    else if (courseNum == COURSE_BITDW) {
        s32 i = PARAM_INDEX_TO_PARAM_POS[6];
        params[i].label = PARAM_LABELS[8];
        params[i].starGet = (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR)) != 0;
        params[i].time = time_trials_get_time(fileNum, courseNum, TIME_TRIALS_BOWSER_KEY_STAR_NUM);
    } else if (courseNum == COURSE_BITFS) {
        s32 i = PARAM_INDEX_TO_PARAM_POS[6];
        params[i].label = PARAM_LABELS[8];
        params[i].starGet = (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)) != 0;
        params[i].time = time_trials_get_time(fileNum, courseNum, TIME_TRIALS_BOWSER_KEY_STAR_NUM);
    } else if (courseNum == COURSE_BITS) {
        s32 i = PARAM_INDEX_TO_PARAM_POS[6];
        params[i].label = PARAM_LABELS[9];
        params[i].starGet = time_trials_get_time(fileNum, courseNum, TIME_TRIALS_BOWSER_KEY_STAR_NUM) != TIME_TRIALS_UNDEFINED_TIME;
        params[i].time = time_trials_get_time(fileNum, courseNum, TIME_TRIALS_BOWSER_KEY_STAR_NUM);
    }
#endif

    // All-Stars (starNum 8)
    if (OMM_STARS_NON_STOP) {
        s32 i = PARAM_INDEX_TO_PARAM_POS[7];
        params[i].label = PARAM_LABELS[10];
        params[i].starGet = time_trials_get_time(fileNum, courseNum, 8) != TIME_TRIALS_UNDEFINED_TIME;
        params[i].time = time_trials_get_time(fileNum, courseNum, 8);
    }

    // Render params
    const u8 **courseNameTable = (const u8 **) COURSE_NAME_TABLE;
    time_trials_render_pause_times(
        params,
        courseNameTable[courseNum - COURSE_MIN],
        time_trials_get_course_time(fileNum, courseNum),
        time_trials_get_total_time(fileNum)
    );
}

Gfx *gTimeTableDisplayListHead;
extern void create_dl_rotation_matrix(s8 pushOp, f32 a, f32 x, f32 y, f32 z);
bool time_trials_render_time_table() {
    if (!sTimeTrialsEnabled) {
        return 0;
    }

    // Handle menu scrolling
    static s8 sTableIndex = 0;
    handle_menu_scrolling(MENU_SCROLL_VERTICAL, &sTableIndex, -1, NUM_AVAILABLE_COURSES);
    if (sTableIndex == NUM_AVAILABLE_COURSES) sTableIndex = 0;
    if (sTableIndex == -1) sTableIndex = NUM_AVAILABLE_COURSES - 1;

    // Render a colorful "TIME TRIALS" above the table
    gDisplayListHead = gTimeTableDisplayListHead;
    const u8 *pause = time_trials_to_sm64_string("TIME TRIALS");
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, _gTextAlpha);
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 - 64, 8, pause);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    
    // Render the black box behind the text
    time_trials_render_centered_box(TIME_TRIALS_BOX_V_MARGIN, GFX_DIMENSIONS_FROM_RIGHT_EDGE(TIME_TRIALS_BOX_H_MARGIN) - GFX_DIMENSIONS_FROM_LEFT_EDGE(TIME_TRIALS_BOX_H_MARGIN), SCREEN_HEIGHT - 2 * TIME_TRIALS_BOX_V_MARGIN);
    create_dl_translation_matrix(MENU_MTX_PUSH, SCREEN_WIDTH / 2 + 8, SCREEN_HEIGHT - TIME_TRIALS_BOX_V_MARGIN + 4, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 90.f, 0, 0, 1.f);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, _gTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    create_dl_translation_matrix(MENU_MTX_PUSH, SCREEN_WIDTH / 2 - 8, TIME_TRIALS_BOX_V_MARGIN - 4, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 270.f, 0, 0, 1.f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Render the text
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, _gTextAlpha);
    time_trials_render_pause_course_times(gCurrSaveFileNum, AVAILABLE_COURSES[sTableIndex]);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    return true;
}

//
// HUD
//

static s32 time_trials_get_x_from_center(const u8 *str) {
    s32 len = 0;
    for (; *str != 0xFF; str++) {
        len += (*str == GLOBAL_CHAR_SPACE ? 8 : 12);
    }
    return (SCREEN_WIDTH / 2) - len;
}

static void time_trials_render_timer(s32 y, const char *text, s32 time, u8 colorFade) {
    const u8 *str = time_trials_to_sm64_string(text);
    s32 x = time_trials_get_x_from_center(str);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 0xFF);
    print_hud_lut_string(HUD_LUT_GLOBAL,                x -  6, y,     str);
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 +  6, y,     time_trials_int_to_sm64_string( (time / 1800)    % 60,  "%01d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 + 26, y,     time_trials_int_to_sm64_string( (time / 30)      % 60,  "%02d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 + 60, y,     time_trials_int_to_sm64_string(((time * 10) / 3) % 100, "%02d"));
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 + 16, y - 8, (const u8 *) (const u8 []) { 56, 0xFF });
    print_hud_lut_string(HUD_LUT_GLOBAL, SCREEN_WIDTH / 2 + 51, y - 8, (const u8 *) (const u8 []) { 57, 0xFF });
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, 0xFF);
}

void time_trials_render_star_select_time(s32 starIndex) {
    if (sTimeTrialsEnabled) {
#if defined(SUPER_MARIO_74_AND_EXTREME_EDITION)
        gCurrAreaIndex = sWarpDest.areaIdx;
#endif
        s16 time = time_trials_get_time(gCurrSaveFileNum, gCurrCourseNum, starIndex + 1);
        if (time != TIME_TRIALS_UNDEFINED_TIME) {
            time_trials_render_timer(12, "HI SCORE", time, 0xFF);
        }
    }
}

//
// Update
//

static void time_trials_write_if_new_record(s32 starNum) {
    GET_FI(gCurrSaveFileNum);
    GET_CI(gCurrCourseNum);
    GET_SI(starNum);
    CHECK_FI();
    CHECK_CI();
    CHECK_SI();
    s32 t = *__time_trials_time(fi, ci, si);
    if (t == TIME_TRIALS_UNDEFINED_TIME || t > sTimeTrialsTimer) {
        __time_trials_write(fi, ci, si);
        sTimeTrialsHiScore = true;
    } else {
        sTimeTrialsHiScore = false;
    }
}

static bool isLevelEntry = false;
void time_trials_level_entry() {
    static s32 sPrevLevelNum = -1;
    if (gCurrLevelNum != LEVEL_BOWSER_1 && gCurrLevelNum != LEVEL_BOWSER_2 && gCurrLevelNum != LEVEL_BOWSER_3 &&
        sPrevLevelNum != LEVEL_BOWSER_1 && sPrevLevelNum != LEVEL_BOWSER_2 && sPrevLevelNum != LEVEL_BOWSER_3) {
        isLevelEntry = true;
    }
    sPrevLevelNum = gCurrLevelNum;
}

void time_trials_update(struct MarioState *m, bool isPaused) {

    // Load all times at startup
    if (sTimeTrialsEnabled) {
        __time_trials_time(0, 0, 0);
    }
    
    // Hold L and press B to enable/disable Time Trials during the pause
    if (isPaused) {
        if ((gPlayer1Controller->buttonDown & L_TRIG) && (gPlayer1Controller->buttonPressed & B_BUTTON)) {
            sTimeTrialsEnabled = !sTimeTrialsEnabled;
            if (sTimeTrialsEnabled) {
                play_sound(SOUND_MENU_STAR_SOUND, gVec3fZero);
            }
        }
    }

    // Reset Time Trials state if not enabled
    if (!sTimeTrialsEnabled) {
        sTimeTrialsTimer = TIME_TRIALS_MAX_ALLOWED_TIME;
        sTimeTrialsState = TT_TIMER_DISABLED;
        time_trials_ghost_unload_all();
        return;
    }

    // Update running ghost, record ghost data, and advance timer if it's running
    if (sTimeTrialsState == TT_TIMER_RUNNING) {
        sTimeTrialsHiScore = false;
            
        // Update ghosts and advance timer if not paused
        if (!isPaused) {
            for (s32 i = 0; i != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++i) {
                time_trials_ghost_update(m, i + 1, sTimeTrialsTimer);
            }
            time_trials_ghost_record(m, sTimeTrialsTimer);
            sTimeTrialsTimer = MIN(MAX(0, sTimeTrialsTimer + 1), TIME_TRIALS_MAX_ALLOWED_TIME);
        }
    }

    // Runs timer if course is TTable
    if (gCurrCourseNum != COURSE_NONE) {
    
        // Start timer and init ghost if a course entry is detected
        if (gMarioObject && isLevelEntry) {
            sTimeTrialsState = TT_TIMER_RUNNING;
            sTimeTrialsHiScore = 0;
            sTimeTrialsTimer = 0;
            isLevelEntry = false;

            // Reset ghost data
            time_trials_ghost_unload_all();
            memset(sTimeTrialsGhostDataW, 0, sizeof(sTimeTrialsGhostDataW));
            for (s32 i = 0; i != TIME_TRIALS_NUM_TIMES_PER_COURSE; ++i) {
                memset(sTimeTrialsGhostDataR[i], 0, sizeof(sTimeTrialsGhostDataR[i]));
            }

            // Load ghosts
            // Main course: Act ghost, 100 coins ghost
            if (COURSE_IS_MAIN_COURSE(gCurrCourseNum)) {
                time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, gDialogCourseActNum, GHOST_MARIO_DEFAULT);
                time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, TIME_TRIALS_100_COINS_STAR_NUM, GHOST_MARIO_100_COINS);
            } else {
#if IS_SM64
                // PSS: Star 1 blue ghost, Star 2 green ghost
                if (gCurrCourseNum == COURSE_PSS) {
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, 1, GHOST_MARIO_DEFAULT);
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, 2, GHOST_MARIO_PSS_2);
                }

                // Bowser course: Bowser fight blue ghost, Red coins red ghost
                else if (COURSE_IS_BOWSER_COURSE(gCurrCourseNum)) {
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, 1, GHOST_MARIO_RED_COINS);
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, TIME_TRIALS_BOWSER_KEY_STAR_NUM, GHOST_MARIO_DEFAULT);
                }

                // Other courses: Star 1 blue ghost
                else {
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, 1, GHOST_MARIO_DEFAULT);
                }
#else
                // Load all ghosts as blue ghosts
                for (s32 starNum = 1; starNum <= TIME_TRIALS_NUM_STARS_PER_COURSE; ++starNum) {
                    time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, starNum, GHOST_MARIO_DEFAULT);
                }
#endif
            }
            
            // All-stars ghost
            if (OMM_STARS_NON_STOP) {
                time_trials_ghost_load(gCurrSaveFileNum, gCurrCourseNum, TIME_TRIALS_NUM_TIMES_PER_COURSE, GHOST_MARIO_ALL_STARS);
            }
        }

        // Stop timer and save time when Mario collects a star, a key or a Grand star
        if ((sTimeTrialsState == TT_TIMER_RUNNING) && (
            (m->action == ACT_STAR_DANCE_EXIT) ||
            (m->action == ACT_STAR_DANCE_NO_EXIT) ||
            (m->action == ACT_STAR_DANCE_WATER) ||
            (m->action == ACT_FALL_AFTER_STAR_GRAB) ||
            (m->action == ACT_JUMBO_STAR_CUTSCENE) ||
            (m->usedObj && m->usedObj->behavior == bhvBowserKey) ||
            (m->interactObj && m->interactObj->behavior == bhvBowserKey) ||
            (m->usedObj && m->usedObj->behavior == bhvGrandStar) ||
            (m->interactObj && m->interactObj->behavior == bhvGrandStar))) {

            // Force Mario to leave the course if he collects a main course star
            // Ignore this if OMM Non-Stop mode is enabled
            if (gCurrCourseNum >= COURSE_MIN && gCurrCourseNum <= COURSE_STAGES_MAX && (!OMM_STARS_NON_STOP || OMM_ALL_STARS)) {
                drop_queued_background_music();
                fadeout_level_music(120);
                m->actionArg = 0;
            }

            // Save the time
            if (sTimeTrialsState == TT_TIMER_RUNNING) {
                s32 starNum = gLastCompletedStarNum;

                // Bowser Key or Grand Star
                if (gCurrLevelNum == LEVEL_BOWSER_1 ||
                    gCurrLevelNum == LEVEL_BOWSER_2 ||
                    gCurrLevelNum == LEVEL_BOWSER_3) {
                    starNum = TIME_TRIALS_BOWSER_KEY_STAR_NUM;
                    sTimeTrialsState = TT_TIMER_STOPPED;
                }

                // Write time and ghost data if new record
                time_trials_write_if_new_record(starNum);
                if (OMM_STARS_NON_STOP && OMM_ALL_STARS) {
                    time_trials_write_if_new_record(TIME_TRIALS_NUM_TIMES_PER_COURSE);
                }

                // Stop the timer if Mario leaves the course
                if (m->actionArg == 0) {
                    sTimeTrialsState = TT_TIMER_STOPPED;
                }
            }
        }
    } else {
        sTimeTrialsTimer = TIME_TRIALS_MAX_ALLOWED_TIME;
        sTimeTrialsState = TT_TIMER_DISABLED;
    }

    // Render timer
    if (gHudDisplay.flags != HUD_DISPLAY_NONE && configHUD && sTimeTrialsState != TT_TIMER_DISABLED) {
        if (sTimeTrialsHiScore && sTimeTrialsState == TT_TIMER_STOPPED) {
            time_trials_render_timer(SCREEN_HEIGHT - 24, "HI SCORE", sTimeTrialsTimer, (sins(gGlobalTimer * 0x1000) * 0x32 + 0xCD));
        } else if (sTimeTrialsTimer > 0) {
            time_trials_render_timer(SCREEN_HEIGHT - 24, "TIME", sTimeTrialsTimer, 0xFF);
        }
    }
}
