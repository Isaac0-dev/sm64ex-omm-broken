#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#include "behavior_commands.h"
#define COURSE_IS_BOWSER_COURSE(courseNum) ((courseNum) >= COURSE_BITDW && (courseNum) <= COURSE_BITS)

// Constants
#define TT_FILENAME_EXT             ".omm.ttg"
#define TT_TIME_LIMIT               ((10 * 60 * 30) - 1) // 9'59"99
#define TT_TIME_UNDEFINED           (-1)
#define TT_STAR_NUM_100_COINS       (7)
#define TT_STAR_NUM_BOWSER_KEY      (TT_NUM_STARS_PER_COURSE + 1)
#define TT_STAR_NUM_ALL_STARS       (TT_NUM_STARS_PER_COURSE + 1)
#define TT_NUM_SAVE_FILES           (NUM_SAVE_FILES)
#define TT_NUM_SAVE_MODES           (NUM_SAVE_MODES)
#define TT_NUM_COURSES              (COURSE_MAX + 1 - COURSE_MIN)
#define TT_NUM_STARS_PER_COURSE     (OMM_NUM_STARS_MAX_PER_COURSE)
#define TT_NUM_TIMES_PER_COURSE     (TT_STAR_NUM_ALL_STARS)

// Checks
#define TT_GET_FI(fileNum)          s32 fi = (fileNum) - 1;
#define TT_GET_MI(modeIndex)        s32 mi = (modeIndex);
#define TT_GET_CI(courseNum)        s32 ci = (courseNum) - COURSE_MIN;
#define TT_GET_SI(starNum)          s32 si = (starNum) - 1;
#define TT_CHECK_FI(x)              if (fi < 0 || fi >= TT_NUM_SAVE_FILES) { return x; }
#define TT_CHECK_MI(x)              if (mi < 0 || mi >= TT_NUM_SAVE_MODES) { return x; }
#define TT_CHECK_CI(x)              if (ci < 0 || ci >= TT_NUM_COURSES) { return x; }
#define TT_CHECK_SI(x)              if (si < 0 || si >= TT_NUM_TIMES_PER_COURSE) { return x; }

bool time_trials_is_enabled() {
    return gOmmTimeTrialsEnabled && !omm_is_main_menu() && gCurrSaveFileNum <= NUM_SAVE_FILES && !OMM_SPARKLY_MODE_IS_ENABLED;
}

//
// Ghost behaviors
//

static const BehaviorScript bhvTimeTrialsGhost[TT_NUM_TIMES_PER_COURSE][2] = {
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BEGIN_LOOP(), BHV_END_LOOP() },
};

static const BehaviorScript bhvTimeTrialsGhostNumber[TT_NUM_TIMES_PER_COURSE][3] = {
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
    { BHV_BILLBOARD(), BHV_BEGIN_LOOP(), BHV_END_LOOP() },
};

//
// Ghost models
//

#include "time_trials_ghost_geo.inl"

enum {
    TT_GHOST_MARIO_DEFAULT,   // Blue
    TT_GHOST_MARIO_BOWSER,    // Red
    TT_GHOST_MARIO_SLIDE,     // Green
    TT_GHOST_MARIO_100_COINS, // Yellow
    TT_GHOST_MARIO_ALL_STARS, // Pink
};

static struct GraphNode *time_trials_get_ghost_mario_graph_node(s32 type) {
    switch (type) {
        case TT_GHOST_MARIO_DEFAULT:   return geo_layout_to_graph_node(NULL, tt_ghost_mario_blue_geo);
        case TT_GHOST_MARIO_BOWSER:    return geo_layout_to_graph_node(NULL, tt_ghost_mario_red_geo);
        case TT_GHOST_MARIO_SLIDE:     return geo_layout_to_graph_node(NULL, tt_ghost_mario_green_geo);
        case TT_GHOST_MARIO_100_COINS: return geo_layout_to_graph_node(NULL, tt_ghost_mario_gold_geo);
        case TT_GHOST_MARIO_ALL_STARS: return geo_layout_to_graph_node(NULL, tt_ghost_mario_pink_geo);
    }
    return NULL;
}

//
// Ghost data
//

typedef struct {
    s16 posX, posY, posZ;
    s8 pitch, yaw, roll;
    u8 scaleX, scaleY, scaleZ;
    u8 animIndex, animFrame;
    u8 levelNum, areaIndex;
} TTGF;
typedef TTGF TTGD[TT_TIME_LIMIT + 1];
typedef TTGF *TTGP;

typedef struct { TTGP data; s32 length; bool cached; } TTCGD;
static TTCGD sTimeTrialsGhostDataCache[TT_NUM_SAVE_FILES][TT_NUM_SAVE_MODES][TT_NUM_COURSES][TT_NUM_TIMES_PER_COURSE];

//
// Time Trials data
//

enum { TT_STATE_DISABLED, TT_STATE_RUNNING, TT_STATE_STOPPED };

static struct {
    s32 state;
    s32 timer;
    bool hiScore;
    TTGD ghostR[TT_NUM_TIMES_PER_COURSE];
    TTGD ghostW;
    bool isLevelEntry;
    s32 prevLevelNum;
} sTimeTrials[1];

//
// Files
//

static const char *time_trials_get_dirname(sys_path_t dst, MODE_INDEX s32 mi) {
    str_fmt(dst, sizeof(sys_path_t), "%s/time_trials_" OMM_GAME_MENU "%s", sys_user_path(),
#if OMM_GAME_IS_SM74
        (mi == OMM_SM74_MODE_EXTREME ? "ee" : "")
#else
        ""
#endif
    );
    return dst;
}

static const char *time_trials_get_filename(sys_path_t dst, s32 fi, s32 mi, s32 ci, s32 si) {
    sys_path_t dirname;
    str_fmt(dst, sizeof(sys_path_t), "%s/%d.%02d.%d" TT_FILENAME_EXT, time_trials_get_dirname(dirname, mi), fi, ci, si);
    return dst;
}

//
// Read
//

static void time_trials_cache_ghost_data(s32 fi, s32 mi, s32 ci, s32 si, TTGP ghostData, s32 length) {
    TTCGD *cachedGhostData = &sTimeTrialsGhostDataCache[fi][mi][ci][si];
    mem_del(cachedGhostData->data);
    cachedGhostData->data = (ghostData ? mem_dup(ghostData, sizeof(TTGF) * length) : NULL);
    cachedGhostData->length = length;
    cachedGhostData->cached = true;
    omm_debug_log("Caching ghost data [%d][%d][%d][%d]: %d\n",, fi, mi, ci, si, length);
}

static s32 time_trials_read(s32 fi, s32 mi, s32 ci, s32 si, TTGP ghostData) {

    // Free buffer
    if (ghostData) {
        mem_zero(ghostData, sizeof(TTGD));
    }

    // Find in cache
    TTCGD *cachedGhostData = &sTimeTrialsGhostDataCache[fi][mi][ci][si];
    if (cachedGhostData->cached) {
        if (cachedGhostData->data) {
            mem_cpy(ghostData, cachedGhostData->data, sizeof(TTGF) * cachedGhostData->length);
        }
        return cachedGhostData->length;
    }

    // Load from file
    sys_path_t filename;
    FILE *f = fopen(time_trials_get_filename(filename, fi, mi, ci, si), "rb");
    if (f) {
        s32 t = 0;
        if (fread(&t, sizeof(s32), 1, f) && t > 0) {
            if (ghostData) {
                omm_debug_log("Loading ghost data [%d][%d][%d][%d] from file: %s\n",, fi, mi, ci, si, filename);
                fread(ghostData, sizeof(TTGF), t, f);
                time_trials_cache_ghost_data(fi, mi, ci, si, ghostData, t);
            }
            fclose(f);
            return t;
        }
        fclose(f);
    }

    // No ghost, cache it as empty
    time_trials_cache_ghost_data(fi, mi, ci, si, NULL, 0);
    return 0;
}

