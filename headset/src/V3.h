#ifndef _V3_H
#define _V3_H

class V3 {
	private:
		float v[3];
		
	public:
		//constructors
		V3();
		V3(float x, float y, float z);
		
		~V3();
		
		//handy functions
		float magnitude();
		V3 cross(const V3 &v2);
		float norm();
		float getX();
		float getY();
		float getZ();
		
		//operators
        V3 & operator=(const V3 &rhs);

        V3 operator+(const V3 &rhs);
        V3 operator+(const float rhs);

        V3 operator-(const V3 &rhs);
        V3 operator-(const float rhs);

        float operator*(const V3 &rhs);
        V3 operator*(const float rhs);
		
        V3 operator/(const float rhs);

        float & operator[](const int index);
        const float & operator[](const int index) const;
};

#endif
