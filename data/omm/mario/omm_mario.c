#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#undef update_mario_platform
#include "game/platform_displacement.h"

//
// Actions
//

bool omm_mario_set_action(struct MarioState *m, u32 action, u32 actionArg, u16 buttons) {
    if (action != 0) set_mario_action(m, action, actionArg);
    m->controller->buttonPressed     &= ~buttons;
    if (buttons & A_BUTTON) m->input &= ~INPUT_A_PRESSED;
    if (buttons & B_BUTTON) m->input &= ~INPUT_B_PRESSED;
    if (buttons & Z_TRIG)   m->input &= ~INPUT_Z_PRESSED;
    return true;
}

bool omm_mario_throw_cappy(struct MarioState *m, u32 action, u32 actionArg, u16 buttons) {
    if (!omm_cappy_spawn(m)) return false;
    return omm_mario_set_action(m, action, actionArg, buttons);
}

void omm_mario_init_next_action(struct MarioState *m) {
    bool setYaw = false;
    bool updateGfx = true;
    bool initSpin = false;
    switch (m->action) {
        case ACT_DIVE: setYaw = (m->prevAction == ACT_GROUND_POUND || m->prevAction == ACT_OMM_SPIN_POUND); break;
        case ACT_FLYING: updateGfx = false; break;
        case ACT_WATER_IDLE: updateGfx = false; break;
        case ACT_WATER_ACTION_END: updateGfx = false; break;
        case ACT_SWIMMING_END: updateGfx = false; break;
        case ACT_BREASTSTROKE: updateGfx = false; break;
        case ACT_FLUTTER_KICK: updateGfx = false; break;
        case ACT_HOLD_WATER_IDLE: updateGfx = false; break;
        case ACT_HOLD_WATER_ACTION_END: updateGfx = false; break;
        case ACT_HOLD_SWIMMING_END: updateGfx = false; break;
        case ACT_HOLD_BREASTSTROKE: updateGfx = false; break;
        case ACT_HOLD_FLUTTER_KICK: updateGfx = false; break;
        case ACT_WATER_THROW: updateGfx = false; break;
        case ACT_WATER_PUNCH: updateGfx = false; break;
        case ACT_OMM_ROLL: setYaw = (m->forwardVel < OMM_MARIO_ROLL_MIN_SPEED); updateGfx = false; break;
        case ACT_OMM_ROLL_AIR: updateGfx = false; break;
        case ACT_OMM_SPIN_GROUND: setYaw = true; initSpin = true; break;
        case ACT_OMM_SPIN_AIR: initSpin = true; break;
        case ACT_OMM_SPIN_JUMP: setYaw = true; break;
        case ACT_OMM_GROUND_POUND_JUMP: setYaw = true; break;
        case ACT_OMM_WATER_GROUND_POUND: updateGfx = false; break;
        case ACT_OMM_WATER_DASH: updateGfx = false; break;
        case ACT_OMM_CAPPY_THROW_GROUND: setYaw = true; break;
        case ACT_OMM_CAPPY_THROW_AIRBORNE: setYaw = true; break;
        case ACT_OMM_CAPPY_THROW_WATER: updateGfx = false; break;
        case ACT_OMM_METAL_WATER_DIVE: setYaw = (m->prevAction == ACT_OMM_METAL_WATER_GROUND_POUND || m->prevAction == ACT_OMM_METAL_WATER_SPIN_POUND); break;
        case ACT_OMM_METAL_WATER_SPIN_GROUND: setYaw = true; initSpin = true; break;
        case ACT_OMM_METAL_WATER_SPIN_AIR: initSpin = true; break;
        case ACT_OMM_METAL_WATER_SPIN_JUMP: setYaw = true; break;
        case ACT_OMM_METAL_WATER_GROUND_POUND_JUMP: setYaw = true; break;
        case ACT_OMM_METAL_WATER_CAPPY_THROW_GROUND: setYaw = true; break;
        case ACT_OMM_METAL_WATER_CAPPY_THROW_AIRBORNE: setYaw = true; break;
        case ACT_OMM_PEACH_VIBE_JOY_ATTACK: setYaw = true; break;
        default: setYaw = (m->prevAction == ACT_OMM_WATER_GROUND_POUND_JUMP);
    }

    // Set Mario's facing direction
    if (setYaw && m->controller->stickMag > 32.f) {
        m->faceAngle[1] = m->intendedYaw;
    }

    // Update Mario's graphics
    if (updateGfx) {
        vec3s_set(m->faceAngle, 0, m->faceAngle[1], 0);
        vec3s_set(m->marioObj->oGfxAngle, 0, m->faceAngle[1], 0);
    }

    // Init spin
    gOmmMario->spin.timer = initSpin * OMM_MARIO_SPIN_DURATION;
    gOmmMario->spin.yaw = 0;
}

bool omm_mario_check_wall_slide(struct MarioState *m) {
    return OMM_MOVESET_ODYSSEY &&
           (m->action != ACT_DIVE) &&
           (m->action != ACT_LONG_JUMP) &&
           (m->action != ACT_OMM_METAL_WATER_DIVE) &&
           (m->action != ACT_OMM_METAL_WATER_LONG_JUMP) &&
           (m->heldObj == NULL) &&
           (m->wall != NULL);
}

bool omm_mario_can_perform_wall_slide(struct MarioState *m) {
    if (m->vel[1] < 0.f && m->forwardVel > 8.f && m->wall->type < SURFACE_PAINTING_WOBBLE_A6) {
        s16 wallAngle = atan2s(m->wall->normal.z, m->wall->normal.x);
        f32 wallHeight = m->pos[1];
        if (gOmmMario->wallSlide.jumped) {
            u16 diffAngle = (u16) abs_s((s32) wallAngle - (s32) gOmmMario->wallSlide.angle);
            f32 diffHeight = wallHeight - gOmmMario->wallSlide.height;
            if (diffAngle < 0x2000 && diffHeight > 0) {
                return false;
            }
        }
        gOmmMario->wallSlide.angle = wallAngle;
        gOmmMario->wallSlide.height = wallHeight;
        return true;
    }
    return false;
}

bool omm_mario_try_to_perform_wall_slide(struct MarioState *m) {
    if (omm_mario_check_wall_slide(m)) {
        if (omm_mario_can_perform_wall_slide(m)) {
            omm_mario_set_action(m, ACT_OMM_WALL_SLIDE, 0, 0);
        }
        return true;
    }
    return false; 
}

bool omm_mario_check_and_perform_wall_slide(struct MarioState *m) {
    return omm_mario_check_wall_slide(m) &&
           omm_mario_can_perform_wall_slide(m) &&
           omm_mario_set_action(m, ACT_OMM_WALL_SLIDE, 0, 0);
}

bool omm_mario_should_stop_wall_slide(struct MarioState *m) {
    return OMM_MOVESET_CLASSIC ||
           (m->wall == NULL) ||
           (m->wall->type >= SURFACE_PAINTING_WOBBLE_A6) ||
           (m->action != ACT_OMM_WALL_SLIDE && m->action != ACT_OMM_METAL_WATER_WALL_SLIDE);
}

bool omm_mario_is_idling(struct MarioState *m) {
    return omm_mario_is_capture(m) ||
           (m->action == ACT_IDLE) ||
           (m->action == ACT_START_SLEEPING) ||
           (m->action == ACT_SLEEPING) ||
           (m->action == ACT_WAKING_UP) ||
           (m->action == ACT_PANTING) ||
           (m->action == ACT_HOLD_PANTING_UNUSED) ||
           (m->action == ACT_HOLD_IDLE) ||
           (m->action == ACT_HOLD_HEAVY_IDLE) ||
           (m->action == ACT_STANDING_AGAINST_WALL) ||
           (m->action == ACT_SHIVERING) ||
           (m->action == ACT_FIRST_PERSON) ||
           (m->action == ACT_WATER_IDLE) ||
           (m->action == ACT_HOLD_WATER_IDLE) ||
           (m->action == ACT_METAL_WATER_STANDING) ||
           (m->action == ACT_HOLD_METAL_WATER_STANDING) ||
           (m->action == ACT_READING_AUTOMATIC_DIALOG) ||
           (m->action == ACT_READING_NPC_DIALOG) ||
           (m->action == ACT_READING_SIGN) ||
           (m->action == ACT_TELEPORT_FADE_OUT) ||
           (m->action == ACT_TELEPORT_FADE_IN) ||
           (m->action == ACT_HOLDING_POLE) ||
           (m->action == ACT_TOP_OF_POLE) ||
           (m->action == ACT_LEDGE_GRAB) ||
           (m->action == ACT_OMM_METAL_WATER_IDLE);
}

