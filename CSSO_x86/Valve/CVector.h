#pragma once
#include <cmath>

class CVector
{
public:
    float x{}, y{}, z{};

    constexpr CVector() noexcept = default;
    constexpr CVector(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}

    constexpr CVector operator+(const CVector& other) const noexcept
    {
        return { x + other.x, y + other.y, z + other.z };
    }

    constexpr CVector operator-(const CVector& other) const noexcept
    {
        return { x - other.x, y - other.y, z - other.z };
    }

    constexpr CVector operator*(const CVector& other) const noexcept
    {
        return { x * other.x, y * other.y, z * other.z };
    }

    constexpr CVector operator/(const CVector& other) const noexcept
    {
        return { x / other.x, y / other.y, z / other.z };
    }

    constexpr CVector operator*(float scalar) const noexcept
    {
        return { x * scalar, y * scalar, z * scalar };
    }

    constexpr CVector operator/(float scalar) const noexcept
    {
        return { x / scalar, y / scalar, z / scalar };
    }

    constexpr CVector& operator+=(const CVector& other) noexcept
    {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    constexpr CVector& operator-=(const CVector& other) noexcept
    {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    constexpr float Dot(const CVector& other) const noexcept
    {
        return x * other.x + y * other.y + z * other.z;
    }

    float Length() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    float Length2D() const noexcept
    {
        return std::sqrt(x * x + y * y);
    }

    float LengthSqr() const noexcept
    {
        return x * x + y * y + z * z;
    }

    CVector Normalize() const noexcept
    {
        float len = Length();
        if (len == 0.0f)
            return { 0.f, 0.f, 0.f };
        return { x / len, y / len, z / len };
    }

    CVector ToAngle() const noexcept
    {
        constexpr float pi = 3.14159265358979323846f;
        return {
            std::atan2(-z, std::hypot(x, y)) * (180.0f / pi),
            std::atan2(y, x) * (180.0f / pi),
            0.0f
        };
    }

    float Distance(const CVector& other) const noexcept
    {
        return std::sqrt(
            (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z)
        );
    }
};

template<typename T>
constexpr T Clamp(const T& value, const T& min, const T& max) noexcept
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline CVector AngleToDirection(const CVector& angles) noexcept
{
    constexpr float pi = 3.14159265358979323846f;
    float pitch = angles.x * (pi / 180.0f);
    float yaw = angles.y * (pi / 180.0f);

    float cp = std::cos(pitch);
    float sp = std::sin(pitch);
    float cy = std::cos(yaw);
    float sy = std::sin(yaw);

    return CVector{ cp * cy, cp * sy, -sp };
}
