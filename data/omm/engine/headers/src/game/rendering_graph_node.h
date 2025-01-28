#ifndef RENDERING_GRAPH_NODE_H
#define RENDERING_GRAPH_NODE_H

#include "graph_node.h"

void geo_get_marios_forearm_pos(Vec3f dest, bool isLeft);
void geo_get_marios_hand_pos(Vec3f dest, bool isLeft);
void geo_get_marios_head_mtx(Mat4 dest);
void geo_get_marios_root_pos(Vec3f dest);
void geo_get_marios_heights(f32 *rootHeight, f32 *headHeight, f32 *armsHeight);
void geo_compute_marios_heights(struct Object *o);
bool geo_compute_capture_cappy_obj_transform(struct Object *o, s32 animParts, Mat4 transform);
void geo_preprocess_object_graph_node(struct Object *o);
void geo_process_node_and_siblings(struct GraphNode *firstNode);
void geo_process_root(struct GraphNodeRoot *node, Vp *viewport1, Vp *viewport2, s32 clearColor);

#endif
