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

