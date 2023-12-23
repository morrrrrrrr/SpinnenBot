
#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <array>
#include <iostream>
#include <cmath>
#include <initializer_list>

namespace vector {

    template <int d, typename T>
    class Vector {
    private:
        std::array<T, d> _data;

    public:
        Vector() { 
            _data = std::array<T, d>(); // initialize array with default values
        }
        Vector(std::array<T, d>&& data) : _data(std::move(data)) { }
        Vector(const std::array<T, d>& data) : _data(data) { }
        Vector(std::initializer_list<T> args) {
            _data = std::array<T, d>(); // initialize array with default values
            auto data_ptr = _data.begin();

            for (const T& arg : args) {
                if (data_ptr - _data.begin() >= d) break;
                
                // i++ increments i but returns the original value
                *(data_ptr++) = arg;
            }
        }

        // copy from a vector of another dimension and or another type (type needs to be static-castable to this type)
        template<int other_d, typename other_T>
        Vector(const Vector<other_d, other_T>& copy) {
            _data = std::array<T, d>(); // initialize array with default values

            // copy over the values
            for (int i = 0; i < other_d && i < d; i++) {
                _data[i] = static_cast<T>(copy.at(i));
            }
        }
        
        Vector& operator=(const Vector& copy) {
            this->_data = copy._data;
            return *this;
        }

        void set(int dimension, const T& value) {
            if (dimension < 0 || dimension >= d) {
                std::cerr << "Tried accessing outside of the vector at setDim\n";
                return _data[0];
            }

            _data[dimension] = value;
        }
        T at(int dimension) const {
            if (dimension < 0 || dimension >= d) {
                std::cerr << "Tried accessing outside of the vector at setDim\n";
                return _data[0];
            }
            return _data[dimension];
        }
        T& at(int dimension) {
            if (dimension < 0 || dimension >= d) {
                std::cerr << "Tried accessing outside of the vector at setDim\n";
                return _data[0];
            }
            return _data[dimension];
        }

        std::array<T, d> data() const {
            return _data;
        }
        std::array<T, d>& data() {
            return _data;
        }

        T operator[](const int& dimension) const {
            return this->at(dimension);
        }
        T& operator[](const int& dimension) {
            return this->at(dimension);
        }

        template <int dimension, typename Type>
        friend std::ostream& operator<<(std::ostream& os, const Vector<dimension, Type>& vector);
        
        template <int dimension, typename Type>
        friend Vector operator+(const Vector& a, const Vector& b);
        template <int dimension, typename Type>
        friend Vector& operator+=(Vector& a, const Vector& b);

        template <int dimension, typename Type>
        friend Vector operator-(const Vector& a, const Vector& b);
        template <int dimension, typename Type>
        friend Vector& operator-=(Vector& a, const Vector& b);

        template <int dimension, typename Type>
        friend Vector operator*(const Vector& a, const T& b);
        template <int dimension, typename Type>
        friend Vector operator*=(const Vector& a, const T& b);

        template <int dimension, typename Type>
        friend Vector operator/(const Vector& a, const T& b);
        template <int dimension, typename Type>
        friend Vector operator/=(Vector& a, const T& b);

        // returns the magnitude of the vector
        T squared_magnitude() const {
            T sq_mag = T();

            for (const T& value : _data) {
                sq_mag = sq_mag + value*value;
            }

            return sq_mag;
        }
        T magnitude() const {
            return static_cast<T>(sqrtl(static_cast<long double>(
                this->squared_magnitude()
            )));
        }
        // returns the normalized vector
        Vector normalized() const {
            const T mag = magnitude();
            return *this / mag;
        }
        // returns the inverted vector
        Vector inverted() const {
            Vector inv = Vector();

            for (int i = 0; i < d; i++) {
                inv.at(i) = 0 - this->at(i);
            }

            return inv;
        }
        Vector abs() const {
            Vector a(*this);
            for (int i = 0; i < d; i++) {
                a.at(i) = std::abs(a.at(i));
            }
            return a;
        }
        // normalizes the vector and returns a reference to it [MODIFIES]
        Vector& normalize() {
            *this /= magnitude();

            return *this;
        }
        // inverts the vector and returns a reference to it [MODIFIES]
        Vector& invert() {
            for (int i = 0; i < d; i++) {
                this->at(i) = 0 - this->at(i);
            }

            return *this;
        }
    }; // class Vector

    template <int d, typename T>
    std::ostream& operator<<(std::ostream& os, const Vector<d, T>& vector) {
        os << vector._data[0];
        for (int i = 1; i < d; i++) {
            os << ", " << vector._data[i];
        }
        return os;
    }

    #pragma region OPERATORS
    #pragma region OPERATOR +
    template <int d, typename T>
    Vector<d, T> operator+(const Vector<d, T>& a, const Vector<d, T>& b) {
        Vector<d, T> output;

        for (int i = 0; i < d; i++) {
            output.at(i) = a.at(i) + b.at(i);
        }

        return output;
    }

    template <int d, typename T>
    Vector<d, T>& operator+=(Vector<d, T>& a, const Vector<d, T>& b) {
        for (int i = 0; i < d; i++) {
            a.at(i) += b.at(i);
        }

        return a;
    }
    #pragma endregion
    #pragma region OPERATOR -
    template <int d, typename T>
    Vector<d, T> operator-(const Vector<d, T>& a, const Vector<d, T>& b) {
        Vector<d, T> output;
        
        for (int i = 0; i < d; i++) {
            output.at(i) = a.at(i) - b.at(i);
        }

        return output;
    }

    template <int d, typename T>
    Vector<d, T>& operator-=(Vector<d, T>& a, const Vector<d, T>& b) {
        for (int i = 0; i < d; i++) {
            a.at(i) -= b.at(i);
        }

        return a;
    }
    #pragma endregion
    #pragma region OPERATOR *
    template <int d, typename T>
    Vector<d, T> operator*(const Vector<d, T>& a, const T& b) {
        Vector<d, T> output;

        for (int i = 0; i < d; i++) {
            output.at(i) = a.at(i) * b;
        }

        return output;
    }

    template <int d, typename T>
    Vector<d, T> operator*=(const Vector<d, T>& a, const T& b) {
        for (int i = 0; i < d; i++) {
            a.at(i) *= b;
        }

        return a;
    }
    #pragma endregion
    #pragma region OPERATOR /
    template <int d, typename T>
    Vector<d, T> operator/(const Vector<d, T>& a, const T& b) {
        Vector<d, T> output;

        for (int i = 0; i < d; i++) {
            output.at(i) = a.at(i) / b;
        }

        return output;
    }

    template <int d, typename T>
    Vector<d, T> operator/=(Vector<d, T>& a, const T& b) {
        for (int i = 0; i < d; i++) {
            a.at(i) /= b;
        }

        return a;
    }
    #pragma endregion
    #pragma endregion

    template<int d, typename T>
    static T dot(const Vector<d, T>& a, const Vector<d, T>& b) {
        T dot_prod = T();

        for (int i = 0; i < d; i++) {
            dot_prod = dot_prod + (a.at(i) * b.at(i));
        }

        return dot_prod;
    }

    template <typename T>
    static Vector<3, T> cross(const Vector<3, T>& a, const Vector<3, T>& b) {
        Vector<3, T> cross_prod;

        cross_prod.at(0) = a.at(1) * b.at(2) - a.at(2) * b.at(1);
        cross_prod.at(1) = a.at(2) * b.at(0) - a.at(0) * b.at(2);
        cross_prod.at(2) = a.at(0) * b.at(1) - a.at(1) * b.at(0);

        return cross_prod;
    }
} // namespace vector

#endif
