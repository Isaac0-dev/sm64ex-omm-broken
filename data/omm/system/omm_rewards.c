#define OMM_ALL_HEADERS
#include "data/omm/omm_includes.h"
#undef OMM_ALL_HEADERS

#define OMM_REWARD_DEFINE_(x) \
[OMM_REWARD_##x] = { \
    OMM_TEXT_REWARD_##x##_NAME, \
    OMM_TEXT_REWARD_##x##_COND, \
    OMM_TEXT_REWARD_##x##_TEXT, \
}

typedef struct {
    const char *name;
    const char *cond;
    const char *text;
} OmmReward;

static OmmReward sOmmRewards[] = {
    OMM_REWARD_DEFINE_(INSTANT_CAPS),
    OMM_REWARD_DEFINE_(UNLIMITED_CAPS),
    OMM_REWARD_DEFINE_(SUMMON_YOSHI),
    OMM_REWARD_DEFINE_(SPARKLY_STARS),
    OMM_REWARD_DEFINE_(SPARKLY_SPARKLES),
    OMM_REWARD_DEFINE_(PLAYABLE_PEACH),
    OMM_REWARD_DEFINE_(PERRY_CHARGE),
    OMM_REWARD_DEFINE_(_REDACTED_),
};

u32 omm_rewards_get_count() {
    return array_length(sOmmRewards);
}

u32 omm_rewards_get_unlocked_count(bool local) {
    u32 count = 0;
    for (u32 reward = 0; reward != omm_rewards_get_count(); ++reward) {
        count += omm_rewards_is_unlocked(reward, local);
    }
    return count;
}

bool omm_rewards_is_unlocked(u32 reward, bool local) {
    switch (reward) {
        case OMM_REWARD_INSTANT_CAPS: {
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

        case OMM_REWARD_UNLIMITED_CAPS: {
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

bool omm_rewards_get(u32 reward, const char **name, const char **cond, const char **text) {
    if (reward < array_length(sOmmRewards)) {
        if (name) *name = sOmmRewards[reward].name;
        if (cond) *cond = sOmmRewards[reward].cond;
        if (text) *text = sOmmRewards[reward].text;
        return true;
    }
    return false;
}
