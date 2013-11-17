
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

float MyGLWindow::floatbuffer[5] = { 0.0f };

MyGLWindow::MyGLWindow(ngl::EGLconfig *_config) : EGLWindow(_config)
{
	std::cout<<"My GL Window Ctor\n";
	makeSurface();
	m_exit=false;
}
MyGLWindow::~MyGLWindow()
{

}

float * MyGLWindow::buffer() {
	return floatbuffer;
}

void MyGLWindow::initializeGL()
{
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
	m_cam->setShape(30,(float)16.0/9.0,0.05,350,ngl::PERSPECTIVE);
	// now create our light this is done after the camera so we can pass the
	// transpose of the projection Mat4 to the light to do correct eye space
	// transformations
	// first we create a mesh from an obj passing in the obj file and texture
	std::cout<<"building mesh\n";
	m_mesh = new ngl::Obj("models/pacman.obj","textures/pacman.jpg");
	t_mesh = new ngl::Obj("models/tetris.obj","textures/tetris.jpg");
	std::cout<<"create vao\n";
	// now we need to create this as a VAO so we can draw it
	m_mesh->createVAO();
	t_mesh->createVAO();
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
	pthread_mutex_lock(&mut);
	result.gps_x = buffer()[0];
	result.gps_y = buffer()[1];
	result.pitch = buffer()[2];
	result.yaw = buffer()[3];
	result.roll = buffer()[4];
	pthread_mutex_unlock(&mut);

	return result;
};

void MyGLWindow::paintGL()
{
	// Start with a clear screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//update camera
	Player p = readSpiData();
	ngl::Vec4 From(p.gps_x, p.gps_y, 0);
	ngl::Vec4 To(p.gps_x+1,p.gps_y,0);
	ngl::Vec4 Up(0,0,1);
	m_cam= new ngl::Camera(From,To,Up,ngl::PERSPECTIVE);
	m_cam->setShape(45,(float)1920.0/1080.0,0.05,350,ngl::PERSPECTIVE);
	m_cam->pitch(-p.yaw);
	m_cam->roll(p.pitch);
	m_cam->yaw(-p.roll);

	ngl::ShaderLib *shader=ngl::ShaderLib::instance();
	(*shader)["Texture"]->use();

	//translate pacman model
	ngl::Transformation trans;
	ngl::Mat4 final;
	final.identity();
	trans.setMatrix(final);
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(5,0,0);
	loadMatricesToShader(m_transformStack);
	
	// draw the mesh
	m_mesh->draw();
	m_transformStack.popTransform();

	//translate model view
	final.identity();
	m_transformStack.setGlobal(trans);
	m_transformStack.pushTransform();
	m_transformStack.setPosition(5,2,0);
	loadMatricesToShader(m_transformStack);
	
	// draw the mesh
	t_mesh->draw();
	m_transformStack.popTransform();
	
	glFlush();
	glFinish();
	swapBuffers();
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
						case SDLK_ESCAPE :  m_exit=true; break;
						default : break;
					}

				break;
		}
	}

}



