#ifndef RENDERING_GRAPH_NODE_H
#define RENDERING_GRAPH_NODE_H

#include "graph_node.h"

f32 *geo_get_marios_forearm_pos(bool isLeft);
f32 *geo_get_marios_hand_pos(bool isLeft);
f32 *geo_get_marios_head_pos();
f32 *geo_get_marios_head_up();
f32 *geo_get_marios_head_forward();
f32 *geo_get_marios_head_right();
f32 *geo_get_marios_root_pos();
bool geo_compute_marios_heights(struct Object *o, f32 *rootHeight, f32 *headHeight, f32 *armsHeight);
bool geo_compute_capture_cappy_obj_transform(struct Object *o, s32 animParts, Mat4 transform);
void geo_preprocess_object_graph_node(struct Object *o);
void geo_process_node_and_siblings(struct GraphNode *firstNode);
void geo_process_root(struct GraphNodeRoot *node, Vp *viewport1, Vp *viewport2, s32 clearColor);

#endif
