#pragma once
#include <math.h>

namespace IA {

    template <typename T>
    struct Vector2 {
        T x;
        T y;

        Vector2<T>() { // Blank Constructor
            x = 0;
            y = 0;
        }

        Vector2<T>(T _x, T _y) { // Constructor
            x = _x;
            y = _y;
        }

        // Negation Operator
        Vector2<T> operator -() {
            return Vector2<T>(-x, -y);
        }

        // Addition Operator
        Vector2<T> operator +(Vector2<T> a) {
            return Vector2<T>(x + a.x, y + a.y);
        }

        Vector2<T>& operator +=(Vector2<T> a) {
            x += a.x;
            y += a.y;
            return *this;
        }

        // Subtraction Operator
        Vector2<T> operator -(Vector2<T> a) {
            return Vector2<T>(x - a.x, y - a.y);
        }

        Vector2<T>& operator -=(Vector2<T> a) {
            x -= a.x;
            y -= a.y;
            return *this;
        }

        // Scalar Operator
        Vector2<T> operator *(double b) {
            return Vector2<T>(b * x, b * y);
        }

        Vector2<T>& operator *=(double a) {
            x *= a;
            y *= a;
            return *this;
        }

        // Inverse Scalar Operator
        Vector2<T> operator /(double b) {
            return Vector2<T>(x / b, y / b);
        }

        Vector2<T>& operator /=(double a) {
            x /= a;
            y /= a;
            return *this;
        }

        // Dot Product
        double dot(Vector2<T> a) {
            return x * a.x + y * a.y;
        }

        // Vector Magnitude
        double magnitude() {
            return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
        }

        // Vector Argument (Angle from horizontal in radians)
        double arg() {
            return std::atan2(y, x);
        }

        // Normalize Vector
        Vector2<T> norm() {
            return *this/magnitude();
        }

        // Distance between two points
        double distance(Vector2<T> a) {
            return (this - a).magnitude();
        }

        // Conversions
        template <typename C>
        operator Vector2<C>() {
            return Vector2<C>(x, y);
        }

        // SFML Vector2 conversions (the reason I made this)
        #ifdef SFML_SYSTEM_HPP
        template <typename C>
        operator sf::Vector2<C>() {
            return sf::Vector2<C>(x, y);
        }

        //Converting Constructor
        template <typename C>
        Vector2<T>(sf::Vector2<C> a) {
            x = a.x;
            y = a.y;
        }

        Vector2<T>& operator =(sf::Vector2i a) {
            x = a.x;
            y = a.y;
            return *this;
        }
        #endif
    };

    template <typename T>
    Vector2<T> operator*(double a, Vector2<T> b) {
        return b * a;
    }

    template <typename T>
    Vector2<T> operator/(double a, Vector2<T> b) {
        return b / a;
    }

    typedef Vector2<double> Vector2d;
    typedef Vector2<float> Vector2f;
    typedef Vector2<int> Vector2i;
    typedef Vector2<unsigned> Vector2u;
}