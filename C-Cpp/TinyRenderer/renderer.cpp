/* Import Libraries */
	
#include <cstdlib>
#include "our_gl.h"
#include "model.h"

/* Initiate Global Variables */
extern mat<4,4> ModelView, Viewport, Perspective;
extern std::vector<double> zbuffer;

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

struct PhongShader : IShader {
    const Model &model;
    // TGAColor color = {};
    /* Triangle in eye co-ordinates */
    // vec3 tri[3];
    /* Triangle norms in eye co-ordinates */
    // vec3 tri_norms[3];
    /* Light direction in eye co-ordinates */
    vec4 l;
    vec2 varying_uv[3];

    PhongShader(const vec3 light, const Model &m) : model(m) {
        /* Transforms the light vector to view co-ordinates */
        // l = normalised((ModelView * vec4{light.x, light.y, light.z, 0.}).xyz());
        l = normalised((ModelView * vec4{light.x, light.y, light.z, 0.}));
    }

    virtual vec4 vertex(const int face, const int vert) {
        /* Current vertext in object co-ordinates */
        // vec3 v = model.vert(face, vert);
        // vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};

        /* Vertex in eye co-ordinates */
        // tri[vert] = gl_Position.xyz();

        // vec3 n = model.norm(face, vert);
        // tri_norms[vert] = (ModelView.invert_transpose() * vec4{n.x, n.y, n.z, 0.}).xyz();

        varying_uv[vert] = model.uv(face, vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);

        /* Vertex in clip co-ordinates */
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        // TGAColor gl_FragColor = { 255, 255, 255, 255 };
        // vec3 n = normalised(tri_norms[0] * bar[0] +
        //                     tri_norms[1] * bar[1] +
        //                     tri_norms[2] * bar[2]); 

        vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        vec4 n = normalised(ModelView.invert_transpose() * model.norm(uv));
        vec4 r = normalised(n * (n * l) * 2 - l);

        double ambient = .3;
        double diff = 1. * std::max(0., n * l);
        /* Specular exponent is set to 35 here */
        /* Camera lies on z-axis, hence r.z */
        // double spec = std::pow(std::max(r.z, 0.), 35);
        double spec = (3.*sample2D(model.specular(), uv)[0]/255.) * std::pow(std::max(r.z, 0.), 35);
        TGAColor gl_FragColor = sample2D(model.diffuse(), uv);

        for (int channel : { 0, 1, 2 }) {
            gl_FragColor[channel] = std::min(255., gl_FragColor[channel] * (ambient + diff + spec));
        }

        return {false, gl_FragColor};
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " model.obj" << std::endl;
        return 1;
    }

    /*  */
    constexpr int width  = 800;
    constexpr int height = 800;

    constexpr vec3  light{ 1, 1, 1}; // light source

    /* Camera position defined using eye, centre and up vectors */
    constexpr vec3 eye{-1,0,2};
    constexpr vec3 centre{0,0,0};
    constexpr vec3 up{0,1,0};

    lookat(eye, centre, up);
    init_perspective(norm(eye - centre));
    init_viewport(width/16, height/16, width*7/8, height*7/8);
    init_zbuffer(width, height);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    /* Iterate over all files */
    for (int m = 1; m < argc; m++) {

        Model model(argv[m]);
        PhongShader shader(light, model);

        /* Iterate through all triangles */
        for (int f = 0; f < model.nfaces(); f++) {

            // for (int c = 0; c < 3; c++) { shader.color[c] = std::rand() % 255; }
            // shader.color[3] = 255;

            /* Assemble the primitive */
            Triangle clip = { shader.vertex(f, 0), shader.vertex(f, 1), shader.vertex(f, 2) };

            rasterise(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}