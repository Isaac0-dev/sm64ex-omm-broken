#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"
#include "game/level_update.h"
#include "levels/scripts.h"
#include "actors/common1.h"
#include "make_const_nonconst.h"

#include "areas/1/custom.model.inc.h"
#include "levels/castle_grounds/header.h"
extern u8 _castle_grounds_segment_ESegmentRomStart[]; 
extern u8 _castle_grounds_segment_ESegmentRomEnd[];
#include "levels/castle_inside/header.h"
const LevelScript level_castle_grounds_entry[] = {
INIT_LEVEL(),
LOAD_MIO0(0x07, _castle_inside_segment_7SegmentRomStart, _castle_inside_segment_7SegmentRomEnd),
LOAD_RAW(0x1A, _castle_insideSegmentRomStart, _castle_insideSegmentRomEnd),
LOAD_RAW(0x0E, _castle_grounds_segment_ESegmentRomStart, _castle_grounds_segment_ESegmentRomEnd),
LOAD_MIO0(0xA,_SkyboxCustom30637760_skybox_mio0SegmentRomStart,_SkyboxCustom30637760_skybox_mio0SegmentRomEnd),
LOAD_MIO0(8,_common0_mio0SegmentRomStart,_common0_mio0SegmentRomEnd),
LOAD_RAW(15,_common0_geoSegmentRomStart,_common0_geoSegmentRomEnd),
LOAD_MIO0(5,_group10_mio0SegmentRomStart,_group10_mio0SegmentRomEnd),
LOAD_RAW(12,_group10_geoSegmentRomStart,_group10_geoSegmentRomEnd),
LOAD_MIO0(6,_group15_mio0SegmentRomStart,_group15_mio0SegmentRomEnd),
LOAD_RAW(13,_group15_geoSegmentRomStart,_group15_geoSegmentRomEnd),
ALLOC_LEVEL_POOL(),
MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_BOWSER_TRAP,        castle_geo_000F18),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WATER_LEVEL_PILLAR, castle_geo_001940),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_MINUTE_HAND,  castle_geo_001530),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_HOUR_HAND,    castle_geo_001548),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CLOCK_PENDULUM,     castle_geo_001518),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CASTLE_DOOR,        castle_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WOODEN_DOOR,        wooden_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_METAL_DOOR,         metal_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_CASTLE_DOOR_UNUSED, castle_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_WOODEN_DOOR_UNUSED, wooden_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_0_STARS,       castle_door_0_star_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_1_STAR,        castle_door_1_star_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_DOOR_3_STARS,       castle_door_3_stars_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_KEY_DOOR,           key_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_30_STARS, castle_geo_000F00),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_8_STARS,  castle_geo_000F00),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_50_STARS, castle_geo_000F00),
LOAD_MODEL_FROM_GEO(MODEL_CASTLE_STAR_DOOR_70_STARS, castle_geo_000F00),
LOAD_MODEL_FROM_GEO(22, warp_pipe_geo),
LOAD_MODEL_FROM_GEO(23, bubbly_tree_geo),
LOAD_MODEL_FROM_GEO(24, spiky_tree_geo),
LOAD_MODEL_FROM_GEO(25, snow_tree_geo),
LOAD_MODEL_FROM_GEO(28, castle_door_geo),
LOAD_MODEL_FROM_GEO(29, wooden_door_geo),
LOAD_MODEL_FROM_GEO(31, metal_door_geo),
LOAD_MODEL_FROM_GEO(32, hazy_maze_door_geo),
LOAD_MODEL_FROM_GEO(34, castle_door_0_star_geo),
LOAD_MODEL_FROM_GEO(35, castle_door_1_star_geo),
LOAD_MODEL_FROM_GEO(36, castle_door_3_stars_geo),
LOAD_MODEL_FROM_GEO(37, key_door_geo),
LOAD_MODEL_FROM_GEO(38, castle_door_geo),
LOAD_MODEL_FROM_GEO(39, wooden_door_geo),
LOAD_MODEL_FROM_GEO(41, metal_door_geo),
JUMP_LINK(script_func_global_1),
JUMP_LINK(script_func_global_11),
JUMP_LINK(script_func_global_16),
JUMP_LINK(local_area_castle_grounds_1_),
FREE_LEVEL_POOL(),
MARIO_POS(1,135,-1500,-509,2102),
CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
CLEAR_LEVEL(),
SLEEP_BEFORE_EXIT(/*frames*/ 1),
EXIT(),
};
const LevelScript local_area_castle_grounds_1_[] = {
AREA(1,Geo_castle_grounds_1_0x19001700),
TERRAIN(col_castle_grounds_1_0xe03c200),
SET_BACKGROUND_MUSIC(0,4),
TERRAIN_TYPE(TERRAIN_STONE),
JUMP_LINK(local_objects_castle_grounds_1_),
JUMP_LINK(local_warps_castle_grounds_1_),
END_AREA(),
RETURN()
};
const LevelScript local_objects_castle_grounds_1_[] = {
OBJECT(0,-1500,-509,2102,0,90,0,0xf30000, bhvSpinAirborneWarp),
OBJECT(0,-200,-50,308,0,90,0,0xf10000, bhvPaintingDeathWarp),
OBJECT(0,-200,-50,308,0,90,0,0xf00000, bhvPaintingStarCollectWarp),
OBJECT(22,3773,-969,-748,0,270,0,0x0, bhvWarpPipe),
OBJECT(22,880,1319,3000,0,0,0,0x40000, bhvWarpPipe),
OBJECT(22,4008,-773,2878,0,0,0,0x50000, bhvWarpPipe),
OBJECT(22,5156,1592,5842,0,180,0,0x30000, bhvWarpPipe),
OBJECT(22,-3100,-479,-4276,0,0,0,0x10000, bhvWarpPipe),
OBJECT(22,880,-556,3000,0,0,0,0x20000, bhvWarpPipe),
OBJECT(0,855,-230,627,0,180,0,0x60000, bhvWarp),
OBJECT(37,-4102,-41,275,0,270,0,0x2140000, bhvDoorWarp),
OBJECT(37,-4102,-41,425,0,90,0,0x2140000, bhvDoorWarp),
OBJECT(22,880,1529,-955,0,0,0,0xc0000, bhvWarpPipe),
OBJECT(22,880,-556,-1438,0,0,0,0xb0000, bhvWarpPipe),
OBJECT(128,880,1634,-285,0,0,0,0x0, bhvCannon),
OBJECT(23,5257,1592,5046,0,90,0,0x0, bhvTree),
OBJECT(208,800,-556,2294,0,0,0,0xf000000, bhvStarDoor),
OBJECT(209,955,-556,2294,0,180,0,0xf000000, bhvStarDoor),
OBJECT(31,4005,-773,1659,0,0,0,0x40a0000, bhvDoor),
OBJECT(208,-33,-556,-630,0,90,0,0x5000000, bhvStarDoor),
OBJECT(208,-33,-556,-788,0,270,0,0x5000000, bhvStarDoor),
OBJECT(29,955,1319,2294,0,180,0,0x0, bhvDoor),
OBJECT(29,810,1319,2294,0,0,0,0x0, bhvDoor),
OBJECT(221,5076,2196,-4541,0,270,0,0x52000000, bhvToadMessage),
OBJECT(221,4850,-969,-294,0,299,0,0x2a000000, bhvToadMessage),
OBJECT(221,-4856,-353,-379,0,135,0,0x2b000000, bhvToadMessage),
OBJECT(221,-3915,-353,688,0,90,0,0x2e000000, bhvToadMessage),
OBJECT(221,1462,-556,2275,0,180,0,0x30000000, bhvToadMessage),
OBJECT(221,1518,-480,-1614,0,270,0,0x31000000, bhvToadMessage),
OBJECT(221,1482,-556,-405,0,174,0,0x32000000, bhvToadMessage),
OBJECT(120,3818,-721,3592,0,0,0,0x0, bhvRecoveryHeart),
OBJECT(129,-5200,-353,576,0,0,0,0x10000, bhvBreakableBox),
OBJECT(129,-5200,-353,376,0,0,0,0x0, bhvBreakableBox),
OBJECT(129,-5200,-153,467,0,0,0,0x0, bhvBreakableBox),
OBJECT(137,272,-208,-2659,0,0,0,0x40000, bhvExclamationBox),
OBJECT(0,-3087,-491,358,0,0,0,0x0, bhvAirborneWarp),
OBJECT(144,1690,-504,-700,0,0,0,0x0, bhvFlame),
OBJECT(124,186,-556,2860,0,270,0,0x80000, bhvMessagePanel),
OBJECT(124,3787,-773,1609,0,180,0,0x670000, bhvMessagePanel),
OBJECT(0,3766,-781,-401,0,180,0,0x520000, bhvDeathWarp),
OBJECT(0,4023,-625,2238,0,0,0,0x710000, bhvDeathWarp),
OBJECT(0,-3078,-289,-3966,0,180,0,0x620000, bhvDeathWarp),
OBJECT(0,864,1491,2682,0,0,0,0x630000, bhvDeathWarp),
OBJECT(0,856,-233,285,0,0,0,0x640000, bhvDeathWarp),
OBJECT(0,906,-350,2607,0,0,0,0x650000, bhvDeathWarp),
OBJECT(0,4732,1709,5781,0,90,0,0x660000, bhvDeathWarp),
OBJECT(0,4023,-625,2238,0,0,0,0x610000, bhvAirborneStarCollectWarp),
OBJECT(0,-3078,-289,-3966,0,180,0,0x720000, bhvAirborneStarCollectWarp),
OBJECT(0,864,1491,2682,0,0,0,0x730000, bhvAirborneStarCollectWarp),
OBJECT(0,856,-233,285,0,0,0,0x740000, bhvAirborneStarCollectWarp),
OBJECT(0,906,-350,2607,0,0,0,0x750000, bhvAirborneStarCollectWarp),
OBJECT(0,4732,1709,5781,0,90,0,0x760000, bhvAirborneStarCollectWarp),
OBJECT(0,3802,-781,-417,0,180,0,0x510000, bhvAirborneStarCollectWarp),
RETURN()
};
const LevelScript local_warps_castle_grounds_1_[] = {
WARP_NODE(1,24,1,10,0),
WARP_NODE(0,9,1,10,0),
WARP_NODE(81,0,0,0,0),
WARP_NODE(82,0,0,0,0),
WARP_NODE(97,0,0,0,0),
WARP_NODE(113,0,0,0,0),
WARP_NODE(98,0,0,0,0),
WARP_NODE(114,0,0,0,0),
WARP_NODE(0,0,0,0,0),
WARP_NODE(99,0,0,0,0),
WARP_NODE(115,0,0,0,0),
WARP_NODE(100,0,0,0,0),
WARP_NODE(116,0,0,0,0),
WARP_NODE(101,0,0,0,0),
WARP_NODE(117,0,0,0,0),
WARP_NODE(102,0,0,0,0),
WARP_NODE(118,0,0,0,0),
WARP_NODE(20,26,1,10,0),
WARP_NODE(10,26,1,9,0),
WARP_NODE(241,16,1,241,0),
WARP_NODE(240,16,1,240,0),
WARP_NODE(243,16,1,243,0),
WARP_NODE(10,26,1,0,0),
WARP_NODE(11,16,1,12,0),
WARP_NODE(12,16,1,11,0),
WARP_NODE(0,9,1,10,0),
WARP_NODE(1,24,1,10,0),
WARP_NODE(2,17,1,10,0),
WARP_NODE(3,29,1,10,0),
WARP_NODE(4,5,1,10,0),
WARP_NODE(5,4,1,10,0),
WARP_NODE(6,12,1,10,0),
RETURN()
};