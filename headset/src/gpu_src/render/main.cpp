#include <irrlicht.h>
#include <iostream>
#include <fstream>
#include "InputReceiver.cpp"
#include "GameObject.h"
#include "SensorReader.h"
#include "genLevel.h"
#include "gpuPyThreadInterface.h"
#include "../../networking_src/packets.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_FOV 3.14f/2.8f
#define MOVEMENT_SPEED 5.0f
#define CAMERA_SPEED 40.0f
#define PROCESS 1
#define HEADSET_VERSION

GameObject *objects;

#ifdef PROCESS
int main(int argc, char *argv[]) {
#else
int render(int argc, char *argv[]) {
#endif
	if (argc != 3) {
		std::cout << "usage: render configFile path/to/models\n";
		return -1;
	}
	
	char *configFileName = argv[1];
	char *modelsPath = argv[2];
	volatile bool quit = false;
	GpuPyThreadInterface pyInterface;
	pyInterface.initServer(&quit);
	int battery = 10;
	int signal = 0;
	int sats = 0;
	float fov = SCREEN_FOV;
	stringw orientString = "y: 0.0 p: 0.0 r 0.0";
	stringw fpsString = "FPS: ???";
	stringw posString = "x: 0.0 y: 0.0";
    //interfaces to other modules
#ifdef HEADSET_VERSION
    SensorReader sensor;
    sensor.initServer();
#endif

    //create the window
    InputReceiver receiver;
    IrrlichtDevice *device = createDevice( video::EDT_OPENGL, dimension2d<u32>(SCREEN_WIDTH, SCREEN_HEIGHT), 32, true, false, true, &receiver);

    //check for successful initialization of irrlicht
    if (!device) {
        return 1;
    }
    IVideoDriver* driver = device->getVideoDriver();
    ISceneManager* smgr = device->getSceneManager();
    IGUIEnvironment* guienv = device->getGUIEnvironment();
    GameObject::setSceneManager(smgr);

    //load level file
    objects = new GameObject[2096];
    writeConfigFile(configFileName, modelsPath);
    GameObject::loadConfigFile("tmp.cfg", objects);

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
    //get font
    IGUIFont* font = device->getGUIEnvironment()->getBuiltInFont();

    //main render loop
    while(device->run() && !quit) {
        driver->beginScene(true,true, SColor(0,0,0,0));
        if(receiver.IsKeyDown(irr::KEY_BACK)) {
			quit = true;
		}
        if(receiver.IsKeyDown(irr::KEY_SUBTRACT)) {
			quit = true;
		}
        if(receiver.IsKeyDown(irr::KEY_ADD)) {
					fov += 0.1;
					std::cout << "Field of view: " << fov << std::endl;
		}
				if(receiver.IsKeyDown(irr::KEY_MULTIPLY)) {
					fov -= 0.1;
					std::cout << "Field of view: " << fov << std::endl;
				}

    camera->setFOV(fov);
        
        stringw str = L"FPS: ";
        str += driver->getFPS();
        device->setWindowCaption(str.c_str());
#ifndef HEADSET_VERSION
	if (receiver.IsKeyDown(irr::KEY_KEY_F))
		printf("FPS: %d\n", driver->getFPS());
#endif

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

        //update the actual camera
        
        #ifdef HEADSET_VERSION
        //sensor camera control
        camerarot = sensor.getOrientation();
        camerapos = sensor.getLocation();
        camera->setPosition(camerapos);
        camera->setRotation(camerarot);
        #else
        camera->setPosition(camerapos);
        camera->setRotation(camerarot);
		#endif

        camera->setTarget(cameraTarget->getAbsolutePosition());
        camera->setUpVector(upTarget->getAbsolutePosition() - camerapos);
        smgr->drawAll();
        //hud stuff
#ifdef HEADSET_VERSION
        battery = sensor.getBatteryStatus();
        signal = sensor.getWifiStatus();
        sats = sensor.getNumSatellites();
#endif
        orientString = stringw("r:") + stringw(camerarot.X) + stringw(" p:") + stringw(camerarot.Y) + stringw(" y:") + stringw(camerarot.Z);
        //sprintf(orientString, "r:%f p:%f y:%f", camerarot.X, camerarot.Y, camerarot.Z);
        posString = stringw("x:") + stringw(camerapos.X)
                    + stringw(" y:") + stringw(camerapos.Y);
        fpsString = stringw("FPS: ") + stringw(driver->getFPS());
	font->draw((sats > 1) ? L"gps locked" : L"gps unlocked", rect<s32>(20,10,300,50), SColor(255,255,255,255));
        font->draw(stringw("battery: ") + stringw(battery) + stringw("/10"), rect<s32>(20,20,300,50), SColor(255,255-(battery),battery,0));
        font->draw( signal ? L"wifi connected" : L"wifi disconnected.", rect<s32>(20,30,300,50), SColor(255,255-(signal),signal,0));
        font->draw(orientString, rect<s32>(20,40,300,50), SColor(255,255,255,255));
	font->draw(fpsString, rect<s32>(20,50,300,50), SColor(255,255,255,255));
	font->draw(posString, rect<s32>(80,60,300,50), SColor(255,255,255,255));
		//printf("\rX: %2.2f, Y: %2.2f, Orientation: %2.2f", camerapos.X, camerapos.Y, camerarot.Z);
        driver->endScene();
    }
    std::cout << "Rendering exit\n";
    device->drop();
    return 0;
}

void updateObjects(objInfo_t *objInfo, int size) {
    int instId;
    objInfo_t objinfo;
    for(int i=0; i<size; i++) {
        objinfo = objInfo[i];
				//printf("Object: instId %d: x3: %f y3: %f\n", objinfo.instId, objinfo.x3, objinfo.y3);
				//printf("Object: roll %f: pitch: %f yaw: %f\n", objinfo.roll, objinfo.pitch, objinfo.yaw);
        instId = objinfo.instId;
        objects[instId].setPosition(objinfo.x3, objinfo.y3, objinfo.z3);
        objects[instId].setRotation(objinfo.roll, objinfo.pitch, objinfo.yaw);
        objects[instId].setVisible(objinfo.typeShow);
    }
}
