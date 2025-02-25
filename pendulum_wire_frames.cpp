#include "pendulum_wire_frames.hpp"


static std::vector<int> get_lines_elements(IVec2 d_2d) {
    std::vector<int> elements {};
    for (int i = 0; i < d_2d[0]*d_2d[1]*4; i++)
        elements.push_back(i);
    return elements;
}

static std::vector<float> get_lines_vertices(IVec2 d_2d) {
    std::vector<float> vertices;
    for (int i = 0; i < d_2d[1]; i++) {
        for (int j = 0; j < d_2d[0]; j++) {
            float u = (float(j) + 0.5F)/float(d_2d[0]);
            float v = (float(i) + 0.5F)/float(d_2d[1]);
            for (int k = 0; k < 4; k++) {
                vertices.push_back(u);
                vertices.push_back(v);
                vertices.push_back(0.0);
                vertices.push_back((float)k);
            }
        }
    }
    return vertices;
}

static std::vector<float> get_points_vertices(IVec2 d_2d) {
    std::vector<float> vertices;
    for (int i = 0; i < d_2d[1]; i++) {
        for (int j = 0; j < d_2d[0]; j++) {
            float u = (float(j) + 0.5F)/float(d_2d[0]);
            float v = (float(i) + 0.5F)/float(d_2d[1]);
            for (int k = 0; k < 2; k++) {
                vertices.push_back(u);
                vertices.push_back(v);
                vertices.push_back(0.0);
                vertices.push_back((float)k);
            }
        }
    }
    return vertices;
}

#define PI 3.141592653589793

static void set_circles_vertices_elements(
    IVec2 d_2d, std::vector<float> &vertices, std::vector<int> &elements) {
    int vertex_count = 0;
    for (int i = 0; i < d_2d[1]; i++) {
        for (int j = 0; j < d_2d[0]; j++) {
            float u = (float(j) + 0.5F)/float(d_2d[0]);
            float v = (float(i) + 0.5F)/float(d_2d[1]);
            int circle_center_elem = 0;
            for (int k = 0; k < 4; k++) {
                if (k == 0 || k == 2) {
                    vertices.push_back(u);
                    vertices.push_back(v);
                    vertices.push_back(0.0);
                    vertices.push_back((float)k);
                    circle_center_elem = vertex_count;
                    vertex_count++;
                } else if (k == 1 || k == 3) {
                    std::vector<int> circle_elements1 {};
                    std::vector<int> circle_elements2 {};
                    for (int angle_ind = 0; angle_ind < 10; angle_ind++) {
                        float angle = 2.0*PI*(float)angle_ind/10.0;
                        vertices.push_back(u);
                        vertices.push_back(v);
                        vertices.push_back(angle);
                        vertices.push_back((float)k);
                        circle_elements1.push_back(vertex_count);
                        vertex_count++;
                    }
                    for (int n = 1; n < circle_elements1.size(); n++)
                        circle_elements2.push_back(circle_elements1[n]);
                    circle_elements2.push_back(circle_elements1[0]);
                    for (int n = 0; n < circle_elements1.size(); n++) {
                        elements.push_back(circle_center_elem);
                        elements.push_back(circle_elements1[n]);
                        elements.push_back(circle_elements2[n]);
                    }
                }
            }
        }
    }
}

WireFrame get_pendulum_lines_wire_frame(IVec2 d_2d) {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<float> vertices = get_lines_vertices(d_2d);
    std::vector<int> elements = get_lines_elements(d_2d);
    return WireFrame(attributes, vertices, elements, WireFrame::LINES);
}

WireFrame get_pendulum_points_wire_frame(IVec2 d_2d) {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<float> vertices = get_points_vertices(d_2d);
    return WireFrame(attributes, vertices, {}, WireFrame::POINTS);
}

WireFrame get_pendulum_circles_wire_frame(IVec2 d_2d) {
    Attributes attributes = {
        {"position", {
            .size=4, .type=GL_FLOAT, .normalized=false, .stride=0, .offset=0
    }}};
    std::vector<float> vertices {};
    std::vector<int> elements {};
    set_circles_vertices_elements(d_2d, vertices, elements);
    return WireFrame(attributes, vertices, elements, WireFrame::TRIANGLES);
}


