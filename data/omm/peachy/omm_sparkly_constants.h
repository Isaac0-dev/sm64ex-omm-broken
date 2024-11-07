#ifndef OMM_SPARKLY_CONSTANTS_H
#define OMM_SPARKLY_CONSTANTS_H

#define OMM_SPARKLY_STARS_MAX                           (90)

#define OMM_SPARKLY_MODE_DISABLED                       (0)
#define OMM_SPARKLY_MODE_NORMAL                         (1)
#define OMM_SPARKLY_MODE_HARD                           (2)
#define OMM_SPARKLY_MODE_LUNATIC                        (3)
#define OMM_SPARKLY_MODE_COUNT                          (3)

#define OMM_SPARKLY_STATE_OK                            (0)
#define OMM_SPARKLY_STATE_FAIL                          (1)
#define OMM_SPARKLY_STATE_INVALID                       (2)

#define OMM_SPARKLY_ENDING_REGULAR                      (0)
#define OMM_SPARKLY_ENDING_BAD                          (1)
#define OMM_SPARKLY_ENDING_PEACH                        (2)

#define OMM_SPARKLY_HINT_NEVER                          (0)
#define OMM_SPARKLY_HINT_ALWAYS                         (1)
#define OMM_SPARKLY_HINT_AREA_1                         (2)
#define OMM_SPARKLY_HINT_AREA_2                         (3)
#define OMM_SPARKLY_HINT_AREA_3                         (4)
#define OMM_SPARKLY_HINT_AREA_4                         (5)

// Requirements
#define OMM_SPARKLY_DATA_COINS                          (0x0000000000FF0000llu)     // Number of coins to collect
#define OMM_SPARKLY_DATA_RED_COINS                      (0x00000000FF000000llu)     // Number of red coins to collect
#define OMM_SPARKLY_DATA_MUSHROOMS                      (0x0000000F00000000llu)     // Number of mushrooms to collect
#define OMM_SPARKLY_DATA_CONDITION                      (0x0000001000000000llu)     // Extra requirements for the Star to spawn

// Restrictions
#define OMM_SPARKLY_DATA_BUTTONS                        (0x000000000000FFFFllu)     // Buttons that should not be pressed
#define OMM_SPARKLY_DATA_ONLY_COIN_Y                    (0x0000002000000000llu)     // Collect only yellow coins
#define OMM_SPARKLY_DATA_ONLY_COIN_R                    (0x0000004000000000llu)     // Collect only red coins
#define OMM_SPARKLY_DATA_ONLY_COIN_B                    (0x0000008000000000llu)     // Collect only blue coins
#define OMM_SPARKLY_DATA_NO_MUSHROOM                    (0x0000010000000000llu)     // Don't collect any mushroom
#define OMM_SPARKLY_DATA_NO_CAPTURE                     (0x0000020000000000llu)     // Don't capture anything
#define OMM_SPARKLY_DATA_UNCAPTURE                      (0x0000040000000000llu)     // Leave capture before starting
#define OMM_SPARKLY_DATA_NO_VIBE                        (0x0000080000000000llu)     // Don't use Vibes
#define OMM_SPARKLY_DATA_NO_VIBE_AUTO_REFILL            (0x0000100000000000llu)     // Vibe gauge doesn't refill automatically
#define OMM_SPARKLY_DATA_WING_CAP                       (0x0000200000000000llu)     // Start with the Wing Cap
#define OMM_SPARKLY_DATA_VANISH_CAP                     (0x0000400000000000llu)     // Start with the Vanish Cap
#define OMM_SPARKLY_DATA_METAL_CAP                      (0x0000800000000000llu)     // Start with the Metal Cap
#define OMM_SPARKLY_DATA_1_HEALTH                       (0x0001000000000000llu)     // Health capped at 1
#define OMM_SPARKLY_DATA_3_HEALTH                       (0x0002000000000000llu)     // Health capped at 3 (4 with Classic Moveset)

// HUD elements
#define OMM_SPARKLY_DATA_DARK_MODE                      (0x0100000000000000llu)     // Hide the surroundings in darkness
#define OMM_SPARKLY_DATA_SHARDS                         (0x0200000000000000llu)     // Display remaining number of Star shards
#define OMM_SPARKLY_DATA_FLAMES                         (0x0400000000000000llu)     // Display remaining number of flames
#define OMM_SPARKLY_DATA_BOXES                          (0x0800000000000000llu)     // Display remaining number of exclamation boxes
#define OMM_SPARKLY_DATA_RINGS                          (0x1000000000000000llu)     // Display remaining number of Star rings
#define OMM_SPARKLY_DATA_ENEMIES                        (0x2000000000000000llu)     // Display remaining number of enemies

