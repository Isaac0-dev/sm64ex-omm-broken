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

#include "levels/sl/header.h"

static const LevelScript level_sl_POSTLOAD[] = {
MARIO(/*model*/ MODEL_MARIO, /*behParam*/ 0x00000001, /*beh*/ bhvMario),
LOAD_MODEL_FROM_GEO(22, warp_pipe_geo),
JUMP_LINK(script_func_global_1),
JUMP_LINK(local_area_sl_1_),
JUMP_LINK(local_area_sl_2_),
FREE_LEVEL_POOL(),
MARIO_POS(1,135,-6558,0,6464),
CALL(/*arg*/ 0, /*func*/ lvl_init_or_update),
CALL_LOOP(/*arg*/ 1, /*func*/ lvl_init_or_update),
CLEAR_LEVEL(),
SLEEP_BEFORE_EXIT(/*frames*/ 1),
EXIT(),
};

//AREA 2 LOADS
static const LevelScript level_sl_area2load[] = {
LOAD_MIO0(0xA,_ccm_skybox_mio0SegmentRomStart,_ccm_skybox_mio0SegmentRomEnd),
LOAD_MIO0(5,_group7_mio0SegmentRomStart,_group7_mio0SegmentRomEnd),
LOAD_RAW(12,_group7_geoSegmentRomStart,_group7_geoSegmentRomEnd),
LOAD_MIO0(6,_group16_mio0SegmentRomStart,_group16_mio0SegmentRomEnd),
LOAD_RAW(13,_group16_geoSegmentRomStart,_group16_geoSegmentRomEnd),
ALLOC_LEVEL_POOL(),
JUMP_LINK(script_func_global_8),
JUMP_LINK(script_func_global_17),
LOAD_MODEL_FROM_GEO(18, snow_tree_geo),
JUMP(level_sl_POSTLOAD)
};


