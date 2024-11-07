#ifndef OMM_OBJECT_FIELDS_H
#define OMM_OBJECT_FIELDS_H

#include "types.h"
#include "object_fields.h"

struct OmmData {
    void (*reset)(void);
    void (*reset_mario)(void);
    void (*reset_object)(void);
    bool (*read_stats)(const char *name, const char *value1, const char *value2, bool *invalid);
    void (*write_stats)();

    // Mario data
    struct {

        // State data
        struct {
            s32 ticks;
            s32 coins;
            s32 breath;
            s32 airCombo;
            f32 peakHeight;
            void *poleObject;
            struct {
                s32 state;
                s32 timer;
            } health;
            struct {
                s32 dmg;
                s32 gfx;
                s32 sfx;
            } freeze;
            struct {
                Vec3f pos;
                Vec4f vel;
                Vec3s angle;
            } previous;
        } state;

        // Grab
        struct {
            struct Object *obj;
        } grab;

        // Dialog
        struct {
            struct Object *obj;
            s32 state;
            s16 id;
            s16 turn;
            bool choice;
        } dialog;

        // Peach-only data
        struct {
            bool floated;
            s32 floatTimer;
            s32 vibeType;
            s32 vibeGauge;
            s32 vibeTimer;
            u32 vibeGfxTimer;
            s32 joySpinYaw;
            s32 perryCharge;
            bool perryBlast;
            struct Object *perry;
        } peach;

        // Wall-slide moves data
        struct {
            bool jumped;
            s16 angle;
            f32 height;
        } wallSlide;

        // Spin moves data
        struct {
            bool pressed;
            s16 yaw;
            s32 timer;
            u8 _buffer;
            u8 _counter;
            s8 _checkpoint;
            s8 _direction;
        } spin;

        // Midair spin move data
        struct {
            s32 timer;
            s32 counter;
        } midairSpin;

        // Metal-water moves data
        struct {
            s32 punchType;
            u32 jumpNext;
            s32 jumpTimer;
        } metalWater;

        // Cappy data
        struct {
            bool bounced;
            struct Object *cappy;
        } cappy;
    
        // Capture data
        struct {
            struct Object *obj;
            struct Object *prev;
            Vec3f animPos[3];
            s32 timer;
            s32 lockTimer;
            bool starDance;
            bool openDoors;
            bool firstPerson;
            f32 stickX;   // [-1, +1], positive is right
            f32 stickY;   // [-1, +1], positive is up
            f32 stickMag; // [0, 1]
            s16 stickYaw; // intended yaw
            u16 buttonPressed;
            u16 buttonDown;
#if OMM_CODE_DEBUG
            f32 hitboxRadius;
            f32 hitboxHeight;
            f32 hitboxWall;
#endif
        } capture;

        // Warp data
        struct {
            struct Object *object;
            const BehaviorScript *behavior;
            const GeoLayout *georef;
            s32 behParams;
            s32 behParams2ndByte;
            s32 state;
            s32 timer;
        } warp;

        // Sparkly stars data
        struct {
            s32 mode;
            s32 ending;
            bool starBlock;
            bool grandStar;
            bool gamePaused;
            bool timeStop;
            bool transition;
            bool marioUpdated;
            bool cheatDetected;
            struct {
                s32 currMsg;
                s32 introId;
                s32 messageId;
                s32 endingId;
                s32 counter;
            } cheats[1];
        } sparkly;
    } mario[1];

    // Object data
    struct {

        // State data
        // Fields with underscore are reserved for specific mechanics and shouldn't be modified
        // in the capture's code directly (except through appropriate macros/functions)
        struct {
            bool actionFlag;
            s32 actionState;
            s32 actionTimer;
            f32 walkDistance;
            Vec3f friction; // ground, air, ice
            struct Object *heldObj;
            u32 _properties;
            s32 _invincTimer;
            s32 _cannonTimer;
            s32 _bullyTimer;
            s32 _powerTimer;
            s32 _squishTimer;
            Vec3f _squishScale;
            Vec3f _initialHome;
            bool _camBehindMario;
        } state;

        // Cappy data
        struct {
            struct Object *object;
            union { struct { f32 tra_x, tra_y, tra_z; }; Vec3f tra; };
            union { struct { s16 rot_x, rot_y, rot_z; }; Vec3s rot; };
            f32 scale;
            bool o_gfx;
            void (*post_update)(struct Object *);
        } cappy;

