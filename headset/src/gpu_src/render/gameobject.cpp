#include "gameobject.h"
#include <string>

GameObject::GameObject() {

}

GameObject::GameObject(float x, float y, float pitch, float yaw, float roll, bool visible, bool threeD, uint8_t id, std::string filename, float scale) {
        this->x = x;
        this->y = y;
        //this->z = 0.0;
        this->pitch = pitch;
        this->yaw = yaw;
        this->roll = roll;
        this->visible = visible;
        this->threeD = threeD;
        this->id = id;
        this->filename = filename;
        this->scale = scale;
        //this->mesh = new ngl::Obj(MODEL_DIR + filename + ".obj", MODEL_DIR + filename + ".jpg");
}
