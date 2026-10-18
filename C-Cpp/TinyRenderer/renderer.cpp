/* Import Libraries */
	
#include "our_gl.h"
#include "model.h"
#include <sys/stat.h>

/* Initiate Global Variables */
extern mat<4,4> ModelView, Viewport, Perspective;
extern std::vector<double> zbuffer;

constexpr int MIN_DARK = 80;
const std::string OUTPUT_DIR = "renders";
const char* C_OUTPUT_DIR = OUTPUT_DIR.c_str();

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* =-= Shaders =-= */

/* For shadow mapping */
struct BlankShader : IShader {
    const Model &model;

    BlankShader(const Model &m) : model(m) {}

    virtual vec4 vertex(const int face, const int vert) {
        vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, {255, 255, 255, 255}};
    }
};

struct PhongShader : IShader {
    const Model &model;

    /* Light direction in eye co-ordinates */
    vec4 l;

    /* Texture Values */
    vec2 varying_uv[3];

    /* Triangle norms in eye co-ordinates */
    vec4 varying_nrm[3];

    /* Triangle in eye co-ordinates */
    vec4 tri[3];

    PhongShader(const vec3 light, const Model &m) : model(m) {
        /* Transforms the light vector to view co-ordinates */
        l = normalised((ModelView * vec4{light.x, light.y, light.z, 0.}));
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face, vert);
        varying_nrm[vert] = ModelView.invert_transpose() * model.norm(face, vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);

        /* Vertex in eye co-ordinates */
        tri[vert] = gl_Position;

        /* Vertex in clip co-ordinates */
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];

        mat<2, 4> E = { tri[1] - tri[0], tri[2] - tri[0] };
        mat<2, 2> U = { varying_uv[1] - varying_uv[0], varying_uv[2] - varying_uv[0] };
        mat<2, 4> T = U.invert() * E;
        mat<4, 4> D = {normalised(T[0]),  // tangent vector
                      normalised(T[1]),  // bitangent vector
                      normalised(varying_nrm[0]*bar[0] + varying_nrm[1]*bar[1] + varying_nrm[2]*bar[2]), // interpolated normal
                      {0,0,0,1}}; // Darboux frame

        vec4 n = normalised(D.transpose() * model.norm(uv));
        vec4 r = normalised(n * (n * l) * 2 - l);

        double ambient = .3;
        double diff = 1. * std::max(0., n * l);
        /* Specular exponent is set to 35 here */
        /* Camera lies on z-axis, hence r.z */
        double spec = (.5 + 2.*sample2D(model.specular(), uv)[0]/255.) * std::pow(std::max(r.z, 0.), 35);
        TGAColor gl_FragColor = sample2D(model.diffuse(), uv);

        for (int channel : { 0, 1, 2 }) {
            gl_FragColor[channel] = std::min(255., gl_FragColor[channel] * (ambient + diff + spec));
        }

        return {false, gl_FragColor};
    }
};

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* =-= Printing/Exporting =-= */

