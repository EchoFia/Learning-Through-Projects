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

/* (v.x, v.y) is an orthogonal projection of the spacial vector (x, y, z) */
/* (v.x, v.y) is scaled to span from -1 to 1 in all axes */
std::tuple<int,int> project(vec3 v) {
    return { (v.x + 1.) *  width/2,
             (v.y + 1.) * height/2 };
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    /* Iterate through all triangles */
    for (int i = 0; i < model.nfaces(); i++) {
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));
        line(ax, ay, bx, by, framebuffer, red);
        line(bx, by, cx, cy, framebuffer, red);
        line(cx, cy, ax, ay, framebuffer, red);
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