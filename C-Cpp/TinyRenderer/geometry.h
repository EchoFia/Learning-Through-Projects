#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec {
    double data[n] = {0};
    double& operator[](const int i)       { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

/* Vector operations */

/* Vector dot product */
template<int n> double operator*(const vec<n>& a, const vec<n>& b) {
    double result = 0;
    for (int i = 0; i < n; i++) {
        result += a[i] * b[i];
    }
    return result;
}

/* Vector addition */
template<int n> vec<n> operator+(const vec<n>& a, const vec<n>& b) {
    vec<n> result = a;
    for (int i = 0; i < n; i++) {
        result[i] += b[i];
    }
    return result;
}

/* Vector subtraction */
template<int n> vec<n> operator-(const vec<n>& a, const vec<n>& b) {
    vec<n> result = a;
    for (int i = 0; i < n; i++) {
        result[i] -= b[i];
    }
    return result;
}

/* Scalar multiplication */
template<int n> vec<n> operator*(const vec<n>& a, const double& k) {
    vec<n> result = a;
    for (int i = 0; i < n; i++) {
        result[i] *= k;
    }
    return result;
}

template<int n> vec<n> operator*(const double& k, const vec<n>& a) {
    return a * k;
}

/* Scalar division */
template<int n> vec<n> operator/(const vec<n>& a, const double& k) {
    vec<n> result = a;
    for (int i = 0; i < n; i++) {
        result[i] /= k;
    }
    return result;
}

/* Vector printing */
template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i)       { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    double  operator[](const int i) const { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    vec<2> xy()  const { return {x, y};    }
    vec<3> xyz() const { return {x, y, z}; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v * v);
}

template<int n> vec<n> normalised(const vec<n>& v) {
    return v / norm(v);
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}

/* Matrices */

template<int n> struct dt; // WHAT DOES THIS DO???

template<int nrows,int ncols> struct mat {
    vec<ncols> rows[nrows] = {{}};

    /* Defines indexing for a matrix */
    vec<ncols>& operator[] (const int idx) { assert(idx >= 0 && idx < nrows); return rows[idx]; }
    const vec<ncols>& operator[] (const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }

    double det() const {
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        mat<nrows-1,ncols-1> submatrix;
        for (int i=nrows-1; i--; )
            for (int j=ncols-1;j--; submatrix[i][j]=rows[i+int(i>=row)][j+int(j>=col)]);
        return submatrix.det() * ((row+col)%2 ? -1 : 1);
    }

    mat<nrows,ncols> invert_transpose() const {
        mat<nrows,ncols> adjugate_transpose; // transpose to ease determinant computation, check the last line
        for (int i=nrows; i--; )
            for (int j=ncols; j--; adjugate_transpose[i][j]=cofactor(i,j));
        return adjugate_transpose/(adjugate_transpose[0]*rows[0]);
    }

    mat<nrows,ncols> invert() const {
        return invert_transpose().transpose();
    }

    mat<ncols,nrows> transpose() const {
        mat<ncols,nrows> ret;
        for (int i=ncols; i--; )
            for (int j=nrows; j--; ret[i][j]=rows[j][i]);
        return ret;
    }
};


template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows>& v, const mat<nrows,ncols>& M) {
    return (mat<1,nrows>{{v}}*M)[0];
}

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows,ncols>& M, const vec<ncols>& v) {
    vec<nrows> result;
    for (int i=nrows; i--; result[i]=M[i]*v);
    return result;
}

template<int R1, int C1, int C2> mat<R1,C2> operator*(const mat<R1,C1>& A, const mat<C1,C2>& B) {
    mat<R1,C2> result;
    for (int i=R1; i--; )
        for (int j=C2; j--; )
            for (int k=C1; k--; result[i][j]+=A[i][k]*B[k][j]);
    return result;
}




template<int nrows,int ncols>mat<nrows,ncols> operator*(const mat<nrows,ncols>& lhs, const double& val) {
    mat<nrows,ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]*val);
    return result;
}

template<int nrows,int ncols>mat<nrows,ncols> operator/(const mat<nrows,ncols>& lhs, const double& val) {
    mat<nrows,ncols> result;
    for (int i=nrows; i--; result[i] = lhs[i]/val);
    return result;
}




template<int n> struct dt { // template metaprogramming to compute the determinant recursively
    static double det(const mat<n,n>& src) {
        double ret = 0;
        for (int i=n; i--; ret += src[0][i] * src.cofactor(0,i));
        return ret;
    }
};

template<> struct dt<1> {   // template specialization to stop the recursion
    static double det(const mat<1,1>& src) {
        return src[0][0];
    }
};