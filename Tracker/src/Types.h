/*
    Gimbal Tracker

    2019 nulluser # gmail.com

    File: Types.cpp
*/

#ifndef TYPES_H
#define TYPES_H

template<typename T> class Vec2
{
  public:
    Vec2() : x(0), y(0) {}
    Vec2(T xx) : x(xx), y(xx) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    Vec2 operator + (const Vec2 &v) const   { return Vec2(x + v.x, y + v.y); }
    Vec2 operator / (const T &r) const      { return Vec2(x / r, y / r); }
    Vec2 operator * (const T &r) const      { return Vec2(x * r, y * r); }
    Vec2& operator /= (const T &r)          { x /= r, y /= r; return *this; }
    Vec2& operator *= (const T &r)          { x *= r, y *= r; return *this; }
    //friend std::ostream& operator << (std::ostream &s, const Vec2<T> &v)
    //{//return s << '[' << v.x << ' ' << v.y << ']'; //}
    friend Vec2 operator * (const T &r, const Vec2<T> &v) { return Vec2(v.x * r, v.y * r); }

    T x, y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;


/*
struct Vec2f
{
    Vec2f() : x(0), y(0) {};
	Vec2f(float x, float y) : x(x), y(y) {};

    float x;
	float y;
};

struct Vec2i
{
    Vec2f() : x(0), y(0) {};
	Vec2f(float x, float y) : x(x), y(y) {};

    float x;
	float y;
};
*/

#endif
