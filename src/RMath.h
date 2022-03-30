#include <math.h>

// Vector with 4 dimensions
struct Vector4 {
    float x, y, z, w;
    constexpr Vector4() : x(0.0001f), y(0.0001f), z(0.0001f), w(0.0001f) {}
    constexpr Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

// Vector with 3 dimensions
struct Vector3 {
    float x, y, z;
    constexpr Vector3() : x(0.0001f), y(0.0001f), z(0.0001f) {}
    constexpr Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    const float &operator[](int i) const {
        return (&x)[i];
    }

    constexpr Vector3 &operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return (*this);
    }

    constexpr Vector3 &operator/=(float s) {
        s = 1.0F / s;
        x *= s;
        y *= s;
        z *= s;
        return (*this);
    }


};

inline Vector3 operator
*(const Vector3 &vector, float s) {
    return (Vector3(vector.x * s, vector.y * s, vector.z * s));
}

inline Vector3 operator
/(const Vector3 &v, float s) {
    s = 1.0F / s;
    return (Vector3(v.x * s, v.y * s, v.z * s));
}

inline Vector3
operator-(const Vector3 &v) {
    return (Vector3(-v.x, -v.y, -v.z));
}

inline float
Magnitude(const Vector3 v) {
    return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline Vector3
Normalized(const Vector3 &v) {
    return (v / Magnitude(v));
}
