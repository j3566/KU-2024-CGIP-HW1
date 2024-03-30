#pragma once

#ifndef COMMONS
#define COMMONS

#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include "glut.h"

struct Vec3 {

    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }

    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }

    Vec3 operator*(float d) const { return Vec3(x * d, y * d, z * d); }
    Vec3 operator/(float d) const { return Vec3(x / d, y / d, z / d); }

    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    Vec3 normalize() const {
        float mag = sqrt(x * x + y * y + z * z);
        return Vec3(x / mag, y / mag, z / mag);
    }

    float norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 pow( float p ) const {

        return Vec3(std::pow(x, p), std::pow(y, p), std::pow(z, p));
    }
};

Vec3 operator*(float d, const Vec3& v) {
    return Vec3(d * v.x, d * v.y, d * v.z);
}

Vec3 operator/(float d, const Vec3& v) {
    return Vec3(d / v.x, d / v.y, d / v.z);
}

struct HitInfo {

    bool hit;
    float distance;
    Vec3 point;

	HitInfo(bool hit = false, float distance = -1, Vec3 point = NULL) : hit(hit), distance(distance), point(point) {}
};


class Shape {

	public:

        Vec3 ka;
        Vec3 kd;
        Vec3 ks;

        float specularPower;

        Shape() {}

        Shape(const Vec3& ka, const Vec3& kd, const Vec3& ks, float specularPower)
            : ka(ka), kd(kd), ks(ks), specularPower(specularPower) {}

	    virtual HitInfo intersect(const Vec3& rayOrigin, const Vec3& rayDirection) const = 0;

        virtual Vec3 getNormalVector(Vec3& point) const = 0;

};

class Sphere : public Shape {

	private:
	    Vec3 center;
	    float radius;

	public:
	    Sphere(const Vec3& center, float radius) : center(center), radius(radius) {}

        Sphere(const Vec3& center, float radius,
            const Vec3& ka, const Vec3& kd, const Vec3& ks, float specularPower) :
            Shape( ka, kd, ks, specularPower),
            center(center), radius(radius) {}

	    HitInfo intersect(const Vec3& rayOrigin, const Vec3& rayDirection) const override {

            Vec3 oc = rayOrigin - center;
            Vec3 rn = rayDirection.normalize();

            float a = rn.dot(rn);
            float b = 2.0 * rn.dot(oc);
            float c = oc.dot(oc) - (radius * radius);

            float discriminant = b * b - 4 * a * c;

            if (discriminant < 0) {
                return {};
            }
            else {

                float threshold = 1e-6;
                float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
                float t2 = (-b - sqrt(discriminant)) / (2.0f * a);
                float t = std::numeric_limits<float>::infinity();

                if (t1 >= threshold && t1 < t) {
                    t = t1;
                }
                if (t2 >= threshold && t2 < t) {
                    t = t2;
                }

                float inf = std::numeric_limits<float>::infinity();
                if (t < inf) {
                    return HitInfo(true, t, rayOrigin + rn * t);
                }
                else {
                    return {};
                }
            }
	    }

        Vec3 getNormalVector(Vec3& point) const override
        {
            return (point - center).normalize();
        }
};

class Plane : public Shape {

	private:
	    Vec3 point;
	    Vec3 normal;

        Vec3 ka;
        Vec3 kd;
        Vec3 ks;

        float specularPower;

	public:

        Plane(const Vec3& point, const Vec3& normal,
            const Vec3& ka, const Vec3& kd, const Vec3& ks, float specularPower) :
            Shape(ka, kd, ks, specularPower),
			point(point), normal(normal.normalize()) {}

	    Plane(const Vec3& point, const Vec3& normal) : point(point), normal(normal.normalize()) {}

	    HitInfo intersect(const Vec3& rayOrigin, const Vec3& rayDirection) const override {

	        float denom = normal.dot(rayDirection);

	        if (std::abs(denom) > 1e-6) {

	            Vec3 pr = point - rayOrigin;
	            float t = pr.dot(normal) / denom;
	            if (t >= 0) {
	                return {true, t, rayOrigin + rayDirection.normalize() * t };
	            }
	        }

	        return {};
	    }

		Vec3 getNormalVector(Vec3& point) const override
        {
            return normal;
        }
};



#endif