bool omm_mario_is_jumping(struct MarioState *m) {
    return
#if OMM_GAME_IS_R96X
           (m->action == ACT_WARIO_TRIPLE_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_WARIO_TRIPLE_JUMP) ||
#endif
           (m->action == ACT_JUMP) ||
           (m->action == ACT_DOUBLE_JUMP) ||
           (m->action == ACT_TRIPLE_JUMP) ||
           (m->action == ACT_BACKFLIP) ||
           (m->action == ACT_STEEP_JUMP) ||
           (m->action == ACT_WALL_KICK_AIR) ||
           (m->action == ACT_SIDE_FLIP) ||
           (m->action == ACT_LONG_JUMP) ||
           (m->action == ACT_WATER_JUMP) ||
           (m->action == ACT_TOP_OF_POLE_JUMP) ||
           (m->action == ACT_FLYING_TRIPLE_JUMP) ||
           (m->action == ACT_RIDING_SHELL_JUMP) ||
           (m->action == ACT_HOLD_JUMP) ||
           (m->action == ACT_HOLD_WATER_JUMP) ||
           (m->action == ACT_SPECIAL_TRIPLE_JUMP) ||
           (m->action == ACT_BURNING_JUMP) ||
           (m->action == ACT_FORWARD_ROLLOUT) || // it does count as a jump in SMO
           (m->action == ACT_BACKWARD_ROLLOUT) || // it does count as a jump in SMO
           (m->action == ACT_METAL_WATER_JUMP) ||
           (m->action == ACT_HOLD_METAL_WATER_JUMP) ||
           (m->action == ACT_OMM_GROUND_POUND_JUMP) ||
           (m->action == ACT_OMM_SPIN_JUMP) ||
           (m->action == ACT_OMM_WATER_GROUND_POUND_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_DOUBLE_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_TRIPLE_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_BACKFLIP) ||
           (m->action == ACT_OMM_METAL_WATER_SIDE_FLIP) ||
           (m->action == ACT_OMM_METAL_WATER_LONG_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_WALL_KICK_AIR) ||
           (m->action == ACT_OMM_METAL_WATER_GROUND_POUND_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_SPIN_JUMP) ||
           (m->action == ACT_OMM_METAL_WATER_HOLD_JUMP);
}

bool omm_mario_is_punching(struct MarioState *m) {
    return (m->action == ACT_PUNCHING) ||
           (m->action == ACT_MOVE_PUNCHING) ||
           (m->action == ACT_OMM_METAL_WATER_PUNCHING) ||
           (m->action == ACT_OMM_PEACH_ATTACK_GROUND);
}

bool omm_mario_is_kicking(struct MarioState *m) {
    return (m->action == ACT_JUMP_KICK) ||
           (m->action == ACT_OMM_METAL_WATER_JUMP_KICK) ||
           (m->action == ACT_OMM_PEACH_ATTACK_AIR);
}

bool omm_mario_is_attacking(struct MarioState *m) {
    return
#if OMM_GAME_IS_R96X
           (m->action == ACT_WARIO_CHARGE) ||
           (m->action == ACT_WARIO_PILE_DRIVER) ||
           (m->action == ACT_OMM_METAL_WATER_WARIO_CHARGE) ||
#endif
           (m->action == ACT_MOVE_PUNCHING) ||
           (m->action == ACT_DIVE) ||
           (m->action == ACT_GROUND_POUND) ||
           (m->action == ACT_SLIDE_KICK) ||
           (m->action == ACT_JUMP_KICK) ||
           (m->action == ACT_WATER_PUNCH) ||
           (m->action == ACT_PUNCHING) ||
           (m->action == ACT_OMM_CAPPY_THROW_GROUND) ||
           (m->action == ACT_OMM_CAPPY_THROW_AIRBORNE) ||
           (m->action == ACT_OMM_SPIN_POUND) ||
           (m->action == ACT_OMM_WATER_GROUND_POUND) ||
           (m->action == ACT_OMM_CAPPY_THROW_WATER) ||
           (m->action == ACT_OMM_METAL_WATER_PUNCHING) ||
           (m->action == ACT_OMM_METAL_WATER_JUMP_KICK) ||
           (m->action == ACT_OMM_METAL_WATER_DIVE) ||
           (m->action == ACT_OMM_METAL_WATER_GROUND_POUND) ||
           (m->action == ACT_OMM_METAL_WATER_CAPPY_THROW_GROUND) ||
           (m->action == ACT_OMM_METAL_WATER_CAPPY_THROW_AIRBORNE) ||
           (m->action == ACT_OMM_METAL_WATER_SPIN_POUND) ||
           (m->action == ACT_OMM_PEACH_ATTACK_GROUND) ||
           (m->action == ACT_OMM_PEACH_ATTACK_FAST) ||
           (m->action == ACT_OMM_PEACH_ATTACK_AIR) ||
           (m->action == ACT_OMM_PEACH_VIBE_JOY_ATTACK);
}

bool omm_mario_is_ground_pounding(struct MarioState *m) {
    return (m->action == ACT_GROUND_POUND) ||
#if OMM_GAME_IS_R96X
           (m->action == ACT_WARIO_PILE_DRIVER) ||
#endif
           (m->action == ACT_OMM_SPIN_POUND) ||
           (m->action == ACT_OMM_WATER_GROUND_POUND) ||
           (m->action == ACT_OMM_METAL_WATER_GROUND_POUND) ||
           (m->action == ACT_OMM_METAL_WATER_SPIN_POUND) ||
           (omm_mario_is_capture(m) && POBJ_IS_GROUND_POUNDING);
}

bool omm_mario_is_ground_pound_landing(struct MarioState *m) {
    return (m->action == ACT_GROUND_POUND_LAND) ||
#if OMM_GAME_IS_R96X
           (m->action == ACT_WARIO_PILE_DRIVER_LAND) ||
#endif
           (m->action == ACT_OMM_SPIN_POUND_LAND) ||
           (m->action == ACT_OMM_WATER_GROUND_POUND_LAND) ||
           (m->action == ACT_OMM_METAL_WATER_GROUND_POUND_LAND) ||
           (m->action == ACT_OMM_METAL_WATER_GROUND_POUND_LAND_STOP) ||
           (m->action == ACT_OMM_METAL_WATER_SPIN_POUND_LAND) ||
           (omm_mario_is_capture(m) && POBJ_IS_GROUND_POUNDING && obj_is_on_ground(gOmmCapture));
}

bool omm_mario_is_roll_landing(struct MarioState *m) {
    return OMM_MOVESET_ODYSSEY &&
           (m->controller->buttonDown & Z_TRIG) &&
           (m->forwardVel >= OMM_MARIO_ROLL_MIN_SPEED) && (
           (m->action == ACT_BUTT_SLIDE) ||
           (m->action == ACT_DIVE_SLIDE) ||
           (m->action == ACT_STOMACH_SLIDE) ||
           (m->action == ACT_LONG_JUMP_LAND));
}

bool omm_mario_is_reading(struct MarioState *m) {
    return (m->action == ACT_READING_SIGN) ||
           (m->action == ACT_READING_NPC_DIALOG) ||
           (m->action == ACT_READING_AUTOMATIC_DIALOG);
}

bool omm_mario_is_picking_up(struct MarioState *m) {
    return (m->action == ACT_PICKING_UP) ||
           (m->action == ACT_DIVE_PICKING_UP) ||
           (m->action == ACT_PICKING_UP_BOWSER) ||
           (m->action == ACT_OMM_GRAB_BOWSER);
}

