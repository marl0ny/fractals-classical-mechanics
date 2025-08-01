#include "simulation.hpp"
#include "pendulum_wire_frames.hpp"

static const double PI = 3.141592653589793;

static WireFrame get_quad_wire_frame() {
    return WireFrame(
        {{"position", Attribute{
            3, GL_FLOAT, false,
            0, 0}}},
        {-1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -1.0, 0.0},
        {0, 1, 2, 0, 2, 3},
        WireFrame::TRIANGLES
    );
}

Programs::Programs() {
    this->copy
        = Quad::make_program_from_path("./shaders/util/copy.frag");
    this->scale
        = Quad::make_program_from_path("./shaders/util/scale.frag");
    this->uniform_color
        = Quad::make_program_from_path("./shaders/util/uniform-color.frag");
    this->sub_window
        = Quad::make_program_from_path("./shaders/util/sub-window.frag");
    this->draw_square
        = Quad::make_program_from_path("./shaders/util/draw-square.frag");
    this->rk4 
        = Quad::make_program_from_path("./shaders/integration/rk4.frag");
    this->forward_euler 
        = Quad::make_program_from_path(
            "./shaders/integration/forward-euler.frag");
    this->double_pendulum_init
        = Quad::make_program_from_path("./shaders/double-pendulum/init.frag");
    this->double_pendulum_dots
        = Quad::make_program_from_path("./shaders/double-pendulum/dots.frag");
    this->double_pendulum_line_view
        = make_program_from_paths(
            "./shaders/double-pendulum/lines-display.vert",
            "./shaders/util/uniform-color.frag");
    this->double_pendulum_points_view
        = make_program_from_paths(
            "./shaders/double-pendulum/points-display.vert",
            "./shaders/double-pendulum/color.frag"
        );
    this->double_pendulum_circles_view
        = make_program_from_paths(
            "./shaders/double-pendulum/circles-display.vert",
            "./shaders/double-pendulum/color.frag"
        );
    this->color
         = Quad::make_program_from_path("./shaders/double-pendulum/color.frag");
    this->energy
         = Quad::make_program_from_path("./shaders/double-pendulum/energy.frag");
}

Frames::Frames(
    int window_width, int window_height,
    int sim_width, int sim_height,
    int sub_width, int sub_height) :
    main_view_tex_params(
        {
            .format=GL_RGBA16F, 
            .width=(uint32_t)window_width,
            .height=(uint32_t)window_height,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_NEAREST,
            .mag_filter=GL_NEAREST,
        }
    ),
    sim_tex_params(
        {
            .format=GL_RGBA32F, 
            .width=(uint32_t)sim_width,
            .height=(uint32_t)sim_height,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_NEAREST,
            .mag_filter=GL_NEAREST,
        }
    ),
    sub_tex_params(
        {
            .format=GL_RGBA32F, 
            .width=(uint32_t)sub_width,
            .height=(uint32_t)sub_height,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_NEAREST,
            .mag_filter=GL_NEAREST,
        }
    ),
    tmp0(Quad{main_view_tex_params}),
    main_render(RenderTarget{main_view_tex_params}),
    trajectories1(RenderTarget{{
        .format=GL_RGBA16F, 
        .width=(uint32_t)window_width/2,
        .height=(uint32_t)window_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_NEAREST,
        .mag_filter=GL_NEAREST,
    }}),
    trajectories2(RenderTarget{{
        .format=GL_RGBA16F, 
        .width=(uint32_t)window_width/2,
        .height=(uint32_t)window_height,
        .wrap_s=GL_CLAMP_TO_EDGE,
        .wrap_t=GL_CLAMP_TO_EDGE,
        .min_filter=GL_NEAREST,
        .mag_filter=GL_NEAREST,
    }}),
    coords(Quad{sim_tex_params}),
    sub_coords(Quad{sub_tex_params}),
    tmp1(Quad{sim_tex_params}),
    tmp2(Quad{sim_tex_params}),
    tmp3(Quad{sim_tex_params}),
    rk4 {
        .ind {
            Quad{sim_tex_params},
            Quad{sim_tex_params},
            Quad{sim_tex_params},
            Quad{sim_tex_params},
            Quad{sim_tex_params}
        }
    },
    double_pendulum_lines(
        get_pendulum_lines_wire_frame(
            IVec2{.ind{(int)sub_tex_params.width, (int)sub_tex_params.height}}
        )
    ),
    double_pendulum_points(
        get_pendulum_points_wire_frame(
            IVec2{.ind{(int)sub_tex_params.width, (int)sub_tex_params.height}}
        )
    ),
    double_pendulum_circles(
        get_pendulum_circles_wire_frame(
            IVec2{.ind{(int)sub_tex_params.width, (int)sub_tex_params.height}}
        )
    ),
    quad_wire_frame(get_quad_wire_frame())
    {

}

