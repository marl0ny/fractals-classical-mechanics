#ifndef _SIMULATION_
#define _SIMULATION_

#include "gl_wrappers.hpp"
#include "parameters.hpp"


struct DoublePendulumParams {
    float mass1, mass2;
    float length1, length2;
    float gravity;
};

struct RK4Frames {
    Quad ind[5];
};

struct Frames {
    TextureParams main_view_tex_params;
    TextureParams sim_tex_params;
    TextureParams sub_tex_params;
    Quad tmp0;
    RenderTarget main_render;
    RenderTarget trajectories1;
    RenderTarget trajectories2;
    Quad coords;
    Quad sub_coords;
    Quad tmp1, tmp2, tmp3;
    RK4Frames rk4;
    WireFrame double_pendulum_lines;
    WireFrame double_pendulum_points;
    WireFrame double_pendulum_circles;
    WireFrame quad_wire_frame;
    Frames(
        int window_width, int window_height,
        int sim_width, int sim_height,
        int sub_width, int sub_height);

};

struct Programs {
    uint32_t copy;
    uint32_t scale;
    uint32_t uniform_color;
    uint32_t sub_window;
    uint32_t draw_square;
    uint32_t forward_euler;
    uint32_t rk4;
    uint32_t double_pendulum_init;
    uint32_t double_pendulum_dots;
    uint32_t double_pendulum_line_view;
    uint32_t double_pendulum_points_view;
    uint32_t double_pendulum_circles_view;
    uint32_t color;
    uint32_t energy;
    Programs();
};

class Simulation {
    Programs m_programs;
    Frames m_frames;
    void draw_square_outline(sim_2d::SimParams params);
    public:
    Simulation(int window_width, int window_height, sim_2d::SimParams params);
    void time_step(sim_2d::SimParams params);
    void clear_view();
    const RenderTarget &view(sim_2d::SimParams params);
    void init_config(sim_2d::SimParams params);
};

#endif