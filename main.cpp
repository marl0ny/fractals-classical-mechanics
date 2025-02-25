#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include "simulation.hpp"
#include "interactor.hpp"
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
#endif
#include <functional>

static std::function <void()> s_loop;
#ifdef __EMSCRIPTEN__
static void s_main_loop() {
    s_loop();
}
#endif

static std::function <void(int, Uniform)> s_sim_params_set;
static std::function <void(int, int, std::string)> s_sim_params_set_string;
static std::function <Uniform(int)> s_sim_params_get;
static std::function<void(int, std::string, float)> s_user_edit_set_value;
static std::function<float(int, std::string)> s_user_edit_get_value;
static std::function<std::string(int)>
    s_user_edit_get_comma_separated_variables;

void double_pendulum(
    MainGLFWQuad main_render, sim_2d::SimParams &params,
    int window_width, int window_height) {
    Interactor interactor(main_render.get_window());
    Simulation sim(window_width, window_height, params);
    s_sim_params_set = [&params, &sim](int c, Uniform u) {
        params.set(c, u);
        if (!(c == params.DT || c == params.STEPS_PER_FRAME) 
                || c == params.PENDULUM_DISPLAY_WITH_INITIAL_ANGLES) {
            sim.init_config(params);
        }
    };
    s_sim_params_get = [&params](int c) -> Uniform {
        return params.get(c);
    };
    s_loop = [&] {
        for (int i = 0; i < params.stepsPerFrame; i++) {
            if (i % 5 == 0)
                main_render.draw(sim.view(params));
            sim.time_step(params);
        }
        auto poll_events = [&] {
            interactor.click_update(main_render.get_window());
            if (interactor.left_pressed()) {
                Vec2 cursor_pos = interactor.get_mouse_position();
                params.pendulumDisplayWithInitialAngles 
                    = Vec2{.ind{cursor_pos.x*2.0F, cursor_pos.y}};
                sim.clear_view();
                main_render.draw(sim.view(params));
            }
            glfwPollEvents();
        };
        poll_events();
        glfwSwapBuffers(main_render.get_window());
    };
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(s_main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(main_render.get_window()))
        s_loop();
    #endif
}


int main(int argc, char *argv[]) {
    int window_width = 2880, window_height = 1440;

    // Construct the main window quad
    if (argc >= 3) {
        window_width = std::atoi(argv[1]);
        window_height = std::atoi(argv[2]);
    }
    auto main_quad = MainGLFWQuad(window_width, window_height);
    sim_2d::SimParams sim_params {};
    double_pendulum(main_quad, sim_params, window_width, window_height);
    return 1;
}









//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/* Setters for the simulation parameters struct, where they act
as the exposed entry point for JavaScript code in the WASM build.
There are multiple functions, one for each type. They all take as the first
argument a param_code representing each field of the parameter struct,
where these codes must be written and enumerated separately in JavaScript.
For the function setters of scalar quantities, the next and final argument is
just the quantity itself. For those that set a vector quantity,
these next arguments in order must be passed into the function: 
the number of elements the vector contains, the index of the vector to change
the value, and lastly the value itself. The actual vector structs
themselves are not passed as argument: this is to avoid the complexity of 
getting non-primitive objects to be passed between JS/C++.
*/

void set_int_param(int param_code, int i) {
    s_sim_params_set(param_code, Uniform((int)i));
}

void set_float_param(int param_code, float f) {
    s_sim_params_set(param_code, Uniform((float)f));
}

void set_bool_param(int param_code, bool b) {
    s_sim_params_set(param_code, Uniform((bool)b));
}

void set_string_param(int param_code, int index, std::string s) {
    s_sim_params_set_string(param_code, index, s);
}

// std::string send_json_string() {
//     return {"This", "is", "some", "text\n";
// }

float user_edit_get_value(int div_code, std::string variable_name) {
    return s_user_edit_get_value(div_code, variable_name);
}

void user_edit_set_value(int div_code, std::string variable_name, float value) {
    s_user_edit_set_value(div_code, variable_name, value);
}

std::string user_edit_get_comma_separated_variables(int div_code) {
    return s_user_edit_get_comma_separated_variables(div_code);
}

void set_vec_param(int param_code, int elem_count, int index, float val) {
    auto u = s_sim_params_get(param_code);
    if (elem_count == 2) {
        u.vec2[index] = val;
    } else if (elem_count == 3) {
        u.vec3[index] = val;
    } else {
        u.vec4[index] = val;
    }
    s_sim_params_set(param_code, u);
}

void set_ivec_param(int param_code, int elem_count, int index, float val) {
    auto u = s_sim_params_get(param_code);
    if (elem_count == 2) {
        u.ivec2[index] = val;
    } else if (elem_count == 3) {
        u.ivec3[index] = val;
    } else {
        u.ivec4[index] = val;
    }
    s_sim_params_set(param_code, u);
}

// void set_mouse_mode(int type) {
//     s_input_type = type;
// }

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
    function("set_float_param", set_float_param);
    function("set_int_param", set_int_param);
    function("set_bool_param", set_bool_param);
    function("set_vec_param", set_vec_param);
    function("set_ivec_param", set_ivec_param);
    // function("set_mouse_mode", set_mouse_mode);
    function("set_string_param", set_string_param);
    function("user_edit_get_value", user_edit_get_value);
    function("user_edit_set_value", user_edit_set_value);
    function("user_edit_get_comma_separated_variables",
             user_edit_get_comma_separated_variables);
}
#endif