#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS

#define OMM_REWARD_TEXT_DATA_(name) { \
OMM_TEXT_REWARD_##name##_TITLE, { \
OMM_TEXT_REWARD_##name##_LINE_0, \
OMM_TEXT_REWARD_##name##_LINE_1, \
OMM_TEXT_REWARD_##name##_LINE_2, \
OMM_TEXT_REWARD_##name##_LINE_3, \
OMM_TEXT_REWARD_##name##_LINE_4, \
OMM_TEXT_REWARD_##name##_LINE_5, \
OMM_TEXT_REWARD_##name##_LINE_6 }, { \
OMM_TEXT_REWARD_##name##_LOCKED_0, \
OMM_TEXT_REWARD_##name##_LOCKED_1, \
OMM_TEXT_REWARD_##name##_LOCKED_2, \
OMM_TEXT_REWARD_##name##_LOCKED_3 } }

typedef struct {
    const char *title;
    const char *lines[7];
    const char *locked[4];
} OmmRewardTextData;

static const OmmRewardTextData OMM_REWARDS_TEXT_DATA[] = {
    OMM_REWARD_TEXT_DATA_(WEAR_ANY_CAP),
    OMM_REWARD_TEXT_DATA_(WEAR_ANY_CAP_ANYWHERE),
    OMM_REWARD_TEXT_DATA_(SUMMON_YOSHI),
    OMM_REWARD_TEXT_DATA_(SPARKLY_STARS),
    OMM_REWARD_TEXT_DATA_(SPARKLY_SPARKLES),
    OMM_REWARD_TEXT_DATA_(PLAYABLE_PEACH),
    OMM_REWARD_TEXT_DATA_(PERRY_CHARGE),
    OMM_REWARD_TEXT_DATA_(_REDACTED_),
};

u32 omm_rewards_get_count() {
    return array_length(OMM_REWARDS_TEXT_DATA);
}

u32 omm_rewards_get_unlocked_count() {
    u32 count = 0;
    for (u32 i = 0; i != omm_rewards_get_count(); ++i) {
        count += omm_rewards_is_unlocked(i, false);
    }
    return count;
}

bool omm_rewards_is_unlocked(s32 index, bool local) {
    switch (index) {
        case OMM_REWARD_WEAR_ANY_CAP: {
            if (local) {
                return (
                    gCurrCourseNum != COURSE_NONE &&
                    !OMM_LEVEL_IS_BOWSER_FIGHT(gCurrLevelNum) &&
                    omm_save_file_get_star_flags(gCurrSaveFileNum - 1, OMM_GAME_MODE, gCurrCourseNum - 1) == omm_stars_get_level_flags(gCurrLevelNum, OMM_GAME_MODE)
                );
            }
            for (s32 courseNum = COURSE_BOB; courseNum <= COURSE_CAKE_END; ++courseNum) {
                u8 starLevelFlags = omm_stars_get_level_flags(gCourseNumToLevelNumTable[courseNum], OMM_GAME_MODE);
                if (starLevelFlags != 0) {
                    u8 starSaveFlags = omm_save_file_get_star_flags(gCurrSaveFileNum - 1, OMM_GAME_MODE, courseNum - 1);
                    if (starSaveFlags == starLevelFlags) {
                        return true;
                    }
                }
            }
        } break;

        case OMM_REWARD_WEAR_ANY_CAP_ANYWHERE: {
            return gMarioState->numStars == omm_stars_get_total_star_count(OMM_GAME_MODE);
        } break;

        case OMM_REWARD_SUMMON_YOSHI: {
            return omm_save_file_get_capture_flags(gCurrSaveFileNum - 1, OMM_GAME_MODE) == OMM_ALL_CAPTURES;
        } break;

        case OMM_REWARD_SPARKLY_STARS: {
            return omm_sparkly_is_unlocked(OMM_SPARKLY_MODE_NORMAL);
        } break;

        case OMM_REWARD_SPARKLY_SPARKLES: {
            for (s32 mode = OMM_SPARKLY_MODE_NORMAL; mode != OMM_SPARKLY_MODE_COUNT; ++mode) {
                if (omm_sparkly_is_completed(mode)) {
                    return true;
                }
            }
        } break;

        case OMM_REWARD_PLAYABLE_PEACH: {
            return omm_sparkly_is_completed(OMM_SPARKLY_MODE_NORMAL);
        } break;

        case OMM_REWARD_PERRY_CHARGE: {
            return omm_sparkly_is_completed(OMM_SPARKLY_MODE_HARD);
        } break;

        case OMM_REWARD__REDACTED_: {
            return omm_sparkly_is_completed(OMM_SPARKLY_MODE_LUNATIC);
        } break;
    }
    return false;
}

const char *omm_rewards_get_title(s32 index) {
    return OMM_REWARDS_TEXT_DATA[index].title;
}

const char *omm_rewards_get_line(s32 index, s32 line, bool locked) {
    if (locked) return OMM_REWARDS_TEXT_DATA[index].locked[line];
    return OMM_REWARDS_TEXT_DATA[index].lines[line];
}
