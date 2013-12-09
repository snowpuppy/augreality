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

#ifndef MYGLWINDOW_H__
#define MYGLWINDOW_H__

#define CHIP_SELECT 1
#define HIGH 1
#define LOW 0
#define SENSOR_SIZE 22
#define MYPI 3.1415926535
#define WINDOW_SIZE 15

#include <ngl/EGLWindow.h>
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/TransformStack.h>
#include <ngl/Obj.h>
#include <ngl/Text.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string>
#include <sstream>

#include "gameobject.h"

extern pthread_mutex_t mut;

struct Player {
	float gps_x;
	float gps_y;
	float pitch;
	float yaw;
	float roll;
	uint8_t rssi;
	uint8_t battery;
};

struct matrix_t
{
    float val[9];
};

struct vector_t
{
    float val[3];
};


// Some functions.
vector_t multiplyMatVec(vector_t vec, matrix_t A);
matrix_t multiplyMat(matrix_t A, matrix_t B);
void setMat(matrix_t *rotX, matrix_t *rotY, matrix_t *rotZ, float pitch, float yaw, float roll);

/// @brief this class create our window by inheriting the features of the EGL Window
class MyGLWindow : public ngl::EGLWindow
{
	public :
		void loadConfigFile(std::string filename) ;
		static float floatbuffer[5];
		static uint8_t charbuffer[2];
		static float *buffer();
		
		/// @brief ctor
		/// @param[in] _config an optional configuration for the buffers etc
		MyGLWindow(ngl::EGLconfig *_config=0);
		/// @brief dtor will close down the vc and re-set EGL
		virtual ~MyGLWindow();
		/// @brief the is the main drawing function should only be called once initalizeGL has
		/// been called
		virtual void paintGL();
		void processEvents();
		inline bool exit(){return m_exit;}
		void print(std::string str);
	protected :
		/// @brief one time OpenGL initialisation
		virtual void initializeGL();
	private :
	  float windowAverage();
	  void addToWindow(float f);
	float window[WINDOW_SIZE];
	int windowIndex;
	ngl::Text *text;
	void readSensorPacket(unsigned char *buf);
	Player readSpiData();
	void loadMatricesToShader(
														 ngl::TransformStack &_tx
													 );




		ngl::TransformStack m_transformStack;
		ngl::Camera *m_cam;
		ngl::Camera *twod_cam;
		ngl::Light *m_light;
		ngl::Obj *m_mesh;
		ngl::Obj *t_mesh;
		ngl::Obj *e_mesh;
		ngl::Obj *s_mesh;
		ngl::Obj *twod_mesh;
		ngl::Obj *rssi1;
		ngl::Obj *rssi2;
		ngl::Obj *rssi3;
		ngl::Obj *rssi4;
		ngl::Obj *rssi5;
		ngl::Obj *batt1;
		ngl::Obj *batt2;
		ngl::Obj *batt3;
		ngl::Obj *batt4;
		ngl::Obj *batt5;
		
		GLuint vboIds[2];
		GLuint m_location;
        float myPitch, myRoll, myYaw, myX, myY;

		    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec4 m_modelPos;
    bool m_exit;
    GameObject objects[256];
    GameObject object;

};

#endif
