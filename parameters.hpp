#include "gl_wrappers.hpp"

namespace sim_2d {

#ifndef _PARAMETERS_
#define _PARAMETERS_

struct SimParams {
    bool useGPU = (bool)(true);
    int stepsPerFrame = (int)(10);
    float dt = (float)(0.001F);
    float mass1 = (float)(1.0F);
    float length1 = (float)(1.0F);
    float mass2 = (float)(1.0F);
    float length2 = (float)(1.0F);
    float gravity = (float)(9.81F);
    Vec2 pendulumDisplayWithInitialAngles = (Vec2)(Vec2 {.ind={0.5, 0.5}});
    float minPhi1 = (float)(-1.0F);
    float maxPhi1 = (float)(1.0F);
    float minPhi2 = (float)(-1.0F);
    float maxPhi2 = (float)(1.0F);
    int gridWidth = (int)(128);
    int gridHeight = (int)(128);
    int subGridWidth = (int)(1);
    int subGridHeight = (int)(1);
    enum {
        USE_G_P_U=0,
        STEPS_PER_FRAME=1,
        DT=2,
        MASS1=3,
        LENGTH1=4,
        MASS2=5,
        LENGTH2=6,
        GRAVITY=7,
        PENDULUM_DISPLAY_WITH_INITIAL_ANGLES=8,
        MIN_PHI1=9,
        MAX_PHI1=10,
        MIN_PHI2=11,
        MAX_PHI2=12,
        GRID_WIDTH=13,
        GRID_HEIGHT=14,
        SUB_GRID_WIDTH=15,
        SUB_GRID_HEIGHT=16,
    };
    void set(int enum_val, Uniform val) {
        switch(enum_val) {
            case USE_G_P_U:
            useGPU = val.b32;
            break;
            case STEPS_PER_FRAME:
            stepsPerFrame = val.i32;
            break;
            case DT:
            dt = val.f32;
            break;
            case MASS1:
            mass1 = val.f32;
            break;
            case LENGTH1:
            length1 = val.f32;
            break;
            case MASS2:
            mass2 = val.f32;
            break;
            case LENGTH2:
            length2 = val.f32;
            break;
            case GRAVITY:
            gravity = val.f32;
            break;
            case PENDULUM_DISPLAY_WITH_INITIAL_ANGLES:
            pendulumDisplayWithInitialAngles = val.vec2;
            break;
            case MIN_PHI1:
            minPhi1 = val.f32;
            break;
            case MAX_PHI1:
            maxPhi1 = val.f32;
            break;
            case MIN_PHI2:
            minPhi2 = val.f32;
            break;
            case MAX_PHI2:
            maxPhi2 = val.f32;
            break;
            case GRID_WIDTH:
            gridWidth = val.i32;
            break;
            case GRID_HEIGHT:
            gridHeight = val.i32;
            break;
            case SUB_GRID_WIDTH:
            subGridWidth = val.i32;
            break;
            case SUB_GRID_HEIGHT:
            subGridHeight = val.i32;
            break;
        }
    }
    Uniform get(int enum_val) const {
        switch(enum_val) {
            case USE_G_P_U:
            return {(bool)useGPU};
            case STEPS_PER_FRAME:
            return {(int)stepsPerFrame};
            case DT:
            return {(float)dt};
            case MASS1:
            return {(float)mass1};
            case LENGTH1:
            return {(float)length1};
            case MASS2:
            return {(float)mass2};
            case LENGTH2:
            return {(float)length2};
            case GRAVITY:
            return {(float)gravity};
            case PENDULUM_DISPLAY_WITH_INITIAL_ANGLES:
            return {(Vec2)pendulumDisplayWithInitialAngles};
            case MIN_PHI1:
            return {(float)minPhi1};
            case MAX_PHI1:
            return {(float)maxPhi1};
            case MIN_PHI2:
            return {(float)minPhi2};
            case MAX_PHI2:
            return {(float)maxPhi2};
            case GRID_WIDTH:
            return {(int)gridWidth};
            case GRID_HEIGHT:
            return {(int)gridHeight};
            case SUB_GRID_WIDTH:
            return {(int)subGridWidth};
            case SUB_GRID_HEIGHT:
            return {(int)subGridHeight};
        }
        return Uniform(0);
    }
    void set(int enum_val, int index, std::string val) {
        switch(enum_val) {
        }
    }
};
#endif
}
