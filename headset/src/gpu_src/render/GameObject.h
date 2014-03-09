#include <irrlicht.h>
#include <string>
#include <map>

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class GameObject {
private:
    static std::map<std::string, irr::scene::IMesh*> Meshes;
    static irr::scene::ISceneManager* smgr;

    irr::scene::ISceneNode* node;

public:
    GameObject();
    GameObject(std::string name,
               irr::core::vector3df position = irr::core::vector3df(0.0f,0.0f,0.0f),
               irr::core::vector3df rotation = irr::core::vector3df(0.0f,0.0f,0.0f),
               irr::core::vector3df scale = irr::core::vector3df(1.0f,1.0f,1.0f),
               bool visible = true);
    void setPosition(irr::core::vector3df position);
    void setRotation(irr::core::vector3df rotation);
    void setScale(irr::core::vector3df scale);
    void setVisible(bool visible);

    static void loadConfigFile(std::string filename, GameObject *objects);
    static void setSceneManager(irr::scene::ISceneManager* manager);
};

#endif