static s32 *time_trials_time(s32 fi, s32 mi, s32 ci, s32 si) {
    static s32 sTimeTrialsTimes[TT_NUM_SAVE_FILES][TT_NUM_SAVE_MODES][TT_NUM_COURSES][TT_NUM_TIMES_PER_COURSE];
    OMM_DO_ONCE {
        for (s32 fi = 0; fi != TT_NUM_SAVE_FILES; ++fi)
        for (s32 mi = 0; mi != TT_NUM_SAVE_MODES; ++mi)
        for (s32 ci = 0; ci != TT_NUM_COURSES; ++ci)
        for (s32 si = 0; si != TT_NUM_TIMES_PER_COURSE; ++si) {
            s32 t = time_trials_read(fi, mi, ci, si, NULL);
            if (t <= 0 || t > TT_TIME_LIMIT) {
                t = TT_TIME_UNDEFINED;
            }
            sTimeTrialsTimes[fi][mi][ci][si] = t;
        }
    }
    return &sTimeTrialsTimes[fi][mi][ci][si];
}

//
// Write
//

static void time_trials_write(s32 fi, s32 mi, s32 ci, s32 si) {
    if (sTimeTrials->timer > 0 && sTimeTrials->timer < TT_TIME_LIMIT) {
        sys_path_t dirname, filename;
        fs_sys_mkdir(time_trials_get_dirname(dirname, mi));
        FILE *f = fopen(time_trials_get_filename(filename, fi, mi, ci, si), "wb");
        if (f) {
            omm_debug_log("Writing ghost data [%d][%d][%d][%d] to file: %s\n",, fi, mi, ci, si, filename);
            fwrite(&sTimeTrials->timer, sizeof(s32), 1, f);
            fwrite(sTimeTrials->ghostW, sizeof(TTGF), sTimeTrials->timer, f);
            fclose(f);
        }
        time_trials_cache_ghost_data(fi, mi, ci, si, sTimeTrials->ghostW, sTimeTrials->timer);
    }
    *time_trials_time(fi, mi, ci, si) = sTimeTrials->timer;
}

static void time_trials_write_if_new_record(s32 starNum) {
    TT_GET_FI(gCurrSaveFileNum);
    TT_GET_MI(OMM_GAME_MODE);
    TT_GET_CI(gCurrCourseNum);
    TT_GET_SI(starNum);
    TT_CHECK_FI();
    TT_CHECK_MI();
    TT_CHECK_CI();
    TT_CHECK_SI();
    s32 t = *time_trials_time(fi, mi, ci, si);
    if (t == TT_TIME_UNDEFINED || t > sTimeTrials->timer) {
        time_trials_write(fi, mi, ci, si);
        sTimeTrials->hiScore = true;
    } else {
        sTimeTrials->hiScore = false;
    }
}

//
// Get
//

static s32 time_trials_get_time(s32 fileNum, s32 modeIndex, s32 courseNum, s32 starNum) {
    TT_GET_FI(fileNum);
    TT_GET_MI(modeIndex);
    TT_GET_CI(courseNum);
    TT_GET_SI(starNum);
    TT_CHECK_FI(TT_TIME_UNDEFINED);
    TT_CHECK_MI(TT_TIME_UNDEFINED);
    TT_CHECK_CI(TT_TIME_UNDEFINED);
    TT_CHECK_SI(TT_TIME_UNDEFINED);
    return *time_trials_time(fi, mi, ci, si);
}

static s32 time_trials_get_course_time(s32 fileNum, s32 modeIndex, s32 courseNum) {
    TT_GET_FI(fileNum);
    TT_GET_MI(modeIndex);
    TT_GET_CI(courseNum);
    TT_CHECK_FI(TT_TIME_UNDEFINED);
    TT_CHECK_MI(TT_TIME_UNDEFINED);
    TT_CHECK_CI(TT_TIME_UNDEFINED);
    s32 courseTime = 0;
    u8 starCourseFlags = omm_stars_get_course_flags(courseNum, modeIndex);
    for (s32 si = 0; si != TT_NUM_STARS_PER_COURSE; ++si) {
        if ((starCourseFlags >> si) & 1) {
            s32 t = *time_trials_time(fi, mi, ci, si);
            if (t == TT_TIME_UNDEFINED) {
                return TT_TIME_UNDEFINED;
            }
            courseTime += t;
        }
    }
    return courseTime;
}

static s32 time_trials_get_total_time(s32 fileNum, s32 modeIndex) {
    TT_GET_FI(fileNum);
    TT_GET_MI(modeIndex);
    TT_CHECK_FI(TT_TIME_UNDEFINED);
    TT_CHECK_MI(TT_TIME_UNDEFINED);
    s32 totalTime = 0;
    for (s32 courseNum = COURSE_MIN; courseNum <= COURSE_MAX; ++courseNum) {
        s32 courseTime = time_trials_get_course_time(fileNum, modeIndex, courseNum);
        if (courseTime == TT_TIME_UNDEFINED) {
            return TT_TIME_UNDEFINED;
        }
        totalTime += courseTime;
    }
    return totalTime;
}

//
// Ghost
//

typedef struct {
    struct Animation anim;
    u8 animationData[0x40000];
    s32 type;
    s32 prevValidAnimIndex;
} TTGA;
static TTGA sTimeTrialsGhostAnimData[TT_NUM_TIMES_PER_COURSE];

static s32 time_trials_ghost_retrieve_current_animation_index() {
    struct_MarioAnimDmaTable *dmaTable = gMarioAnimDmaTable;
    void *a = gMarioCurrAnimAddr;
    for (s32 i = 0; i != (s32) dmaTable->count; ++i) {
        void *b = dmaTable->srcAddr + dmaTable->anim[i].offset;
        if (a == b) {
            return i;
        }
    }
    return 0;
}

static bool time_trials_ghost_load_animation(TTGA *animData, s32 animIndex) {
    struct_MarioAnimDmaTable *dmaTable = gMarioAnimDmaTable;
    if (animIndex < 0 || animIndex >= (s32) dmaTable->count) {
        return false;
    }

    // Raw data
    u8 *addr = dmaTable->srcAddr + dmaTable->anim[animIndex].offset;
    u32 size = dmaTable->anim[animIndex].size;        
    mem_cpy(animData->animationData, addr, size);

    // Seek index and values pointers
    struct Animation *anm = (struct Animation *) animData->animationData;
    const u16 *indexStart = (const u16 *) ((uintptr_t) (animData->animationData) + (uintptr_t) (anm->index));
    const s16 *valueStart = (const s16 *) ((uintptr_t) (animData->animationData) + (uintptr_t) (anm->values));

    // Fill ghost anim data
    mem_cpy(&animData->anim, animData->animationData, sizeof(struct Animation));
    animData->anim.index = indexStart;
    animData->anim.values = valueStart;
    return true;
}