bool omm_mario_is_holding(struct MarioState *m) {
    return (m->action == ACT_HOLD_PANTING_UNUSED) ||
           (m->action == ACT_HOLD_IDLE) ||
           (m->action == ACT_HOLD_HEAVY_IDLE) ||
           (m->action == ACT_HOLD_JUMP_LAND_STOP) ||
           (m->action == ACT_HOLD_FREEFALL_LAND_STOP) ||
           (m->action == ACT_HOLD_BUTT_SLIDE_STOP) ||
           (m->action == ACT_HOLD_WALKING) ||
           (m->action == ACT_HOLD_HEAVY_WALKING) ||
           (m->action == ACT_HOLD_DECELERATING) ||
           (m->action == ACT_HOLD_BEGIN_SLIDING) ||
           (m->action == ACT_HOLD_BUTT_SLIDE) ||
           (m->action == ACT_HOLD_STOMACH_SLIDE) ||
           (m->action == ACT_HOLD_JUMP_LAND) ||
           (m->action == ACT_HOLD_FREEFALL_LAND) ||
           (m->action == ACT_HOLD_QUICKSAND_JUMP_LAND) ||
           (m->action == ACT_HOLD_JUMP) ||
           (m->action == ACT_HOLD_FREEFALL) ||
           (m->action == ACT_HOLD_BUTT_SLIDE_AIR) ||
           (m->action == ACT_HOLD_WATER_JUMP) ||
           (m->action == ACT_HOLD_WATER_IDLE) ||
           (m->action == ACT_HOLD_WATER_ACTION_END) ||
           (m->action == ACT_HOLD_BREASTSTROKE) ||
           (m->action == ACT_HOLD_SWIMMING_END) ||
           (m->action == ACT_HOLD_FLUTTER_KICK) ||
           (m->action == ACT_HOLD_METAL_WATER_STANDING) ||
           (m->action == ACT_HOLD_METAL_WATER_WALKING) ||
           (m->action == ACT_HOLD_METAL_WATER_FALLING) ||
           (m->action == ACT_HOLD_METAL_WATER_FALL_LAND) ||
           (m->action == ACT_HOLD_METAL_WATER_JUMP) ||
           (m->action == ACT_HOLD_METAL_WATER_JUMP_LAND) ||
           (m->action == ACT_HOLDING_POLE) ||
           (m->action == ACT_HOLDING_BOWSER) ||
           (m->action == ACT_OMM_HOLD_BOWSER);
}

bool omm_mario_is_hanging(struct MarioState *m) {
    return (m->action == ACT_START_HANGING) ||
           (m->action == ACT_HANGING) ||
           (m->action == ACT_HANG_MOVING);
}

bool omm_mario_is_burning(struct MarioState *m) {
    return (m->action == ACT_BURNING_GROUND) ||
           (m->action == ACT_BURNING_JUMP) ||
           (m->action == ACT_BURNING_FALL);
}

bool omm_mario_is_sliding(struct MarioState *m) {
    return (m->action == ACT_BEGIN_SLIDING) ||
           (m->action == ACT_HOLD_BEGIN_SLIDING) ||
           (m->action == ACT_BUTT_SLIDE) ||
           (m->action == ACT_STOMACH_SLIDE) ||
           (m->action == ACT_HOLD_BUTT_SLIDE) ||
           (m->action == ACT_HOLD_STOMACH_SLIDE) ||
           (m->action == ACT_DIVE_SLIDE) ||
           (m->action == ACT_CROUCH_SLIDE) ||
           (m->action == ACT_SLIDE_KICK_SLIDE);
}

bool omm_mario_is_grabbed(struct MarioState *m) {
    return (m->action == ACT_GRABBED) ||
           (m->action == ACT_OMM_GRABBED_WATER);
}

bool omm_mario_is_stuck_in_ground_after_fall(struct MarioState *m) {
    u16 terrainType = (m->area->terrainType & TERRAIN_MASK);
    return (terrainType == TERRAIN_SNOW || terrainType == TERRAIN_SAND) &&
           (m->floor != NULL) &&
           (m->floor->type != SURFACE_BURNING) &&
           (m->floor->type != SURFACE_HARD) &&
           (m->floor->type != SURFACE_HARD_SLIPPERY) &&
           (m->floor->type != SURFACE_HARD_NOT_SLIPPERY) &&
           (m->floor->type != SURFACE_HARD_VERY_SLIPPERY) &&
          !(m->floor->flags & SURFACE_FLAG_DYNAMIC) &&
           (m->floor->normal.y >= 0.866f); // ~30 deg
}

bool omm_mario_is_star_dancing(struct MarioState *m) {
    return (omm_mario_is_capture(m) && POBJ_IS_STAR_DANCING) ||
           (m->action == ACT_STAR_DANCE_EXIT) ||
           (m->action == ACT_STAR_DANCE_NO_EXIT) ||
           (m->action == ACT_STAR_DANCE_WATER) ||
           (m->action == ACT_FALL_AFTER_STAR_GRAB) ||
           (m->action == ACT_OMM_STAR_DANCE) ||
           (m->action == ACT_OMM_SPARKLY_STAR_DANCE);
}

bool omm_mario_is_emerging_from_pipe(struct MarioState *m) {
    return (omm_mario_is_capture(m) && POBJ_IS_EMERGING_FROM_PIPE) ||
           (m->action == ACT_EMERGE_FROM_PIPE);
}

bool omm_mario_is_ready_for_dialog(struct MarioState *m) {
    return (m->action == ACT_IDLE) ||
           (m->action == ACT_START_SLEEPING) ||
           (m->action == ACT_SLEEPING) ||
           (m->action == ACT_WAKING_UP) ||
           (m->action == ACT_PANTING) ||
           (m->action == ACT_HOLD_PANTING_UNUSED) ||
           (m->action == ACT_HOLD_IDLE) ||
           (m->action == ACT_HOLD_HEAVY_IDLE) ||
           (m->action == ACT_STANDING_AGAINST_WALL) ||
           (m->action == ACT_SHIVERING) ||
           (m->action == ACT_WALKING);
}

bool omm_mario_is_ready_to_speak(struct MarioState *m) {
    u32 actionGroup = (m->action & ACT_GROUP_MASK);
    return omm_mario_is_capture(m) || 
           (m->action == ACT_WAITING_FOR_DIALOG) || (
           (m->action != ACT_FIRST_PERSON) &&
           (m->action & ACT_FLAG_RIDING_SHELL) == 0 &&
           (m->action & ACT_FLAG_INVULNERABLE) == 0 && (
           (actionGroup == ACT_GROUP_STATIONARY) ||
           (actionGroup == ACT_GROUP_MOVING)));
}

bool omm_mario_is_capture(struct MarioState *m) {
    return (gOmmCapture != NULL) && (
           (m->action == ACT_OMM_POSSESSION) ||
           (m->action == ACT_OMM_POSSESSION_UNDERWATER));
}

bool omm_mario_allow_first_person(struct MarioState *m) {
    if (omm_mario_is_capture(m)) {
        struct Object *o = gOmmCapture;
        return !POBJ_IS_WALKING &&
               !POBJ_IS_STAR_DANCING &&
               !POBJ_IS_OPENING_DOORS &&
               !omm_mario_is_locked(m) &&
                obj_is_on_ground(o);
    }
    return (m->action & ACT_FLAG_ALLOW_FIRST_PERSON) != 0;
}

bool omm_mario_should_walk(struct MarioState *m) {
    return OMM_PLAYER_IS_PEACH || (max_3_f(4.f, m->intendedMag, m->forwardVel) < 18.f);
}

bool omm_mario_should_run(struct MarioState *m) {
    return !OMM_PLAYER_IS_PEACH && (max_3_f(4.f, m->intendedMag, m->forwardVel) > 22.f);
}

bool omm_mario_has_wing_cap(struct MarioState *m) {
    return OMM_POWER_UPS_IMPROVED && (!omm_mario_is_capture(m) || omm_capture_get_type(gOmmCapture) == OMM_CAPTURE_YOSHI) && (m->flags & MARIO_WING_CAP);
}

bool omm_mario_has_vanish_cap(struct MarioState *m) {
    return OMM_POWER_UPS_IMPROVED && (!omm_mario_is_capture(m) || omm_capture_get_type(gOmmCapture) == OMM_CAPTURE_YOSHI) && (m->flags & MARIO_VANISH_CAP);
}

bool omm_mario_has_metal_cap(struct MarioState *m) {
    return OMM_POWER_UPS_IMPROVED && (!omm_mario_is_capture(m) || omm_capture_get_type(gOmmCapture) == OMM_CAPTURE_YOSHI) && (m->flags & MARIO_METAL_CAP);
}

bool omm_mario_cap_is_flickering(struct MarioState *m) {
    return m->capTimer > 0 && m->capTimer < 0x40 && ((1llu << m->capTimer) & sCapFlickerFrames) != 0;
}

bool omm_mario_is_dead(struct MarioState *m) {
    return m->health <= OMM_HEALTH_DEAD;
}

