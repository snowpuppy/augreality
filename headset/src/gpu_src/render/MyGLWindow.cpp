
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

float MyGLWindow::floatbuffer[5] = { 0.0f };
uint8_t MyGLWindow::charbuffer[2] = { 0 };

void MyGLWindow::addToWindow(float f) {
	window[windowIndex] = f;
	windowIndex = (windowIndex+1) % (WINDOW_SIZE-1);
}

int compare(const void * a, const void * b) {
	return( *(int *)a - *(int *)b );
}

float MyGLWindow::windowAverage() {
	/*
	//sliding window filter
	float sum = 0.0f;
	for(int i=0; i<WINDOW_SIZE; i++)
	  sum+=window[i];
	return sum/(float)WINDOW_SIZE;
	*/
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
	for(int i=0; i<WINDOW_SIZE; i++) window[i]=0.0f;
}
MyGLWindow::~MyGLWindow()
{

}

float * MyGLWindow::buffer() {
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
	// set the shape using FOV 45 Aspect Ratio based on Width and Height
	// The final two are near and far clipping planes of 0.5 and 10
	m_cam->setShape(30.0,10.0/7.0,0.05,350.0,ngl::PERSPECTIVE);
	// now create our light this is done after the camera so we can pass the
	// transpose of the projection Mat4 to the light to do correct eye space
	// transformations
	// first we create a mesh from an obj passing in the obj file and texture
	std::cout<<"building mesh\n";
	m_mesh = new ngl::Obj("models/pacman.obj", "models/pikachu.png");
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
}


void MyGLWindow::loadMatricesToShader(ngl::TransformStack &_tx)
{
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	ngl::Mat4 MVP=_tx.getCurrAndGlobal().getMatrix()*m_cam->getVPMatrix();
	shader->setShaderParamFromMatrix("MVP",MVP);
}

Player MyGLWindow::readSpiData() {
	Player result;
	//lock mutex
	pthread_mutex_lock(&mut);
	//read buffer
	result.gps_x = buffer()[1];
	result.gps_y = buffer()[0];
	result.roll = buffer()[2];
	result.pitch = -buffer()[3];
	//addToWindow(-(buffer()[4]+90));
	//result.yaw = windowAverage();
	result.yaw = -(buffer()[4] + 90);
	addToWindow((float)(unsigned int)charbuffer[0]);
	result.rssi = (int)windowAverage();
	//result.rssi = charbuffer[0];
	result.battery = charbuffer[1];
	printf("RSSI: %d Battery: %d Yaw:%.3f\n", (int)result.rssi, (int)result.battery, result.yaw);
	//unlock mutex
	pthread_mutex_unlock(&mut);

	return result;
};

