#inclulde "GameObject.h"

GameObject::GameObject() {

}

GameObject::GameObject(float x, float y, float pitch, float yaw, float roll, boolean visible, boolean threeD, uint8_t id, String filename) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
	this->visible = visible;
	this->threeD = threeD;
	this->id = id;
	this->filename = filename;
	this->mesh = new ngl::Obj(MODEL_DIR + filename + ".obj", MODEL_DIR + filename + ".jpg");
}