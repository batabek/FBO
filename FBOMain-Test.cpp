// ===========================================================
//   File      : FBOMain.cpp
//   Desc	   : Frame Buffer Object Test code   
//   Author    : Berk Atabek - Copyright 2012 
//
//============================================================

#pragma comment (lib , "glut32.lib") // Link GLUT  library
#pragma comment(lib, "glew32.lib") // Link GLEW library
#define _CRT_SECURE_NO_DEPRECATE // disable VS deprecation warnings

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/glew.h>     // for GLEW
#include <GL/glut.h>     // for GLUT
#include "FrameBufferObject.h"

//=========================================
// Window size
//=========================================
#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768
#define WINDOWX_POS   500
#define WINDOWY_POS   100

//=========================================
// Render Buffer dimensions
//=========================================
#define DEPTH_BUFFER_W 512
#define DEPTH_BUFFER_H 512

#define TEXTURE_BUFFER_W 512
#define TEXTURE_BUFFER_W 512

FrameBufferObject* g_fbo = NULL;
GLfloat angle = 0.0f;


void DrawQuad()
{
	// draw a screen aligned quad
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	// bottom left corner
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	// bottom right corner
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	// upper right corner
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	// upper left corner
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();

}


static void release(void)
{
	delete g_fbo;
}


// Keyboard button event handler callback
void keyboardhandler(unsigned char key, int x, int y)
{
		
	switch(key)
	{
	   // Quit from App.
	   case 27:
		   release();
		   exit(0);
		   break;
	   
	}
	
	glutPostRedisplay();
}

// Special Virtual key code event handler callback
void special(int k, int x, int y)
{
	
   switch(k)
   {
      case GLUT_KEY_UP:
		  break;
      case GLUT_KEY_DOWN:
		  break;
	  case GLUT_KEY_LEFT:
		  break;
	  case GLUT_KEY_RIGHT:
		  break;
	  case GLUT_KEY_PAGE_UP:
		  break;
	  case GLUT_KEY_PAGE_DOWN:
		  break;
     
   }
     
   glutPostRedisplay();
}


// Reshape function
void reshape(int nWidth, int nHeight)
{
	// Set Viewport to cover full window
	glViewport(0, 0, (GLsizei)nWidth, (GLsizei)nHeight);
	
	// Update projection transform parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLdouble)nWidth / (GLdouble)nHeight, 1.0f, 100.0f);
    
}


// Render callback function
void display(void)
{
	
	// prepare to render onto the texture
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_fbo->getID());
	
	// Do not forget to set viewport, it must be same size with the FBO
	glViewport(0, 0, 512, 512);
	
	// clear renderbuffers.
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.0, 100.0);
	
	// rendering to texture starts in here
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 3.0,
		      0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	//glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glutSolidTeapot(0.6);
	glPopMatrix();
		
	// now switch back to the window system provided buffer to display the texture
	g_fbo->switchToDefaultSystemBuffers();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update projection transform parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLdouble)WINDOW_WIDTH / (GLdouble)WINDOW_HEIGHT, 1.0f, 100.0f);

	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, fbo->getTextureBufferID("2DTextureBuffer1"));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(angle, 1.0, 0.0, 0.0);
	DrawQuad();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
		
	//Swap  back & front buffers
	glutSwapBuffers();
  	
}

void initFBO()
{
	// create a fbo
	g_fbo = new FrameBufferObject(BTM_WRITE);
	if(g_fbo)
	{
		std::cerr << "Success: FBO is created with the GL id: " << g_fbo->getID() << std::endl; 
	}

	const std::string db1 = "DepthBuffer1";
	const std::string tb1 = "2DTextureBuffer1";
	const GLsizei w=1<<9, h=w;

	// create a texture buffer and a depth buffer
	g_fbo->createRenderBuffer(db1, RBT_DEPTH, GL_DEPTH_COMPONENT24, w, h);
	g_fbo->attachRenderBuffer(db1);
	g_fbo->attach2DTexture(tb1, TBT_COLOR, w, h, 0);

}

void init()
{
	
	// init GLEW
	glewExperimental=GL_TRUE;
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// Check for shaders support
	if(GLEW_ARB_shader_objects) {
		printf("Shaders are supported\n");
	}
	
	// check for FBO support
	if(GLEW_ARB_framebuffer_object)
	{
		printf("Frame Buffer Objects are supported\n");
	}
	
	GLint numMaxColorAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &numMaxColorAttach);
	printf("Max Color Attachments: %d\n", numMaxColorAttach);

	// init buffer objects
	initFBO();

	// set texture parameters
	glBindTexture(GL_TEXTURE_2D, g_fbo->getTextureBufferID("2DTextureBuffer1"));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// Smooth Shading
	glShadeModel(GL_SMOOTH);
	//glDisable(GL_LIGHTING);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	
}

void idle()
{
	angle += 0.2f;
	glutPostRedisplay();
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(WINDOWX_POS, WINDOWY_POS);
	glutCreateWindow("OpenGL FBO Test App");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardhandler);
	glutSpecialFunc(special);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}