static void time_trials_ghost_update_animation(TTGA *animData, struct Object *obj, s32 animIndex, s32 animFrame) {
    if (!obj->oCurrAnim) {
        animData->prevValidAnimIndex = -1;
    }

    // Load & set anim
    if (animIndex != animData->prevValidAnimIndex && time_trials_ghost_load_animation(animData, animIndex)) {
        obj->oCurrAnim = &animData->anim;
        obj->oAnimInfo.animAccel = 0;
        animData->prevValidAnimIndex = animIndex;
    }

    // Set frame
    if (obj->oCurrAnim) {
        obj->oAnimInfo.animFrame = min_s(animFrame, obj->oCurrAnim->mLoopEnd - 1);
    }
}

static void time_trials_ghost_load(s32 fileNum, s32 modeIndex, s32 courseNum, s32 starNum, s32 type) {
    TT_GET_FI(fileNum);
    TT_GET_MI(modeIndex);
    TT_GET_CI(courseNum);
    TT_GET_SI(starNum);
    TT_CHECK_FI();
    TT_CHECK_MI();
    TT_CHECK_CI();
    TT_CHECK_SI();

    // Should we load the ghost?
    if ((type == TT_GHOST_MARIO_DEFAULT && !gOmmTimeTrialsShowStarGhosts) ||
        (type == TT_GHOST_MARIO_BOWSER && !gOmmTimeTrialsShowBowserGhosts) ||
        (type == TT_GHOST_MARIO_SLIDE && !gOmmTimeTrialsShowSlideGhosts) ||
        (type == TT_GHOST_MARIO_100_COINS && !gOmmTimeTrialsShowCoinsGhosts) ||
        (type == TT_GHOST_MARIO_ALL_STARS && !gOmmTimeTrialsShowAllStarsGhosts)) {
        return;
    }

    // Load the ghost
    if (time_trials_read(fi, mi, ci, si, sTimeTrials->ghostR[si])) {
        mem_zero(&sTimeTrialsGhostAnimData[si], sizeof(sTimeTrialsGhostAnimData[si]));
        sTimeTrialsGhostAnimData[si].type = type;
        sTimeTrialsGhostAnimData[si].prevValidAnimIndex = -1;
    }
}

static void time_trials_ghost_load_stars(s32 fileNum, s32 modeIndex, s32 courseNum, s32 numStars, s32 type) {
    u8 starCourseFlags = omm_stars_get_course_flags(courseNum, modeIndex);
    for (s32 si = 0; si != numStars; ++si) {
        if (starCourseFlags & (1 << si)) {
            time_trials_ghost_load(fileNum, modeIndex, courseNum, si + 1, type);
        }
    }
}

static void time_trials_ghost_update(struct MarioState *m, s32 starNum, s32 frame) {
    TT_GET_SI(starNum);
    TT_CHECK_SI();
    TTGP ghostData = sTimeTrials->ghostR[si] + frame;
    TTGA *animData = &sTimeTrialsGhostAnimData[si];
    const BehaviorScript *ghostBhv = bhvTimeTrialsGhost[si];
    const BehaviorScript *numberBhv = bhvTimeTrialsGhostNumber[si];
    struct Object *ghostObj = obj_get_first_with_behavior(ghostBhv);
    struct Object *numberObj = obj_get_first_with_behavior(numberBhv);

    // Should we unload the ghost?
    if ((animData->type == TT_GHOST_MARIO_DEFAULT && !gOmmTimeTrialsShowStarGhosts) ||
        (animData->type == TT_GHOST_MARIO_BOWSER && !gOmmTimeTrialsShowBowserGhosts) ||
        (animData->type == TT_GHOST_MARIO_SLIDE && !gOmmTimeTrialsShowSlideGhosts) ||
        (animData->type == TT_GHOST_MARIO_100_COINS && !gOmmTimeTrialsShowCoinsGhosts) ||
        (animData->type == TT_GHOST_MARIO_ALL_STARS && !gOmmTimeTrialsShowAllStarsGhosts)) {
        if (ghostObj) obj_mark_for_deletion(ghostObj);
        if (numberObj) obj_mark_for_deletion(numberObj);
        return;
    }

    // If timer reached its max or frame data is ended, unload the ghost
    if (frame < 0 || frame >= TT_TIME_LIMIT || !ghostData->levelNum) {
        if (ghostObj) obj_mark_for_deletion(ghostObj);
        if (numberObj) obj_mark_for_deletion(numberObj);
        return;
    }

    // Spawn ghost if not loaded
    if (!ghostObj) {
        ghostObj = spawn_object(m->marioObj, 0, ghostBhv);
        numberObj = spawn_object(m->marioObj, MODEL_NUMBER, numberBhv);
    }

    // Hide ghost if disabled, on first frame or its level or area differs from Mario
    if (frame == 0 || ghostData->levelNum != (u8) gCurrLevelNum || ghostData->areaIndex != (u8) gCurrAreaIndex) {
        ghostObj->oNodeFlags |= GRAPH_RENDER_INVISIBLE;
        numberObj->oNodeFlags |= GRAPH_RENDER_INVISIBLE;
        return;
    }

    // Update ghost
    obj_reset_hitbox(ghostObj, 0, 0, 0, 0, 0, 0);
    obj_set_params(ghostObj, 0, 0, 0, 0, false);
    obj_set_xyz(ghostObj, ghostData->posX, ghostData->posY, ghostData->posZ);
    obj_set_angle(ghostObj, ghostData->pitch * 0x100, ghostData->yaw * 0x100, ghostData->roll * 0x100);
    obj_set_scale(ghostObj, ghostData->scaleX / 100.f, ghostData->scaleY / 100.f, ghostData->scaleZ / 100.f);
    obj_update_gfx(ghostObj);
    ghostObj->oOpacity = 0xA0;
    ghostObj->oNodeFlags &= ~GRAPH_RENDER_INVISIBLE;
    ghostObj->oGraphNode = time_trials_get_ghost_mario_graph_node(animData->type);
    time_trials_ghost_update_animation(animData, ghostObj, (u16) ghostData->animIndex, (u16) ghostData->animFrame);

    // Update number
    obj_reset_hitbox(numberObj, 0, 0, 0, 0, 0, 0);
    obj_set_params(numberObj, 0, 0, 0, 0, false);
    obj_set_xyz(numberObj, ghostData->posX, ghostData->posY + ghostData->scaleY * 2.f, ghostData->posZ);
    obj_set_angle(numberObj, 0, 0, 0);
    obj_set_scale(numberObj, 0.8f, 0.8f, 0.8f);
    obj_update_gfx(numberObj);
    numberObj->oAnimState = starNum;

    // Hide number for 100 coins star ghost, Bowser key ghost or All-stars ghost
    if ((starNum == TT_STAR_NUM_100_COINS && COURSE_IS_MAIN_COURSE(gCurrCourseNum)) ||
        (starNum == TT_STAR_NUM_BOWSER_KEY && COURSE_IS_BOWSER_COURSE(gCurrCourseNum)) ||
        (starNum == TT_STAR_NUM_ALL_STARS)) {
        numberObj->oNodeFlags |= GRAPH_RENDER_INVISIBLE;
    } else {
        numberObj->oNodeFlags &= ~GRAPH_RENDER_INVISIBLE;
    }
}