const LevelScript level_sl_entry[] = {
INIT_LEVEL(),
LOAD_MIO0(0x07, _sl_segment_7SegmentRomStart, _sl_segment_7SegmentRomEnd),
LOAD_MIO0(        /*seg*/ 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
LOAD_MIO0(8,_common0_mio0SegmentRomStart,_common0_mio0SegmentRomEnd),
LOAD_RAW(15,_common0_geoSegmentRomStart,_common0_geoSegmentRomEnd),
JUMP_AREA(0,2,level_sl_area2load),
//AREA 1 LOADS
LOAD_MIO0(0xA,_bits_skybox_mio0SegmentRomStart,_bits_skybox_mio0SegmentRomEnd),
LOAD_MIO0(5,_group1_mio0SegmentRomStart,_group1_mio0SegmentRomEnd),
LOAD_RAW(12,_group1_geoSegmentRomStart,_group1_geoSegmentRomEnd),
LOAD_MIO0(6,_group14_mio0SegmentRomStart,_group14_mio0SegmentRomEnd),
LOAD_RAW(13,_group14_geoSegmentRomStart,_group14_geoSegmentRomEnd),
ALLOC_LEVEL_POOL(),
JUMP_LINK(script_func_global_2),
JUMP_LINK(script_func_global_15),
LOAD_MODEL_FROM_GEO(24, spiky_tree_geo),
JUMP(level_sl_POSTLOAD)
};


const LevelScript local_area_sl_1_[] = {
AREA(1,Geo_sl_1_0x19001700),
TERRAIN(col_sl_1_0xe039568),
SET_BACKGROUND_MUSIC(0,5),
TERRAIN_TYPE(0),
JUMP_LINK(local_objects_sl_1_),
JUMP_LINK(local_warps_sl_1_),
END_AREA(),
RETURN()
};
const LevelScript local_objects_sl_1_[] = {
OBJECT_WITH_ACTS(0,-4088,-1190,-6519,0,0,0,0xa0000, bhvSpinAirborneWarp,31),
OBJECT_WITH_ACTS(0,1005,-138,3903,0,0,0,0x0, bhvKoopaRaceEndpoint,31),
OBJECT_WITH_ACTS(104,-1918,-1502,-6179,0,0,0,0x30000, bhvKoopa,31),
OBJECT_WITH_ACTS(122,1026,2883,585,0,0,0,0x2000000, bhvStar,31),
OBJECT_WITH_ACTS(122,-3742,3060,2972,0,0,0,0x3000000, bhvStar,31),
OBJECT_WITH_ACTS(122,4629,885,-6264,0,0,0,0x5000000, bhvStar,31),
OBJECT_WITH_ACTS(0,2143,-1123,-868,0,0,0,0x1000000, bhvHiddenRedCoinStar,31),
OBJECT_WITH_ACTS(137,-2430,-1147,-1216,0,0,0,0x50000, bhvExclamationBox,31),
OBJECT_WITH_ACTS(217,-2083,-1512,-3802,0,0,0,0x0, bhvPushableMetalBox,31),
OBJECT_WITH_ACTS(215,-5678,-1652,4673,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,-3895,-677,-1770,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,-5053,-208,-7398,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,5449,-1458,-7382,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,-4842,-1458,-2004,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,6221,-1510,4195,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,2014,-1956,2923,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,-5888,-1868,1073,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(207,-1797,3157,4908,0,0,0,0x0, bhvFloorSwitchHiddenObjects,31),
OBJECT_WITH_ACTS(129,-729,2865,4896,0,0,0,0x0, bhvHiddenObject,31),
OBJECT_WITH_ACTS(129,1146,2656,4896,0,0,0,0x0, bhvHiddenObject,31),
OBJECT_WITH_ACTS(129,3125,2448,4896,0,0,0,0x0, bhvHiddenObject,31),
OBJECT_WITH_ACTS(122,4479,2396,4896,0,0,0,0x4000000, bhvStar,31),
OBJECT_WITH_ACTS(140,-5997,3391,4753,0,25,0,0x0, bhvBlueCoinSwitch,31),
OBJECT_WITH_ACTS(118,-5000,3360,4323,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,-4531,3011,3802,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,-3687,2747,3490,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,-2760,2615,3490,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,-1979,2296,4375,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,-365,2238,4896,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(100,6711,-1496,-6637,0,0,0,0x0, bhvPiranhaPlant,31),
OBJECT_WITH_ACTS(100,5389,-1496,-6289,0,0,0,0x0, bhvPiranhaPlant,31),
OBJECT_WITH_ACTS(100,-5500,-1520,-2531,0,0,0,0x0, bhvPiranhaPlant,31),
OBJECT_WITH_ACTS(100,5979,-1545,4521,0,0,0,0x0, bhvPiranhaPlant,31),
OBJECT_WITH_ACTS(223,4734,415,383,0,0,0,0x0, bhvChuckya,31),
OBJECT_WITH_ACTS(180,1034,-142,413,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(89,987,1892,-2823,0,0,0,0x0, bhvHeaveHo,31),
OBJECT_WITH_ACTS(107,-2115,-1536,1870,0,0,0,0x0, bhvWoodenPost,31),
OBJECT_WITH_ACTS(0,-4427,-1512,-5365,0,0,0,0x0, bhvGoombaTripletSpawner,31),
OBJECT_WITH_ACTS(0,4948,-1704,2083,0,0,0,0x0, bhvGoombaTripletSpawner,31),
OBJECT_WITH_ACTS(0,-3698,-1704,4792,0,0,0,0x0, bhvGoombaTripletSpawner,31),
OBJECT_WITH_ACTS(212,-4372,-1652,-3547,0,0,0,0x0, bhv1Up,31),
OBJECT_WITH_ACTS(129,-573,-1704,5260,0,22,0,0x10000, bhvBreakableBox,31),
OBJECT_WITH_ACTS(129,-260,-1704,5313,0,-18,0,0x10000, bhvBreakableBox,31),
OBJECT_WITH_ACTS(129,-573,-1704,5521,0,0,0,0x10000, bhvBreakableBox,31),
OBJECT_WITH_ACTS(129,-313,-1704,5573,0,13,0,0x10000, bhvBreakableBox,31),
OBJECT_WITH_ACTS(129,-417,-1504,5417,0,0,0,0x10000, bhvBreakableBox,31),
OBJECT_WITH_ACTS(0,1094,-856,-5365,0,0,0,0x0, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,3438,1340,-3021,0,90,0,0x0, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,1823,774,4896,0,90,0,0x0, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,-2448,-1512,-2656,0,0,0,0x0, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,3177,0,729,0,0,0,0x110000, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,938,878,4063,0,0,0,0x110000, bhvCoinFormation,31),
OBJECT_WITH_ACTS(24,5781,-1496,-6875,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,5990,-1496,-7396,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,6563,-1496,-7083,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,7292,-1496,-6458,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-5000,-1520,-2344,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-2448,-1536,1354,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-1615,-1536,2188,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-4896,-1512,-4635,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-3073,-1512,-5521,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-1979,-1512,-7448,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-6146,-1704,3073,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-4896,-1704,3333,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-5156,-1704,3906,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,2031,-1704,-7396,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,2813,-1704,-7292,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,2292,-1704,-6458,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,2604,-1704,-5885,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,-260,-1704,-6146,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,469,-1704,-5833,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,2708,-1704,4479,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(24,3854,-1704,3698,0,0,0,0x0, bhvTree,31),
RETURN()
};
const LevelScript local_warps_sl_1_[] = {
WARP_NODE(10,9,1,10,0),
WARP_NODE(11,9,1,12,0),
WARP_NODE(12,9,1,11,0),
WARP_NODE(13,9,1,14,0),
WARP_NODE(14,9,1,13,0),
WARP_NODE(240,6,1,62,0),
WARP_NODE(241,6,1,63,0),
WARP_NODE(0,9,1,10,0),
WARP_NODE(1,9,1,10,0),
WARP_NODE(2,9,1,10,0),
WARP_NODE(3,9,1,10,0),
WARP_NODE(4,9,1,10,0),
WARP_NODE(5,9,1,10,0),
WARP_NODE(6,9,1,10,0),
RETURN()
};
const LevelScript local_area_sl_2_[] = {
AREA(2,Geo_sl_2_0x19001700),
TERRAIN(col_sl_2_0xe03eeb8),
SET_BACKGROUND_MUSIC(0,0x28),
TERRAIN_TYPE(2),
JUMP_LINK(local_objects_sl_2_),
JUMP_LINK(local_warps_sl_2_),
END_AREA(),
RETURN()
};
const LevelScript local_objects_sl_2_[] = {
OBJECT_WITH_ACTS(0,2239,-33,-3389,0,0,0,0xa0000, bhvSpinAirborneWarp,31),
OBJECT_WITH_ACTS(120,-2125,-1373,655,0,0,0,0x0, bhvRecoveryHeart,31),
OBJECT_WITH_ACTS(120,2043,-1321,1768,0,0,0,0x0, bhvRecoveryHeart,31),
OBJECT_WITH_ACTS(124,-775,-503,98,0,142,0,0x130000, bhvMessagePanel,31),
OBJECT_WITH_ACTS(124,4700,-290,3814,0,0,0,0x270000, bhvMessagePanel,31),
OBJECT_WITH_ACTS(0,1200,2373,-3198,0,0,0,0x0, bhvHiddenStarTrigger,31),
OBJECT_WITH_ACTS(0,606,2704,-3175,0,0,0,0x0, bhvHiddenStarTrigger,31),
OBJECT_WITH_ACTS(0,948,1434,-2434,0,0,0,0x0, bhvHiddenStarTrigger,31),
OBJECT_WITH_ACTS(0,-22,2917,-2773,0,0,0,0x0, bhvHiddenStarTrigger,31),
OBJECT_WITH_ACTS(0,-522,1045,-3136,0,0,0,0x0, bhvHiddenStarTrigger,31),
OBJECT_WITH_ACTS(0,4670,-52,4494,0,0,0,0x0, bhvHiddenStar,31),
OBJECT_WITH_ACTS(122,-2275,4710,3267,0,0,0,0x1000000, bhvStar,31),
OBJECT_WITH_ACTS(122,4518,2083,3304,0,6,0,0x2000000, bhvStar,31),
OBJECT_WITH_ACTS(122,5816,2289,-6647,0,0,0,0x3000000, bhvStar,31),
OBJECT_WITH_ACTS(122,4497,-1233,3262,0,0,0,0x4000000, bhvStar,31),
OBJECT_WITH_ACTS(0,-4116,2878,-4573,0,0,0,0x5000000, bhvHiddenRedCoinStar,31),
OBJECT_WITH_ACTS(207,3653,2879,5047,0,-17,0,0x0, bhvFloorSwitchHiddenObjects,31),
OBJECT_WITH_ACTS(129,4480,1467,3173,0,8,0,0x0, bhvHiddenObject,31),
OBJECT_WITH_ACTS(129,5159,1162,-1934,0,-2,0,0x0, bhvHiddenObject,31),
OBJECT_WITH_ACTS(215,4950,1774,-2054,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,-6298,-1261,-3094,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,2067,-1387,3088,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,3690,-1387,1047,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,2515,-1387,336,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,1709,-1387,-610,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,3888,-1387,-100,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(215,620,-1387,2909,0,0,0,0x0, bhvRedCoin,31),
OBJECT_WITH_ACTS(18,5063,-399,-1254,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,4678,-399,-237,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,4869,-399,1466,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,829,-399,-5386,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,-2500,1699,-5154,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,-4622,2406,-4740,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,-3510,1699,-5965,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,3657,1077,-6137,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,4875,1589,-4957,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,3725,3429,4677,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(18,5344,2295,3454,0,0,0,0x0, bhvTree,31),
OBJECT_WITH_ACTS(223,4735,2184,738,0,0,0,0x0, bhvChuckya,31),
OBJECT_WITH_ACTS(223,4612,2169,405,0,0,0,0x0, bhvChuckya,31),
OBJECT_WITH_ACTS(223,4587,2250,2148,0,0,0,0x0, bhvChuckya,31),
OBJECT_WITH_ACTS(223,-704,3387,4628,0,0,0,0x0, bhvChuckya,31),
OBJECT_WITH_ACTS(116,-5118,-399,3718,0,0,0,0x0, bhvMoneybagHidden,31),
OBJECT_WITH_ACTS(116,4786,2062,-2715,0,0,0,0x0, bhvMoneybagHidden,31),
OBJECT_WITH_ACTS(116,4706,2062,-2745,0,0,0,0x0, bhvMoneybagHidden,31),
OBJECT_WITH_ACTS(116,2883,-367,2795,0,0,0,0x0, bhvOneCoin,31),
OBJECT_WITH_ACTS(116,-340,-354,-4106,0,0,0,0x0, bhvOneCoin,31),
OBJECT_WITH_ACTS(116,-5950,1699,-6740,0,0,0,0x0, bhvOneCoin,31),
OBJECT_WITH_ACTS(84,-3587,-503,4863,0,0,0,0x0, bhvSpindrift,31),
OBJECT_WITH_ACTS(84,-3690,-503,2198,0,0,0,0x0, bhvSpindrift,31),
OBJECT_WITH_ACTS(84,-2250,-503,-1387,0,0,0,0x0, bhvSpindrift,31),
OBJECT_WITH_ACTS(84,-4161,-503,-2329,0,0,0,0x0, bhvSpindrift,31),
OBJECT_WITH_ACTS(84,-5888,-503,-3454,0,0,0,0x0, bhvSpindrift,31),
OBJECT_WITH_ACTS(212,-6707,-1282,5846,0,0,0,0x0, bhv1Up,31),
OBJECT_WITH_ACTS(140,3613,799,-6820,0,3,0,0x0, bhvBlueCoinSwitch,31),
OBJECT_WITH_ACTS(118,5206,-372,-2317,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,5263,-372,-2491,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,5437,-372,-2435,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(118,5381,-372,-2261,0,0,0,0x0, bhvHiddenBlueCoin,31),
OBJECT_WITH_ACTS(129,-5083,-332,-481,0,0,0,0x0, bhvJumpingBox,31),
OBJECT_WITH_ACTS(137,-4594,-81,-5963,0,-2,0,0x60000, bhvExclamationBox,31),
OBJECT_WITH_ACTS(0,2683,948,4111,0,-53,0,0x0, bhvCoinFormation,31),
OBJECT_WITH_ACTS(0,2920,1176,-629,0,0,0,0x110000, bhvCoinFormation,31),
OBJECT_WITH_ACTS(130,3204,171,-889,0,0,0,0x0, bhvBreakableBoxSmall,31),
OBJECT_WITH_ACTS(180,-1541,3439,3287,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(180,-1104,3439,3275,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(180,-747,3439,3273,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(180,-741,3439,4391,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(180,-1976,3439,3282,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(180,-744,3439,3917,0,0,0,0x0, bhvFireSpitter,31),
OBJECT_WITH_ACTS(124,1900,-309,-3356,0,98,0,0x150000, bhvMessagePanel,31),
RETURN()
};
const LevelScript local_warps_sl_2_[] = {
WARP_NODE(10,9,2,10,0),
WARP_NODE(11,9,2,12,0),
WARP_NODE(12,9,2,11,0),
WARP_NODE(13,9,2,14,0),
WARP_NODE(14,9,2,13,0),
WARP_NODE(240,6,2,1,0),
WARP_NODE(241,6,2,2,0),
WARP_NODE(0,9,2,10,0),
WARP_NODE(1,9,2,10,0),
WARP_NODE(2,9,2,10,0),
WARP_NODE(3,9,2,10,0),
WARP_NODE(4,9,2,10,0),
WARP_NODE(5,9,2,10,0),
WARP_NODE(6,9,2,10,0),
RETURN()
};