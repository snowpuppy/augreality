#ifndef GAMEOBJECT_H__
#define GAMEOBJECT_H__

#include <string>
#define MODEL_DIR "models/"
typedef unsigned char uint8_t;

class GameObject {

public:
        //constructors
        GameObject();
        GameObject(float x, float y, float pitch, float yaw, float roll, bool visible, bool threeD, uint8_t id, std::string filename, float scale);

        void update(float x, float y, float pitch, float yaw, float roll);

        //accessors
        float getX() {return x;}
        float getY() {return y;}
        float getPitch() {return pitch;}
        float getRoll() {return roll;}
        float getYaw() {return yaw;}
        bool isVisible() {return visible;}

//private:
        float x;
        float y;
        float pitch;
        float roll;
        float yaw;
        uint8_t id;
        bool visible;
        bool threeD;
        float scale;
        std::string filename;
        //ngl::Obj *mesh;
};

#endif