void MyGLWindow::paintGL()
{
	// Start with a clear screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//update camera
	Player p = readSpiData();
	ngl::Vec4 From(p.gps_x+myX, p.gps_y+myY, 0);
	ngl::Vec4 To(p.gps_x+myX,p.gps_y+myY+1,0);
	ngl::Vec4 Up(0,0,1);
	m_cam->set(From,To,Up);
	m_cam->setShape(30,(float)m_width/(float)m_height,0.05,350,ngl::PERSPECTIVE);
	
	m_cam->yaw(p.yaw+myYaw);
	m_cam->roll(p.roll+myRoll);
	m_cam->pitch(p.pitch+myPitch);
	
	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	//yellow pacman model
	ngl::Transformation trans;
	ngl::Mat4 final;
	final.identity();
	final.translate(0,5,0);
	//trans.setMatrix(final);
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	//m_transformStack.setPosition(10+myX, myY, 0);
	m_transformStack.setPosition(0, 5, 0);
	loadMatricesToShader(m_transformStack);
	m_mesh->draw();
	m_transformStack.popTransform();

	for(int i=0; i-1<p.rssi/10; i++) {
	//purple tetris model
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(5,0,i);
	loadMatricesToShader(m_transformStack);
	t_mesh->draw();
	m_transformStack.popTransform();
	}

	for(int i=0; i-1<p.battery/10; i++) {
	//yellow tetris model
	final.rotateZ(i*10);
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(-5,0,i);
	loadMatricesToShader(m_transformStack);
	e_mesh->draw();
	m_transformStack.popTransform();
	}

	//blue pacman model
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(0,-5,0);
	loadMatricesToShader(m_transformStack);
	s_mesh->draw();
	m_transformStack.popTransform();
	
	//setup for 2d drawing
	ngl::Vec4 From2d(0, 0, 0);
	ngl::Vec4 To2d(0, -1, 0);
	ngl::Vec4 Up2d(0, 0, -1);
	m_cam = new ngl::Camera(From2d,To2d,Up2d,ngl::PERSPECTIVE);
	m_cam->setShape(45,(float)m_width/(float)m_height,0.05,350,ngl::PERSPECTIVE);
	
	glDisable(GL_DEPTH_TEST);
	
	//draw battery indicator
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(-6,-15,-6);
	loadMatricesToShader(m_transformStack);
	if(p.battery <= 20) {
		batt1->draw();
	} else if(p.battery <=40) {
		batt2->draw();
	} else if(p.battery <=60) {
		batt3->draw();
	} else if(p.battery <=80) {
		batt4->draw();
	} else if(p.battery <=100) {
		batt5->draw();
	}
	m_transformStack.popTransform();
	
	//draw rssi indicator
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(6,-15,-6);
	loadMatricesToShader(m_transformStack);
	if(p.rssi <= 15) {
		rssi1->draw();
	} else if(p.rssi <=30) {
		rssi2->draw();
	} else if(p.rssi <=45) {
		rssi3->draw();
	} else if(p.rssi <=60) {
		rssi4->draw();
	} else if(p.rssi <=75) {
		rssi5->draw();
	}
	m_transformStack.popTransform();
	
	glEnable(GL_DEPTH_TEST);
	
	/*
	ngl::Transformation trans;
	ngl::Mat4 final;
	ngl::Mat4 translate;
	ngl::Mat4 rotX;
	ngl::Mat4 rotY;
	ngl::Mat4 rotZ;
	for(int i=0; i<256; i++) {
		object = objects[i];
		//check if this object should be drawn
		if(object.isVisible()) {
				//move and rotate object appropriately
				translate.translate(object.getX(), object.getY(), 0.0f);
				rotX.rotateX(object.getRoll());
				rotY.rotateY(object.getPitch());
				rotZ.rotateZ(object.getYaw());
				final = translate*rotX*rotY*rotZ;
				trans.setMatrix(final);
				m_transformStack.setGlobal(trans);
				m_transformStack.pushTransform();
				loadMatricesToShader(m_transformStack);
	
				//draw the object
				object.getMesh()->draw();
				m_transformStack.popTransform();
		}
	}
	*/
	
	//draw text
	//std::ostringstream oss;
	//oss << "Battery: " << p.battery << "% RSSI: " << p.rssi;
	//const std::string status = oss.str();
	//text->renderText(100, 50, "hello world");
	
	//wait for next frame
	glFlush();
	glFinish();
	swapBuffers();
	delete m_cam;
}

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
                        myYaw -= 5;
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

// Perform matrix*Vector multiplication
vector_t multiplyMatVec(vector_t vec, matrix_t A)
{
    vector_t rVec = {{0}};
    rVec.val[0] = A.val[0]*vec.val[0] + A.val[1]*vec.val[1] + A.val[2]*vec.val[2];
    rVec.val[1] = A.val[3]*vec.val[0] + A.val[4]*vec.val[1] + A.val[5]*vec.val[2];
    rVec.val[2] = A.val[6]*vec.val[0] + A.val[7]*vec.val[1] + A.val[8]*vec.val[2];
    return rVec;
}

// Perform 3x3 matrix multiplication.
matrix_t multiplyMat(matrix_t A, matrix_t B)
{
    matrix_t C = {{0}};
    C.val[0] = A.val[0]*B.val[0] + A.val[1]*B.val[3] + A.val[2]*B.val[6];
    C.val[1] = A.val[0]*B.val[1] + A.val[1]*B.val[4] + A.val[2]*B.val[7];
    C.val[2] = A.val[0]*B.val[2] + A.val[1]*B.val[5] + A.val[2]*B.val[8];
    C.val[3] = A.val[3]*B.val[0] + A.val[4]*B.val[3] + A.val[5]*B.val[6];
    C.val[4] = A.val[3]*B.val[1] + A.val[4]*B.val[4] + A.val[5]*B.val[7];
    C.val[5] = A.val[3]*B.val[2] + A.val[4]*B.val[5] + A.val[5]*B.val[8];
    C.val[6] = A.val[6]*B.val[0] + A.val[7]*B.val[3] + A.val[8]*B.val[6];
    C.val[7] = A.val[6]*B.val[1] + A.val[7]*B.val[4] + A.val[8]*B.val[7];
    C.val[8] = A.val[6]*B.val[2] + A.val[7]*B.val[5] + A.val[8]*B.val[8];

    return C;
}