/* Exports the zbuffer as a .tga image */
void drop_zbuffer(std::string filename, std::vector<double> &zbuffer, int width, int height) {

    /* Initialise the image object */
    TGAImage zimg(width, height, TGAImage::GRAYSCALE, {0, 0, 0, 0});

    double min_z = +1000;
    double max_z = -1000;

    /* Find the minimum and maximum depths, ignoring the background */
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x + y * width];

            /* Background is set to -1000 */
            if (z < -100) continue;

            min_z = std::min(z, min_z);
            max_z = std::max(z, max_z);
        }
    }

    /* Scale zbuffer to go from 0 to 255 */
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x + y * width];
            
            /* Background is set to -1000 */
            if (z < -100) continue;

            z = (z - min_z)/(max_z - min_z) * 255;
            zimg.set(x, y, {static_cast<uint8_t>(z), 255, 255, 255});
        }
    }

    zimg.write_tga_file(OUTPUT_DIR + '/' + filename);
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* =-= Main =-= */

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " model.obj" << std::endl;
        return 1;
    }

    // if (mkdir(OUTPUT_DIR, 0777) == -1) {
    //     std::cerr << "Error :  " << strerror(errno) << std::endl;
    // }
    int check = mkdir(C_OUTPUT_DIR, 0777);
    if (check) {
        std::cerr << "Unsuccessful in creating directory - May already exist" << std::endl;
    }

    /* Define the image size */
    constexpr int width  = 800;
    constexpr int height = 800;

    /* Define the shadow map size */
    constexpr int shadow_w = width * 10;
    constexpr int shadow_h = height * 10;

    /* Light & Camera position defined using eye, centre and up vectors */
    constexpr vec3 light{1, 1, 1};
    constexpr vec3 eye{-1, 0, 2};
    constexpr vec3 centre{0, 0, 0};
    constexpr vec3 up{0, 1, 0};

    /* Initial rendering pass with no shadow-mapping */
    lookat(eye, centre, up);
    init_perspective(norm(eye - centre));
    init_viewport(width/16, height/16, width*7/8, height*7/8);
    init_zbuffer(width, height);

    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    /* Iterate over all files */
    for (int m = 1; m < argc; m++) {

        Model model(argv[m]);
        PhongShader shader(light, model);

        /* Iterate through all primitives */
        for (int f = 0; f < model.nfaces(); f++) {

            /* Assemble the primitive */
            Triangle clip = { shader.vertex(f, 0), shader.vertex(f, 1), shader.vertex(f, 2) };

            rasterise(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file(OUTPUT_DIR + "/no_shadow.tga");
    drop_zbuffer("zbuffer_camera.tga", zbuffer, width, height);

    std::vector<bool> mask(width * height, false);
    std::vector<double> zbuffer_copy = zbuffer;
    mat<4,4> M = (Viewport * Perspective * ModelView).invert();

    /* Shadow rendering - New set of local variables */
    // WHY IS IT IN ITS OWN LOCAL THING.... WHAT DOESN'T GET SAVED GLOBALLY?
    {
        lookat(light, centre, up);
        init_perspective(norm(eye-centre));
        init_viewport(shadow_w/16, shadow_h/16, shadow_w*7/8, shadow_h*7/8);
        init_zbuffer(shadow_w, shadow_h);

        TGAImage trash(shadow_w, shadow_h, TGAImage::RGB, {177, 195, 209, 255});

        /* Iterate over all files */
        for (int m = 1; m < argc; m++) {

            Model model(argv[m]);
            BlankShader shader{model};

            /* Iterate over all primitives */
            for (int f = 0; f < model.nfaces(); f++) {

                /* Assemble the primitive */
                Triangle clip = { shader.vertex(f, 0), shader.vertex(f, 1), shader.vertex(f, 2) };

                rasterise(clip, shader, trash);
            }
        }
        trash.write_tga_file(OUTPUT_DIR + "/shadowmap.tga");
    }

    drop_zbuffer("zbuffer_light.tga", zbuffer, shadow_w, shadow_h);

    mat<4,4> N = Viewport * Perspective * ModelView;

    /* Post-Processing */

    /* Create the mask of what's in the light */
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            vec4 fragment = M * vec4{static_cast<double>(x), static_cast<double>(y), zbuffer_copy[x + y * width], 1.};
            vec4 q = N * fragment;
            vec3 p = q.xyz() / q.w;
            bool lit =  (fragment.z < -100 ||                                   // it's the background or
                        (p.x < 0 || p.x >= shadow_w || p.y < 0 || p.y >= shadow_h) ||   // it is out of bounds of the shadow buffer
                        (p.z > zbuffer[int(p.x) + int(p.y) * shadow_w] - .08));  // it is visible
            mask[x + y * width] = lit;
        }
    }

    /* Create an image to represent the mask */
    TGAImage maskimg(width, height, TGAImage::GRAYSCALE, {255, 255, 255, 255});
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mask[x + y * width]) continue;
            maskimg.set(x, y, {0, 0, 0, 0});
        }
    }

    maskimg.write_tga_file(OUTPUT_DIR + "/mask.tga");

    /* Apply the mask to the image */
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mask[x + y * width]) continue;
            TGAColor c = framebuffer.get(x, y);
            vec3 a;
            for (int i : { 0, 1, 2 }) { a[i] = c[i]; }
            if (norm(a) < MIN_DARK) continue;
            a = normalised(a) * MIN_DARK;
            framebuffer.set(x, y, { static_cast<std::uint8_t>(a[0]), 
                                    static_cast<std::uint8_t>(a[1]), 
                                    static_cast<std::uint8_t>(a[2]), 
                                    255 });
        }
    }
    framebuffer.write_tga_file(OUTPUT_DIR + "/shadow.tga");

    return 0;
}