bool omm_mario_check_dead(struct MarioState *m, s32 health) {
    if (gOmmGlobals->marioTimer) {
        m->health = (OMM_MOVESET_ODYSSEY || gGlobalTimer - gOmmGlobals->marioTimer > 30 ? OMM_HEALTH_DEAD : m->health);
        m->hurtCounter = 30 * OMM_MOVESET_CLASSIC;
        m->healCounter = 0;
        m->capTimer = 1;
        return false;
    }

    // Don't trigger Odyssey death outside of courses
    if (OMM_LEVEL_NO_WARP(gCurrLevelNum)) {
        return false;
    }

    // Anti-softlock
    if (m->action == ACT_OMM_DEATH ||
        m->action == ACT_OMM_DEATH_WATER ||
        m->action == ACT_OMM_DEATH_FALL ||
        m->action == ACT_OMM_DEATH_FROZEN ||
        m->action == ACT_OMM_DEATH_EATEN_BY_BUBBA ||
        m->action == ACT_OMM_DEATH_QUICKSAND ||
        m->action == ACT_OMM_DEATH_SQUISHED) {
        return false;
    }

    // Non-Stop mode only
    if (OMM_STARS_CLASSIC) {
        return false;
    }

    // SM64 Death actions
    u32 act_omm_death_water = (omm_world_is_cold() ? ACT_OMM_DEATH_FROZEN : ACT_OMM_DEATH_WATER);
    switch (m->action) {
        case ACT_STANDING_DEATH:    omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);                  return true;
        case ACT_ELECTROCUTION:     omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);                  return true;
        case ACT_SUFFOCATION:       omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);                  return true;
        case ACT_DEATH_ON_BACK:     omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);                  return true;
        case ACT_DEATH_ON_STOMACH:  omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);                  return true;
        case ACT_QUICKSAND_DEATH:   omm_mario_set_action(m, ACT_OMM_DEATH_QUICKSAND, 0, 0xFFFF);        return true;
        case ACT_EATEN_BY_BUBBA:    omm_mario_set_action(m, ACT_OMM_DEATH_EATEN_BY_BUBBA, 0, 0xFFFF);   return true;
        case ACT_DROWNING:          omm_mario_set_action(m, act_omm_death_water, 0, 0xFFFF);            return true;
        case ACT_WATER_DEATH:       omm_mario_set_action(m, act_omm_death_water, 0, 0xFFFF);            return true;
    }

    // Default: Mario's health <= OMM_HEALTH_DEAD
    if (health <= OMM_HEALTH_DEAD) {
        omm_mario_unpossess_object(m, OMM_MARIO_UNPOSSESS_ACT_NONE, 0);
        if (m->squishTimer == 0xFF) {
            omm_mario_set_action(m, ACT_OMM_DEATH_SQUISHED, 0, 0xFFFF);
        } else if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED) {
            omm_mario_set_action(m, act_omm_death_water, 0, 0xFFFF);
        } else {
            omm_mario_set_action(m, ACT_OMM_DEATH, 0, 0xFFFF);
        }
        return true;
    }
    return false;
}

bool omm_mario_check_death_warp(struct MarioState *m, s32 warpOp) {
    if (warpOp == WARP_OP_STAR_EXIT) {
        omm_speedrun_split(OMM_SPLIT_EXIT);
    } else if (OMM_STARS_NON_STOP && !OMM_LEVEL_NO_WARP(gCurrLevelNum) && !gOmmGlobals->marioTimer) {
        switch (warpOp) {
            case WARP_OP_DEATH: {
                return omm_mario_check_dead(m, OMM_HEALTH_DEAD);
            } break;

            case WARP_OP_WARP_FLOOR: {
                if (!OMM_CHEAT_WALK_ON_DEATH_BARRIER && m->pos[1] < m->floorHeight + 2048.f && m->floor && SURFACE_IS_DEATH_PLANE(m->floor->type)) {
                    omm_mario_set_action(m, ACT_OMM_DEATH_FALL, 0, 0);
                    return true;
                }
            } break;
        }
    }
    return false;
}

const bool IS_BOWSER_INTERACTABLE[] = {
    true,  // bowser_act_default
    true,  // bowser_act_thrown_dropped
    false, // bowser_act_jump_onto_stage
    true,  // bowser_act_dance
    false, // bowser_act_dead
    false, // bowser_act_text_wait
    false, // bowser_act_intro_walk
    true,  // bowser_act_charge_mario
    true,  // bowser_act_spit_fire_into_sky
    true,  // bowser_act_spit_fire_onto_floor
    true,  // bowser_act_hit_edge
    true,  // bowser_act_turn_from_edge
    true,  // bowser_act_hit_mine
    true,  // bowser_act_jump
    true,  // bowser_act_walk_to_mario
    true,  // bowser_act_breath_fire
    false, // bowser_act_teleport
    true,  // bowser_act_jump_towards_mario
    true,  // bowser_act_unused_slow_walk
    false, // bowser_act_ride_tilting_platform
};
bool omm_mario_check_grab(struct MarioState *m, struct Object *o, bool ignoreAngles) {
    if (!(o->oInteractionSubtype & INT_SUBTYPE_NOT_GRABBABLE)) {

        // Bowser's tail?
        if (o->behavior == bhvBowserTailAnchor) {
            struct Object *bowser = o->parentObj;

            // Vanilla Bowser?
            if (bowser && !obj_is_dormant(bowser)) {
#if OMM_GAME_IS_R96X
                // Spamba Bowser
                if (bowser->oInteractType == INTERACT_DAMAGE) {
                    return false;
                }
#endif
                // Is Bowser grabbable and is Mario facing Bowser's tail, in a 120 deg arc?
                if (IS_BOWSER_INTERACTABLE[bowser->oAction] && (ignoreAngles || abs_s((s16) (m->faceAngle[1] - bowser->oFaceAngleYaw)) < 0x5555)) {
                    gOmmMario->grab.obj = bowser;
                    return true;
                }
            }
        }

        // Is Mario on ground?
        else if (!(m->action & ACT_FLAG_AIR)) {

            // Grabbable object?
            if (omm_obj_is_grabbable(o)) {
                s16 deltaYaw = m->faceAngle[1] - ((o->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO) ? o->oFaceAngleYaw : mario_obj_angle_to_object(m, o));

                // Is Mario facing the object, in a 60 deg arc?
                if (ignoreAngles || abs_s(deltaYaw) < 0x2AAA) {
                    gOmmMario->grab.obj = o;
                    return true;
                }
            }
        }
    }

    // Not a valid grab
    return false;
}

bool omm_mario_check_npc_dialog(struct MarioState *m, s32 actionArg, s32 *dialogState) {
    if (!omm_mario_is_capture(m)) {
        return false;
    }

    // Bowser arena entry cutscene
    if (gCamera->cutscene == CUTSCENE_ENTER_BOWSER_ARENA) {
        if (actionArg == 0) {
            omm_mario_unlock(m);
            *dialogState = 2;
            return true;
        }
        omm_mario_lock(m, -1);
        *dialogState = 0;
        return true;
    }

    // Skip dialog
    gCamera->cutscene = 0;
    gDialogResponse = 1;
    sObjectCutscene = 0;
    sCutsceneDialogResponse = 1;
    gRecentCutscene = CUTSCENE_DIALOG;
    *dialogState = 2;
    return true;
}

bool omm_mario_check_flooded(struct MarioState *m) {
    if (omm_world_is_flooded()) {
        vec3s_set(m->faceAngle, 0, m->faceAngle[1], 0);
        vec3s_set(m->angleVel, 0, 0, 0);
        return omm_mario_set_action(m, m->heldObj ? ACT_HOLD_WATER_IDLE : ACT_WATER_IDLE, 0, 0);
    }
    return false;
}

//
// Update
//

// Remove the platform ref during the capture sequence and talking so platforms are not triggered
void omm_mario_update_platform(struct MarioState *m) {
    if (omm_mario_is_capture(m) && (gOmmMario->capture.timer <= 20 || POBJ_IS_TALKING)) {
        m->marioObj->platform = NULL;
        gOmmCapture->platform = NULL;
    } else {
        update_mario_platform();
    }
}

void omm_mario_lock_camera(struct MarioState *m, bool isStarCutscene) {
    if (isStarCutscene) {
        gLakituState.posHSpeed = 0.1f;
        gLakituState.posVSpeed = 0.1f;
        gLakituState.focHSpeed = 0.2f;
        gLakituState.focVSpeed = 0.2f;
        gLakituState.goalPos[0] = m->pos[0] + 1024.f * sins(m->faceAngle[1]);
        gLakituState.goalPos[1] = m->pos[1] + 256.f;
        gLakituState.goalPos[2] = m->pos[2] + 1024.f * coss(m->faceAngle[1]);
        gLakituState.goalFocus[0] = m->pos[0];
        gLakituState.goalFocus[1] = m->pos[1];
        gLakituState.goalFocus[2] = m->pos[2];
    } else {
        gLakituState.posHSpeed = 0;
        gLakituState.posVSpeed = 0;
        gLakituState.goalFocus[0] = m->pos[0];
        gLakituState.goalFocus[1] = m->pos[1];
        gLakituState.goalFocus[2] = m->pos[2];
    }
}