#define OMM_SPARKLY_DATA_GET_BUTTONS(flags)             (((flags) & OMM_SPARKLY_DATA_BUTTONS)   >>  0llu)
#define OMM_SPARKLY_DATA_GET_COINS(flags)               (((flags) & OMM_SPARKLY_DATA_COINS)     >> 16llu)
#define OMM_SPARKLY_DATA_GET_RED_COINS(flags)           (((flags) & OMM_SPARKLY_DATA_RED_COINS) >> 24llu)
#define OMM_SPARKLY_DATA_GET_MUSHROOMS(flags)           (((flags) & OMM_SPARKLY_DATA_MUSHROOMS) >> 32llu)
#define OMM_SPARKLY_DATA_SET_BUTTONS(value)             ((((u64) value) <<  0llu) & OMM_SPARKLY_DATA_BUTTONS  )
#define OMM_SPARKLY_DATA_SET_COINS(value)               ((((u64) value) << 16llu) & OMM_SPARKLY_DATA_COINS    )
#define OMM_SPARKLY_DATA_SET_RED_COINS(value)           ((((u64) value) << 24llu) & OMM_SPARKLY_DATA_RED_COINS)
#define OMM_SPARKLY_DATA_SET_MUSHROOMS(value)           ((((u64) value) << 32llu) & OMM_SPARKLY_DATA_MUSHROOMS)
#define OMM_SPARKLY_DATA_CAPS                           (OMM_SPARKLY_DATA_WING_CAP | OMM_SPARKLY_DATA_VANISH_CAP | OMM_SPARKLY_DATA_METAL_CAP)

#define OMM_SPARKLY_BLOCK_GEO                           (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_block, omm_geo_sparkly_star_2_block, omm_geo_sparkly_star_3_block })
#define OMM_SPARKLY_BLOCK_BHV                           (array_of(const BehaviorScript *) { NULL, bhvOmmSparklyStarBlock1, bhvOmmSparklyStarBlock2, bhvOmmSparklyStarBlock3 })
#define OMM_SPARKLY_STAR_GEO_OPAQUE                     (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_opaque, omm_geo_sparkly_star_2_opaque, omm_geo_sparkly_star_3_opaque })
#define OMM_SPARKLY_STAR_GEO_TRANSPARENT                (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_transparent, omm_geo_sparkly_star_2_transparent, omm_geo_sparkly_star_3_transparent })
#define OMM_SPARKLY_SPARKLE_GEO                         (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_sparkle, omm_geo_sparkly_star_2_sparkle, omm_geo_sparkly_star_3_sparkle })
#define OMM_SPARKLY_HINT_GEO                            (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_hint, omm_geo_sparkly_star_2_hint, omm_geo_sparkly_star_3_hint })
#define OMM_SPARKLY_BOX_GEO                             (array_of(const GeoLayout *) { NULL, omm_geo_sparkly_star_1_box, omm_geo_sparkly_star_2_box, omm_geo_sparkly_star_3_box })
#define OMM_SPARKLY_TEXT_STAR                           (array_of(const char *) { NULL, OMM_TEXT_SPARKLY_STAR_1, OMM_TEXT_SPARKLY_STAR_2, OMM_TEXT_SPARKLY_STAR_3 })
#define OMM_SPARKLY_TEXT_STARS                          (array_of(const char *) { NULL, OMM_TEXT_SPARKLY_STARS_1, OMM_TEXT_SPARKLY_STARS_2, OMM_TEXT_SPARKLY_STARS_3 })
#define OMM_SPARKLY_HUD_GLYPH                           (array_of(const char *) { NULL, OMM_TEXTURE_STAR_FULL_17, OMM_TEXTURE_STAR_FULL_18, OMM_TEXTURE_STAR_FULL_19 })
#define OMM_SPARKLY_HUD_COLOR                           (array_of(Vec3s) { { 0xFF, 0xFF, 0xFF }, { 0xFF, 0xE0, 0x80 }, { 0xC0, 0xE0, 0xFF }, { 0xFF, 0x40, 0x80 } })

#endif