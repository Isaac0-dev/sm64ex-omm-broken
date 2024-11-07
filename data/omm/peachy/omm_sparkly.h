#ifndef OMM_SPARKLY_H
#define OMM_SPARKLY_H

#include "omm_sparkly_constants.h"
#include "omm_sparkly_structs.h"
#include "omm_sparkly_functions.h"

#define OMM_SPARKLY_IS_MODE_COMPLETED           (omm_sparkly_is_completed(gOmmSparklyMode))

#define OMM_SPARKLY_BYPASS_RESTRICTIONS         (OMM_SPARKLY_MODE_IS_NORMAL && gOmmSparklyContext->starSpawned)
#define OMM_SPARKLY_BYPASS_RULES                (OMM_SPARKLY_IS_MODE_COMPLETED)
#define OMM_SPARKLY_BYPASS_BOWSER_RULES         (OMM_SPARKLY_IS_MODE_COMPLETED)
#define OMM_SPARKLY_ENABLE_PLAYER_PHYSICS       (omm_sparkly_is_completed(OMM_SPARKLY_MODE_LUNATIC) || !OMM_SPARKLY_MODE_IS_LUNATIC)
#define OMM_SPARKLY_NEBULA_CURSE                (!omm_sparkly_is_completed(OMM_SPARKLY_MODE_LUNATIC) && OMM_SPARKLY_MODE_IS_LUNATIC)

#define OMM_SPARKLY_ALLOW_CHEATS                (OMM_SPARKLY_IS_MODE_COMPLETED)
#define OMM_SPARKLY_ALLOW_WARPS                 (OMM_SPARKLY_IS_MODE_COMPLETED)
#define OMM_SPARKLY_ALLOW_CAPTURES              (OMM_SPARKLY_IS_MODE_COMPLETED || !omm_sparkly_is_bowser_4_battle())
#define OMM_SPARKLY_ALLOW_CAP_MODIFIER          (OMM_SPARKLY_IS_MODE_COMPLETED || !omm_sparkly_is_bowser_4_battle())
#define OMM_SPARKLY_ALLOW_YOSHI_SUMMON          (OMM_SPARKLY_IS_MODE_COMPLETED || !omm_sparkly_is_bowser_4_battle())
#define OMM_SPARKLY_ALLOW_VIBES                 (OMM_SPARKLY_IS_MODE_COMPLETED || !omm_sparkly_is_bowser_4_battle())

#endif