CPUIntegration::CPUIntegration() {
    this->coords = std::vector<Coord>(0);
    this->tmp_coords = std::vector<Coord>(0);
    this->rk4 = std::vector<std::vector<Coord>>(0);
    this->f_coords = std::vector<float>(0);
}

void CPUIntegration::init_config(sim_2d::SimParams params) {
    // printf("%d. %d\n", params.gridWidth, params.gridHeight);
    double min_phi1 = PI*params.minPhi1;
    double min_phi2 = PI*params.minPhi2;
    double max_phi1 = PI*params.maxPhi1;
    double max_phi2 = PI*params.maxPhi2;
    this->coords = std::vector<Coord>(
        params.gridWidth*params.gridHeight, 
        {0.0, 0.0, 0.0, 0.0}
    );
    this->tmp_coords = std::vector<Coord>(
        params.gridWidth*params.gridHeight, 
        {0.0, 0.0, 0.0, 0.0}
    );
    this->f_coords = std::vector<float>(
        params.gridWidth*params.gridHeight*4, 0.0);
    this->rk4.resize(0);
    for (int i = 0; i < 5; i++)
        this->rk4.push_back(std::vector<Coord>(
            params.gridWidth*params.gridHeight, 
            {0.0, 0.0, 0.0, 0.0}));
    for (int i = 0; i < params.gridHeight; i++) {
        for (int j = 0; j < params.gridWidth; j++) {
            int index = i*params.gridWidth + j;
            double u = double(j + 0.5)/double(params.gridWidth);
            double v = double(i + 0.5)/double(params.gridHeight);
            this->coords[index].pi1 = 0.0;
            this->coords[index].pi2 = 0.0;
            double phi1 = min_phi1 + u*(max_phi1 - min_phi1);
            double phi2 = min_phi2 + v*(max_phi2 - min_phi2);
            this->coords[index].phi1 = phi1;
            this->coords[index].phi2 = phi2;
        }
    }
}