bool omm_mario_start_dialog(struct MarioState *m, struct Object *o, s16 dialogId, bool dialogWithChoice, bool freezeObject, s16 angleVelTurnObjectTowardsMario) {
    if ((m->action != ACT_FIRST_PERSON) &&
       !(m->action & ACT_FLAG_RIDING_SHELL) &&
       !(m->action & ACT_FLAG_INVULNERABLE) && (
        (m->action == ACT_WAITING_FOR_DIALOG) ||
        (m->action & ACT_GROUP_MASK) == ACT_GROUP_STATIONARY ||
        (m->action & ACT_GROUP_MASK) == ACT_GROUP_MOVING)) {
        enable_time_stop();
        if (freezeObject) {
            o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
        } else {
            o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
        }
        o->oDialogResponse = 0;
        gOmmMario->dialog.obj = o;
        gOmmMario->dialog.state = 0;
        gOmmMario->dialog.id = dialogId;
        gOmmMario->dialog.turn = angleVelTurnObjectTowardsMario;
        gOmmMario->dialog.choice = dialogWithChoice;
        return true;
    }
    return false;
}

static void omm_mario_update_dialog(struct MarioState *m) {
    struct Object *o = gOmmMario->dialog.obj;
    if (o) {
        enable_time_stop();
        switch (gOmmMario->dialog.state) {
            case 0: {
                
                // Rotate Mario to the object
                s16 startYaw = m->faceAngle[1];
                m->faceAngle[1] = approach_s16_symmetric(m->faceAngle[1], obj_get_object1_angle_yaw_to_object2(m->marioObj, o), 0x800);
                bool doneTurning = (m->faceAngle[1] - startYaw) == 0;

                // Rotate the object to Mario
                if (gOmmMario->dialog.turn != 0) {
                    s16 startYaw = (s16) o->oFaceAngleYaw;
                    o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, obj_get_object1_angle_yaw_to_object2(o, m->marioObj), gOmmMario->dialog.turn);
                    o->oAngleVelYaw = (s16) ((s16) o->oFaceAngleYaw - startYaw);
                    doneTurning = (o->oAngleVelYaw == 0);
                }

                if (set_mario_npc_dialog(2) == 2 && doneTurning) {
                    gOmmMario->dialog.state = 1;
                }
            } break;

            case 1: {
                o->oDialogResponse = cutscene_object_with_dialog(gOmmMario->dialog.choice ? CUTSCENE_RACE_DIALOG : CUTSCENE_DIALOG, o, gOmmMario->dialog.id);
                if (o->oDialogResponse != 0) {
                    gOmmMario->dialog.state = 2;
                }
            } break;

            case 2: {
                if (m->action != ACT_READING_NPC_DIALOG) {
                    disable_time_stop();
                    o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
                    gOmmMario->dialog.obj = NULL;
                } else {
                    set_mario_npc_dialog(0);
                }
            } break;
        }
    }
}

static void omm_mario_update_grab(struct MarioState *m) {
    struct Object *o = gOmmMario->grab.obj;
    if (o) {
        gOmmMario->grab.obj = NULL;
        s16 angleMarioToObject = mario_obj_angle_to_object(m, o);
        m->interactObj = o;
        m->usedObj = o;
        m->input &= ~INPUT_INTERACT_OBJ_GRABBABLE;
        m->collidedObjInteractTypes &= ~INTERACT_GRABBABLE;
        if (o->behavior != bhvJumpingBox && (m->action & ACT_GROUP_MASK) != ACT_GROUP_SUBMERGED) {
            m->faceAngle[1] = angleMarioToObject;
        }

        // Bowser
        if (o->behavior == bhvBowser) {
            o->oFaceAngleYaw = angleMarioToObject;
            o->oIntangibleTimer = 0;
            obj_get_first_with_behavior(bhvBowserTailAnchor)->oAction = 2;
            omm_mario_set_action(m, ACT_PICKING_UP_BOWSER, 0, 0);
        }
        
        // Water punch
        else if (m->action == ACT_WATER_PUNCH) {
            mario_grab_used_object(m);
            m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
            m->actionState = 2;
        }
        
        // Dive
        else if ((m->action & ACT_FLAG_DIVING) && !(o->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)) {
            mario_grab_used_object(m);
            omm_mario_set_action(m, ACT_DIVE_PICKING_UP, 0, 0);
        }

        // Underwater grab
        else if (m->action & ACT_FLAG_SWIMMING) {
            ANM(MARIO_ANIM_WATER_PICK_UP_OBJ, 1.f);
            omm_mario_set_action(m, ACT_WATER_PUNCH, 1, 0);
            mario_grab_used_object(m);
            m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
            m->actionState = 2;
        }
        
        // Punch
        else {
            ANM(MARIO_ANIM_FIRST_PUNCH, 1.f);
            omm_mario_set_action(m, ACT_PICKING_UP, 0, 0);
        }
    }
}

#define OMM_SPIN_CHECKPOINT(angle) (((u16) (angle)) / 0x4000)
#define OMM_SPIN_INTENDED_NEXT(cp, dir) (((cp) + (dir) + 4) % 4)

static void omm_mario_update_spin(struct MarioState *m) {

    // Disable spin in Classic moveset
    if (OMM_MOVESET_CLASSIC) {
        gOmmMario->spin.timer = 0;
        return;
    }

    // Update midair spin timer
    if (!OMM_PLAYER_IS_PEACH) {
        if (gOmmMario->midairSpin.timer > 0) {
            gOmmMario->midairSpin.timer--;
        }
    }

    // Update spin timer
    if (gOmmMario->spin.timer > 0) {
        gOmmMario->spin.timer--;
    }

    // Update spin buffer
    if (m->controller->stickMag >= 40.f) {
        s8 cp = OMM_SPIN_CHECKPOINT(atan2s(m->controller->stickY, m->controller->stickX));

        // Ignore the cp -1
        if (cp != -1) {

            // We first set the first cp
            if (gOmmMario->spin._checkpoint == -1) {
                gOmmMario->spin._checkpoint = cp;
                gOmmMario->spin._counter++;
                gOmmMario->spin._buffer = OMM_MARIO_SPIN_BUFFER_DURATION;
            }

            // Then we set the direction
            else if (gOmmMario->spin._direction == 0) {
                if (cp != gOmmMario->spin._checkpoint) {
                    if (cp == OMM_SPIN_INTENDED_NEXT(gOmmMario->spin._checkpoint, -1)) {
                        gOmmMario->spin._checkpoint = cp;
                        gOmmMario->spin._direction = -1;
                        gOmmMario->spin._counter++;
                        gOmmMario->spin._buffer = OMM_MARIO_SPIN_BUFFER_DURATION;
                    } else if (cp == OMM_SPIN_INTENDED_NEXT(gOmmMario->spin._checkpoint, +1)) {
                        gOmmMario->spin._checkpoint = cp;
                        gOmmMario->spin._direction = +1;
                        gOmmMario->spin._counter++;
                        gOmmMario->spin._buffer = OMM_MARIO_SPIN_BUFFER_DURATION;
                    } else {
                        gOmmMario->spin._buffer = 0;
                    }
                }
            }

            // And we check if the hit cp is the intended next
            else if (cp != gOmmMario->spin._checkpoint) {
                if (cp == OMM_SPIN_INTENDED_NEXT(gOmmMario->spin._checkpoint, gOmmMario->spin._direction)) {
                    gOmmMario->spin._checkpoint = cp;
                    gOmmMario->spin._counter++;
                    gOmmMario->spin._buffer = OMM_MARIO_SPIN_BUFFER_DURATION;
                } else {
                    gOmmMario->spin._buffer = 0;
                }
            }
        }
    } else {
        gOmmMario->spin._buffer = 0;
    }

    // Hidden spin shortcut (joystick button or mouse scroll click)
    if (gOmmMario->spin.pressed) {
        gOmmMario->spin._counter = OMM_MARIO_SPIN_MIN_HIT_CHECKPOINTS;
    }

    // If Mario has the Improved Vanish cap, press (A) after a Midair Spin or during an Air Spin to perform or extend an Air Spin
    if (omm_mario_has_vanish_cap(m) && (m->prevAction == ACT_OMM_MIDAIR_SPIN || m->action == ACT_OMM_SPIN_AIR) && (m->controller->buttonPressed & A_BUTTON)) {
        gOmmMario->spin._counter = OMM_MARIO_SPIN_MIN_HIT_CHECKPOINTS;
    }

    // If we successfully hit OMM_MARIO_SPIN_MIN_HIT_CHECKPOINTS checkpoints in a row, Mario can start spinning
    if (gOmmMario->spin._counter == OMM_MARIO_SPIN_MIN_HIT_CHECKPOINTS) {
        gOmmMario->spin.timer = OMM_MARIO_SPIN_DURATION;
        gOmmMario->spin._buffer = 0;
    }

    // Update spin buffer timer
    if (gOmmMario->spin._buffer == 0) {
        gOmmMario->spin._counter = 0;
        gOmmMario->spin._checkpoint = -1;
        gOmmMario->spin._direction = 0;
    } else {
        gOmmMario->spin._buffer--;
    }
}

