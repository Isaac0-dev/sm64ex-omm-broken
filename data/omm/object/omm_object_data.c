#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#define STATS_ "stats_"

struct OmmData gOmmData[1];
s32 gMouseDeltaX;
s32 gMouseDeltaY;

//
// Stats
//

#define read_single_stat(x) \
if (strstr(name, #x)) { \
    if (!*value1) { \
        *invalid = true; \
        return false; \
    } \
    sscanf(value1, "%llu", &gOmmData->stats->x); \
    return true; \
}

#define read_double_stat(x) \
if (strstr(name, #x)) { \
    if (!*value1 || !*value2) { \
        *invalid = true; \
        return false; \
    } \
    sscanf(value1, "%llu", &gOmmData->stats->x[0]); \
    sscanf(value2, "%llu", &gOmmData->stats->x[1]); \
    return true; \
}

static bool omm_data_read_stats(const char *name, const char *value1, const char *value2, bool *invalid) {
    if (strstr(name, STATS_) == name) {
        read_single_stat(starsCollected);
        read_single_stat(sparklyStarsCollected);
        read_single_stat(coinsCollected);
        read_single_stat(capsCollected);
        read_single_stat(mushrooms1upCollected);
        read_single_stat(secretsCollected);
        read_single_stat(exclamationBoxesBroken);
        read_single_stat(enemiesDefeated);
        read_single_stat(bowsersDefeated);
        read_single_stat(aPresses);
        read_single_stat(jumps);
        read_single_stat(attacks);
        read_single_stat(cappyThrows);
        read_single_stat(cappyBounces);
        read_single_stat(captures);
        read_single_stat(hitsTaken);
        read_single_stat(restarts);
        read_single_stat(deaths);
        read_double_stat(distanceTotal);
        read_double_stat(distanceOnGround);
        read_double_stat(distanceAirborne);
        read_double_stat(distanceUnderwater);
        read_double_stat(distanceWingCap);
        read_double_stat(distanceMetalCap);
        read_double_stat(distanceVanishCap);
        read_double_stat(timeTotal);
        read_double_stat(timeOnGround);
        read_double_stat(timeAirborne);
        read_double_stat(timeUnderwater);
        read_double_stat(timeWingCap);
        read_double_stat(timeMetalCap);
        read_double_stat(timeVanishCap);
    }
    return false;
}

#define write_single_stat(x) omm_save_file_write_buffer(STATS_ #x " = %llu\n", gOmmData->stats->x)
#define write_double_stat(x) omm_save_file_write_buffer(STATS_ #x " = %llu %llu\n", gOmmData->stats->x[0], gOmmData->stats->x[1])
static void omm_data_write_stats() {
    omm_save_file_write_buffer(OMM_SAVE_FILE_SECTION_STATS "\n");
    write_single_stat(starsCollected);
    write_single_stat(sparklyStarsCollected);
    write_single_stat(coinsCollected);
    write_single_stat(capsCollected);
    write_single_stat(mushrooms1upCollected);
    write_single_stat(secretsCollected);
    write_single_stat(exclamationBoxesBroken);
    write_single_stat(enemiesDefeated);
    write_single_stat(bowsersDefeated);
    write_single_stat(aPresses);
    write_single_stat(jumps);
    write_single_stat(attacks);
    write_single_stat(cappyThrows);
    write_single_stat(cappyBounces);
    write_single_stat(captures);
    write_single_stat(hitsTaken);
    write_single_stat(restarts);
    write_single_stat(deaths);
    write_double_stat(distanceTotal);
    write_double_stat(distanceOnGround);
    write_double_stat(distanceAirborne);
    write_double_stat(distanceUnderwater);
    write_double_stat(distanceWingCap);
    write_double_stat(distanceMetalCap);
    write_double_stat(distanceVanishCap);
    write_double_stat(timeTotal);
    write_double_stat(timeOnGround);
    write_double_stat(timeAirborne);
    write_double_stat(timeUnderwater);
    write_double_stat(timeWingCap);
    write_double_stat(timeMetalCap);
    write_double_stat(timeVanishCap);
    omm_save_file_write_buffer("\n");
}

//
// Reset
//

static void omm_data_reset_mario() {

    // State data
    gOmmData->mario->state.ticks = 0;
    gOmmData->mario->state.coins = 0;
    gOmmData->mario->state.breath = 0;
    gOmmData->mario->state.airCombo = 0;
    gOmmData->mario->state.peakHeight = FLOOR_LOWER_LIMIT;
    gOmmData->mario->state.poleObject = NULL;
    gOmmData->mario->state.health.state = OMM_HEALTH_STATE_NONE;
    gOmmData->mario->state.health.timer = 0;
    gOmmData->mario->state.freeze.dmg = 0;
    gOmmData->mario->state.freeze.gfx = 0;
    gOmmData->mario->state.freeze.sfx = 0;
    gOmmData->mario->state.previous.pos[0] = 0.f;
    gOmmData->mario->state.previous.pos[1] = 0.f;
    gOmmData->mario->state.previous.pos[2] = 0.f;
    gOmmData->mario->state.previous.vel[0] = 0.f;
    gOmmData->mario->state.previous.vel[1] = 0.f;
    gOmmData->mario->state.previous.vel[2] = 0.f;
    gOmmData->mario->state.previous.vel[3] = 0.f;
    gOmmData->mario->state.previous.angle[0] = 0;
    gOmmData->mario->state.previous.angle[1] = 0;
    gOmmData->mario->state.previous.angle[2] = 0;

    // Grab data
    gOmmData->mario->grab.obj = NULL;

    // Dialog data
    gOmmData->mario->dialog.obj = NULL;
    gOmmData->mario->dialog.state = 0;
    gOmmData->mario->dialog.id = -1;
    gOmmData->mario->dialog.turn = 0;
    gOmmData->mario->dialog.choice = 0;

    // Peach-only data
    gOmmData->mario->peach.floated = false;
    gOmmData->mario->peach.floatTimer = 0;
    gOmmData->mario->peach.vibeType = OMM_PEACH_VIBE_TYPE_NONE;
    gOmmData->mario->peach.vibeGauge = 0;
    gOmmData->mario->peach.vibeTimer = OMM_PEACH_VIBE_COOLDOWN;
    gOmmData->mario->peach.vibeGfxTimer = 0;
    gOmmData->mario->peach.joySpinYaw = 0;
    gOmmData->mario->peach.perryCharge = 0;
    gOmmData->mario->peach.perryBlast = false;
    gOmmData->mario->peach.perry = NULL;

    // Wall-slide moves data
    gOmmData->mario->wallSlide.jumped = false;
    gOmmData->mario->wallSlide.angle = 0;
    gOmmData->mario->wallSlide.height = CELL_HEIGHT_LIMIT;

    // Spin moves data
    gOmmData->mario->spin.pressed = false;
    gOmmData->mario->spin.yaw = 0;
    gOmmData->mario->spin.timer = 0;
    gOmmData->mario->spin._buffer = 0;
    gOmmData->mario->spin._counter = 0;
    gOmmData->mario->spin._checkpoint = -1;
    gOmmData->mario->spin._direction = 0;

    // Midair spin move data
    gOmmData->mario->midairSpin.timer = 0;
    gOmmData->mario->midairSpin.counter = 0;

    // Metal-water moves data
    gOmmData->mario->metalWater.punchType = 0;
    gOmmData->mario->metalWater.jumpNext = ACT_OMM_METAL_WATER_JUMP;
    gOmmData->mario->metalWater.jumpTimer = 0;

    // Cappy data
    gOmmData->mario->cappy.bounced = false;
    gOmmData->mario->cappy.cappy = NULL;

    // Capture data
    gOmmData->mario->capture.obj = NULL;
    gOmmData->mario->capture.prev = NULL;
    gOmmData->mario->capture.animPos[0][0] = 0.f;
    gOmmData->mario->capture.animPos[0][1] = 0.f;
    gOmmData->mario->capture.animPos[0][2] = 0.f;
    gOmmData->mario->capture.animPos[1][0] = 0.f;
    gOmmData->mario->capture.animPos[1][1] = 0.f;
    gOmmData->mario->capture.animPos[1][2] = 0.f;
    gOmmData->mario->capture.animPos[2][0] = 0.f;
    gOmmData->mario->capture.animPos[2][1] = 0.f;
    gOmmData->mario->capture.animPos[2][2] = 0.f;
    gOmmData->mario->capture.timer = 0;
    gOmmData->mario->capture.lockTimer = 0;
    gOmmData->mario->capture.starDance = false;
    gOmmData->mario->capture.openDoors = false;
    gOmmData->mario->capture.firstPerson = false;
    gOmmData->mario->capture.stickX = 0.f;
    gOmmData->mario->capture.stickY = 0.f;
    gOmmData->mario->capture.stickMag = 0.f;
    gOmmData->mario->capture.stickYaw = 0;
    gOmmData->mario->capture.buttonPressed = 0;
    gOmmData->mario->capture.buttonDown = 0;
#if OMM_CODE_DEBUG
    gOmmData->mario->capture.hitboxRadius = 0.f;
    gOmmData->mario->capture.hitboxHeight = 0.f;
    gOmmData->mario->capture.hitboxWall = 0.f;
#endif
}

static void omm_data_reset_object() {

    // State data
    gOmmData->object->state.actionFlag = false;
    gOmmData->object->state.actionState = 0;
    gOmmData->object->state.actionTimer = 0;
    gOmmData->object->state.walkDistance = 0.f;
    gOmmData->object->state.friction[0] = 0.f;
    gOmmData->object->state.friction[1] = 0.f;
    gOmmData->object->state.friction[2] = 0.f;
    gOmmData->object->state.heldObj = NULL;
    gOmmData->object->state._properties = 0;
    gOmmData->object->state._invincTimer = 0;
    gOmmData->object->state._cannonTimer = 0;
    gOmmData->object->state._bullyTimer = 0;
    gOmmData->object->state._powerTimer = 0;
    gOmmData->object->state._squishTimer = 0;
    gOmmData->object->state._squishScale[0] = 0.f;
    gOmmData->object->state._squishScale[1] = 0.f;
    gOmmData->object->state._squishScale[2] = 0.f;
    gOmmData->object->state._initialHome[0] = 0.f;
    gOmmData->object->state._initialHome[1] = 0.f;
    gOmmData->object->state._initialHome[2] = 0.f;
    gOmmData->object->state._camBehindMario = false;

    // Cappy data
    gOmmData->object->cappy.object = NULL;
    gOmmData->object->cappy.tra_x = 0.f;
    gOmmData->object->cappy.tra_y = 0.f;
    gOmmData->object->cappy.tra_z = 0.f;
    gOmmData->object->cappy.rot_x = 0;
    gOmmData->object->cappy.rot_y = 0;
    gOmmData->object->cappy.rot_z = 0;
    gOmmData->object->cappy.scale = 0.f;
    gOmmData->object->cappy.o_gfx = false;
    gOmmData->object->cappy.post_update = NULL;

    // Door data
    gOmmData->object->door.vel[0][0] = 0.f;
    gOmmData->object->door.vel[0][1] = 0.f;
    gOmmData->object->door.vel[1][0] = 0.f;
    gOmmData->object->door.vel[1][1] = 0.f;
    gOmmData->object->door.timer = 0;
}

static void omm_data_reset_level() {
    gRedCoinsCollected = 0;
    for (s32 areaIndex = 0; areaIndex != 8; ++areaIndex) {
        gOmmData->level[areaIndex].redCoins = 0;
        gOmmData->level[areaIndex].redCoinStarIndex = -1;
        gOmmData->level[areaIndex].secrets = 0;
        gOmmData->level[areaIndex].secretStarIndex = -1;
    }
}

OMM_ROUTINE_LEVEL_ENTRY(omm_data_reset) {
    omm_data_reset_mario();
    omm_data_reset_object();
    omm_data_reset_level();
}

//
// Init
//

OMM_AT_STARTUP void omm_data_init() {
    OMM_DO_ONCE {
        gOmmData->reset = omm_data_reset;
        gOmmData->reset_mario = omm_data_reset_mario;
        gOmmData->reset_object = omm_data_reset_object;
        gOmmData->read_stats = omm_data_read_stats;
        gOmmData->write_stats = omm_data_write_stats;
        gOmmData->reset();
        gOmmData->globals->cameraSnapshotMode = false;
        gOmmData->globals->cameraUpdate = false;
        gOmmData->globals->cameraNoInit = false;
        gOmmData->globals->configNoSave = false;
        gOmmData->globals->findFloorForCutsceneStar = false;
        gOmmData->globals->hideHudCamera = false;
        gOmmData->globals->hideHudRadar = false;
        gOmmData->globals->isMirrorObj = false;
        gOmmData->globals->isMirrorRoom = false;
        gOmmData->globals->mirrorRoomX = 1700.f;
        gOmmData->globals->mirrorX = 4331.5f;
        gOmmData->globals->marioTimer = 0;
        gOmmData->globals->mouseDeltaX = 0;
        gOmmData->globals->mouseDeltaY = 0;
        gOmmData->globals->mouseWheelX = 0;
        gOmmData->globals->mouseWheelY = 0;
    }
}

//
// Update
//

static void omm_data_update_mario() {
    struct MarioState *m = gMarioState;

    // State data
    gOmmData->mario->state.previous.pos[0] = m->pos[0];
    gOmmData->mario->state.previous.pos[1] = m->pos[1];
    gOmmData->mario->state.previous.pos[2] = m->pos[2];
    gOmmData->mario->state.previous.vel[0] = m->vel[0];
    gOmmData->mario->state.previous.vel[1] = m->vel[1];
    gOmmData->mario->state.previous.vel[2] = m->vel[2];
    gOmmData->mario->state.previous.vel[3] = m->forwardVel;
    gOmmData->mario->state.previous.angle[0] = m->faceAngle[0];
    gOmmData->mario->state.previous.angle[1] = m->faceAngle[1];
    gOmmData->mario->state.previous.angle[2] = m->faceAngle[2];

    // Peach-only data
    if (!OMM_PLAYER_IS_PEACH) {
        gOmmData->mario->peach.floated = false;
        gOmmData->mario->peach.floatTimer = 0;
        gOmmData->mario->peach.vibeType = OMM_PEACH_VIBE_TYPE_NONE;
        gOmmData->mario->peach.vibeGauge = 0;
        gOmmData->mario->peach.vibeTimer = OMM_PEACH_VIBE_COOLDOWN;
        gOmmData->mario->peach.vibeGfxTimer = 0;
        gOmmData->mario->peach.joySpinYaw = 0;
    }

    // Perry data
    if (OMM_PERRY_SWORD_ACTION) {
        if (!omm_perry_get_object()) {
            gOmmData->mario->peach.perry = NULL;
        }
    } else {
        gOmmData->mario->peach.perryCharge = 0;
        gOmmData->mario->peach.perryBlast = false;
        gOmmData->mario->peach.perry = NULL;
    }

    // Cappy data
    if (!omm_cappy_get_object()) {
        gOmmData->mario->cappy.cappy = NULL;
    }

    // Capture data
    if (!omm_mario_is_capture(m)) {
        if (gOmmData->mario->capture.obj) {
            gOmmData->mario->capture.obj->oCaptureData = NULL;
            gOmmData->mario->capture.obj = NULL;
        }
        if (!(m->action & ACT_FLAG_AIR)) {
            gOmmData->mario->capture.prev = NULL;
        } else if (gOmmData->mario->capture.prev) {
            struct Object *o = gOmmData->mario->capture.prev;
            if (!o->activeFlags) {
                gOmmData->mario->capture.prev = NULL;
            } else {
                o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloor);
                if (obj_is_on_ground(o)) {
                    gOmmData->mario->capture.prev = NULL;
                }
            }
        }
    }

    // Warp data
    if (omm_is_main_menu() || omm_is_ending_cutscene()) {
        mem_zero(&gOmmData->mario->warp, sizeof(gOmmData->mario->warp));
    }
}

static void omm_data_update_object() {
}

static void omm_data_update_stats() {
    if (gMarioObject && !omm_is_game_paused()) {
        struct MarioState *m = gMarioState;

        // A presses
        gOmmStats->aPresses += (gPlayer1Controller->buttonPressed & A_BUTTON) != 0;

        // Actions
        // Only checks Mario's action; in SMO, capture actions (jumps) don't count as Mario actions
        static u32 sPrevAction = 0;
        if (m->action != sPrevAction) {
            gOmmStats->jumps += omm_mario_is_jumping(m);
            gOmmStats->attacks += omm_mario_is_attacking(m);
        }
        sPrevAction = m->action;

        // Timers
        if (omm_mario_is_capture(m) && gOmmCapture) {
            gOmmStats->timeTotal[1]      += 1;
            gOmmStats->timeOnGround[1]   += obj_is_on_ground(gOmmCapture);
            gOmmStats->timeAirborne[1]   += !obj_is_on_ground(gOmmCapture);
            gOmmStats->timeUnderwater[1] += obj_is_underwater(gOmmCapture, find_water_level(gOmmCapture->oPosX, gOmmCapture->oPosZ));
            gOmmStats->timeWingCap[1]    += (m->flags & MARIO_WING_CAP) != 0;
            gOmmStats->timeMetalCap[1]   += (m->flags & MARIO_METAL_CAP) != 0;
            gOmmStats->timeVanishCap[1]  += (m->flags & MARIO_VANISH_CAP) != 0;
        } else {
            bool underwater = (m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) != 0;
            bool airborne = (m->action & ACT_FLAG_AIR) != 0;
            gOmmStats->timeTotal[0]      += 1;
            gOmmStats->timeOnGround[0]   += !underwater && !airborne;
            gOmmStats->timeAirborne[0]   += !underwater && airborne;
            gOmmStats->timeUnderwater[0] += underwater;
            gOmmStats->timeWingCap[0]    += (m->flags & MARIO_WING_CAP) != 0;
            gOmmStats->timeMetalCap[0]   += (m->flags & MARIO_METAL_CAP) != 0;
            gOmmStats->timeVanishCap[0]  += (m->flags & MARIO_VANISH_CAP) != 0;
        }
    }
}

static void omm_data_update_globals() {
    gMouseDeltaX = gOmmGlobals->mouseDeltaX * (BETTER_CAM_IS_PUPPY_CAM ? 1 : -1);
    gMouseDeltaY = gOmmGlobals->mouseDeltaY * (BETTER_CAM_IS_PUPPY_CAM ? 1 : -1);
    if (gOmmGlobals->marioTimer) {
        gPlayer1Controller->buttonPressed &= ~START_BUTTON;
        gPlayer2Controller->buttonPressed &= ~START_BUTTON;
        gPlayer3Controller->buttonPressed &= ~START_BUTTON;
    }
}

OMM_ROUTINE_UPDATE(omm_data_update) {
    omm_data_update_mario();
    omm_data_update_object();
    omm_data_update_stats();
    omm_data_update_globals();
}