        // Door data
        struct {
            Vec2f vel[2];
            s32 timer;
        } door;

        // Per object data
        union {

            // Goomba
            struct {
                struct Object *stackObj[OBJ_GOOMBA_STACK_MAX];
                s32 stackCount;
            } goomba;

            // Koopa
            struct {
                f32 scale;
            } koopa;

            // Chain chomp
            struct {
                bool isFreed;
                bool isBiting;
            } chain_chomp;

            // Bullet bill
            struct {
                f32 scale;
            } bullet_bill;

            // Snowman's body
            struct {
                f32 peakHeight;
                bool headFound;
            } snowmans_body;

            // Scuttlebug
            struct {
                struct SurfaceData data[1];
            } scuttlebug;

            // Swoop
            struct {
                struct SurfaceData data[1];
            } swoop;

            // Monty mole
            struct {
                struct Object *holes[32];
                s32 count;
                s32 current;
            } monty_mole;

            // Pokey
            struct {
                struct Object *body[OBJ_POKEY_BALLS_MAX];
                s32 parts;
                f32 top;
            } pokey;

            // Ukiki
            struct {
                struct SurfaceData data[1];
            } ukiki;

            // Toad
            struct {
                s16 dialogId;
                s32 spawnStar;
            } toad;

            // Flaming bob-omb
            struct {
                s32 interactedTimer;
                bool interactedFire;
                bool captureDuringAscent;
            } flaming_bobomb;

            // Yoshi
            struct {
                s32 flutterTimer;
                s32 tongueTimer;
                f32 tongueSine;
                struct Object *tongued;
            } yoshi;
        };
    } object[1];

    // Level data
    struct {
        s32 redCoins;
        s32 redCoinStarIndex;
        s32 secrets;
        s32 secretStarIndex;
    } level[8];

    // Stats data
    struct {

        // Objects
        u64 starsCollected;
        u64 sparklyStarsCollected;
        u64 coinsCollected;
        u64 capsCollected;
        u64 mushrooms1upCollected;
        u64 secretsCollected;
        u64 exclamationBoxesBroken;
        u64 enemiesDefeated;
        u64 bowsersDefeated;

        // Actions
        u64 aPresses;
        u64 jumps;
        u64 attacks;
        u64 cappyThrows;
        u64 cappyBounces;
        u64 captures;
        u64 hitsTaken;
        u64 restarts;
        u64 deaths;

        // Distance (Mario/Capture)
        u64 distanceTotal[2];
        u64 distanceOnGround[2];
        u64 distanceAirborne[2];
        u64 distanceUnderwater[2];
        u64 distanceWingCap[2];
        u64 distanceMetalCap[2];
        u64 distanceVanishCap[2];

        // Time (Mario/Capture)
        u64 timeTotal[2];
        u64 timeOnGround[2];
        u64 timeAirborne[2];
        u64 timeUnderwater[2];
        u64 timeWingCap[2];
        u64 timeMetalCap[2];
        u64 timeVanishCap[2];
    } stats[1];

    // Globals
    struct {
        bool cameraSnapshotMode;
        bool cameraUpdate;
        bool cameraNoInit;
        bool configNoSave;
        bool findFloorForCutsceneStar;
        bool hideHudCamera;
        bool hideHudRadar;
        bool isMirrorObj;
        bool isMirrorRoom;
        f32 mirrorRoomX;
        f32 mirrorX;
        u32 marioTimer;
        s32 mouseDeltaX;
        s32 mouseDeltaY;
        s32 mouseWheelX;
        s32 mouseWheelY;
    } globals[1];
};
extern struct OmmData gOmmData[1];
#define gOmmMario     gOmmData->mario
#define gOmmPeach   (&gOmmData->mario->peach)
#define gOmmCappy     gOmmData->mario->cappy.cappy
#define gOmmPerry     gOmmData->mario->peach.perry
#define gOmmCapture   gOmmData->mario->capture.obj
#define gOmmWarp    (&gOmmData->mario->warp)
#define gOmmObject    gOmmData->object
#define gOmmLevel     gOmmData->level
#define gOmmArea    (&gOmmData->level[gCurrAreaIndex])
#define gOmmStats     gOmmData->stats
#define gOmmGlobals   gOmmData->globals

#endif // OMM_OBJECT_FIELDS_H
