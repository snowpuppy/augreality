#include <irrlicht.h>
#include <iostream>
#include <fstream>
#include "InputReceiver.cpp"
#include "GameObject.h"
#include "SensorReader.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define SCREEN_FOV 3.14f/2.5f
#define MOVEMENT_SPEED 5.0f
#define CAMERA_SPEED 40.0f

int main(int argc, char *argv[]) {
    bool quit = false;

    //interfaces to other modules
    SensorReader *sensor = new SensorReader();


    //create the window
    InputReceiver receiver;
    IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT), 16, true, false, true, &receiver);

    //check for successful initialization of irrlicht
    if (!device) {
        return 1;
    }
    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();
    GameObject::setSceneManager(smgr);

    //load level file
    GameObject *objects = new GameObject[2096];
    GameObject::loadConfigFile("config.txt", objects);

    //add camera
    ICameraSceneNode* camera = smgr->addCameraSceneNode();
    ISceneNode* cameraTarget = smgr->addEmptySceneNode(camera);
    cameraTarget->setPosition(vector3df(1,0,0));
    ISceneNode* upTarget = smgr->addEmptySceneNode(camera);
    upTarget->setPosition(vector3df(0,0,1));
    camera->setFOV(SCREEN_FOV);
    device->getCursorControl()->setVisible(false);
    vector3df camerapos = camera->getPosition();
    vector3df camerarot = vector3df(0, 0, 0);
		//IImage screenshot;

    //main render loop
    while(device->run() && !quit) {
        driver->beginScene(true,true, SColor(0,0,0,0));
        quit = receiver.IsKeyDown(irr::KEY_ESCAPE);
        
        stringw str = L"FPS: ";
        str += driver->getFPS();
        device->setWindowCaption(str.c_str());

        //keyboard camera control

        //move camera
        if(receiver.IsKeyDown(irr::KEY_KEY_A))
            camerapos -= camera->getUpVector().crossProduct((camera->getTarget() - camera->getPosition())) * MOVEMENT_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_KEY_D))
            camerapos += camera->getUpVector().crossProduct((camera->getTarget() - camera->getPosition())) * MOVEMENT_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_KEY_W))
            camerapos += (camera->getTarget() - camera->getPosition()) * MOVEMENT_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_KEY_S))
            camerapos -= (camera->getTarget() - camera->getPosition()) * MOVEMENT_SPEED / 60.0f;
				if(receiver.IsKeyDown(irr::KEY_KEY_R))
            camerapos.Z += MOVEMENT_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_KEY_F))
            camerapos.Z -= MOVEMENT_SPEED / 60.0f;

        //set camera roll
        if(receiver.IsKeyDown(irr::KEY_KEY_Q))
            camerarot.X -= CAMERA_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_KEY_E))
            camerarot.X += CAMERA_SPEED / 60.0f;

        //set camera pitch
        if(receiver.IsKeyDown(irr::KEY_UP))
            camerarot.Y -= CAMERA_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_DOWN))
            camerarot.Y += CAMERA_SPEED / 60.0f;

        //set camera yaw
        if(receiver.IsKeyDown(irr::KEY_LEFT))
            camerarot.Z -= CAMERA_SPEED / 60.0f;
        if(receiver.IsKeyDown(irr::KEY_RIGHT))
            camerarot.Z += CAMERA_SPEED / 60.0f;

				if(receiver.IsKeyDown(irr::KEY_KEY_P)) {
						//screenshot = driver->createScreenShot();
						//driver->WriteImageIntoFile(screenshot, "screen.jpg");	
         }

        //update the actual camera
        camera->setPosition(camerapos);
        camera->setRotation(camerarot);

        //sensor camera control
        //sensor->poll();
        //camera->setPosition(sensor->getLocation();
        //camera->setRotation(sensor->getOrientation();
        camera->setTarget(cameraTarget->getAbsolutePosition());
        camera->setUpVector(upTarget->getAbsolutePosition() - camerapos);
        smgr->drawAll();
        driver->endScene();
    }

    device->drop();
    return 0;
}
