#ifndef GAMEOBJECT_H__
#define GAMEOBJECT_H__

#define MODEL_DIR "models/"

class GameObject {

public:
	//constructors
	GameObject();
	GameObject(float x, float y, float pitch, float yaw, float roll, boolean visible, boolean threeD, uint8_t id, String filename);
	
	update(float x, float y, float pitch, float yaw, float roll);

	//accessors
	float getX() {return x;}
	float getY() {return y;}
	float getPitch() {return pitch;}
	float getRoll() {return roll;}
	float getYaw() {return yaw;}
	boolean isVisible() {return visible;}
	
private:
	float x;
	float y;
	float pitch;
	float roll;
	float yaw;
	uint8_t id;
	boolean visible;
	boolean threeD;
	std::string filename;
	ngl::Obj *mesh;
};

#endif