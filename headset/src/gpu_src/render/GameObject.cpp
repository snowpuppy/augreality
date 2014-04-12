#include "GameObject.h"
#include <irrlicht.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

irr::scene::ISceneManager* GameObject::smgr = NULL;
std::map<std::string, irr::scene::IMesh*> GameObject::Meshes;

GameObject::GameObject() { }
/** \brief Primary GameObject constructor.
 * \param name The name of the mesh file to load.
 * \param position The initial position to place the object.
 * \param rotation The rotation to initially apply to the object, about (X,Y,Z) axes.
 * \param visible Whether or not the object should be diplayed by the application.
 */
GameObject::GameObject(std::string name,
                       irr::core::vector3df position,
                       irr::core::vector3df rotation,
                       irr::core::vector3df scale,
                       bool visible
                      ) {
    irr::scene::IMesh* mesh = NULL;

    //check if the mesh has already been loaded
    if (Meshes.count(name) > 0) {
        //if it has, use it
        mesh = Meshes[name];
    } else {
        //load mesh from file
        if(access((name + ".obj").c_str(), R_OK) != -1)
			mesh = smgr->getMesh((name + ".obj").c_str());
    }

    //create scene node from the mesh
    node = smgr->addMeshSceneNode(mesh);

    //disable lighting on the node
    if(node != NULL) {
        node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }

    setPosition(position);
    setRotation(rotation);
    setScale(scale);
    setVisible(visible);
}

void GameObject::setPosition(irr::core::vector3df position) {
    node->setPosition(position);
}

void GameObject::setRotation(irr::core::vector3df rotation) {
    node->setRotation(rotation);
}

void GameObject::setPosition(float x, float y , float z) {
    node->setPosition(irr::core::vector3df(x, y, z));
}

void GameObject::setRotation(float x, float y , float z) {
    node->setPosition(irr::core::vector3df(x, y, z));
}

void GameObject::setScale(irr::core::vector3df scale) {
    node->setScale(scale);
}

void GameObject::setVisible(bool visible) {
    node->setVisible(visible);
}

void GameObject::setSceneManager(irr::scene::ISceneManager* manager) {
    smgr = manager;
}

/** \brief Fills an array of GameObjects with those described in a file.
 * \param filename The filename of the configuration file to load.
 * \param objects The array of GameObjects to be written to.
 */
void GameObject::loadConfigFile(std::string filename, GameObject *objects) {
    int index;
    bool threed;
    float locx, locy, locz, orx, ory, orz;
    bool show;
    float scale;
    std::string id;

    std::ifstream file(filename.c_str());

    //reads each game object into the objects array
    while(file >> index) {
        file >> threed >> locx >> locy >> locz >> orx >> ory >> orz >> filename >> show >> scale;
        objects[index] = GameObject(filename,
                                    irr::core::vector3df(locx, locy, locz),
                                    irr::core::vector3df(orx, ory, orz),
                                    irr::core::vector3df(scale, scale,scale),
                                    show);
    }
    
    
}
