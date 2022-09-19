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
#include "levels/ccm/header.h"
extern u8 _ccm_segment_ESegmentRomStart[]; 
extern u8 _ccm_segment_ESegmentRomEnd[];
const LevelScript level_ccm_entry[] = {
INIT_LEVEL(),
LOAD_MIO0(0x07, _ccm_segment_7SegmentRomStart, _ccm_segment_7SegmentRomEnd),
LOAD_RAW(0x1A, _ccmSegmentRomStart, _ccmSegmentRomEnd),
LOAD_RAW(0x0E, _ccm_segment_ESegmentRomStart, _ccm_segment_ESegmentRomEnd),
LOAD_MIO0(        /*seg*/ 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
LOAD_MIO0(0xA,_SkyboxCustom20545216_skybox_mio0SegmentRomStart,_SkyboxCustom20545216_skybox_mio0SegmentRomEnd),
LOAD_MIO0(8,_common0_mio0SegmentRomStart,_common0_mio0SegmentRomEnd),
LOAD_RAW(15,_common0_geoSegmentRomStart,_common0_geoSegmentRomEnd),
LOAD_MIO0(5,_group7_mio0SegmentRomStart,_group7_mio0SegmentRomEnd),
LOAD_RAW(12,_group7_geoSegmentRomStart,_group7_geoSegmentRomEnd),
LOAD_MIO0(6,_group16_mio0SegmentRomStart,_group16_mio0SegmentRomEnd),
LOAD_RAW(13,_group16_geoSegmentRomStart,_group16_geoSegmentRomEnd),
ALLOC_LEVEL_POOL(),
MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, ccm_geo_00042C),
LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04, ccm_geo_00045C),
LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05, ccm_geo_000494),
LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06, ccm_geo_0004BC),
LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07, ccm_geo_0004E4),
LOAD_MODEL_FROM_GEO(MODEL_CCM_CABIN_DOOR,    cabin_door_geo),
LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOW_TREE,     snow_tree_geo),
LOAD_MODEL_FROM_GEO(MODEL_CCM_ROPEWAY_LIFT,  ccm_geo_0003D0),
LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOWMAN_BASE,  ccm_geo_0003F0),
LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOWMAN_HEAD,  ccm_geo_00040C),
LOAD_MODEL_FROM_GEO(22, warp_pipe_geo),
LOAD_MODEL_FROM_GEO(23, bubbly_tree_geo),
LOAD_MODEL_FROM_GEO(24, spiky_tree_geo),
LOAD_MODEL_FROM_GEO(25, snow_tree_geo),
LOAD_MODEL_FROM_GEO(31, metal_door_geo),
LOAD_MODEL_FROM_GEO(32, hazy_maze_door_geo),
LOAD_MODEL_FROM_GEO(34, castle_door_0_star_geo),
LOAD_MODEL_FROM_GEO(35, castle_door_1_star_geo),
LOAD_MODEL_FROM_GEO(36, castle_door_3_stars_geo),
LOAD_MODEL_FROM_GEO(37, key_door_geo),
LOAD_MODEL_FROM_GEO(38, castle_door_geo),
LOAD_MODEL_FROM_GEO(39, cabin_door_geo),
// LOAD_MODEL_FROM_GEO(100,0x06003754),
// LOAD_MODEL_FROM_GEO(101,0x06003874),
// LOAD_MODEL_FROM_DL(132,0x08025f08,4),
// LOAD_MODEL_FROM_DL(158,0x0302c8a0,4),
// LOAD_MODEL_FROM_DL(159,0x0302bcd0,4),
// LOAD_MODEL_FROM_DL(161,0x0301cb00,4),
// LOAD_MODEL_FROM_DL(164,0x04032a18,4),
// LOAD_MODEL_FROM_DL(201,0x080048e0,4),
// LOAD_MODEL_FROM_DL(218,0x08024bb8,4),
JUMP_LINK(script_func_global_1),
JUMP_LINK(script_func_global_8),
JUMP_LINK(script_func_global_17),
JUMP_LINK(local_area_ccm_1_),
FREE_LEVEL_POOL(),
MARIO_POS(1,135,-6558,0,6464),
CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
CLEAR_LEVEL(),
SLEEP_BEFORE_EXIT(/*frames*/ 1),
EXIT(),
};
const LevelScript local_area_ccm_1_[] = {
AREA(1,Geo_ccm_1_0x19001700),
TERRAIN(col_ccm_1_0xe03a300),
SET_BACKGROUND_MUSIC(0,8),
TERRAIN_TYPE(TERRAIN_SNOW),
JUMP_LINK(local_objects_ccm_1_),
JUMP_LINK(local_warps_ccm_1_),
END_AREA(),
RETURN()
};
const LevelScript local_objects_ccm_1_[] = {
OBJECT(0,-4574,-1082,2343,0,180,0,0xa0000, bhvSpinAirborneWarp),
OBJECT(22,630,1808,-2390,0,90,0,0xc0000, bhvWarpPipe),
OBJECT(22,-1303,-690,1688,0,180,0,0xb0000, bhvWarpPipe),
OBJECT(22,252,-12,2443,0,180,0,0xd0000, bhvWarpPipe),
OBJECT(22,3048,-2149,1057,0,0,0,0xe0000, bhvWarpPipe),
OBJECT(0,3048,-1858,1057,0,0,0,0x0, bhvHiddenStar),
OBJECT(122,4512,5386,-1188,0,0,0,0x1000000, bhvStar),
OBJECT(87,-3343,-256,-2779,0,0,0,0x3000000, bhvTuxiesMother),
OBJECT(0,-927,-377,1104,0,0,0,0x4000000, bhvHiddenRedCoinStar),
OBJECT(122,-4661,-1580,8039,0,0,0,0x2000000, bhvStar),
OBJECT(122,-2582,-2484,-4443,0,0,0,0x5000000, bhvStar),
OBJECT(87,-175,115,-4099,0,0,0,0x0, bhvSmallPenguin),
OBJECT(MODEL_PURPLE_MARBLE,4053,-2853,-249,0,0,0,0x0, bhvHiddenStarTrigger),
OBJECT(MODEL_PURPLE_MARBLE,1926,-2853,-290,0,0,0,0x0, bhvHiddenStarTrigger),
OBJECT(MODEL_PURPLE_MARBLE,1617,-2853,1016,0,0,0,0x0, bhvHiddenStarTrigger),
OBJECT(MODEL_PURPLE_MARBLE,3920,-2853,2201,0,0,0,0x0, bhvHiddenStarTrigger),
OBJECT(MODEL_PURPLE_MARBLE,1839,-2853,2233,0,0,0,0x0, bhvHiddenStarTrigger),
OBJECT(87,-4212,-165,-1639,0,0,0,0x10000, bhvSmallPenguin),
OBJECT(87,-3046,-165,-1782,0,180,0,0x10000, bhvSmallPenguin),
OBJECT(87,-4195,-165,-542,0,90,0,0x10000, bhvSmallPenguin),
OBJECT(215,7446,555,-2330,0,0,0,0x0, bhvRedCoin),
OBJECT(215,-4627,1256,-3129,0,0,0,0x0, bhvRedCoin),
OBJECT(215,-5030,-323,1390,0,0,0,0x0, bhvRedCoin),
OBJECT(215,-3030,-334,85,0,0,0,0x0, bhvRedCoin),
OBJECT(215,634,-12,2725,0,0,0,0x0, bhvRedCoin),
OBJECT(215,594,1808,-2875,0,0,0,0x0, bhvRedCoin),
OBJECT(215,4266,-624,1441,0,0,0,0x0, bhvRedCoin),
OBJECT(215,2846,-2091,153,0,0,0,0x0, bhvRedCoin),
OBJECT(84,-4882,-1161,3265,0,0,0,0x0, bhvSpindrift),
OBJECT(86,614,-12,1159,0,0,0,0x0, bhvMrBlizzard),
OBJECT(86,-1921,-690,1755,0,0,0,0x0, bhvMrBlizzard),
OBJECT(86,-4007,-1840,7872,0,0,0,0x0, bhvMrBlizzard),
OBJECT(86,7051,555,-1644,0,0,0,0x0, bhvMrBlizzard),
OBJECT(86,-5845,-165,-1850,0,0,0,0x0, bhvMrBlizzard),
OBJECT(0,1605,1808,-2836,0,0,0,0x0, bhvGoombaTripletSpawner),
OBJECT(192,-4015,-1082,1332,0,0,0,0x0, bhvGoomba),
OBJECT(192,-3948,-165,-2054,0,0,0,0x0, bhvGoomba),
OBJECT(118,1640,1808,-2340,0,0,0,0x0, bhvHiddenBlueCoin),
OBJECT(116,-4927,-165,245,0,0,0,0x0, bhvMoneybagHidden),
OBJECT(223,-1941,-690,949,0,0,0,0x0, bhvChuckya),
OBJECT(0,4270,1167,-3976,0,90,0,0x0, bhvCoinFormation),
OBJECT(0,-2629,-363,-350,0,45,0,0x0, bhvCoinFormation),
OBJECT(0,-5324,649,-703,0,12,0,0x0, bhvCoinFormation),
OBJECT(0,4965,-649,1814,0,0,0,0x0, bhvCoinFormation),
OBJECT(0,6888,607,-3235,0,0,0,0x110000, bhvCoinFormation),
OBJECT(0,2798,-1506,-2097,0,90,0,0x0, bhvCoinFormation),
OBJECT(0,-5374,-94,-734,90,0,0,0x130000, bhvCoinFormation),
OBJECT(0,545,40,1939,90,0,0,0x130000, bhvCoinFormation),
OBJECT(0,3105,3879,-136,0,5,0,0x0, bhvCoinFormation),
OBJECT(140,2122,1808,-2561,0,0,0,0x0, bhvBlueCoinSwitch),
OBJECT(118,1739,1192,-1260,0,0,0,0x0, bhvHiddenBlueCoin),
OBJECT(118,2000,604,172,0,0,0,0x0, bhvHiddenBlueCoin),
OBJECT(118,2300,287,2140,0,0,0,0x0, bhvHiddenBlueCoin),
OBJECT(25,6566,555,-2001,0,0,0,0x0, bhvTree),
OBJECT(25,-5420,-1082,2463,0,0,0,0x0, bhvTree),
OBJECT(25,-6133,-165,-756,0,0,0,0x0, bhvTree),
OBJECT(207,5568,2746,1118,0,0,0,0x0, bhvFloorSwitchHiddenObjects),
OBJECT(207,1607,395,-4215,0,270,0,0x0, bhvFloorSwitchHiddenObjects),
OBJECT(129,5250,2746,1118,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,5050,2946,1118,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,4850,3146,1118,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,4650,3346,1118,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,1080,395,-4300,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,880,595,-4300,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,680,795,-4300,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,680,995,-4100,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,680,1195,-3900,0,0,0,0x0, bhvHiddenObject),
OBJECT(129,680,1395,-3700,0,0,0,0x0, bhvHiddenObject),
OBJECT(116,4053,-2853,-249,0,0,0,0x0, bhvYellowCoin),
OBJECT(116,1926,-2853,-290,0,0,0,0x0, bhvYellowCoin),
OBJECT(116,1617,-2853,1016,0,0,0,0x0, bhvYellowCoin),
OBJECT(116,3920,-2853,2201,0,0,0,0x0, bhvYellowCoin),
OBJECT(116,1839,-2853,2233,0,0,0,0x0, bhvYellowCoin),
OBJECT(120,2959,-2083,1615,0,0,0,0x0, bhvRecoveryHeart),
OBJECT(124,2696,-2091,954,0,270,0,0x5a0000, bhvMessagePanel),
OBJECT(124,-1554,422,-3005,0,301,0,0x5b0000, bhvMessagePanel),
RETURN()
};
const LevelScript local_warps_ccm_1_[] = {
WARP_NODE(10,5,1,10,0),
WARP_NODE(11,5,1,12,0),
WARP_NODE(12,5,1,11,0),
WARP_NODE(13,5,1,13,0),
WARP_NODE(14,5,1,13,0),
WARP_NODE(240,16,1,115,0),
WARP_NODE(241,16,1,99,0),
WARP_NODE(0,9,1,10,0),
WARP_NODE(1,9,1,10,0),
WARP_NODE(2,9,1,10,0),
WARP_NODE(3,9,1,10,0),
WARP_NODE(4,9,1,10,0),
WARP_NODE(5,9,1,10,0),
WARP_NODE(6,9,1,10,0),
RETURN()
};