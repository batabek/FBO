// =================================================================
//   File      : FrameBufferObject.h
//   Desc	   : A Simple C++ wrapper for handling the OpenGL Frame
//				 Buffer Object(FBO). The client can access using his
//				 own customized buffer names rather than using GL ids
//				 since this makes the management of fbos better.
//   Version   : 1.0
//   Author    : Berk Atabek - Copyright 2012 
//
//==================================================================

#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>

// Buffer's target mode parameter
enum BUFFER_TARGET_MODE {BTM_READ=0, BTM_WRITE, BTM_READ_WRITE };
// Render buffer type
enum RBUFFER_TYPE {RBT_COLOR=0, RBT_DEPTH, RBT_STENCIL};
// texture buffer
enum TEXTURE_BUFFER_TYPE {TBT_COLOR=0,TBT_DEPTH,TBT_STENCIL,TBT_DEPTH_AND_STENCIL};
enum TEXTURE_TYPE {TT_1D=0, TT_2D, TT_3D};


class FrameBufferObject
{
public:

	 // Constructor/Destructor
	 FrameBufferObject();
	 FrameBufferObject(BUFFER_TARGET_MODE mode);
	~FrameBufferObject();

	// switch to window-system provided buffers
	void switchToDefaultSystemBuffers();

	// Renderbuffer management methods
	void createRenderBuffer(std::string name, RBUFFER_TYPE type, GLenum internalFormat, GLsizei width, GLsizei height);
	void createRenderBufferAndAttach(std::string name, RBUFFER_TYPE type, GLenum internalFormat, GLsizei width, GLsizei height);
	void deleteRenderBuffer(std::string name);
	void attachRenderBuffer(std::string name);
	void detachRenderBuffer(std::string name);
	bool isRenderBufferUsed(std::string name);

	// Texture Object Attachment
	void attach1DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLint level);
	void attach2DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLsizei height, GLint level);
	void attach3DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLsizei height, GLsizei depth, GLint level, GLint layer);
	void detachTexture(std::string name);

	// Accessors
			GLuint				getID() const;
	const	GLuint				getRenderBufferID(std::string name)const;
	const	GLuint				getTextureBufferID(std::string name)const;
			bool				isUsed()const;
			unsigned			getNumRenderbuffers() const;
			unsigned			getNumAttachedTexturebuffers() const;
			BUFFER_TARGET_MODE	getBufferTargetMode()const;	
	
private:
	
	// renderbuffer state
	struct RenderBufferFormat {
		GLsizei width; // buffer's width
		GLsizei height; // buffer's height
		GLenum  intFormat;
		RBUFFER_TYPE bufferType;
	};

	// texture buffer state
	struct TextureBufferFormat {
		TEXTURE_BUFFER_TYPE attachmentPoint;
		TEXTURE_TYPE type;
		GLsizei width;
		GLsizei height;
		GLsizei depth; // buffer's depth , for volumetric textures
	};

	// each attachment can be identified by its unique name
	// which maps to its associated OpenGL id
	std::map<std::string, GLuint>m_renderBufferNames;
	std::map<std::string, GLuint>m_attachedTextureNames;
	
	// currently attached buffers list for this fbo
	std::map<GLuint, RenderBufferFormat>m_renderbuffers;
	std::map<GLuint, TextureBufferFormat>m_texturebuffers;

	GLuint					m_Id; // FBO id
	BUFFER_TARGET_MODE		m_BufferTargetMode;
	
};

#endif