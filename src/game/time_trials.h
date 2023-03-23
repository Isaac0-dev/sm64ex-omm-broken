#ifndef TIME_TRIALS_H
#define TIME_TRIALS_H

#include <stdbool.h>
#include "types.h"

#define TIME_TRIALS time_trials_enabled()

extern Gfx *gTimeTableDisplayListHead;
bool time_trials_enabled();
void time_trials_level_entry();
void time_trials_update(struct MarioState *m, bool isPaused);
bool time_trials_render_time_table();
void time_trials_render_star_select_time(s32 starIndex);

#endif // TIME_TRIALS_H
