#ifndef _V4_H
#define _V4_H

class V4 {
	private:
		float v[4];
		
	public:
		//constructors
		V4();
		V4(float x, float y, float z, float w);
		
		~V4();
		
		//handy functions
		float magnitude();
		float norm();
		float getX();
		float getY();
		float getZ();
		float getW();
		
		//operators
        V4 & operator=(const V4 &rhs);

        V4 operator+(const V4 &rhs);
        V4 operator+(const float rhs);

        V4 operator-(const V4 &rhs);
        V4 operator-(const float rhs);

        float operator*(const V4 &rhs);
        V4 operator*(const float rhs);
		
        V4 operator/(const float rhs);

        float & operator[](const int index);
        const float & operator[](const int index) const;
};

#endif
