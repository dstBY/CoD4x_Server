#include "bg_pmove.hpp"
#include "null_client.hpp"
#include "sv_world.hpp"
#include "g_active.hpp"

extern "C"
{
    viewLerpWaypoint_s viewLerp_CrouchStand[] =
    {
        { 0, 40.0, 0 },
        { 5, 40.5, 0 },
        { 10, 41.5, 0 },
        { 20, 44.0, 0 },
        { 70, 56.0, 0 },
        { 96, 58.5, 0 },
        { 99, 59.5, 0 },
        { 100, 60.0, 000 },
        { -1, 0.0, 0 },
    };
    const int viewLerp_CrouchStandCount = sizeof(viewLerp_CrouchStand) / sizeof(viewLerpWaypoint_s);


    viewLerpWaypoint_s viewLerp_ProneCrouch[] =
    {
        { 0, 11.0, 0 },
        { 5, 10.0, 0 },
        { 30, 21.0, 0 },
        { 50, 25.0, 0 },
        { 67, 31.0, 0 },
        { 83, 34.0, 0 },
        { 100, 40.0, 0 },
        { -1, 0.0, 0 }
    };
    const int viewLerp_ProneCrouchCount = sizeof(viewLerp_ProneCrouch) / sizeof(viewLerpWaypoint_s);


    viewLerpWaypoint_s viewLerp_StandCrouch[] =
    {
        { 0, 60.0, 0 },
        { 1, 59.5, 0 },
        { 4, 58.5, 0 },
        { 30, 56.0, 0 },
        { 80, 44.0, 0 },
        { 90, 41.5, 0 },
        { 95, 40.5, 0 },
        { 100, 40.0, 0 },
        { -1, 0.0, 0 }
    };
    const int viewLerp_StandCrouchCount = sizeof(viewLerp_StandCrouch) / sizeof(viewLerpWaypoint_s);


    viewLerpWaypoint_s viewLerp_CrouchProne[] =
    {
        { 0, 40.0, 0 },
        { 11, 38.0, 0 },
        { 22, 33.0, 0 },
        { 34, 25.0, 0 },
        { 45, 16.0, 0 },
        { 50, 15.0, 0 },
        { 55, 16.0, 0 },
        { 70, 18.0, 0 },
        { 90, 17.0, 0 },
        { 100, 11.0, 0 },
        { -1, 0.0, 0 }
    };
    const int viewLerp_CrouchProneCount = sizeof(viewLerp_CrouchProne) / sizeof(viewLerpWaypoint_s);


    pmoveHandler_t pmoveHandlers[] =
    {
        { CG_TraceCapsule, nullptr },
        { G_TraceCapsule, G_PlayerEvent }
    };

    float CorrectSolidDeltas[26][3] =
    {
        { 0.0, 0.0, 1.0 },
        { -1.0, 0.0, 1.0 },
        { 0.0, -1.0, 1.0 },
        { 1.0, 0.0, 1.0 },
        { 0.0, 1.0, 1.0 },
        { -1.0, 0.0, 0.0 },
        { 0.0, -1.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, -1.0 },
        { -1.0, 0.0, -1.0 },
        { 0.0, -1.0, -1.0 },
        { 1.0, 0.0, -1.0 },
        { 0.0, 1.0, -1.0 },
        { -1.0, -1.0, 1.0 },
        { 1.0, -1.0, 1.0 },
        { 1.0, 1.0, 1.0 },
        { -1.0, 1.0, 1.0 },
        { -1.0, -1.0, 0.0 },
        { 1.0, -1.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { -1.0, 1.0, 0.0 },
        { -1.0, -1.0, -1.0 },
        { 1.0, -1.0, -1.0 },
        { 1.0, 1.0, -1.0 },
        { -1.0, 1.0, -1.0 },
    };


    scriptAnimMoveTypes_t notMovingAnims[3][3] =
    {
        { ANIM_MT_IDLE, ANIM_MT_TURNRIGHT, ANIM_MT_TURNLEFT },
        { ANIM_MT_IDLEPRONE, ANIM_MT_IDLEPRONE, ANIM_MT_IDLEPRONE },
        { ANIM_MT_IDLECR, ANIM_MT_TURNRIGHTCR, ANIM_MT_TURNLEFTCR}
    };

    float bobFactorTable[6][2] =
    {
        { 0.335f, 0.305f },
        { 0.25f, 0.24f },
        { 0.34f, 0.315f },
        { 0.36f, 0.325f },
        { 0.25f, 0.24f },
        { 0.34f, 0.315f }
    };


    scriptAnimMoveTypes_t moveAnimTable[6][2][2] =
    {
        {
            {ANIM_MT_RUN, ANIM_MT_STUMBLE_FORWARD},
            {ANIM_MT_WALK, ANIM_MT_STUMBLE_WALK_FORWARD},
        },
        {
            {ANIM_MT_WALKPRONE, ANIM_MT_WALKPRONE},
            {ANIM_MT_WALKPRONE, ANIM_MT_WALKPRONE},
        },
        {
            {ANIM_MT_RUNCR, ANIM_MT_STUMBLE_CROUCH_FORWARD},
            {ANIM_MT_WALKCR, ANIM_MT_STUMBLE_CROUCH_FORWARD},
        },
        {
            {ANIM_MT_RUNBK, ANIM_MT_STUMBLE_BACKWARD},
            {ANIM_MT_WALKBK, ANIM_MT_STUMBLE_WALK_BACKWARD},
        },
        {
            {ANIM_MT_WALKPRONEBK, ANIM_MT_WALKPRONEBK},
            {ANIM_MT_WALKPRONEBK, ANIM_MT_WALKPRONEBK},
        },
        {
            {ANIM_MT_RUNCRBK, ANIM_MT_STUMBLE_CROUCH_BACKWARD},
            {ANIM_MT_WALKCRBK, ANIM_MT_STUMBLE_CROUCH_BACKWARD},
        }
    };

    // No idea what this data is but looks like it's referenced in ASM code.
    // It was concatenated to moveAnimTable but it quite long.
    // So since I cut off moveAnimTableData, I also patched BG_CalculateViewAngles_530
    // label in _bg_weapons.asm offset. There was moveAnimTable+0x2a0, now
    // moveAnimTable_unknownData + (0x2a0 - sizeof(moveAnimTable)).
    unsigned int moveAnimTable_unknownData[] =
    {
        0x7fffffff, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0, 0x7fffffff,
        0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0, 0x7fffffff, 0x0, 0x0,
        0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x7fffffff, 0x0, 0x0, 0x0,
        0x80000000, 0x0, 0x0, 0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x80000000,
        0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0,
        0x0, 0x80000000, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0,
        0x7fffffff, 0x0, 0x0, 0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x80000000,
        0x0, 0x0, 0x0, 0x3546c, 0x35a24, 0x356e3, 0x35d9d, 0x35b3b,
        0x353a5, 0x35569, 0x3546c, 0x35a24, 0x0, 0x0, 0x0, 0x80000000,
        0x0, 0x0, 0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0,
        0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0,
        0x80000000, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0, 0x0, 0x7fffffff,
        0x0, 0x0, 0x0, 0x3d116, 0x3d39b, 0x3d39b, 0x3d836, 0x3d836,
        0x3d116, 0x3d116, 0x3d412, 0x3d412, 0x3d64a, 0x3d64a, 0x3d39b,
        0x3daaa, 0x3d75b, 0x3d383, 0x3d7bc, 0x3d7ca, 0x3d535, 0x3d5c0,
        0x3d607, 0x3d383, 0x3d80e, 0x3d3d4, 0x3c940, 0x3d383, 0x3d3c8,
        0x3d3c8, 0x0, 0x80000000, 0x0, 0x0, 0x0, 0x80000000, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x41177, 0x411d5, 0x41236, 0x41297,
        0x413ba, 0x412f8, 0x41359, 0x4141b, 0x4147c, 0x41700, 0x41761,
        0x415b1, 0x415dd, 0x41884, 0x418e5, 0x414ef, 0x41550, 0x417c2,
        0x41823, 0x4163e, 0x4169f, 0x41946, 0x41a05, 0x0, 0x42269,
        0x42270, 0x422af, 0x422af, 0x42277, 0x42277, 0x42265, 0x42265,
        0x42265, 0x42265, 0x42265, 0x42265, 0x42265, 0x42265, 0x42265,
        0x42265, 0x4227e, 0x4227e, 0x42265, 0x42265, 0x42265, 0x42285,
        0x42285, 0x42285, 0x4228c, 0x4228c, 0x42265, 0x42293, 0x4229a,
        0x422a1, 0x422a8, 0x422a8, 0x422af, 0x422af, 0x42265, 0x42265,
        0x42265, 0x422b6, 0x422b6, 0x0, 0x0, 0x80000000, 0x0, 0x0,
        0x42ffd, 0x4303b, 0x4307a, 0x4309b, 0x430bc, 0x430e3, 0x42de6,
        0x42de6, 0x42de6, 0x42de6, 0x42de6, 0x42de6, 0x42de6, 0x42de6,
        0x42de6, 0x42de6, 0x43104, 0x42f7c, 0x42de6, 0x42de6, 0x42de6,
        0x42f87, 0x42f9a, 0x42fc7, 0x42fe1, 0x43129, 0x42de6, 0x4310f,
        0x4311c, 0x42f31, 0x42f73, 0x42f3e, 0x42f59, 0x42f05, 0x42de6,
        0x42de6, 0x42de6, 0x42daf, 0x42edb, 0x42d8f, 0x42d8f, 0x42d8f,
        0x42d8f, 0x42d8f, 0x42d8f, 0x42c8d, 0x42c8d, 0x42c8d, 0x42c8d,
        0x42c8d, 0x42d0b, 0x42d0b, 0x42c8d, 0x42c8d, 0x42c8d, 0x42d8f,
        0x42d8f, 0x42c8d, 0x42c8d, 0x42c8d, 0x42d8f, 0x42d8f, 0x42d8f,
        0x42d8f, 0x42d65, 0x42c8d, 0x42d8f, 0x42d8f, 0x42d8f, 0x42d73,
        0x42d73, 0x42d8f, 0x42d8f, 0x42c8d, 0x42c8d, 0x42c8d, 0x42d8f,
        0x42d8f, 0x42ea1, 0x42ea8, 0x42e7e, 0x42e7e, 0x42e85, 0x42e85,
        0x42e40, 0x42e40, 0x42e40, 0x42e40, 0x42e40, 0x42e40, 0x42e40,
        0x42e40, 0x42e40, 0x42e40, 0x42e8c, 0x42e8c, 0x42e40, 0x42e40,
        0x42e40, 0x42e93, 0x42e93, 0x42e93, 0x42e9a, 0x42e9a, 0x42e40,
        0x42eaf, 0x42eb6, 0x42ebd, 0x42ec7, 0x42ec7, 0x42e7e, 0x42e7e,
        0x42e40, 0x42e40, 0x42e40, 0x42ed1, 0x42ed1, 0x0, 0x0, 0x0,
        0x7fffffff, 0x0, 0x0, 0x0, 0x0, 0x41e00000, 0x0, 0x0, 0xffe00000,
        0x41efffff, 0x0, 0x0, 0x7fffffff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x473e2, 0x472db, 0x472db, 0x472db, 0x472db, 0x473d8,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x473ec, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x472db, 0x472db, 0x472db, 0x473f6, 0x472db, 0x472db, 0x472db,
        0x472db, 0x47400, 0x4740a, 0x472db, 0x472db, 0x472db, 0x47414,
        0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db, 0x472db,
        0x4741e, 0x472db, 0x472db, 0x472db, 0x47428, 0x472db, 0x47432,
        0x472db, 0x4743c, 0x472db, 0x47399, 0x0, 0x0, 0x0, 0x0, 0x0
    };

    vec3_t playerMins = { -15.0, -15.0, 0.0 };
    vec3_t playerMaxs = { 15.0, 15.0, 70.0 };
}