void setMat(matrix_t *rotX, matrix_t *rotY, matrix_t *rotZ, float pitch, float yaw, float roll)
{
    float sinRoll = sin(roll*MYPI/180);
    float cosRoll = cos(roll*MYPI/180);
    float sinYaw  = sin(yaw*MYPI/180);
    float cosYaw  = cos(yaw*MYPI/180);
    float sinPitch = sin(pitch*MYPI/180);
    float cosPitch = cos(pitch*MYPI/180);
    rotX->val[0] = 1;
    rotX->val[1] = 0;
    rotX->val[2] = 0;
    rotX->val[3] = 0;
    rotX->val[4] = cosPitch;
    rotX->val[5] = sinPitch;
    rotX->val[6] = 0;
    rotX->val[7] = -sinPitch;
    rotX->val[8] = cosPitch;

    rotY->val[0] = cosYaw;
    rotY->val[1] = 0;
    rotY->val[2] = -sinYaw;
    rotY->val[3] = 0;
    rotY->val[4] = 1;
    rotY->val[5] = 0;
    rotY->val[6] = sinYaw;
    rotY->val[7] = 0;
    rotY->val[8] = cosYaw;


    rotZ->val[0] = cosRoll;
    rotZ->val[1] = sinRoll;
    rotZ->val[2] = 0;
    rotZ->val[3] = -sinRoll;
    rotZ->val[4] = cosRoll;
    rotZ->val[5] = 0;
    rotZ->val[6] = 0;
    rotZ->val[7] = 0;
    rotZ->val[8] = 1;
    /*
    matrix_t rotX = {1       ,0,        0,
                     0        cosPitch, sinPitch,
                     0,       -sinPitch,cosPitch};
    matrix_t rotY = {cosYaw  ,0,        -sinYaw,
                     0        1,        0,
                     sinYaw,  0,        cosYaw};
    matrix_t rotZ = {cosRoll, sinRoll,  0,
                     -sinRoll,cosRoll,  0,
                     0,       0,        1};
    */
}
/*
void MyGLWindow::print(std::string str) {
	glDisable(GL_DEPTH_TEST);
	//buffer to hold 'pixel' data
	bool pixels[4][5];
	
	ngl::Transformation trans;
	ngl::Mat4 final;
	
	//setup new camera matrix
	ngl::Vec4 From(0, 0, 0);
	ngl::Vec4 To(0, 0, 1);
	ngl::Vec4 Up(0,1, 0);
	m_cam= new ngl::Camera(From,To,Up,ngl::PERSPECTIVE);
	m_cam->setShape(45,(float)m_width/(float)m_height,0.05,350,ngl::PERSPECTIVE);

	//loop through string
	for(int i=0; i<str.length; i++) {
		switch(str[i]) {
		case '0':
			pixels = {	{true,true,true,true},
						{true,false,false,true},
						{true,false,false,true},
						{true,false,false,true},
						{true,true,true,true} };
			break;
		
		case '1':
			pixels = {	{false,false,true,false},
						{false,true,true,false},
						{false,false,true,false},
						{false,false,true,false},
						{false,true,true,true} };
			break;
			
		case '2':
			pixels = {	{true,true,true,true},
						{false,false,false,true},
						{true,true,true,true},
						{true,false,false,false},
						{true,true,true,true} };
			break;
			
		case '3':
			pixels = {	{true,true,true,true},
						{false,false,false,true},
						{true,true,true,true},
						{false,false,false,true},
						{true,true,true,true} };
			break;
			
		case '4':
			pixels = {	{true,false,false,true},
						{true,false,false,true},
						{true,true,true,true},
						{false,false,false,true},
						{false,false,false,true} };
			break;
		case '5':
			pixels = {	{true,false,false,true},
						{true,false,false,false},
						{true,true,true,true},
						{false,false,false,true},
						{true,true,true,true} };
			break;
		default:
			pixels = {false};
			break;
		}
		//loop through pixels in character
		for(int j=0; j<4; j++) {
			for(int k=0; k<5; k++) {
				//draw single 'pixel' if it should be drawn
				if (pixels[j][k]) {
					//translate pacman model
					final.identity();
					trans.setMatrix(final);
					m_transformStack.setGlobal(trans);
					m_transformStack.pushTransform();
					m_transformStack.setPosition((5*i)+j,k,50);
					loadMatricesToShader(m_transformStack);
					// draw the mesh
					m_mesh->draw();
					m_transformStack.popTransform();
				}
			}
		}
	}
	glEnable(GL_DEPTH_TEST);
}
*/
