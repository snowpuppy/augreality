/*
  Copyright (C) 2012 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received m_a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MyGLWindow.h"
#include <SDL/SDL.h>
#include <ngl/Colour.h>
#include <ngl/Mat4.h>
#include <ngl/Transformation.h>
#include <ngl/TransformStack.h>
#include <ngl/Material.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <iostream>
#include <math.h>
#include "gameobject.h"

float MyGLWindow::floatbuffer[5] = { 0.0f };
uint8_t MyGLWindow::charbuffer[2] = { 0 };

//add to window used for RSSI sliding window filter
void MyGLWindow::addToWindow(float f)
{
	window[windowIndex] = f;
	windowIndex = (windowIndex+1) % (WINDOW_SIZE-1);
}

int compare(const void * a, const void * b)
{
	return( *(int *)a - *(int *)b );
}

//sliding window median average for RSSI data
float MyGLWindow::windowAverage()
{
	 //median filter
	float tmp[WINDOW_SIZE];
	for(int i=0; i<WINDOW_SIZE; i++) tmp[i]=window[i];
	qsort(tmp, WINDOW_SIZE, sizeof(int), compare);
	return tmp[WINDOW_SIZE/2];
}

MyGLWindow::MyGLWindow(ngl::EGLconfig *_config) : EGLWindow(_config)
{
	std::cout<<"My GL Window Ctor\n";
	makeSurface();
	m_exit=false;
	myX = myY = 0.0f;
	windowIndex = 0;
	for(int i=0; i<WINDOW_SIZE; i++) window[i]=0.0;
}

MyGLWindow::~MyGLWindow()
{

}

float * MyGLWindow::buffer()
{
	return floatbuffer;
}

void MyGLWindow::initializeGL()
{
	// Now we will create a basic Camera from the graphics library
	// This is a static camera so it only needs to be set once
	// First create Values for the camera position
	ngl::Vec4 From(0,0,0);
	ngl::Vec4 To(1,0,0);
	ngl::Vec4 Up(0,0,1);
	// now load to our new camera
	m_cam= new ngl::Camera(From,To,Up,ngl::PERSPECTIVE);
	//setup for 2d drawing
	ngl::Vec4 From2d(0, 0, 0);
	ngl::Vec4 To2d(0, -1, 0);
	ngl::Vec4 Up2d(0, 0, -1);
	m_cam2 = new ngl::Camera(From2d,To2d,Up2d,ngl::PERSPECTIVE);
	m_cam2->setShape(45,(float)m_width/(float)m_height,0.05,350,ngl::PERSPECTIVE);
	// set the shape using FOV 45 Aspect Ratio based on Width and Height
	// The final two are near and far clipping planes of 0.5 and 10
	m_cam->setShape(30.0,10.0/7.0,0.05,350.0,ngl::PERSPECTIVE);
	// now create our light this is done after the camera so we can pass the
	// transpose of the projection Mat4 to the light to do correct eye space
	// transformations
	// first we create a mesh from an obj passing in the obj file and texture
	std::cout<<"building mesh\n";
	m_mesh = new ngl::Obj("models/pacman.obj", "textures/pacman.jpg");
	t_mesh = new ngl::Obj("models/square.obj", "textures/tetris.jpg");
	e_mesh = new ngl::Obj("models/square.obj", "textures/pikachu.png");
	s_mesh = new ngl::Obj("models/square.obj","textures/tetris.jpg");
	
	batt1 = new ngl::Obj("models/square.obj", "textures/batt1.png");
	batt2 = new ngl::Obj("models/square.obj", "textures/batt2.png");
	batt3 = new ngl::Obj("models/square.obj", "textures/batt3.png");
	batt4 = new ngl::Obj("models/square.obj", "textures/batt4.png");
	batt5 = new ngl::Obj("models/square.obj", "textures/batt5.png");
	
	rssi1 = new ngl::Obj("models/square.obj", "textures/rssi1.png");
	rssi2 = new ngl::Obj("models/square.obj", "textures/rssi2.png");
	rssi3 = new ngl::Obj("models/square.obj", "textures/rssi3.png");
	rssi4 = new ngl::Obj("models/square.obj", "textures/rssi4.png");
	rssi5 = new ngl::Obj("models/square.obj", "textures/rssi5.png");

	GLuint renderbuffers[2];

	// Let's create multiple names/ids at once.
	// To do this we declared our variable as a pointer *renderbuffers.
	glGenRenderbuffers(2, renderbuffers);

	// The index 0 will be our color render buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffers[0]);

	// The index 1 will be our depth render buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffers[1]);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffers[0]);
	// Set background color and clear buffers
	glClearColor(0.0, 0.0,0.0,1.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	std::cout<<"init GL\n";
	// Enable back face culling.
	glEnable(GL_DEPTH_TEST);
	// now to load the shader and set the values
	// grab an instance of shader manager
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	// we are creating a shader called Texture
	shader->createShaderProgram("Texture");
	// now we are going to create empty shaders for Frag and Vert
	shader->attachShader("TextureVertex",ngl::VERTEX);
	shader->attachShader("TextureFragment",ngl::FRAGMENT);
	// attach the source
	shader->loadShaderSource("TextureVertex","shaders/Vertex.vs");
	shader->loadShaderSource("TextureFragment","shaders/Fragment.fs");
	// compile the shaders
	shader->compileShader("TextureVertex");
	shader->compileShader("TextureFragment");
	// add them to the program
	shader->attachShaderToProgram("Texture","TextureVertex");
	shader->attachShaderToProgram("Texture","TextureFragment");
	// now bind the shader attributes for most NGL primitives we use the following
	// layout attribute 0 is the vertex data (x,y,z)
	shader->bindAttribute("Texture",0,"inVert");
	// attribute 1 is the UV data u,v (if present)
	shader->bindAttribute("Texture",1,"inUV");
	shader->bindAttribute("Texture",2,"inNormal");
	m_location=shader->getAttribLocation("Texture","tex");

	// now we have associated this data we can link the shader
	shader->linkProgramObject("Texture");

	(*shader)["Texture"]->use();
	std::cout<<"create vao\n";
	// now we need to create this as a VAO so we can draw it
	m_mesh->createVAO();
	t_mesh->createVAO();
	e_mesh->createVAO();
	s_mesh->createVAO();
	
	rssi1->createVAO();
	rssi2->createVAO();
	rssi3->createVAO();
	rssi4->createVAO();
	rssi5->createVAO();
	
	batt1->createVAO();
	batt2->createVAO();
	batt3->createVAO();
	batt4->createVAO();
	batt5->createVAO();
	
	//loads scene data
	loadConfigFile("config.txt");
}

void MyGLWindow::loadMatricesToShader(ngl::TransformStack &_tx) {
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	ngl::Mat4 MVP=_tx.getCurrAndGlobal().getMatrix()*m_cam->getVPMatrix();
	shader->setShaderParamFromMatrix("MVP",MVP);
}

void MyGLWindow::loadMatricesToShader2(ngl::TransformStack &_tx) {
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	ngl::Mat4 MVP=_tx.getCurrAndGlobal().getMatrix()*m_cam2->getVPMatrix();
	shader->setShaderParamFromMatrix("MVP",MVP);
}

//reads camera location and orientation from the SPI buffer
Player MyGLWindow::readSpiData()
{
	Player result;
	//lock mutex
	printf("locking mutex 3...\n");
	//pthread_mutex_lock(&mut);
	//read buffer
	result.gps_x = buffer()[1];
	result.gps_y = buffer()[0];
	result.roll = buffer()[2];
	result.pitch = -buffer()[3];
	result.yaw = -(buffer()[4] + 90);
	addToWindow((float)(unsigned int)charbuffer[0]);
	result.rssi = (int)windowAverage();
	result.battery = charbuffer[1];
	//unlock mutex
	//pthread_mutex_unlock(&mut);
	printf("Unlocked mutex 3...\n");

	return result;
};

void MyGLWindow::paintGL()
{
	// Start with a clear screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//update camera
	Player p = readSpiData();
	ngl::Vec4 From(p.gps_x+myX, p.gps_y+myY, myPitch);
	ngl::Vec4 To(p.gps_x+myX,p.gps_y+myY+5,myPitch);
	ngl::Vec4 Up(0,0,1);
	m_cam->set(From,To,Up);
	m_cam->setShape(30,(float)m_width/(float)m_height,0.05,350,ngl::PERSPECTIVE);
	
	m_cam->yaw(p.yaw + myYaw);
	m_cam->roll(p.roll + myRoll);
	m_cam->pitch(p.pitch + myPitch);
	
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	//yellow pacman model
	ngl::Transformation trans;
	ngl::Mat4 final;
	final.identity();

	//loop through game objects
	printf("locking mutex 4...\n");
	//pthread_mutex_lock(&mut);
	for(int i=0; i<256; i++) {
		object = objects[i];
		//check if this object should be drawn
		if(object.isVisible()) {
				//move and rotate object appropriately
				m_transformStack.setGlobal(trans);
				m_transformStack.pushTransform();
				m_transformStack.setPosition(object.x, object.y, 0.0);
				m_transformStack.setRotation(object.yaw, object.pitch, object.roll);
				ngl::Vec3 scaler = ngl::Vec3(object.scale, object.scale, object.scale);
				m_transformStack.setScale(scaler);
				loadMatricesToShader(m_transformStack);
				std::cout << i << " " << object.x << " " << object.y << " " << object.filename << "\n";
				//draw the object
				object.mesh->draw();
				m_transformStack.popTransform();
		}
	}
	//pthread_mutex_unlock(&mut);
	printf("Unlocked mutex 4...\n");
	
	
	glDisable(GL_DEPTH_TEST);
	
	//draw battery indicator
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(-6,-15,-6);
	loadMatricesToShader2(m_transformStack);
	if(p.battery <= 20) {
		batt1->draw();
	} 
	else if(p.battery <=40)
	{
		batt2->draw();
	} 
	else if(p.battery <=60) 
	{
		batt3->draw();
	} 
	else if(p.battery <=80) 
	{
		batt4->draw();
	} 
	else if(p.battery <=100)
	{
		batt5->draw();
	}
	m_transformStack.popTransform();
	
	//draw rssi indicator
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(6,-15,-6);
	loadMatricesToShader2(m_transformStack);
	if(p.rssi <= 15) {
		rssi1->draw();
	} 
	else if(p.rssi <=30)
	{
		rssi2->draw();
	} 
	else if(p.rssi <=45)
	{
		rssi3->draw();
	} 
	else if(p.rssi <=60)
	{
		rssi4->draw();
	} 
	else if(p.rssi <=75)
	{
		rssi5->draw();
	}
	m_transformStack.popTransform();
	
	glEnable(GL_DEPTH_TEST);
	
	//wait for next frame
	glFlush();
	glFinish();
	swapBuffers();
	//delete m_cam;
}

/*
Uses SDL to take keyboard input.
WASD moves the camera in X/Y,
left and right arrow keys roll the camera,
and up/down arrow keys moves the camera in Z
*/
void MyGLWindow::processEvents()
{
SDL_Event event;

	while ( SDL_PollEvent(&event) )
	{
		switch (event.type)
		{

		   case SDL_KEYDOWN:

					switch( event.key.keysym.sym )
					{
                    case SDLK_ESCAPE :
                        m_exit=true;
                        break;
					case SDLK_p:
						m_exit = true;
						break;
                    case SDLK_LEFT:
                        //myYaw -= 5;
			loadConfigFile("config.txt");
                        break;
                    case SDLK_RIGHT:
                        myYaw += 5;
                        break;
                    case SDLK_UP:
                        myPitch += 5;
                        break;
                    case SDLK_DOWN:
                        myPitch -= 5;
                        break;
					case SDLK_q:
						myRoll -= 5;
						break;
					case SDLK_e:
						myRoll += 5;
						break;
                    case SDLK_w:
						myX++;
						break;
					case SDLK_s:
						myX--;
						break;
					case SDLK_a:
						myY++;
						break;
					case SDLK_d:
						myY--;
						break;
                    default:
                        break;
					}

				break;
		}
	}
}

//loads a file describing the initial state of the 3D scene to be rendered
void MyGLWindow::loadConfigFile(std::string filename)
{
	for(int i=0; i<256; i++) {
		objects[i] = GameObject();
		//if (objects[i].mesh != NULL)
		//	delete objects[i].mesh;
	}
	int index;
	bool threed;
	float locx, locy, locz, orx, ory, orz;
	bool show;
	float scale;
	std::string id;

	std::ifstream file("config.txt");
	if(!file) {
		std::cout << "Configuration file not found!";
		exit();
	}

	//reads each game object into the objects array
	while(file >> index)
	{
		file >> threed >> locx >> locy >> locz >> orx >> ory >> orz >> filename >> show >> scale;
		objects[index] = GameObject(locx, locy, locz, orx, ory, orz, show, threed, index, filename, scale);
		std::cout << objects[index].x << " " << objects[index].y << " " << objects[index].isVisible() << "\n";
	}
}