static void omm_mario_update_fall(struct MarioState *m) {
    if (OMM_CHEAT_NO_FALL_DAMAGE) {
        m->peakHeight = gOmmMario->state.peakHeight = m->pos[1];
        return;
    }

    // Odyssey fall
    if (OMM_MOVESET_ODYSSEY) {
        s32 actionGroup = (m->action & ACT_GROUP_MASK);
        
        // Airborne
        if (actionGroup == ACT_GROUP_AIRBORNE) {

            // Set peak height
            if (omm_mario_has_metal_cap(m) ||
                omm_mario_is_ground_pounding(m) ||
                (m->vel[1] >= 0.f) ||
                (m->action == ACT_FLYING) ||
                (m->action == ACT_TWIRLING) ||
                (m->action == ACT_SHOT_FROM_CANNON) ||
                (m->action == ACT_RIDING_SHELL_JUMP) ||
                (m->action == ACT_RIDING_SHELL_FALL) ||
                (m->action == ACT_OMM_SPIN_AIR) ||
                (m->action == ACT_OMM_MIDAIR_SPIN) ||
                (m->action == ACT_OMM_ROLL_AIR) ||
                (m->action == ACT_OMM_WALL_SLIDE) ||
                (m->action == ACT_OMM_PEACH_GLIDE) ||
                (m->action == ACT_OMM_PEACH_PERRY_CHARGE_AIR)) {
                gOmmMario->state.peakHeight = m->pos[1];
            }

            // Play the WAAAOOOW sound after falling for a while
            f32 fallHeight = gOmmMario->state.peakHeight - m->pos[1];
            if (fallHeight > OMM_MARIO_FALL_DAMAGE_HEIGHT - 400.f && !(m->action & ACT_FLAG_INVULNERABLE) && !(m->flags & MARIO_UNKNOWN_13)) {
                SFX(SOUND_MARIO_WAAAOOOW);
                m->flags |= (MARIO_UNKNOWN_18 | MARIO_UNKNOWN_13);
            }

        } else {

            // On ground
            if (actionGroup == ACT_GROUP_MOVING || actionGroup == ACT_GROUP_STATIONARY) {

                // Interrupts Mario's action if he lands on a non-slippery surface
                if ((gOmmMario->state.peakHeight - m->pos[1] > OMM_MARIO_FALL_DAMAGE_HEIGHT) &&
                    (m->vel[1] < -50) &&
                    !omm_mario_is_dead(m) &&
                    !mario_floor_is_slippery(m) &&
                    !omm_mario_has_metal_cap(m) &&
                    !omm_mario_is_roll_landing(m) &&
                    !omm_mario_is_ground_pound_landing(m) &&
                    (m->floor->type != SURFACE_BURNING) &&
                    (m->action != ACT_RIDING_SHELL_GROUND) &&
                    (m->action != ACT_OMM_ROLL)) {

                    // Stuck in ground
                    if (omm_mario_is_stuck_in_ground_after_fall(m)) {
                        SFX(SOUND_MARIO_OOOF2);
                        PFX(PARTICLE_MIST_CIRCLE);

                        // Head stuck
                        if ((m->action == ACT_DIVE_SLIDE || m->action == ACT_DIVE_PICKING_UP)) {
                            drop_and_set_mario_action(m, ACT_HEAD_STUCK_IN_GROUND, 0);
                        }

                        // Feet stuck
                        else {
                            drop_and_set_mario_action(m, ACT_FEET_STUCK_IN_GROUND, 0);
                        }
                    }

                    // Shocked
                    else {
                        drop_and_set_mario_action(m, ACT_OMM_SHOCKED_FROM_HIGH_FALL, 0);
                    }
                }
            }

            // Reset peak height (and stop the WAAAOOOW sound)
            gOmmMario->state.peakHeight = m->pos[1];
            if (m->flags & MARIO_UNKNOWN_13) {
                omm_sound_stop_character_sound_n64(SOUND_MARIO_WAAAOOOW, m->marioObj->oCameraToObject);
                m->flags &= ~(MARIO_UNKNOWN_18 | MARIO_UNKNOWN_13);
            }
        }
    }

    // Update classic peak height
    if (OMM_MOVESET_ODYSSEY || m->vel[1] >= 0) {
        m->peakHeight = m->pos[1];
    }
}

static void omm_mario_update_caps(struct MarioState *m) {

    // Improved Wing cap
    if (omm_mario_has_wing_cap(m)) {
        omm_obj_spawn_wing_glow_and_trail(m->marioObj);
    }

    // Improved Vanish cap
    if (omm_mario_has_vanish_cap(m)) {
        omm_obj_spawn_vanish_mist(m->marioObj);
    }

    // Improved Metal cap
    if (omm_mario_has_metal_cap(m)) {
        if (gGlobalTimer & 1) {
            omm_obj_spawn_metal_sparkle(m->marioObj);
        }

        // Cannot die, unless if he's already dead
        if (!omm_mario_is_dead(m)) {
            m->health = max_s(m->health, OMM_HEALTH_1_SEGMENT);
            m->hurtCounter = 0;
        }

        // Destroy objects that try to squish Mario
        if (m->action == ACT_SQUISHED && m->marioObj->oScaleY <= 0.05f &&
            m->ceil && m->ceil->object && m->ceil->object->activeFlags) {
            obj_destroy(m->ceil->object);
            obj_scale(m->marioObj, 1.f);
            m->squishTimer = 0;
            m->input &= ~INPUT_SQUISHED;
            omm_mario_set_action(m, ACT_IDLE, 0, 0);
        }
    }

    // Stop cap music if no cap
    if (m->capTimer == 0 && (m->flags & (MARIO_VANISH_CAP | MARIO_METAL_CAP | MARIO_WING_CAP)) == 0) {
        audio_stop_cap_music();
    }
}

static void omm_mario_update_burn(struct MarioState *m) {
    if (omm_mario_is_dead(m)) {
        m->invincTimer = 0;
    } else if (omm_mario_is_burning(m)) {
        m->invincTimer = 30;
        m->marioObj->oNodeFlags &= ~GRAPH_RENDER_INVISIBLE;
    }
}

