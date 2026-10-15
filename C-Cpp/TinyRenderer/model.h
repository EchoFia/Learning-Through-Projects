#include "geometry.h"
#include "tgaimage.h"

class Model {
    std::vector<vec4> verts = {};    // array of vertices
    std::vector<int> facet_vrt = {}; // per-triangle index in the above array
    std::vector<vec4> norms = {};
    std::vector<int> facet_nrm = {};
    std::vector<vec2> tex = {};
    std::vector<int> facet_tex = {};
    TGAImage normalmap = {};
    TGAImage diffusemap = {};
    TGAImage specmap = {};
public:
    Model(const std::string filename);
    int nverts() const; // number of vertices
    int nfaces() const; // number of triangles
    vec4 vert(const int i) const;                          // 0 <= i < nverts()
    vec4 vert(const int iface, const int nthvert) const;   // 0 <= iface <= nfaces(), 0 <= nthvert < 3
    vec4 norm(const int i) const;
    vec4 norm(const int iface, const int nthvert) const;
    vec4 norm(const vec2 &uv) const;
    vec2 uv(const int iface, const int nthvert) const;
    const TGAImage& diffuse() const;
    const TGAImage& specular() const;
};