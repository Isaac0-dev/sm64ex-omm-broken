#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS

//
// Update animations
//

static s32 get_mario_current_anim_index() {
    struct_MarioAnimDmaTable *anim_dma_table = gMarioAnimDmaTable;
    for (s32 i = 0; i != (s32) anim_dma_table->count; ++i) {
        void *anim_addr = anim_dma_table->srcAddr + anim_dma_table->anim[i].offset;
        if (anim_addr == gMarioCurrAnimAddr) {
            return i;
        }
    }
    return -1;
}

// As we don't know the length of the table, let's hope that we'll always find the animation...
static s32 get_obj_current_anim_index(struct Object *obj) {
    if (!obj->oAnimations || !obj->oCurrAnim) {
        return -1;
    }
    for (s32 i = 0; obj->oAnimations[i] != NULL; ++i) {
        if (obj->oAnimations[i] == obj->oCurrAnim) {
            return i;
        }
    }
    return -1;
}

void omm_models_swap_animations(void *ptr) {
    static struct Animation *default_anim = NULL;
    static struct Animation gfx_data_anim[1];

    // Does the object has a model?
    struct Object *obj = (struct Object *) ptr;
    if (!obj->oGraphNode) {
        return;
    }

    // Swap the current animation with the one from the Gfx data
    if (!default_anim) {
        default_anim = obj->oCurrAnim;

        // Actor index
        s32 actor_index = omm_models_get_actor_index(obj->oGraphNode->georef);
        if (actor_index == -1) {
            return;
        }

        // Gfx data
        OmmGfxData *gfx_data = ((OmmActorGfx *) omm_array_get(gOmmActorList, ptr, actor_index))->gfx_data;
        if (!gfx_data) {
            return;
        }

        // Animation table
        if (omm_array_count(gfx_data->animation_table) == 0) {
            return;
        }

        // Animation index
        s32 anim_index = (obj == gMarioObject ? get_mario_current_anim_index() : get_obj_current_anim_index(obj));
        if (anim_index == -1) {
            return;
        }

        // Animation data
        const OmmAnimData *anim_data = (const OmmAnimData *) omm_array_get(gfx_data->animation_table, ptr, anim_index);
        if (anim_data) {
            gfx_data_anim->flags = anim_data->flags;
            gfx_data_anim->mStartFrame = anim_data->start_frame;
            gfx_data_anim->mLoopStart = anim_data->loop_start;
            gfx_data_anim->mLoopEnd = anim_data->loop_end;
            gfx_data_anim->values = anim_data->values;
            gfx_data_anim->index = anim_data->index;
            gfx_data_anim->length = anim_data->length;
            obj->oCurrAnim = gfx_data_anim;
        }

    // Restore the default animation
    } else {
        obj->oCurrAnim = default_anim;
        default_anim = NULL;
    }
}

//
// Update models
//

void omm_models_update_object(struct Object *obj) {
    if (obj->activeFlags && obj->oGraphNode) {

        // Actor index
        s32 actor_index = omm_models_get_actor_index(obj->oGraphNode->georef);
        if (actor_index != -1) {

            // Replace the object's model and animations
            OmmActorGfx *actor_gfx = (OmmActorGfx *) omm_array_get(gOmmActorList, ptr, actor_index);
            omm_array_for_each(gOmmPackList, p_pack) {
                const OmmPackData *pack = (const OmmPackData *) p_pack->as_ptr;
                if (!pack || !pack->exists) continue;

                // If pack is enabled
                // load the pack's model
                if (pack->enabled) {
                    OmmGfxData *gfx_data = omm_models_load_from_binary(pack->path, omm_models_get_actor_names(actor_index), NULL);

                    // If no pack is selected for that actor or the loaded model has higher priority
                    // replace the actor's current model
                    if (gfx_data && (actor_gfx->pack_index == -1 || gfx_data->priority > actor_gfx->gfx_data->priority)) {
                        OmmDataNode_GeoLayout *geo_layout_node = (OmmDataNode_GeoLayout *) omm_array_get(gfx_data->geo_layouts, ptr, omm_array_count(gfx_data->geo_layouts) - 1);
                        actor_gfx->pack_index = i_p_pack;
                        actor_gfx->gfx_data = gfx_data;
                        actor_gfx->graph_node = (struct GraphNode *) geo_layout_to_graph_node(NULL, geo_layout_node->data);
                        actor_gfx->graph_node->georef = omm_models_get_actor_layout(actor_index);
                        actor_gfx->graph_node->noBillboard = gfx_data->disable_billboard;
                        break;
                    }
                }

                // If disabled and this pack is the one selected for that actor
                // replace the actor's model by the default one
                else if (actor_gfx->pack_index == i_p_pack) {
                    actor_gfx->pack_index = -1;
                    actor_gfx->gfx_data = NULL;
                    actor_gfx->graph_node = (struct GraphNode *) geo_layout_to_graph_node(NULL, (const GeoLayout *) omm_models_get_actor_layout(actor_index));
                    actor_gfx->graph_node->noBillboard = false;
                }
            }

            // Update object
            obj->oGraphNode = actor_gfx->graph_node;
            obj->oNodeFlags &= ~((GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_CYLBOARD) * actor_gfx->graph_node->noBillboard);
        }
    }
}

void omm_models_update() {
    if (gObjectLists && omm_array_count(gOmmPackList) != 0) {
        omm_models_load_all(NULL);
        for_each_object_in_all_lists(obj) {
            omm_models_update_object(obj);
        }
    }
}