static void omm_mario_update_action(struct MarioState *m) {

    // Count frames for how long A is held
    if (!(m->controller->buttonDown & A_BUTTON)) {
        m->framesSinceA = 0xFF;
    }

    // If dead, drop held object
    if (m->health <= OMM_HEALTH_DEAD) {
        mario_stop_riding_and_holding(m);
    }

    // If capture, set the corresponding action
    if (omm_mario_is_capture(m)) {
        m->action = (m->pos[1] < m->waterLevel ? ACT_OMM_POSSESSION_UNDERWATER : ACT_OMM_POSSESSION);
    }

    // Instant squish: Mario takes damage from some attacks even if not fully squished
    if (m->action == ACT_SQUISHED && m->ceil && m->ceil->object) {
        f32 heightDiff = m->ceilHeight - m->floorHeight;
        f32 hitboxHeight = m->marioObj->hitboxHeight;
        if (m->ceil->object->behavior == bhvEyerokHand && heightDiff < hitboxHeight / 2.f) {
            m->ceilHeight = m->floorHeight;
        }
    }

    // Camera fix after an exit cutscene
    static bool sWasExitCutscene = false;
    if (m->action == ACT_EXIT_AIRBORNE ||
        m->action == ACT_EXIT_LAND_SAVE_DIALOG ||
        m->action == ACT_DEATH_EXIT ||
        m->action == ACT_UNUSED_DEATH_EXIT ||
        m->action == ACT_FALLING_DEATH_EXIT ||
        m->action == ACT_SPECIAL_EXIT_AIRBORNE ||
        m->action == ACT_SPECIAL_DEATH_EXIT ||
        m->action == ACT_FALLING_EXIT_AIRBORNE) {
        sWasExitCutscene = true;
    } else if (sWasExitCutscene && (
        m->action == ACT_IDLE ||
        m->action == ACT_WALKING)) {
        cutscene_exit_painting_end(gCamera);
        sModeTransition.framesLeft = 0;
        sWasExitCutscene = false;
    }

    // Finish initializing Mario
    if (!unused80339F10) {
        u32 action = m->action;
        u32 actionArg = m->actionArg;
        if (omm_mario_possess_object_after_warp(m)) {
            struct Object *o = gOmmCapture;
            switch (action) {
                case ACT_WARP_DOOR_SPAWN: {
                    gOmmWarp->state = POBJ_WARP_STATE_HAS_WARPED;
                    m->usedObj->oInteractStatus = (actionArg & 1) ? INT_STATUS_EXITING_WARP_DOOR : INT_STATUS_EXITING_WARP_DOOR_2; // needed to spawn on the right side of the door
                    stop_and_set_height_to_floor(m);
                    omm_mario_lock(m, 30);
                } break;

                case ACT_EMERGE_FROM_PIPE: {
                    gOmmWarp->state = POBJ_WARP_STATE_EMERGING_FROM_PIPE;
                    obj_set_forward_and_y_vel(o, 8.f, 52.f);
                    omm_mario_lock(m, 30);
                    obj_play_sound(gOmmCapture, SOUND_MENU_EXIT_PIPE);
                } break;

                case ACT_TELEPORT_FADE_IN: {
                    gOmmWarp->state = POBJ_WARP_STATE_FADING_IN;
                    m->fadeWarpOpacity = 0x07;
                    stop_and_set_height_to_floor(m);
                    omm_mario_lock(m, 32);
                    obj_play_sound(gOmmCapture, SOUND_ACTION_TELEPORT);
                } break;

                default: {
                    gOmmWarp->state = POBJ_WARP_STATE_HAS_WARPED;
                    omm_mario_lock(m, 15);
                } break;
            }
        } else {
            gOmmWarp->state = POBJ_WARP_STATE_NOT_WARPING;
            m->marioObj->oNodeFlags |= GRAPH_RENDER_ACTIVE;
            m->marioObj->oNodeFlags &= ~GRAPH_RENDER_INVISIBLE;
        }
        gOmmGlobals->marioTimer = 0;
        unused80339F10 = TRUE;
    }
}

static void omm_mario_update_camera_mode(struct MarioState *m) {
    if (!BETTER_CAM_IS_ENABLED && m->area && m->area->camera) {
        
        // Return to default camera when leaving water
        if(((m->area->camera->mode) == CAMERA_MODE_WATER_SURFACE ||
            (m->area->camera->mode) == CAMERA_MODE_BEHIND_MARIO) && (
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_STATIONARY ||
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_MOVING     ||
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE   ||
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE   ||
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_OBJECT)) {
            set_camera_mode(m->area->camera, m->area->camera->defMode, 0);
        }

        // Set close camera if underwater with Metal Mario
        if(((m->area->camera->mode) == CAMERA_MODE_WATER_SURFACE ||
            (m->area->camera->mode) == CAMERA_MODE_BEHIND_MARIO) && (
            (m->action & ACT_GROUP_MASK) == ACT_GROUP_METAL_WATER)) {
            set_camera_mode(m->area->camera, CAMERA_MODE_CLOSE, 0);
        }

        // Return to default camera when leaving water with Metal Mario
        if ((m->area->camera->mode) == CAMERA_MODE_CLOSE &&
            omm_mario_has_metal_cap(m) && !(m->action & ACT_FLAG_METAL_WATER)) {
            set_camera_mode(m->area->camera, m->area->camera->defMode, 0);
        }
    }
}

static void omm_mario_update_inputs(struct MarioState *m) {
#if OMM_GAME_IS_R96X
    // Fix the infinite death loop during Chaos: Deadly Objects
    if (Cheats.EnableCheats && Cheats.ChaosMode && (m->action == ACT_QUICKSAND_DEATH || m->action == ACT_OMM_DEATH_QUICKSAND)) {
        m->collidedObjInteractTypes = m->marioObj->collidedObjInteractTypes = 0;
    }
#endif
    update_mario_inputs(m);
    if (OMM_CHEAT_MOON_JUMP && (m->controller->buttonDown & L_TRIG)) {
        m->vel[1] = +30.f;
    }
    if (OMM_CHEAT_INVINCIBLE) {
        m->invincTimer = max_s(m->invincTimer, 1);
        if (omm_mario_is_capture(m)) {
            gOmmObject->state._invincTimer = max_s(gOmmObject->state._invincTimer, 2);
        }
    }
}

//
// Mario update
//

extern s32 omm_mario_execute_stationary_action(struct MarioState *m);
extern s32 omm_mario_execute_moving_action(struct MarioState *m);
extern s32 omm_mario_execute_airborne_action(struct MarioState *m);
extern s32 omm_mario_execute_submerged_action(struct MarioState *m);
extern s32 omm_mario_execute_cutscene_action(struct MarioState *m);
extern s32 omm_mario_execute_automatic_action(struct MarioState *m);
extern s32 omm_mario_execute_object_action(struct MarioState *m);
extern s32 omm_mario_execute_metal_water_action(struct MarioState *m);

static s32 (*sExecuteOmmActionFunction[(ACT_GROUP_MASK >> 6) + 1])(struct MarioState *) = {
    omm_mario_execute_stationary_action,    // 0x000 Stationary
    omm_mario_execute_moving_action,        // 0x040 Moving
    omm_mario_execute_airborne_action,      // 0x080 Airborne
    omm_mario_execute_submerged_action,     // 0x0C0 Submerged
    omm_mario_execute_cutscene_action,      // 0x100 Cutscene
    omm_mario_execute_automatic_action,     // 0x140 Automatic
    omm_mario_execute_object_action,        // 0x180 Object
    omm_mario_execute_metal_water_action,   // 0x1C0 Metal Water
};

static s32 (*sExecuteSm64ActionFunction[(ACT_GROUP_MASK >> 6) + 1])(struct MarioState *) = {
    mario_execute_stationary_action,        // 0x000 Stationary
    mario_execute_moving_action,            // 0x040 Moving
    mario_execute_airborne_action,          // 0x080 Airborne
    mario_execute_submerged_action,         // 0x0C0 Submerged
    mario_execute_cutscene_action,          // 0x100 Cutscene
    mario_execute_automatic_action,         // 0x140 Automatic
    mario_execute_object_action,            // 0x180 Object
    NULL,                                   // 0x1C0 Metal Water
};

