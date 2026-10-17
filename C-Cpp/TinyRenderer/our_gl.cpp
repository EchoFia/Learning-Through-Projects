#include <algorithm>
#include "our_gl.h"

/* Initiate Global Variables */
mat<4,4> ModelView, Viewport, Perspective;
std::vector<double> zbuffer;

void lookat(const vec3 eye, const vec3 centre, const vec3 up) {
    vec3 n = normalised(eye - centre);
    vec3 l = normalised(cross(up, n));
    vec3 m = normalised(cross(n, l));
    ModelView = mat<4,4>{{{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}}} *
                mat<4,4>{{{1,0,0,-centre.x}, {0,1,0,-centre.y}, {0,0,1,-centre.z}, {0,0,0,1}}};
}

void init_viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void init_perspective(const double f) {
    Perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

void init_zbuffer(const int width, const int height) {
    zbuffer = std::vector<double>(width * height, -1000.);
}

void rasterise(const Triangle &clip, const IShader &shader, TGAImage &framebuffer) {
    /* Normalised device co-ordinates */
    vec4 ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w }; 

    /* Projected co-ordinates to the screen */
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() };

    mat<3,3> ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} }};
    /* Backface culling & discard triangles that cover less than a pixel */
    if (ABC.det() < 1) return;

    auto [bb_min_x, bb_max_x] = std::minmax({ screen[0].x, screen[1].x, screen[2].x });
    auto [bb_min_y, bb_max_y] = std::minmax({ screen[0].y, screen[1].y, screen[2].y });

#pragma omp parallel for
    for (int x = std::max<int>(bb_min_x, 0); x <= std::min<int>(bb_max_x, framebuffer.width() - 1); x++) {
        for (int y = std::max<int>(bb_min_y, 0); y <= std::min<int>(bb_max_y, framebuffer.height() - 1); y++) {
            vec3 bc_screen = ABC.invert_transpose() * vec3{static_cast<double>(x), static_cast<double>(y), 1.};
            vec3 bc_clip = { bc_screen.x / clip[0].w, bc_screen.y / clip[1].w, bc_screen.z / clip[2].w };
            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) { continue; }
            double z = bc_screen * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };
            if (z <= zbuffer[x + y * framebuffer.width()]) { continue; }
            auto [discard, color] = shader.fragment(bc_clip);
            if (discard) { continue; }
            zbuffer[x + y * framebuffer.width()] = z;
            framebuffer.set(x, y, color);
        }
    }

}