static void time_trials_ghost_record(struct MarioState *m, s32 frame) {
    if (frame >= 0 && frame < TT_TIME_LIMIT) {
        if (m->marioObj && m->marioObj->oCurrAnim) {
            sTimeTrials->ghostW[frame].posX      = (s16) m->marioObj->oGfxPos[0];
            sTimeTrials->ghostW[frame].posY      = (s16) m->marioObj->oGfxPos[1];
            sTimeTrials->ghostW[frame].posZ      = (s16) m->marioObj->oGfxPos[2];
            sTimeTrials->ghostW[frame].pitch     = (s8) (m->marioObj->oGfxAngle[0] / 0x100);
            sTimeTrials->ghostW[frame].yaw       = (s8) (m->marioObj->oGfxAngle[1] / 0x100);
            sTimeTrials->ghostW[frame].roll      = (s8) (m->marioObj->oGfxAngle[2] / 0x100);
            sTimeTrials->ghostW[frame].scaleX    = (u8) clamp_f(m->marioObj->oGfxScale[0] * 100.f, 0.f, 255.f);
            sTimeTrials->ghostW[frame].scaleY    = (u8) clamp_f(m->marioObj->oGfxScale[1] * 100.f, 0.f, 255.f);
            sTimeTrials->ghostW[frame].scaleZ    = (u8) clamp_f(m->marioObj->oGfxScale[2] * 100.f, 0.f, 255.f);
            sTimeTrials->ghostW[frame].animIndex = (u8) time_trials_ghost_retrieve_current_animation_index();
            sTimeTrials->ghostW[frame].animFrame = (u8) clamp_s(min_s(m->marioObj->oAnimInfo.animFrame, m->marioObj->oCurrAnim->mLoopEnd - 1), 0, 255);
            sTimeTrials->ghostW[frame].levelNum  = (u8) gCurrLevelNum;
            sTimeTrials->ghostW[frame].areaIndex = (u8) gCurrAreaIndex;
        } else if (frame > 0) {
            sTimeTrials->ghostW[frame]           = sTimeTrials->ghostW[frame - 1];
        }
    }
}

static void time_trials_ghost_unload_all() {
    for (s32 si = 0; si != TT_NUM_TIMES_PER_COURSE; ++si) {
        obj_unload_all_with_behavior(bhvTimeTrialsGhost[si]);
        obj_unload_all_with_behavior(bhvTimeTrialsGhostNumber[si]);
    }
}

//
// Level entry
//

OMM_ROUTINE_LEVEL_ENTRY(time_trials_level_entry) {
    sTimeTrials->isLevelEntry = true;

    // Entering or exiting a Bowser fight doesn't reset the timer
    if ((gCurrLevelNum == LEVEL_BOWSER_1 && sTimeTrials->prevLevelNum == LEVEL_BITDW   ) ||
        (gCurrLevelNum == LEVEL_BITDW    && sTimeTrials->prevLevelNum == LEVEL_BOWSER_1) ||
        (gCurrLevelNum == LEVEL_BOWSER_2 && sTimeTrials->prevLevelNum == LEVEL_BITFS   ) ||
        (gCurrLevelNum == LEVEL_BITFS    && sTimeTrials->prevLevelNum == LEVEL_BOWSER_2) ||
        (gCurrLevelNum == LEVEL_BOWSER_3 && sTimeTrials->prevLevelNum == LEVEL_BITS    ) ||
        (gCurrLevelNum == LEVEL_BITS     && sTimeTrials->prevLevelNum == LEVEL_BOWSER_3)) {
        sTimeTrials->isLevelEntry = false;
    }

    // Warping directly to a Bowser fight sets the timer to 9'59"99
    if ((gCurrLevelNum == LEVEL_BOWSER_1 && sTimeTrials->prevLevelNum != LEVEL_BITDW) ||
        (gCurrLevelNum == LEVEL_BOWSER_2 && sTimeTrials->prevLevelNum != LEVEL_BITFS) ||
        (gCurrLevelNum == LEVEL_BOWSER_3 && sTimeTrials->prevLevelNum != LEVEL_BITS)) {
        sTimeTrials->timer = TT_TIME_LIMIT;
        sTimeTrials->isLevelEntry = false;
    }

    sTimeTrials->prevLevelNum = gCurrLevelNum;
}

//
// Update
//