void CPUIntegration
::compute_double_pendulum_dots(
    std::vector<Coord> &dot_coords,
    const std::vector<Coord> &coords,
    DoublePendulumParams params) {
    double mass1 = params.mass1;
    double mass2 = params.mass2;
    double length1 = params.length1;
    double length2 = params.length2;
    double gravity = params.gravity;
    for (int i = 0; i < this->coords.size(); i++) {
        Coord coord = coords[i];
        double pi1 = coord.pi1, pi2 = coord.pi2;
        double phi1 = coord.phi1, phi2 = coord.phi2;
        double m11 = (mass1 + mass2)*length1;
        double m12 = mass2*length1*length2*cos(phi1 - phi2);
        double m21 = mass2*length1*length2*cos(phi1 - phi2);
        double m22 = mass2*length2;
        double dot_phi1 
            = -m22*pi1/(m12*m21 - m22*m11) + m12*pi2/(m12*m21 - m22*m11);
        double dot_phi2
            = m21*pi1/(m12*m21 - m22*m11) - m11*pi2/(m12*m21 - m22*m11);
        double dot_pi1 = -gravity*length1*(mass1+mass2)
            *sin(phi1)+4.0*length1*length2*mass2*(0.5*mass1+0.5*mass2)
            *pow((length1*pi2*cos(phi1-phi2)-pi1),2.0)*sin(phi1-phi2)
            *cos(phi1-phi2)/pow((length1*length2*mass2
                *pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)+4.0
                *length1*length2*mass2
                *(length1*pi2*cos(phi1-phi2)-pi1)
                *(length2*mass2*pi1*cos(phi1-phi2)-pi2*(mass1+mass2))
                *sin(phi1-phi2)*pow(cos(phi1-phi2),2.0)
                /pow((length1*length2*mass2
                    *pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)
                    +2.0*length1*length2
                    *pow((length2*mass2*pi1*cos(phi1-phi2)
                    -pi2*(mass1+mass2)),2.0)
                    *sin(phi1-phi2)*cos(phi1-phi2)/pow((length1*length2*mass2
                        *pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)
                        -2.0*length1*pi2*(0.5*mass1+0.5*mass2)
                        *(length1*pi2*cos(phi1-phi2)-pi1)
                        *sin(phi1-phi2)
                        /pow((length1*length2*mass2
                            *pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)
                            -3.0*length1*pi2*(length2*mass2*pi1
                                *cos(phi1-phi2)-pi2*(mass1+mass2))
                                *sin(phi1-phi2)
                                *cos(phi1-phi2)/pow((length1*length2*mass2
                                    *pow(cos(phi1-phi2),2.0)
                                    -mass1-mass2),2.0)-3.0*length2
                                    *mass2*pi1
                                    *(length1*pi2*cos(phi1-phi2)-pi1)
                                    *sin(phi1-phi2)*cos(phi1-phi2)
                                    /pow((length1*length2*mass2
                                        *pow(cos(phi1-phi2),2.0)
                                        -mass1-mass2),2.0)-1.0
                                        *length2*pi1
                                        *(length2*mass2
                                            *pi1*cos(phi1-phi2)
                                            -pi2*(mass1+mass2))
                                            *sin(phi1-phi2)
                                            /pow((length1*length2*mass2
                                                *pow(cos(phi1-phi2),2.0)
                                                -mass1-mass2),2.0)+2.0
                                                *pi1*pi2*sin(phi1-phi2)
                                                /(length1*length2*mass2
                                                    *pow(cos(phi1-phi2),2.0)
                                                    -mass1-mass2)
                                                    -(length1*pi2*
                                                        cos(phi1-phi2)-pi1)
                                                        *(length2*mass2*pi1
                                                            *cos(phi1-phi2)
                                                        -pi2*(mass1+mass2))
                                                        *sin(phi1-phi2)
                                                        /pow((length1
                                                            *length2*mass2
                                                            *pow(
                                                                cos(phi1
                                                                    -phi2),
                                                                2.0)
                                                            -mass1
                                                            -mass2),2.0);
        double dot_pi2 
            = -gravity*length2*mass2
            *sin(phi2)-4.0*length1
            *length2*mass2*(0.5*mass1+0.5*mass2)
            *pow((length1*pi2*cos(phi1-phi2)-pi1),2.0)
            *sin(phi1-phi2)*cos(phi1-phi2)
            /pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)
            -mass1-mass2),3.0)-4.0*length1*length2*mass2
            *(length1*pi2*cos(phi1-phi2)-pi1)
            *(length2*mass2*pi1*cos(phi1-phi2)-pi2
            *(mass1+mass2))*sin(phi1-phi2)
            *pow(cos(phi1-phi2),2.0)
            /pow((length1*length2*mass2
                *pow(cos(phi1-phi2),2.0)-mass1-mass2),3.0)
                -2.0*length1*length2
                *pow((length2*mass2*pi1*cos(phi1-phi2)
                -pi2*(mass1+mass2)),2.0)*sin(phi1-phi2)
                *cos(phi1-phi2)
                /pow((length1*length2*mass2*pow(cos(phi1-phi2),2.0)
                -mass1-mass2),3.0)+2.0*length1*pi2
                *(0.5*mass1+0.5*mass2)
                *(length1*pi2*cos(phi1-phi2)-pi1)
                *sin(phi1-phi2)
                /pow((length1*length2*mass2
                    *pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)
                    +3.0*length1*pi2*(length2*mass2*pi1
                        *cos(phi1-phi2)-pi2*(mass1+mass2))
                        *sin(phi1-phi2)*cos(phi1-phi2)
                        /pow((length1*length2*mass2
                            *pow(cos(phi1-phi2),2.0)-mass1-mass2),2.0)
                            +3.0*length2*mass2*pi1
                            *(length1*pi2*cos(phi1-phi2)-pi1)
                            *sin(phi1-phi2)
                            *cos(phi1-phi2)
                            /pow((length1*length2
                                *mass2*pow(cos(phi1-phi2),2.0)
                                -mass1-mass2),2.0)
                                +1.0*length2
                                *pi1*(length2*mass2*pi1
                                    *cos(phi1-phi2)-pi2*(mass1+mass2))
                                    *sin(phi1-phi2)
                                    /pow((length1*length2
                                        *mass2*pow(cos(phi1-phi2)
                                    ,2.0)-mass1-mass2),2.0)
                                    -2.0*pi1*pi2*sin(phi1-phi2)
                                    /(length1*length2*mass2
                                        *pow(cos(phi1-phi2),2.0)
                                        -mass1-mass2)
                                        +(length1*pi2
                                            *cos(phi1-phi2)-pi1)
                                            *(length2*mass2*pi1
                                                *cos(phi1-phi2)
                                                -pi2*(mass1+mass2))
                                                *sin(phi1-phi2)
                                                /pow((length1*length2
                                                    *mass2*pow(cos(phi1-phi2)
                                                    ,2.0)-mass1-mass2),2.0);
        dot_coords[i].pi1 = dot_pi1;
        dot_coords[i].pi2 = dot_pi2;
        dot_coords[i].phi1 = dot_phi1;
        dot_coords[i].phi2 = dot_phi2;
    }
}

void CPUIntegration::rk4_time_step(
    DoublePendulumParams params, double dt
) {
    int size = this->coords.size();
    for (int i = 0; i < size; i++)
        this->rk4[0][i] = this->coords[i];
    // q1
    compute_double_pendulum_dots(
        this->rk4[1], this->coords, params);
    // q2
    for (int i = 0; i < size; i++)
        this->tmp_coords[i] = this->coords[i] + this->rk4[1][i]*(dt/2.0);
    compute_double_pendulum_dots(this->rk4[1], this->tmp_coords, params);
    // q3
    for (int i = 0; i < size; i++)
        this->tmp_coords[i] = this->coords[i] + this->rk4[2][i]*(dt/2.0);
    compute_double_pendulum_dots(this->rk4[3], this->tmp_coords, params);
    // q4
    for (int i = 0; i < size; i++)
        this->tmp_coords[i] = this->coords[i] + this->rk4[3][i]*(dt);
    compute_double_pendulum_dots(this->rk4[4], this->tmp_coords, params);
    for (int i = 0; i < size; i++)
        this->coords[i] = this->rk4[0][i] + (
            this->rk4[1][i] 
            + this->rk4[2][i]*2.0 + this->rk4[3][i]*2.0 
            + this->rk4[4][i])*(dt/6.0);
}

void CPUIntegration::transfer_to_quad(Quad &dst) {
    for (int i = 0; i < this->coords.size(); i++) {
        this->f_coords[4*i] = this->coords[i].pi1;
        this->f_coords[4*i + 1] = this->coords[i].pi2;
        this->f_coords[4*i + 2] = this->coords[i].phi1;
        this->f_coords[4*i + 3] = this->coords[i].phi2;
    }
    dst.set_pixels((float *)&this->f_coords[0]);
}


static void add2(
    Quad &dst, const Quad &a, const Quad &b, 
    uint32_t add2_program) {
    dst.draw(
        add2_program,
        {
            {"tex1", {&a}},
            {"tex2", {&b}},
        }
    );
}

static void forward_euler(
    Quad &q1, const Quad &q0, const Quad &q0_dot, 
    uint32_t forward_euler_program, float dt) {
    q1.draw(
        forward_euler_program,
        {
            {"dt", dt},
            {"qTex", &q0},
            {"qDotTex", &q0_dot},
            {"weights", Vec4{.ind{1.0, 1.0, 1.0, 1.0}}}
        }
    );
}

static void compute_double_pendulum_dots(
    Quad &dot_q, const Quad &q, uint32_t double_pendulum_dots_program,
    DoublePendulumParams params) {
    dot_q.draw(
        double_pendulum_dots_program,
        {
            {"coordinateTex", &q},
            {"mass1", params.mass1},
            {"mass2", params.mass2},
            {"length1", params.length1},
            {"length2", params.length2},
            {"gravity", params.gravity}
        }
    );
}

enum StepType {
    STEP_X=0, STEP_P
};

struct SubStepWeight {
    StepType type;
    float value;
};

static void double_pendulum_symplectic_sub_step(
    Quad &result, Quad &intermediate_quant, const Quad &input,
    Programs programs,
    DoublePendulumParams params,
    SubStepWeight step_weight, float dt
) {
    uint32_t double_pendulum_program = programs.double_pendulum_dots;
    compute_double_pendulum_dots(
        intermediate_quant, 
        input, double_pendulum_program, params);
    result.draw(
        programs.forward_euler,
        {
            {"dt", dt},
            {"qTex", &input},
            {"qDotTex", &intermediate_quant},
            {"weights", 
                (step_weight.type == STEP_X)?
                    Vec4{.ind{0.0, 0.0, 
                              step_weight.value, step_weight.value}}:
                    Vec4{.ind{step_weight.value, step_weight.value,
                               0.0, 0.0}}
            }
        }
    );
}

static void double_pendulum_rk4_time_step(
    Quad &result, 
    RK4Frames &rk4_frames, Quad &coord_intermediate,
    const Quad &coord,
    Programs programs, 
    DoublePendulumParams params, float dt) {
    uint32_t double_pendulum_program = programs.double_pendulum_dots;
    rk4_frames.ind[0].draw(
        programs.copy,
        {{"tex", &coord}});
    // q1
    compute_double_pendulum_dots(
        rk4_frames.ind[1], 
        coord, double_pendulum_program, params);
    // q2
    forward_euler(
        coord_intermediate, 
        coord, rk4_frames.ind[1], programs.forward_euler, dt/2.0);
    compute_double_pendulum_dots(
        rk4_frames.ind[2], 
        coord_intermediate, double_pendulum_program, params);
    // q3
    forward_euler(
        coord_intermediate, 
        coord, rk4_frames.ind[2], programs.forward_euler, dt/2.0);
    compute_double_pendulum_dots(
        rk4_frames.ind[3], 
        coord_intermediate, double_pendulum_program, params);
    // q4
    forward_euler(
        coord_intermediate, 
        coord, rk4_frames.ind[3], programs.forward_euler, dt);
    compute_double_pendulum_dots(
        rk4_frames.ind[4],
        coord_intermediate, double_pendulum_program, params);
    result.draw(
        programs.rk4,
        {
            {"qTex", &rk4_frames.ind[0]},
            {"qDotTex1", &rk4_frames.ind[1]},
            {"qDotTex2", &rk4_frames.ind[2]},
            {"qDotTex3", &rk4_frames.ind[3]},
            {"qDotTex4", &rk4_frames.ind[4]},
            {"dt", dt}
        }
    );
}

Simulation::Simulation(int width, int height, sim_2d::SimParams params) :
    m_programs (),
    m_frames (
        width, height, 
        params.gridWidth, params.gridHeight, 
        params.subGridWidth, params.subGridHeight),
    m_cpu_int() {
    m_frames.coords.draw(
        m_programs.double_pendulum_init,
        {
            {"minPhi1",float(PI*params.minPhi1)},
            {"maxPhi1",float(PI*params.maxPhi1)},
            {"minPhi2",float(PI*params.minPhi2)},
            {"maxPhi2",float(PI*params.maxPhi2)}
        }
    );
}

void Simulation::init_config(sim_2d::SimParams params) {
    if (!params.useGPU)
        m_cpu_int.init_config(params);
    m_frames.sim_tex_params = 
        {
            .format=GL_RGBA32F, 
            .width=(uint32_t)params.gridWidth,
            .height=(uint32_t)params.gridHeight,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_NEAREST,
            .mag_filter=GL_NEAREST,
        };
    m_frames.sub_tex_params =
        {
            .format=GL_RGBA32F, 
            .width=(uint32_t)params.subGridWidth,
            .height=(uint32_t)params.subGridHeight,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
            .min_filter=GL_NEAREST,
            .mag_filter=GL_NEAREST,
        };
    m_frames.coords.reset(m_frames.sim_tex_params);
    m_frames.coords.draw(
        m_programs.double_pendulum_init,
        {
            {"minPhi1", float(PI*params.minPhi1)},
            {"maxPhi1", float(PI*params.maxPhi1)},
            {"minPhi2", float(PI*params.minPhi2)},
            {"maxPhi2", float(PI*params.maxPhi2)}
        }
    );
    // m_frames.sub_coords.reset(m_frames.sim_tex_params);
    m_frames.tmp1.reset(m_frames.sim_tex_params);
    m_frames.tmp2.reset(m_frames.sim_tex_params);
    m_frames.tmp3.reset(m_frames.sim_tex_params);
    for (int i = 0; i < 5; i++)
        m_frames.rk4.ind[i].reset(m_frames.sim_tex_params);
    m_frames.sub_coords.reset(m_frames.sub_tex_params);
    IVec2 d_2d = IVec2{.ind{
        (int)m_frames.sub_tex_params.width,
        (int)m_frames.sub_tex_params.height}};
    m_frames.double_pendulum_lines
        = get_pendulum_lines_wire_frame(d_2d);
    m_frames.double_pendulum_points
        = get_pendulum_points_wire_frame(d_2d);
    m_frames.double_pendulum_circles
        = get_pendulum_circles_wire_frame(d_2d);
}


void Simulation::time_step(sim_2d::SimParams sim_params) {
    DoublePendulumParams params {
        .mass1=sim_params.mass1, 
        .mass2=sim_params.mass2,
        .length1=sim_params.length1,
        .length2=sim_params.length2, 
        .gravity=sim_params.gravity,
    };
    float dt = sim_params.dt;
    // std::vector<SubStepWeight> weights = {
    //     {STEP_P, 1.0}, 
    //     {STEP_X, 1.0},
    // std::vector<SubStepWeight> weights = {
    //     {STEP_P, 0.5}, 
    //     {STEP_X, 1.0},
    //     {STEP_P, 0.5}};
    // std::vector<SubStepWeight> weights = {
    //     {STEP_X, 1.0},
    //     {STEP_P, -1.0/24.0}, 
    //     {STEP_X, -2.0/3.0},
    //     {STEP_P, 3.0/4.0},
    //     {STEP_X, 2.0/3.0},
    //     {STEP_P, 7.0/24.0}
    // };
    // float third = 1.0/3.0;
    // std::vector<SubStepWeight> weights = {
    //     // 1
    //     {STEP_X, (float)(1.0/(2.0*(2.0 - pow(2.0, third))))},
    //     {STEP_P, (float)(1.0/(2.0 - pow(2.0, third)))}, 
    //     // 2
    //     {STEP_X, (float)((1.0 - pow(2.0, third))/(2.0*(2.0 - pow(2.0, third))))},
    //     {STEP_P, (float)(-pow(2.0, third)/(2.0 - pow(2.0, third)))},
    //     // 3
    //     {STEP_X, (float)((1.0 - pow(2.0, third))/(2.0*(2.0 - pow(2.0, third))))},
    //     {STEP_P, (float)(1.0/(2.0 - pow(2.0, third)))}, 
    //     // 4
    //     {STEP_X, (float)(1.0/(2.0*(2.0 - pow(2.0, third))))} 
    // };
    /* for (int k = weights.size() - 1; k >= 0; k--) {
        SubStepWeight weight = weights[k];
        double_pendulum_symplectic_sub_step(
            (k == 0)? 
                m_frames.coords: ((k % 2)? m_frames.tmp3: m_frames.tmp2),  
            m_frames.tmp1, 
            (k == (weights.size() - 1))? 
                m_frames.coords: ((k % 2)? m_frames.tmp2: m_frames.tmp3), 
            m_programs, params, weight, dt);
    }*/
    if (!sim_params.useGPU) {
        m_cpu_int.rk4_time_step(params, dt);
        return;
    }
    ::double_pendulum_rk4_time_step(
        m_frames.coords, m_frames.rk4, m_frames.tmp1, m_frames.coords,
        m_programs, params, dt);
}

void Simulation::clear_view() {
    // m_frames.main_render.clear();
    m_frames.trajectories1.clear();
}

void Simulation::draw_square_outline(sim_2d::SimParams sim_params) {
    float x_sub_width 
    = float(m_frames.sub_tex_params.width)
        /float(m_frames.sim_tex_params.width);
    float y_sub_height
        = float(m_frames.sub_tex_params.height)
            /float(m_frames.sim_tex_params.height);
    for (int i = 0; i < 4; i++) {
        m_frames.main_render.draw(
            m_programs.uniform_color,
            {
                {"color", Vec4{.ind{1.0, 1.0, 1.0, 1.0}}}
            },
            m_frames.quad_wire_frame,
            Config::viewport(
                int(sim_params.pendulumDisplayWithInitialAngles.x
                    *m_frames.main_view_tex_params.width/2.0
                    + ((i != 3)? 
                    0: x_sub_width*m_frames.main_view_tex_params.height)),
                int(sim_params.pendulumDisplayWithInitialAngles.y
                    *m_frames.main_view_tex_params.height
                    + ((i != 2)?
                    0: y_sub_height*m_frames.main_view_tex_params.height)),
                (i == 0 || i == 2)? 
                    x_sub_width*m_frames.main_view_tex_params.height: 1,
                (i == 1 || i == 3)? 
                    y_sub_height*m_frames.main_view_tex_params.height: 1
            )
        );
    }
}

const RenderTarget &Simulation::view(sim_2d::SimParams sim_params) {
    if (!sim_params.useGPU)
        m_cpu_int.transfer_to_quad(m_frames.coords);
    DoublePendulumParams params {
        .mass1=sim_params.mass1,
        .mass2=sim_params.mass2,
        .length1=sim_params.length1,
        .length2=sim_params.length2,
        .gravity=sim_params.gravity,
    };
    // m_frames.main_render.clear();
    m_frames.main_render.draw(
        m_programs.color,
        // m_programs.energy,
        // m_programs.double_pendulum_line_view,
        {
            // {"coordTex", &m_frames.coords},
            {"mass1", params.mass1},
            {"mass2", params.mass2},
            {"length1", params.length1},
            {"length2", params.length2},
            {"gravity", params.gravity},
            {"viewScale", 0.25F},
            {"viewOffset", Vec2{.ind{0.0, 0.0}}},
            {"coordFragTex", &m_frames.coords}
        },
        m_frames.quad_wire_frame,
        Config::viewport(
            0, 0,
            m_frames.main_view_tex_params.height, 
            m_frames.main_view_tex_params.height
        )
        // m_frames.double_pendulum_lines
    );
    float x_sub_width 
        = float(m_frames.sub_tex_params.width)
            /float(m_frames.sim_tex_params.width);
    float y_sub_height
         = float(m_frames.sub_tex_params.height)
            /float(m_frames.sim_tex_params.height);
    draw_square_outline(sim_params);
    m_frames.sub_coords.draw(
        m_programs.sub_window,
        {
            {"tex", &m_frames.coords},
            {"viewport", Vec4{.ind{
                sim_params.pendulumDisplayWithInitialAngles.x,
                sim_params.pendulumDisplayWithInitialAngles.y, 
                x_sub_width, 
                y_sub_height}}}
        }
    );
    m_frames.trajectories1.draw(
        // m_programs.color,
        // m_programs.energy,
        m_programs.double_pendulum_circles_view,
        {
            {"coordTex", &m_frames.sub_coords},
            {"mass1", params.mass1},
            {"mass2", params.mass2},
            {"length1", params.length1},
            {"length2", params.length2},
            {"gravity", params.gravity},
            {"viewScale", 0.4F},
            {"viewOffset", Vec2{.x=0.0, .y=0.0}},
            {"circleRadius", 0.005F},
            {"coordFragTex", &m_frames.sub_coords}
        },
        // m_frames.quad_wire_frame,
        m_frames.double_pendulum_circles
        // Config::viewport(1440, 0, 1440, 1440)
    );
    m_frames.main_render.draw(
        m_programs.copy,
        {
            {"tex", &m_frames.trajectories1}
        },
        m_frames.quad_wire_frame,
        Config::viewport(
            m_frames.main_view_tex_params.height, 0, 
            m_frames.main_view_tex_params.height,
            m_frames.main_view_tex_params.height)
    );
    m_frames.main_render.draw(
        // m_programs.color,
        // m_programs.energy,
        m_programs.double_pendulum_line_view,
        {
            {"coordTex", &m_frames.sub_coords},
            {"mass1", params.mass1},
            {"mass2", params.mass2},
            {"length1", params.length1},
            {"length2", params.length2},
            {"gravity", params.gravity},
            {"color", Vec4{.r=1.0, .g=1.0, .b=1.0, .a=1.0}},
            {"viewScale", 0.4F},
            {"viewOffset", Vec2{.x=0.0, .y=0.0}},
            {"circleRadius", 0.005F},
            {"coordFragTex", &m_frames.sub_coords}
        },
        // m_frames.quad_wire_frame,
        m_frames.double_pendulum_lines,
        Config::viewport(
            m_frames.main_view_tex_params.width/2.0, 0, 
            m_frames.main_view_tex_params.height,
            m_frames.main_view_tex_params.height)
    );
    m_frames.trajectories2.draw(
        m_programs.scale,
        {
            {"tex", &m_frames.trajectories1},
            {"scale", 0.996F}
            // {"scale", 0.9999F}
        },
        m_frames.quad_wire_frame
    );
    m_frames.trajectories1.draw(
        m_programs.copy,
        {
            {"tex", &m_frames.trajectories2},
        },
        m_frames.quad_wire_frame
    );
    return m_frames.main_render;
}

