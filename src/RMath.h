#include <math.h>

typedef struct
Vector2DData {
    // x,y,z
    float data[2];

    Vector2DData() = default;

    Vector2DData(float a, float b) {
        data[0] = a;
        data[1] = b;
    }

    float &operator[](int i) {
        return (&data[0])[i];
    }

    const float &operator[](int i) const {
        return ((&data[0])[i]);
    }

    Vector2DData& operator *= (float s) {
        data[0] *= s;
        data[1] *= s;
        return (*this);
    }

    Vector2DData& operator /= (float s) {
        s = 1.0F / s;
        data[0] *= s;
        data[1] *= s;
        return (*this);
    }

} Vector2;


typedef struct
Vector3DData {
    // x,y,z
    float data[3];

    Vector3DData() = default;

    Vector3DData(float a, float b, float c) {
        data[0] = a;
        data[1] = b;
        data[2] = c;
    }

    Vector3DData(Vector2DData vector2) {
        data[0] = vector2.data[0];
        data[1] = vector2.data[1];
        data[2] = .0;
    }

    float &operator[](int i) {
        return (&data[0])[i];
    }

    const float &operator[](int i) const {
        return ((&data[0])[i]);
    }

    Vector3DData& operator *= (float s) {
        data[0] *= s;
        data[1] *= s;
        data[2] *= s;
        return (*this);
    }

    Vector3DData& operator /= (float s) {
        s = 1.0F / s;
        data[0] *= s;
        data[1] *= s;
        data[2] *= s;
        return (*this);
    }

} Vector3;

inline Vector3DData operator *(const Vector3DData& vector, float s) {
    return (Vector3DData(vector[0] * s, vector[1] * s, vector[2] * s));
}

inline Vector3DData operator / (const Vector3DData& v, float s) {
    s = 1.0F / s;
    return (Vector3DData(v[0] * s , v[1] * s, v[2] * s));
}

inline Vector3DData operator -(const Vector3DData& v) {
    return (Vector3DData(-v[0], - v[1], -v[2]));
}

inline float Magintude(const Vector3DData v) {
    return (sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));
}

inline Vector3DData Normalized(const Vector3DData& v) {
    return (v / Magintude(v));
}