OMM_ROUTINE_PRE_RENDER(time_trials_update) {
    struct MarioState *m = gMarioState;

    // Reset Time Trials state if not available
    if (!time_trials_is_enabled()) {
        time_trials_ghost_unload_all();
        sTimeTrials->timer = TT_TIME_LIMIT;
        sTimeTrials->state = TT_STATE_DISABLED;
        sTimeTrials->isLevelEntry = false;
        return;
    }

    // Load all times at startup
    OMM_DO_ONCE {
        time_trials_time(0, 0, 0, 0);
    }

    // Update running ghosts, record ghost data, and advance timer if it's running
    if (sTimeTrials->state == TT_STATE_RUNNING) {
        sTimeTrials->hiScore = false;

        // Update ghosts and advance timer if not paused
        if (!omm_is_game_paused()) {
            for (s32 i = 0; i != TT_NUM_TIMES_PER_COURSE; ++i) {
                time_trials_ghost_update(m, i + 1, sTimeTrials->timer);
            }
            time_trials_ghost_record(m, sTimeTrials->timer);
            sTimeTrials->timer = clamp_s(sTimeTrials->timer + 1, 0, TT_TIME_LIMIT);
        }
    }

    // Runs timer if course is TTable
    if (gCurrCourseNum != COURSE_NONE) {

        // Start timer and init ghost if a course entry is detected
        if (gMarioObject && sTimeTrials->isLevelEntry) {
            sTimeTrials->state = TT_STATE_RUNNING;
            sTimeTrials->hiScore = false;
            sTimeTrials->timer = 0;
            sTimeTrials->isLevelEntry = false;

            // Reset ghost data
            time_trials_ghost_unload_all();
            mem_zero(sTimeTrials->ghostW, sizeof(sTimeTrials->ghostW));
            mem_zero(sTimeTrials->ghostR, sizeof(sTimeTrials->ghostR));

            // Main courses
            // Load selected act ghost or all 6 stars ghosts if Non-Stop and 100 coins ghost
            if (COURSE_IS_MAIN_COURSE(gCurrCourseNum)) {
                if (OMM_STARS_NON_STOP) {
                    time_trials_ghost_load_stars(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_NUM_STARS_PER_COURSE - 1, TT_GHOST_MARIO_DEFAULT);
                } else {
                    time_trials_ghost_load(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, gCurrActNum, TT_GHOST_MARIO_DEFAULT);
                }
                time_trials_ghost_load(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_STAR_NUM_100_COINS, TT_GHOST_MARIO_100_COINS);
            }

            // Bowser course
            // Load all stars ghosts as Blue ghosts and Bowser fight as Red ghost
            else if (COURSE_IS_BOWSER_COURSE(gCurrCourseNum)) {
                time_trials_ghost_load_stars(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_NUM_STARS_PER_COURSE, TT_GHOST_MARIO_DEFAULT);
#if !OMM_GAME_IS_SMMS
                time_trials_ghost_load(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_STAR_NUM_BOWSER_KEY, TT_GHOST_MARIO_BOWSER);
#endif
            }

            // Sliding level
            // Load all stars ghosts as blue ghosts but Slide star as Green ghost
            else if (gCurrLevelNum == OMM_LEVEL_SLIDE) {
                time_trials_ghost_load_stars(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_NUM_STARS_PER_COURSE, TT_GHOST_MARIO_DEFAULT);
                time_trials_ghost_load(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, 2, TT_GHOST_MARIO_SLIDE);
            }

            // Other courses
            // Load all stars ghosts as blue ghosts
            else {
                time_trials_ghost_load_stars(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_NUM_STARS_PER_COURSE, TT_GHOST_MARIO_DEFAULT);
            }

            // All-stars ghost
            if (OMM_STARS_NON_STOP && !COURSE_IS_BOWSER_COURSE(gCurrCourseNum)) {
                time_trials_ghost_load(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, TT_NUM_TIMES_PER_COURSE, TT_GHOST_MARIO_ALL_STARS);
            }
        }

        // Stop timer and save time when Mario collects a star, a key or a Grand star
        if ((sTimeTrials->state == TT_STATE_RUNNING) && (
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
            if (COURSE_IS_MAIN_COURSE(gCurrCourseNum) && (!OMM_STARS_NON_STOP || OMM_ALL_STARS)) {
                drop_queued_background_music();
                fadeout_level_music(120);
                m->actionArg = 0;
            }

            // Save the time
            if (sTimeTrials->state == TT_STATE_RUNNING) {
                s32 starNum = gLastCompletedStarNum;

                // Bowser Key or Grand Star
                if (gCurrLevelNum == LEVEL_BOWSER_1 ||
                    gCurrLevelNum == LEVEL_BOWSER_2 ||
                    gCurrLevelNum == LEVEL_BOWSER_3) {
                    starNum = TT_STAR_NUM_BOWSER_KEY;
                    sTimeTrials->state = TT_STATE_STOPPED;
                }

                // Write time and ghost data if new record
                time_trials_write_if_new_record(starNum);
                if (OMM_STARS_NON_STOP && OMM_ALL_STARS) {
                    time_trials_write_if_new_record(TT_STAR_NUM_ALL_STARS);
                }

                // Stop the timer if Mario leaves the course
                if (m->actionArg == 0) {
                    sTimeTrials->state = TT_STATE_STOPPED;
                }
            }
        }
    } else {
        sTimeTrials->timer = TT_TIME_LIMIT;
        sTimeTrials->state = TT_STATE_DISABLED;
    }
}

//
// HUD
//

InterpData gTimeTrialsTimer[1];

#define TT_HUD_TIMER_RUNNING_CLOCK_W        ((OMM_RENDER_GLYPH_SIZE * 8) / 5)
#define TT_HUD_TIMER_RUNNING_CLOCK_X        (OMM_RENDER_TIMER_X - (TT_HUD_TIMER_RUNNING_CLOCK_W * 3) / 2)
#define TT_HUD_TIMER_RUNNING_CLOCK_Y        (SCREEN_HEIGHT - (OMM_RENDER_TIMER_Y + (TT_HUD_TIMER_RUNNING_CLOCK_W - OMM_RENDER_GLYPH_SIZE) / 2) - OMM_RENDER_GLYPH_SIZE)

#define TT_HUD_TIMER_RUNNING_CLOCK_HAND_W   ((TT_HUD_TIMER_RUNNING_CLOCK_W * 3) / 4)
#define TT_HUD_TIMER_RUNNING_CLOCK_HAND_X   (TT_HUD_TIMER_RUNNING_CLOCK_X + (TT_HUD_TIMER_RUNNING_CLOCK_W / 2))
#define TT_HUD_TIMER_RUNNING_CLOCK_HAND_Y   (TT_HUD_TIMER_RUNNING_CLOCK_Y + (TT_HUD_TIMER_RUNNING_CLOCK_W / 2))

#define TT_HUD_TIMER_RUNNING_DIGIT_W        (OMM_RENDER_GLYPH_SIZE)
#define TT_HUD_TIMER_RUNNING_DIGIT_X        (OMM_RENDER_TIMER_X)
#define TT_HUD_TIMER_RUNNING_DIGIT_Y        (SCREEN_HEIGHT - OMM_RENDER_TIMER_Y - OMM_RENDER_GLYPH_SIZE)

#define TT_HUD_TIMER_NEW_RECORD_CROWN_W     ((OMM_RENDER_GLYPH_SIZE * 8) / 5)
#define TT_HUD_TIMER_NEW_RECORD_CROWN_X     (OMM_RENDER_TIMER_X - (TT_HUD_TIMER_RUNNING_CLOCK_W * 3) / 2)
#define TT_HUD_TIMER_NEW_RECORD_CROWN_Y     (SCREEN_HEIGHT - (OMM_RENDER_TIMER_Y + (TT_HUD_TIMER_RUNNING_CLOCK_W - OMM_RENDER_GLYPH_SIZE) / 2) - OMM_RENDER_GLYPH_SIZE + 1)

#define TT_HUD_TIMER_STAR_SELECT_CROWN_W    ((OMM_RENDER_GLYPH_SIZE * 6) / 5)
#define TT_HUD_TIMER_STAR_SELECT_CROWN_X    ((SCREEN_WIDTH / 2) - 8)
#define TT_HUD_TIMER_STAR_SELECT_CROWN_Y    (OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2 - ((OMM_RENDER_GLYPH_SIZE - 8) / 2))

#define TT_HUD_TIMER_STAR_SELECT_DIGIT_W    (OMM_RENDER_GLYPH_SIZE)
#define TT_HUD_TIMER_STAR_SELECT_DIGIT_X    (TT_HUD_TIMER_STAR_SELECT_CROWN_X + 1 + (OMM_RENDER_VALUE_NUMBER_X - OMM_RENDER_VALUE_GLYPH_X))
#define TT_HUD_TIMER_STAR_SELECT_DIGIT_Y    (TT_HUD_TIMER_STAR_SELECT_CROWN_Y + (TT_HUD_TIMER_STAR_SELECT_CROWN_W - TT_HUD_TIMER_STAR_SELECT_DIGIT_W) / 2)

static Gfx *time_trials_render_timer_clock_hand(Gfx *pos, Vtx *vtx, f32 timer) {
    s16 handW = TT_HUD_TIMER_RUNNING_CLOCK_HAND_W;
    s16 handX = TT_HUD_TIMER_RUNNING_CLOCK_HAND_X;
    s16 handY = TT_HUD_TIMER_RUNNING_CLOCK_HAND_Y;
    s16 handR = ((timer * 0x10000) / 30.f);
    vtx[0] = (Vtx) {{{ handX + handW * coss(0xA000 - handR), handY + handW * sins(0xA000 - handR), 0 }, 0, { 0x0000, 0x2000 }, { 0xFF, 0xFF, 0xFF, 0xFF }}};
    vtx[1] = (Vtx) {{{ handX + handW * coss(0xE000 - handR), handY + handW * sins(0xE000 - handR), 0 }, 0, { 0x2000, 0x2000 }, { 0xFF, 0xFF, 0xFF, 0xFF }}};
    vtx[2] = (Vtx) {{{ handX + handW * coss(0x2000 - handR), handY + handW * sins(0x2000 - handR), 0 }, 0, { 0x2000, 0x0000 }, { 0xFF, 0xFF, 0xFF, 0xFF }}};
    vtx[3] = (Vtx) {{{ handX + handW * coss(0x6000 - handR), handY + handW * sins(0x6000 - handR), 0 }, 0, { 0x0000, 0x0000 }, { 0xFF, 0xFF, 0xFF, 0xFF }}};
    gDPSetTexturePersp(pos++, G_TP_NONE);
    gDPSetRenderMode(pos++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    gDPSetCombineLERP(pos++, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0);
    gDPSetEnvColor(pos++, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetTextureFilter(pos++, G_TF_POINT);
    gSPTexture(pos++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPLoadTextureBlock(pos++, OMM_TEXTURE_HUD_CLOCK_HAND, G_IM_FMT_RGBA, G_IM_SIZ_32b, 256, 256, 0, G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
    gSPVertex(pos++, vtx, 4, 0);
    gSP2Triangles(pos++, 0, 1, 2, 0, 0, 2, 3, 0);
    gDPSetTexturePersp(pos++, G_TP_PERSP);
    gDPSetRenderMode(pos++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetCombineLERP(pos++, 0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE);
    gDPSetEnvColor(pos++, 0xFF, 0xFF, 0xFF, 0xFF);
    gDPSetTextureFilter(pos++, G_TF_BILERP);
    gSPTexture(pos++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    return pos;
}

static Gfx *time_trials_render_timer_digits(Gfx *pos, f32 timer, s16 x, s16 y, f32 color, f32 alpha) {
    static const char *TIME_TRIALS_TIMER_GLYPHS[] = {
        OMM_TEXTURE_HUD_0, OMM_TEXTURE_HUD_1,
        OMM_TEXTURE_HUD_2, OMM_TEXTURE_HUD_3,
        OMM_TEXTURE_HUD_4, OMM_TEXTURE_HUD_5,
        OMM_TEXTURE_HUD_6, OMM_TEXTURE_HUD_7,
        OMM_TEXTURE_HUD_8, OMM_TEXTURE_HUD_9,
        OMM_TEXTURE_HUD_M, OMM_TEXTURE_HUD_S,
    };

    s32 ms = timer >= TT_TIME_LIMIT ? 599999 : (s32) (1000.f * (timer / 30.f));
    s32 glyphs[7] = {
        ((ms /  60000) % 10),
        10,
        ((ms /  10000) %  6),
        ((ms /   1000) % 10),
        11,
        ((ms /    100) % 10),
        ((ms /     10) % 10),
    };
    for (s32 i = 0, j = 0; i != 7; ++i) {
        j = max_s(j, glyphs[i] * (glyphs[i] < 10));
        s16 dx = i * OMM_RENDER_TIMER_OFFSET_X;
        gDPSetTexturePersp(pos++, G_TP_NONE);
        gDPSetRenderMode(pos++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
        gDPSetCombineLERP(pos++, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0);
        gDPSetEnvColor(pos++, 0xFF * color, 0xFF * color, 0xFF * color, (0xFF - (0xAA * (j == 0))) * alpha);
        gDPSetTextureFilter(pos++, G_TF_POINT);
        gSPTexture(pos++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(pos++, TIME_TRIALS_TIMER_GLYPHS[glyphs[i]], G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0, G_TX_CLAMP, G_TX_CLAMP, 0, 0, 0, 0);
        gSPTextureRectangle(pos++, (x + dx) << 2, (SCREEN_HEIGHT - TT_HUD_TIMER_RUNNING_DIGIT_W - y) << 2, (x + dx + TT_HUD_TIMER_RUNNING_DIGIT_W) << 2, (SCREEN_HEIGHT - y) << 2, G_TX_RENDERTILE, 0, 0, (0x4000 / TT_HUD_TIMER_RUNNING_DIGIT_W), (0x4000 / TT_HUD_TIMER_RUNNING_DIGIT_W));
        gDPSetTexturePersp(pos++, G_TP_PERSP);
        gDPSetRenderMode(pos++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
        gDPSetCombineLERP(pos++, 0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE, 0, 0, 0, SHADE);
        gDPSetEnvColor(pos++, 0xFF, 0xFF, 0xFF, 0xFF);
        gDPSetTextureFilter(pos++, G_TF_BILERP);
        gSPTexture(pos++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    }
    return pos;
}

static void time_trials_render_clock_or_crown(const char *texture, s16 x, s16 y, s16 w, f32 color, f32 alpha) {
    omm_render_texrect(x, y, w, w, 16, 16, 0xFF * color, 0xFF * color, 0xFF * color, 0xFF * alpha, texture, false);
}

Gfx *time_trials_render_timer_interpolated(Gfx *pos, Vtx *vtx, f32 timer) {
    pos = time_trials_render_timer_clock_hand(pos, vtx, timer);
    pos = time_trials_render_timer_digits(pos, timer, TT_HUD_TIMER_RUNNING_DIGIT_X, TT_HUD_TIMER_RUNNING_DIGIT_Y, 1.f, 1.f);
    return pos;
}

static void time_trials_render_timer_running() {
    Vtx *vtx = omm_alloc_vtx(4);
    time_trials_render_clock_or_crown(OMM_TEXTURE_HUD_CLOCK, TT_HUD_TIMER_RUNNING_CLOCK_X, TT_HUD_TIMER_RUNNING_CLOCK_Y, TT_HUD_TIMER_RUNNING_CLOCK_W, 1.f, 1.f);
    interp_data_update(gTimeTrialsTimer, gTimeTrialsTimer->s1 >= (f32) gGlobalTimer - 1, gDisplayListHead, NULL, vtx, 0, 0, 0, 0, gGlobalTimer, sTimeTrials->timer);
    gDisplayListHead = time_trials_render_timer_interpolated(gDisplayListHead, gTimeTrialsTimer->vtx, gTimeTrialsTimer->t0);
}

static void time_trials_render_timer_new_record(f32 color) {
    time_trials_render_clock_or_crown(OMM_TEXTURE_HUD_CROWN, TT_HUD_TIMER_NEW_RECORD_CROWN_X, TT_HUD_TIMER_NEW_RECORD_CROWN_Y, TT_HUD_TIMER_NEW_RECORD_CROWN_W, color, 1.f);
    interp_data_reset(gTimeTrialsTimer);
    gDisplayListHead = time_trials_render_timer_digits(gDisplayListHead, sTimeTrials->timer, TT_HUD_TIMER_RUNNING_DIGIT_X, TT_HUD_TIMER_RUNNING_DIGIT_Y, color, 1.f);
}

static void time_trials_render_timer_star_select(s32 timer, f32 alpha) {
    time_trials_render_clock_or_crown(OMM_TEXTURE_HUD_CROWN, TT_HUD_TIMER_STAR_SELECT_CROWN_X, TT_HUD_TIMER_STAR_SELECT_CROWN_Y, TT_HUD_TIMER_STAR_SELECT_CROWN_W, 1.f, alpha);
    interp_data_reset(gTimeTrialsTimer);
    gDisplayListHead = time_trials_render_timer_digits(gDisplayListHead, timer, TT_HUD_TIMER_STAR_SELECT_DIGIT_X, TT_HUD_TIMER_STAR_SELECT_DIGIT_Y, 1.f, alpha);
}

void time_trials_render_hud_timer() {
    if (sTimeTrials->state != TT_STATE_DISABLED) {
        if (sTimeTrials->hiScore && sTimeTrials->state == TT_STATE_STOPPED) {
            time_trials_render_timer_new_record(relerp_0_1_f(sins(gGlobalTimer * 0x1000), -1.f, 1.f, 0.5f, 1.f));
        } else if (sTimeTrials->timer > 0) {
            time_trials_render_timer_running();
        } else {
            interp_data_reset(gTimeTrialsTimer);
        }
    }
}

void time_trials_render_star_select_time(s32 starIndex, u8 alpha) {
    if (time_trials_is_enabled()) {
        s32 bestTime = time_trials_get_time(gCurrSaveFileNum, OMM_GAME_MODE, gCurrCourseNum, starIndex + 1);
        if (bestTime != TT_TIME_UNDEFINED) {
            u8 *bestTimeText = omm_text_convert(OMM_TEXT_TT_BEST_TIME, false);
            s32 bestTimeX = (SCREEN_WIDTH / 2) - (omm_render_get_string_width(bestTimeText) + 11);
            omm_render_string(bestTimeX - 1, OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2,     0x40, 0x40, 0x00, alpha, bestTimeText, false);
            omm_render_string(bestTimeX + 1, OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2,     0x40, 0x40, 0x00, alpha, bestTimeText, false);
            omm_render_string(bestTimeX,     OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2 - 1, 0x40, 0x40, 0x00, alpha, bestTimeText, false);
            omm_render_string(bestTimeX,     OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2 + 1, 0x40, 0x40, 0x00, alpha, bestTimeText, false);
            omm_render_string(bestTimeX,     OMM_RENDER_STAR_SELECT_SCORE_Y + OMM_RENDER_GLYPH_SIZE * 2,     0xFF, 0xFF, 0x00, alpha, bestTimeText, false);
            time_trials_render_timer_star_select(bestTime, alpha / 255.f);
        }
    }
}

//
// Pause menu
//

#define TT_PAUSE_BACKGROUND_W               (300)
#define TT_PAUSE_BACKGROUND_H               (4 * TT_PAUSE_OFFSET_LINE_Y + 2 * TT_PAUSE_OFFSET_LINE_3_2_Y + 2 * TT_PAUSE_OFFSET_Y - 8)
#define TT_PAUSE_BACKGROUND_X               (SCREEN_WIDTH / 2)
#define TT_PAUSE_BACKGROUND_Y               (SCREEN_HEIGHT / 2 - 16)

#define TT_PAUSE_TITLE_Y                    (TT_PAUSE_BACKGROUND_Y + TT_PAUSE_BACKGROUND_H / 2 + 20)
#define TT_PAUSE_TRIANGLE_UP_Y              (TT_PAUSE_BACKGROUND_Y + TT_PAUSE_BACKGROUND_H / 2 + 4)
#define TT_PAUSE_TRIANGLE_DOWN_Y            (TT_PAUSE_BACKGROUND_Y - TT_PAUSE_BACKGROUND_H / 2 - 4)

#define TT_PAUSE_OFFSET_X                   (10)
#define TT_PAUSE_OFFSET_Y                   (10)
#define TT_PAUSE_OFFSET_LINE_Y              (16)
#define TT_PAUSE_OFFSET_LINE_3_2_Y          ((TT_PAUSE_OFFSET_LINE_Y * 3) / 2)

#define TT_PAUSE_LEFT_X                     (TT_PAUSE_BACKGROUND_X - TT_PAUSE_BACKGROUND_W / 2 + TT_PAUSE_OFFSET_X)
#define TT_PAUSE_LEFT_TIME_X                (TT_PAUSE_BACKGROUND_X - TT_PAUSE_OFFSET_X)
#define TT_PAUSE_RIGHT_X                    (TT_PAUSE_BACKGROUND_X + TT_PAUSE_OFFSET_X)
#define TT_PAUSE_RIGHT_TIME_X               (TT_PAUSE_BACKGROUND_X + TT_PAUSE_BACKGROUND_W / 2 - TT_PAUSE_OFFSET_X)

#define TT_PAUSE_COURSE_NAME_Y              (TT_PAUSE_BACKGROUND_Y + TT_PAUSE_BACKGROUND_H / 2 - TT_PAUSE_OFFSET_Y - 8)
#define TT_PAUSE_STAR_Y0                    (TT_PAUSE_COURSE_NAME_Y - TT_PAUSE_OFFSET_LINE_3_2_Y)
#define TT_PAUSE_ALL_STARS_Y                (TT_PAUSE_STAR_Y0 - 3 * TT_PAUSE_OFFSET_LINE_Y)
#define TT_PAUSE_COURSE_Y                   (TT_PAUSE_STAR_Y0 - 3 * TT_PAUSE_OFFSET_LINE_Y - TT_PAUSE_OFFSET_LINE_3_2_Y)
#define TT_PAUSE_TOTAL_Y                    (TT_PAUSE_COURSE_Y)

static s16 time_trials_render_time_char(s16 x, s16 y, s16 w, u8 rgb, u8 alpha, u8 c) {
    u8 text[] = { c, 0xFF };
    omm_render_string(x - w, y, rgb, rgb, rgb, alpha, text, false);
    return w;
}

static void time_trials_render_time(s16 x, s16 y, u8 alpha, s32 time, s32 star) {
    s32 time_ms = (s32) (1000.f * (time / 30.f));
    u8 time_rgb = (time == TT_TIME_UNDEFINED ? 0x80 : 0xFF);

    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, time == TT_TIME_UNDEFINED ? 0x9F : ((time_ms / 10) % 10));
    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, time == TT_TIME_UNDEFINED ? 0x9F : ((time_ms / 100) % 10));
    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, 0xF6);
    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, time == TT_TIME_UNDEFINED ? 0x9F : ((time_ms / 1000) % 10));
    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, time == TT_TIME_UNDEFINED ? 0x9F : ((time_ms / 10000) % 6));
    x -= time_trials_render_time_char(x, y, 4, time_rgb, alpha, 0x3E);
    x -= time_trials_render_time_char(x, y, 7, time_rgb, alpha, time == TT_TIME_UNDEFINED ? 0x9F : ((time_ms / 60000) % 10));
    x -= (time_ms >= 600000) ? time_trials_render_time_char(x, y, 7, time_rgb, alpha, (time_ms / 600000) % 10) : 0;
    x -= (time_ms >= 6000000) ? time_trials_render_time_char(x, y, 7, time_rgb, alpha, (time_ms / 6000000) % 10) : 0;
    x -= (star >= 0) ? time_trials_render_time_char(x, y, 14, 0x80 + 0x7F * (star == 1), alpha, DIALOG_CHAR_STAR_FILLED) : 0;
}

bool time_trials_render_pause_castle(u8 alpha) {
    if (!time_trials_is_enabled()) {
        return false;
    }

    // Handle menu scrolling
    static s8 sTimeTrialsCourseIndex = 0;
    s8 prevCourseIndex = sTimeTrialsCourseIndex;
    handle_menu_scrolling(MENU_SCROLL_VERTICAL, &sTimeTrialsCourseIndex, -1, TT_NUM_COURSES);
    s32 dir = sign_s(sTimeTrialsCourseIndex - prevCourseIndex);
    for (;;) {
        if (sTimeTrialsCourseIndex < 0) { sTimeTrialsCourseIndex = TT_NUM_COURSES - 1; }
        if (sTimeTrialsCourseIndex >= TT_NUM_COURSES) { sTimeTrialsCourseIndex = 0; }
        if (omm_stars_get_course_flags(sTimeTrialsCourseIndex + 1, OMM_GAME_MODE)) { break; }
        sTimeTrialsCourseIndex += dir;
    }
    s32 courseNum = sTimeTrialsCourseIndex + 1;
    s32 levelNum = omm_level_from_course(courseNum);

    // Time Trials
    u8 *textTimeTrials = omm_text_convert(OMM_TEXT_TT_TITLE, false);
    omm_render_string_hud_centered(TT_PAUSE_TITLE_Y, 0xFF, 0xFF, 0xFF, alpha, textTimeTrials, false);

    // Black box
    static const Vtx sBlackBoxVtx[] = {
        { { { -TT_PAUSE_BACKGROUND_W / 2, -TT_PAUSE_BACKGROUND_H / 2, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { { -TT_PAUSE_BACKGROUND_W / 2, +TT_PAUSE_BACKGROUND_H / 2, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { { +TT_PAUSE_BACKGROUND_W / 2, -TT_PAUSE_BACKGROUND_H / 2, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { { +TT_PAUSE_BACKGROUND_W / 2, +TT_PAUSE_BACKGROUND_H / 2, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
    };
    create_dl_translation_matrix(MENU_MTX_PUSH, TT_PAUSE_BACKGROUND_X, TT_PAUSE_BACKGROUND_Y, 0);
    gDPSetEnvColor(gDisplayListHead++, 0x00, 0x00, 0x00, alpha / 2);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gDPSetCombineLERP(gDisplayListHead++, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPVertex(gDisplayListHead++, sBlackBoxVtx, 4, 0);
    gSP2Triangles(gDisplayListHead++, 2, 1, 0, 0, 1, 2, 3, 0);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Vertical arrows
    static const Vtx sWhiteTriVtx[] = {
        { { {  0, -8, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { { -8,  0, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { { +8,  0, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
        { { {  0, +8, 0 }, 0, { 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF } } },
    };
    create_dl_translation_matrix(MENU_MTX_PUSH, TT_PAUSE_BACKGROUND_X, TT_PAUSE_TRIANGLE_DOWN_Y, 0);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, alpha);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gDPSetCombineLERP(gDisplayListHead++, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPVertex(gDisplayListHead++, sWhiteTriVtx + 0, 3, 0);
    gSP1Triangle(gDisplayListHead++, 2, 1, 0, 0);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    create_dl_translation_matrix(MENU_MTX_PUSH, TT_PAUSE_BACKGROUND_X, TT_PAUSE_TRIANGLE_UP_Y, 0);
    gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0xFF, alpha);
    gSPClearGeometryMode(gDisplayListHead++, G_LIGHTING);
    gDPSetCombineLERP(gDisplayListHead++, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0, SHADE, 0, ENVIRONMENT, 0);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPVertex(gDisplayListHead++, sWhiteTriVtx + 1, 3, 0);
    gSP1Triangle(gDisplayListHead++, 0, 1, 2, 0);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Course name
    ustr_t textCourseName;
    omm_level_get_course_name(textCourseName, levelNum, OMM_GAME_MODE, false, true);
    omm_render_string_centered(TT_PAUSE_COURSE_NAME_Y, 0xFF, 0xFF, 0xFF, alpha, textCourseName, true);

    // Stars 1-7
    u8 starCourseFlags = omm_stars_get_course_flags(courseNum, OMM_GAME_MODE);
    for (s32 starIndex = 0, k = 0; starIndex != OMM_NUM_STARS_MAX_PER_COURSE; ++starIndex) {
        if (starCourseFlags & (1 << starIndex)) {
            const char *stringStar = NULL;
#if OMM_GAME_IS_SMSR
            const BehaviorScript *bhv = NULL;
            if (omm_stars_get_star_data(levelNum, 1, starIndex, &bhv, NULL) && bhv == bhvCustomSMSRStarReplica) {
                stringStar = OMM_TEXT_TT_STAR_REPLICA;
            } else
#endif
            if (COURSE_IS_MAIN_COURSE(courseNum) && starIndex == 6) {
                stringStar = OMM_TEXT_TT_STAR_COIN;
            } else switch (k) {
                case 0: stringStar = OMM_TEXT_TT_STAR_1; break;
                case 1: stringStar = OMM_TEXT_TT_STAR_2; break;
                case 2: stringStar = OMM_TEXT_TT_STAR_3; break;
                case 3: stringStar = OMM_TEXT_TT_STAR_4; break;
                case 4: stringStar = OMM_TEXT_TT_STAR_5; break;
                case 5: stringStar = OMM_TEXT_TT_STAR_6; break;
                case 6: stringStar = OMM_TEXT_TT_STAR_7; break;
            }

            // Star
            const u8 *textStar = omm_text_convert(stringStar, false);
            s16 x = ((k & 1) ? TT_PAUSE_RIGHT_X : TT_PAUSE_LEFT_X);
            s16 y = TT_PAUSE_STAR_Y0 - (k >> 1) * TT_PAUSE_OFFSET_LINE_Y;
            omm_render_string(x, y, 0xFF, 0xFF, 0xFF, alpha, textStar, false);

            // Time
            s32 time = time_trials_get_time(gCurrSaveFileNum, OMM_GAME_MODE, courseNum, starIndex + 1);
            s32 star = ((omm_save_file_get_star_flags(gCurrSaveFileNum - 1, OMM_GAME_MODE, courseNum - 1) >> starIndex) & 1);
            time_trials_render_time((k & 1) ? TT_PAUSE_RIGHT_TIME_X : TT_PAUSE_LEFT_TIME_X, y, alpha, time, star);
            k++;
        }
    }

    // All Stars
    if (!COURSE_IS_BOWSER_COURSE(courseNum)) {
        const u8 *textAllStars = omm_text_convert(OMM_TEXT_TT_ALL_STARS, false);
        omm_render_string(TT_PAUSE_RIGHT_X, TT_PAUSE_ALL_STARS_Y, 0xFF, 0xFF, 0x00, alpha, textAllStars, false);
        s32 time = time_trials_get_time(gCurrSaveFileNum, OMM_GAME_MODE, courseNum, TT_STAR_NUM_ALL_STARS);
        time_trials_render_time(TT_PAUSE_RIGHT_TIME_X, TT_PAUSE_ALL_STARS_Y, alpha, time, -1);
    }

#if !OMM_GAME_IS_SMMS
    // Bowser
    else {
        const u8 *textBowser = omm_text_convert(OMM_TEXT_TT_BOWSER, false);
        omm_render_string(TT_PAUSE_RIGHT_X, TT_PAUSE_ALL_STARS_Y, 0xFF, 0x80, 0x00, alpha, textBowser, false);
        s32 time = time_trials_get_time(gCurrSaveFileNum, OMM_GAME_MODE, courseNum, TT_STAR_NUM_BOWSER_KEY);
        time_trials_render_time(TT_PAUSE_RIGHT_TIME_X, TT_PAUSE_ALL_STARS_Y, alpha, time, -1);
    }
#endif

    // Course time
    const u8 *textCourse = omm_text_convert(OMM_TEXT_TT_COURSE, false);
    omm_render_string(TT_PAUSE_LEFT_X, TT_PAUSE_COURSE_Y, 0xFF, 0xFF, 0xFF, alpha, textCourse, true);
    s32 courseTime = time_trials_get_course_time(gCurrSaveFileNum, OMM_GAME_MODE, courseNum);
    time_trials_render_time(TT_PAUSE_LEFT_TIME_X, TT_PAUSE_COURSE_Y, alpha, courseTime, -1);

    // Total time
    const u8 *textTotal = omm_text_convert(OMM_TEXT_TT_TOTAL, false);
    omm_render_string(TT_PAUSE_RIGHT_X, TT_PAUSE_TOTAL_Y, 0xFF, 0xFF, 0xFF, alpha, textTotal, true);
    s32 totalTime = time_trials_get_total_time(gCurrSaveFileNum, OMM_GAME_MODE);
    time_trials_render_time(TT_PAUSE_RIGHT_TIME_X, TT_PAUSE_TOTAL_Y, alpha, totalTime, -1);

    return true;
}
