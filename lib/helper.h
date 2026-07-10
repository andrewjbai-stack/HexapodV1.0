#ifndef HELPER_H
#define HELPER_H

#include <Adafruit_PWMServoDriver.h>

template <typename T, int SIZE>
class Queue {
    private:
        T buffer_[SIZE];
        int head_ = 0;
        int tail_ = 0;
        int count_ = 0;

    public:
        bool push(const T& item){
            if(count_ >= SIZE) return false;
            buffer_[tail_] = item;
            tail_ = (tail_ + 1) % SIZE;
            count_++;
            return true;
        }

        bool pop(T& out){
            if(count_ == 0) return false;
            out = buffer_[head_];
            head_ = (head_ + 1) % SIZE;
            count_--;
            return true;
        }

        bool isEmpty() const { return count_ == 0; }
        bool isFull()  const { return count_ >= SIZE; }
        int  count()   const { return count_; }

        //Empties the queue
        void clear() { head_ = 0; tail_ = 0; count_ = 0; }
};
//Creates a vector 3 object and has some methods. yessir
struct Vec3 {
    float x, y, z;

    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    //Operator overloads between vector 3s
    Vec3  operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3  operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3  operator*(float s)        const { return {x * s,   y * s,   z * s};   }

    float magnitudeSquared() const { return x*x + y*y + z*z; }
    float magnitude()        const { return sqrt(magnitudeSquared()); }

//Returns the norm of a vector    
    Vec3 normalized() const {
        float m = magnitude();
        if (m < 1e-6f) return {0, 0, 0};
        return {x / m, y / m, z / m};
    }
};

//Creates a vector 2 object and has some methods. yessir
struct Vec2 {
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    //Operator overloads between vector 2s
    Vec2  operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2  operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2  operator*(float s)        const { return {x * s,   y * s};   }

    float magnitudeSquared() const { return x*x + y*y; }
    float magnitude()        const { return sqrt(magnitudeSquared()); }

    //Returns the norm of a vector    
    Vec2 normalized() const {
        float m = magnitude();
        if (m < 1e-6f) return {0, 0};
        return {x / m, y / m};
    }

    //Rotates the vector by angle_degrees (counterclockwise)
    Vec2 rotated(float angle_degrees) const {
        float theta = angle_degrees * (PI / 180.0f);
        float cosT = cosf(theta);
        float sinT = sinf(theta);
        return {x * cosT - y * sinT, x * sinT + y * cosT};
    }
};
#endif