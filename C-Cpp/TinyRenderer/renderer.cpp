#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

/* Colours are stored in BGRA order */
constexpr TGAColor white   = { 255, 255, 255, 255 };
constexpr TGAColor green   = {   0, 255,   0, 255 };
constexpr TGAColor red     = {   0,   0, 255, 255 };
constexpr TGAColor blue    = { 255, 128,  64, 255 };
constexpr TGAColor yellow  = {   0, 200, 255, 255 };

/* Draws a line from (ax, ay) to (bx, by) */
void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    /* If the line is steep, i.e. longer in the y-axis than x-axis, transpose the image */
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    /* Draw everything from left to right */
    if (ax>bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x = ax; x <= bx; x++) {
        /* If transposed, de-transpose */
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
        }
    }
}

/* Deprecated */
void scanline_triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    if (ay > by) { std::swap(ax, bx); std::swap(ay, by); }
    if (ay > cy) { std::swap(ax, cx); std::swap(ay, cy); }
    if (by > cy) { std::swap(bx, cx); std::swap(by, cy); }
    int total_height = cy - ay;

    if (ay != by) { // if the bottom half is not degenerate
        int segment_height = by - ay;
        for (int y = ay; y <= by; y++) { // sweep the horizontal line from ay to by
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = ax + ((bx - ax) * (y - ay)) / segment_height;
            line(x1, y, x2, y, framebuffer, color);
        }
    }

    if (by != cy) { // if the upper half is not degenerate
        int segment_height = cy - by;
        for (int y = by; y <= cy; y++) { // sweep the horizontal line from ay to by
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = bx + ((cx - bx) * (y - by)) / segment_height;
            line(x1, y, x2, y, framebuffer, color);
        }
    }
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int bb_min_x = std::min(std::min(ax, bx), cx);
    int bb_min_y = std::min(std::min(ay, by), cy);
    int bb_max_x = std::max(std::max(ax, bx), cx);
    int bb_max_y = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    /* Simple version of back-face culling */
    if (total_area < 1) { return; }

#pragma omp parallel for
    for (int x = bb_min_x; x <= bb_max_x; x++) {
        for (int y = bb_min_y; y <= bb_max_y; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            /* Negative barycentric co-ordinate => the pixel is outside of triangle */
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            framebuffer.set(x, y, color);
        }
    }
}

/* (v.x, v.y) is an orthogonal projection of the spacial vector (x, y, z) */
/* (x, y, z) were scaled to span from -1 to 1 in all axes */
std::tuple<int,int> project(vec3 v) {
    return { (v.x + 1.) *  width / 2,
             (v.y + 1.) * height / 2 };
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    /* Iterate through all triangles */
    TGAColor rnd;
    for (int i = 0; i < model.nfaces(); i++) {
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        for (int c = 0; c < 3; c++) rnd[c] = std::rand()%255;
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }

    /* Iterate through all vertices */
    for (int i = 0; i < model.nverts(); i++) {
        vec3 v = model.vert(i);
        auto [x, y] = project(v);
        framebuffer.set(x, y, white);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}