void bhv_mario_update() {
    struct MarioState *m = gMarioState;
    if (!m->action) {
        return;
    }

    // Update old Cheats flags
#if !OMM_GAME_IS_R96X
    mem_zero(&Cheats, sizeof(Cheats));
    Cheats.EnableCheats = gOmmCheatEnable;
    Cheats.Responsive = OMM_CHEAT_SUPER_RESPONSIVE;
#endif

    // Infinite health cheat
    if (OMM_CHEAT_GOD_MODE) {
        m->health = OMM_HEALTH_MAX;
    }

    // Reset state
    m->marioObj->oNodeFlags &= ~GRAPH_RENDER_INVISIBLE;
    m->marioObj->oFlags &= ~OBJ_FLAG_SHADOW_COPY_OBJ_POS;
    mario_reset_bodystate(m);

    // Inputs
    if (omm_peach_vibe_is_gloom() && m->controller->stickMag != 0.f) {
        m->controller->stickX  *= 64.f / m->controller->stickMag;
        m->controller->stickY  *= 64.f / m->controller->stickMag;
        m->controller->stickMag = 64.f;
    }
    omm_mario_update_inputs(m);
    if (omm_peach_vibe_is_gloom() && ((m->input & INPUT_NONZERO_ANALOG) || (abs_f(m->forwardVel) > 16.f))) {
        m->input &= ~INPUT_ABOVE_SLIDE;
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        m->intendedYaw = omm_camera_get_intended_yaw(m);
    }

    // Actions
    omm_mario_update_grab(m);
    omm_mario_update_action(m);
    omm_mario_update_spin(m);

    // Interactions
    mario_handle_special_floors(m);
    if (!omm_mario_is_capture(m)) {
        omm_mario_process_interactions(m);

        // Mario bounced off an object, apply his jump modifier.
        // Nice side effect: the flag MARIO_UNKNOWN_08 allows the player to
        // control Mario's jump height for his regular jumps by holding down A.
        // Resetting this flag after a bounce allows the player to control
        // the height of the bounce, like in Super Mario Bros. games.
        if (!(m->action & ACT_FLAG_INTANGIBLE) && !(m->flags & MARIO_UNKNOWN_08)) {
            m->vel[1] *= (omm_player_physics_get_selected_jump() * ((m->action & ACT_FLAG_CONTROL_JUMP_HEIGHT) ? 1.5f : 1.f));
            m->flags |= MARIO_UNKNOWN_08;
        }
    }

    // Out of Bounds
    if (!m->floor) {
        return;
    }

    // Adjust camera depending on Mario's action
    omm_mario_update_camera_mode(m);

    // Execute actions
    for (;;) {

        // Check if Mario is dead, and change his action if needed
        omm_mario_check_dead(m, m->health);

        // Determine execute functions depending on Mario's action
        s32 group = ((m->action & ACT_GROUP_MASK) >> 6);
        s32 actResult = OMM_MARIO_ACTION_RESULT_CONTINUE;
        s32 (*execOmm)(struct MarioState *) = sExecuteOmmActionFunction[group];
        s32 (*execSm64)(struct MarioState *) = sExecuteSm64ActionFunction[group];

        // Execute OMM actions
        if (execOmm) {
            actResult = execOmm(m);
        }

        // Execute SM64 actions
        if (actResult == OMM_MARIO_ACTION_RESULT_CONTINUE) {
            if (!(m->action & ACT_FLAG_OMM_ACTION) && execSm64) {
                actResult = execSm64(m);
            } else {
                actResult = OMM_MARIO_ACTION_RESULT_BREAK;
            }
        }

        // Cancel? -> loop again
        // Break? -> end loop
        if (actResult == OMM_MARIO_ACTION_RESULT_CANCEL) {
            continue;
        } else if (actResult == OMM_MARIO_ACTION_RESULT_BREAK) {
            break;
        }
    }

    // Action post-update
    omm_mario_update_action(m);
    omm_mario_update_l_actions(m);
    omm_mario_update_fall(m);
    omm_mario_update_caps(m);
    omm_mario_update_dialog(m);

    // Check instant warp
    Vec3f posBeforeWarp, displacement;
    vec3f_copy(posBeforeWarp, m->pos);
    check_instant_warp();
    vec3f_dif(displacement, m->pos, posBeforeWarp);
    if (!vec3f_eq(displacement, gVec3fZero)) {
        m->floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &m->floor);
        m->marioObj->oFlags &= ~OBJ_FLAG_GFX_INITED;
        if (omm_mario_is_capture(m)) {
            vec3f_add(gOmmMario->capture.animPos[0], displacement);
            vec3f_add(gOmmMario->capture.animPos[1], displacement);
            vec3f_add(gOmmMario->capture.animPos[2], displacement);
            vec3f_add(&gOmmCapture->oPosX, displacement);
            vec3f_add(gOmmCapture->oGfxPos, displacement);
            gOmmCapture->oFlags &= ~OBJ_FLAG_GFX_INITED;
            struct Object *cap = obj_get_first_with_behavior(bhvOmmPossessedObjectCap);
            if (cap) {
                vec3f_add(&cap->oPosX, displacement);
                vec3f_add(cap->oGfxPos, displacement);
                cap->oFlags &= ~OBJ_FLAG_GFX_INITED;
            }
        }
    }

    // World objects
    omm_world_update(m);

    // Gfx & camera stuff
#if OMM_GAME_IS_R96X
    if (!m->milk) squish_mario_model(m);
#else
    squish_mario_model(m);
#endif
    sink_mario_in_quicksand(m);
    set_submerged_cam_preset_and_spawn_bubbles(m);
    update_mario_info_for_cam(m);
    mario_update_hitbox_and_cap_model(m);
    omm_mario_update_burn(m);

    // Vibes (Peach only)
    if (OMM_PLAYER_IS_PEACH) {
        omm_peach_vibe_update(m);
    }

    // Health
    omm_health_update(m);

    // Cappy
    omm_cappy_update(m);

    // Wind gfx/sfx
    if (m->floor->type == SURFACE_HORIZONTAL_WIND) {
        spawn_wind_particles(0, (m->floor->force << 8));
        SFX(SOUND_ENV_WIND2);
    } else if (m->floor->type == SURFACE_VERTICAL_WIND) {
        spawn_wind_particles(1, 0);
        SFX(SOUND_ENV_WIND2);
    }

    // Misc
#if !OMM_GAME_IS_R96X
    play_infinite_stairs_music();
#endif
    m->numLives = max_s(m->numLives, 1);
    m->numStars = omm_save_file_get_total_star_count(gCurrSaveFileNum - 1, OMM_GAME_MODE);

    // Update Mario object
    obj_set_xyz(gMarioObject, m->pos[0], m->pos[1], m->pos[2]);
    obj_set_vel(gMarioObject, m->vel[0], m->vel[1], m->vel[2]);
    obj_set_angle(gMarioObject, gMarioObject->oGfxAngle[0], gMarioObject->oGfxAngle[1], gMarioObject->oGfxAngle[2]);
    obj_set_angle_vel(gMarioObject, m->angleVel[0], m->angleVel[1], m->angleVel[2]);
    gMarioObject->oInteractStatus = 0;
    gMarioObject->oMarioParticleFlags = m->particleFlags;

    // Update Peach's body state
    if (OMM_PLAYER_IS_PEACH) {

        // Torso
        // Between Mario and Beytah
        if (!(m->action & ACT_FLAG_RIDING_SHELL)) {
            m->marioBodyState->torsoAngle[0] *= 0.0f;
            m->marioBodyState->torsoAngle[1] *= 1.0f;
            m->marioBodyState->torsoAngle[2] *= 0.5f;
        }

        // Head
        // Move the head up and down if she's crying
        if (m->action == ACT_IDLE && omm_peach_vibe_is_gloom()) {
            f32 tAnim = (f32) m->marioObj->oAnimFrame / (f32) m->marioObj->oCurrAnim->mLoopEnd;
            m->marioBodyState->action |= ACT_FLAG_ENABLE_HEAD_ROTATION;
            m->marioBodyState->headAngle[0] = relerp_f(sins(tAnim * 0x30000), -1.f, +1.f, -0x1000, -0x1800);
            m->marioBodyState->headAngle[1] = 0;
            m->marioBodyState->headAngle[2] = 0;
        }

        // Eyes
        // Each Vibe has a unique expression
        if (omm_peach_vibe_is_joy()) {
            m->marioBodyState->eyeState = 4;
        } else if (omm_peach_vibe_is_rage()) {
            m->marioBodyState->eyeState = 5;
        } else if (omm_peach_vibe_is_gloom()) {
            m->marioBodyState->eyeState = 6;
        } else if (m->marioBodyState->handState == MARIO_HAND_PEACE_SIGN) {
            m->marioBodyState->eyeState = 7;
        }

        // Hands
        // Show Peach's crown or Tiara in her right hand
        if (OMM_EXTRAS_CAPPY_AND_TIARA && m->marioBodyState->handState == MARIO_HAND_HOLDING_CAP) {
            m->marioBodyState->handState = MARIO_HAND_HOLDING_WING_CAP;
        } else if (!OMM_EXTRAS_CAPPY_AND_TIARA && m->marioBodyState->handState == MARIO_HAND_HOLDING_WING_CAP) {
            m->marioBodyState->handState = MARIO_HAND_HOLDING_CAP;
        }

        // Cap
        // Peach's wings are displayed or hidden depending on the Wing cap state
        // Hide the wings if invisible mode is enabled
        if (OMM_EXTRAS_INVISIBLE_MODE) {
            m->marioBodyState->capState = MARIO_HAS_DEFAULT_CAP_OFF;
        } else if (m->flags & MARIO_WING_CAP) {
            m->marioBodyState->capState |= MARIO_HAS_WING_CAP_ON;
        } else {
            m->marioBodyState->capState &= ~MARIO_HAS_WING_CAP_ON;
        }
    }

    // Mario transparency
    // If Mario gets too close to the camera, make him progressively transparent
    if (omm_camera_is_available(m)) {
        f32 dist = vec3f_dist(gLakituState.curFocus, gLakituState.curPos);
        if (dist < 200.f) {
            u8 alpha = (u8) relerp_0_1_f(dist, 100.f, 200.f, 0x00, 0xFF);
            m->marioBodyState->modelState &= ~0xFF;
            m->marioBodyState->modelState |= (0x100 | alpha);
        }
    }

    // Graph node preprocessing
    // Compute Mario's hands, arms, head and root positions
    geo_preprocess_object_graph_node(gMarioObject);

    // Spawn particles
    for (u32 i = 0; i != 32; ++i) {
        u32 particle = (1u << i);
        if (m->particleFlags & particle) {
            obj_spawn_particle_preset(m->marioObj, particle, true);
        }
    }
}
