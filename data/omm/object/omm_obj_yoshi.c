#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS
#include "behavior_commands.h"

//
// Behavior
//

static bool bhv_omm_yoshi_is_safe(struct Object *o) {

    // Out of bounds
    if (!o->oFloor) {
        return false;
    }

    // Far from Mario
    if (obj_get_distance(o, gMarioObject) > 4096.f) {
        return false;
    }

    // Airborne
    if (!obj_is_on_ground(o)) {
        return true;
    }

    // Lethal ground
    if (o->oFloor->type == SURFACE_DEATH_PLANE ||
        o->oFloor->type == SURFACE_VERTICAL_WIND ||
        o->oFloor->type == SURFACE_INSTANT_QUICKSAND ||
        o->oFloor->type == SURFACE_INSTANT_MOVING_QUICKSAND ||
        o->oFloor->type == SURFACE_BURNING) {
        return false;
    }

    // Safe
    return true;
}

static void bhv_omm_yoshi_loop() {
    struct Object *o = gCurrentObject;
    obj_reset_hitbox(o, 80, 180, 0, 0, 60, 0);

    // Step
    obj_set_forward_vel(o, 0, 0, 0);
    bool wasUnderwater = obj_is_underwater(o, find_water_level(o->oPosX, o->oPosZ));
    perform_object_step(o, OBJ_STEP_UPDATE_HOME | OBJ_STEP_STOP_IF_OUT_OF_BOUNDS);

    // Check ground
    if (!bhv_omm_yoshi_is_safe(o)) {
        obj_spawn_white_puff(o, SOUND_OBJ_DEFAULT_DEATH);
        obj_mark_for_deletion(o);
        return;
    }

    // Update vel
    f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);
    bool isOnGround = obj_is_on_ground(o);
    bool isUnderwater = obj_is_underwater(o, waterLevel);
    if (isOnGround) {
        o->oVelY = 0.f;
    } else if (isUnderwater) {
        o->oVelY = max_f(o->oVelY - 2.f, -40.f);
    } else {
        o->oVelY = max_f(o->oVelY - 4.f, -75.f);
    }

    // Water effects
    obj_produce_water_effects(o, isOnGround, isUnderwater, wasUnderwater, waterLevel);

    // Gfx
    obj_update_gfx(o);
    if (isOnGround) {
        obj_anim_play(o, 0, 1.f);
    } else {
        obj_anim_play(o, 2, 1.f);
        obj_anim_extend(o);
    }
}

const BehaviorScript bhvOmmYoshi[] = {
    OBJ_TYPE_SPECIAL,
    BHV_OR_INT(oFlags, OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE),
    BHV_LOAD_ANIMATIONS(oAnimations, yoshi_seg5_anims_05024100),
    BHV_SET_INTERACT_TYPE(INTERACT_TEXT),
    BHV_SET_INTERACT_SUBTYPE(0),
    BHV_ANIMATE(0),
    BHV_BEGIN_LOOP(),
        BHV_SET_INT(oIntangibleTimer, 0),
        BHV_CALL_NATIVE(bhv_omm_yoshi_loop),
    BHV_END_LOOP(),
};

//
// Spawner
//

struct Object *omm_obj_spawn_yoshi(struct Object *o, s16 faceAngleYaw) {
    obj_unload_all_with_behavior(bhvOmmYoshi);
#if OMM_GAME_IS_SM64
    obj_unload_all_with_behavior(bhvYoshi);
#endif
    struct Object *yoshi = obj_spawn_from_geo(o, yoshi_geo, bhvOmmYoshi);
    yoshi->oPosX = o->oPosX + 250.f * sins(faceAngleYaw);
    yoshi->oPosY = o->oPosY + 150.f;
    yoshi->oPosZ = o->oPosZ + 250.f * coss(faceAngleYaw);
    obj_set_angle(yoshi, 0, faceAngleYaw + 0x8000, 0);
    obj_scale(yoshi, 1.f);
    obj_drop_to_floor(yoshi);
    obj_spawn_white_puff(yoshi, SOUND_GENERAL_YOSHI_TALK);
    return yoshi;
}
