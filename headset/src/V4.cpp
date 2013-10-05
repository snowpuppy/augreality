#include "V4.h"
#include <math.h>

//constructors
V4::V4() {
	v[0] = v[1] = v[2] = 0.0;
}

V4::V4(float x, float y, float z, float w) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

//destructor
V4::~V4() {}

//accessors
float V4::getX() {
    return v[0];
}

float V4::getY() {
    return v[1];
}

float V4::getZ() {
    return v[2];
}

float V4::getW() {
    return v[3];
}

//useful functions
float V4::magnitude() {
	return sqrt( (v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]) + (v[3]*v[3]) );
}

//operators

//indexing
float & V4::operator[](const int index) {
    return v[index];
}

const float & V4::operator[](const int index) const {
    return v[index];
}

//add vector to vector
V4 V4::operator+(const V4 &rhs) {
    return V4(v[0] + rhs[0], v[1] + rhs[1], v[2] + rhs[2], v[3] + rhs[3]);
}

//add float to vector
V4 V4::operator+(const float rhs) {
    return V4(v[0]+rhs, v[1]+rhs, v[2]+rhs, v[3]+rhs);
}

//subtract vector from vector
V4 V4::operator-(const V4 &rhs) {
    return V4(v[0] - rhs[0], v[1] - rhs[1], v[2] - rhs[2]);
}

//subtract float from vector
V4 V4::operator-(const float rhs) {
    return V4(v[0]-rhs, v[1]-rhs, v[2]-rhs, v[3]-rhs);
}

//dot product
float V4::operator*(const V4 &rhs) {
    return v[0]*rhs[0] + v[1]*rhs[1] + v[2]*rhs[2]  + v[3]*rhs[3];
}

//scale vector by float
V4 V4::operator*(const float rhs) {
    return V4(v[0]*rhs, v[1]*rhs, v[2]*rhs, v[3]*rhs);
}

//divide vector by float
V4 V4::operator/(const float rhs) {
    return V4(v[0]/rhs, v[1]/rhs, v[2]/rhs, v[3]/rhs);
}

//assignment
V4 & V4::operator=(const V4 &rhs) {
    //check for self-assignment
    if (this == &rhs)
        return *this;
    v[0] = rhs[0];
    v[1] = rhs[1];
    v[2] = rhs[2];
    v[3] = rhs[3];

    return *this;
}
