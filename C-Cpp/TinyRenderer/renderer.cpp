/* Import Libraries */

#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

/* Initiate Global Variables */
mat<4,4> ModelView, Viewport, Perspective;

constexpr int width  = 800;
constexpr int height = 800;

/* Colours are stored in BGRA order */
constexpr TGAColor white   = { 255, 255, 255, 255 };
constexpr TGAColor green   = {   0, 255,   0, 255 };
constexpr TGAColor red     = {   0,   0, 255, 255 };
constexpr TGAColor blue    = { 255, 128,  64, 255 };
constexpr TGAColor yellow  = {   0, 200, 255, 255 };

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Draws a line from (ax, ay) to (bx, by) */
// void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
//     bool steep = std::abs(ax-bx) < std::abs(ay-by);
//     /* If the line is steep, i.e. longer in the y-axis than x-axis, transpose the image */
//     if (steep) {
//         std::swap(ax, ay);
//         std::swap(bx, by);
//     }
//     /* Draw everything from left to right */
//     if (ax>bx) {
//         std::swap(ax, bx);
//         std::swap(ay, by);
//     }
//     int y = ay;
//     int ierror = 0;
//     for (int x = ax; x <= bx; x++) {
//         /* If transposed, de-transpose */
//         if (steep)
//             framebuffer.set(y, x, color);
//         else
//             framebuffer.set(x, y, color);
//         ierror += 2 * std::abs(by-ay);
//         if (ierror > bx - ax) {
//             y += by > ay ? 1 : -1;
//             ierror -= 2 * (bx-ax);
//         }
//     }
// }

// double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
//     return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
// }

// void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &zbuffer, TGAImage &framebuffer, TGAColor color) {
//     int bb_min_x = std::min(std::min(ax, bx), cx);
//     int bb_min_y = std::min(std::min(ay, by), cy);
//     int bb_max_x = std::max(std::max(ax, bx), cx);
//     int bb_max_y = std::max(std::max(ay, by), cy);
//     double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
//     /* Simple version of back-face culling */
//     if (total_area < 1) { return; }

// #pragma omp parallel for
//     for (int x = bb_min_x; x <= bb_max_x; x++) {
//         for (int y = bb_min_y; y <= bb_max_y; y++) {
//             double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
//             double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
//             double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
//             /* Negative barycentric co-ordinate => the pixel is outside of triangle */
//             if (alpha < 0 || beta < 0 || gamma < 0) continue;
//             unsigned char z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
//             if (z <= zbuffer.get(x, y)[0]) continue;
//             framebuffer.set(x, y, color);
//             zbuffer.set(x, y, {z});
//         }
//     }
// }

void lookat(const vec3 eye, const vec3 center, const vec3 up) {
    vec3 n = normalised(eye-center);
    vec3 l = normalised(cross(up,n));
    vec3 m = normalised(cross(n, l));
    ModelView = mat<4,4>{{{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}}} *
                mat<4,4>{{{1,0,0,-center.x}, {0,1,0,-center.y}, {0,0,1,-center.z}, {0,0,0,1}}};
}

void viewport(const int x, const int y, const int w, const int h) {
    Viewport = {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void perspective(const double f) {
    Perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

void rasterise(const vec4 clip[3], std::vector<double> &zbuffer, TGAImage &framebuffer, const TGAColor color) {
    /* Normalised co-ordinates */
    vec4 ndc[3] = { clip[0] / clip[0].w, clip[1] / clip[1].w, clip[2] / clip[2].w }; 

    /* Projected co-ordinates to the screen */
    vec2 screen[3] = { (Viewport * ndc[0]).xy(), (Viewport * ndc[1]).xy(), (Viewport * ndc[2]).xy() };

    mat<3,3> ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} }};
    /* Backface culling & discard triangles that cover less than a pixel */
    if (ABC.det()<1) return;

    auto [bb_min_x, bb_max_x] = std::minmax({ screen[0].x, screen[1].x, screen[2].x });
    auto [bb_min_y, bb_max_y] = std::minmax({ screen[0].y, screen[1].y, screen[2].y });
#pragma omp parallel for
    for (int x = std::max<int>(bb_min_x, 0); x <= std::min<int>(bb_max_x, framebuffer.width() - 1); x++) {
        std::cout << x << '\n';
        for (int y = std::max<int>(bb_min_y, 0); y <= std::min<int>(bb_max_y, framebuffer.width() - 1); y++) {
            vec3 bc = ABC.invert_transpose() * vec3{static_cast<double>(x), static_cast<double>(y), 1.};
            if (bc.x<0 || bc.y<0 || bc.z<0) { continue; }
            double z = bc * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };
            if (z <= zbuffer[x + y * framebuffer.width()]) { continue; }
            zbuffer[x + y * framebuffer.width()] = z;
            framebuffer.set(x, y, color);
        }
    }

}




int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " model.obj" << std::endl;
        return 1;
    }

    /* Camera position defined using eye, centre and up vectors */
    constexpr vec3 eye{-1,0,2};
    constexpr vec3 centre{0,0,0};
    constexpr vec3 up{0,1,0};

    lookat(eye, centre, up);
    perspective(norm(eye - centre));
    viewport(width/16, height/16, width*7/8, height*7/8);


    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<double> zbuffer(width*height, -std::numeric_limits<double>::max());

    Model model(argv[1]);
    TGAColor rnd;

    /* Iterate through all triangles */
    for (int i = 0; i < model.nfaces(); i++) {
        vec4 clip[3];
        for (int d : {0, 1, 2}) {
            vec3 v = model.vert(i, d);
            clip[d] = Perspective * ModelView * vec4{v.x, v.y, v.z, 1.};
        }

        for (int c = 0; c < 3; c++) rnd[c] = std::rand() % 255;
        rasterise(clip, zbuffer, framebuffer, rnd);
    }



    framebuffer.write_tga_file("framebuffer.tga");
    // zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}