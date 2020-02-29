// =================================================================
//   File      : FrameBufferObject.cpp
//   Desc	   : A Simple C++ wrapper for handling the OpenGL Frame
//				 Buffer Object(FBO). The client can access using his
//				 own customized buffer names rather than using GL ids
//				 since this makes the management of fbos better.
//   Version   : 1.0
//   Author    : Berk Atabek - Copyright 2012 
//
//==================================================================

#include "FrameBufferObject.h"

FrameBufferObject::FrameBufferObject(): m_BufferTargetMode(BTM_WRITE)
{
	
	glGenFramebuffers(1, &m_Id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id); // set default target mode to write

}

FrameBufferObject::FrameBufferObject(BUFFER_TARGET_MODE mode) : m_BufferTargetMode(mode)
{
	glGenFramebuffers(1, &m_Id);

	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	glBindFramebuffer(target, m_Id); //allocate storage for the generated FBO

}


FrameBufferObject::~FrameBufferObject()
{
	// To Do: delete textures
	for(std::map<std::string, GLuint>::iterator it = m_renderBufferNames.begin(); it != m_renderBufferNames.end(); ++it) 
	{
		std::string rbname = it->first;
		GLuint rbid = it->second;
		if(isRenderBufferUsed(rbname))
			glDeleteRenderbuffers(1, &rbid);
	}
	
	glDeleteFramebuffers(1, &m_Id);

};


void FrameBufferObject::createRenderBuffer(std::string name, RBUFFER_TYPE type, GLenum internalFormat, GLsizei width, GLsizei height)
{
	// fill in buffer data
	RenderBufferFormat bf;
	bf.bufferType = type;
	bf.intFormat=internalFormat;
	bf.width = width;
	bf.height = height;
	
	GLuint idRenderBuffer;
	glGenRenderbuffers(1, &idRenderBuffer);
	m_renderBufferNames[name] = idRenderBuffer;
	m_renderbuffers[idRenderBuffer] = bf;
	
	glBindRenderbuffer(GL_RENDERBUFFER, idRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, bf.intFormat, bf.width, bf.height);
}


void FrameBufferObject::createRenderBufferAndAttach(std::string name, RBUFFER_TYPE type, GLenum internalFormat, GLsizei width, GLsizei height)
{
	RenderBufferFormat bf;
	bf.bufferType = type;
	bf.intFormat=internalFormat;
	bf.width = width;
	bf.height = height;
	
	GLuint idRenderBuffer;
	glGenRenderbuffers(1, &idRenderBuffer);
	m_renderBufferNames[name] = idRenderBuffer;
	m_renderbuffers[idRenderBuffer] = bf;
	glBindRenderbuffer(GL_RENDERBUFFER, idRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, bf.intFormat, bf.width, bf.height);
	// attach to FBO
	// check to see if created fbo is the currently bound
	// i.e. glBindFramebuffer()...
	GLenum attachmentType;
	switch(type)
	{
		case RBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case RBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case RBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, idRenderBuffer);
	

}

void FrameBufferObject::attachRenderBuffer(std::string name)
{
	// attach to the currently bound FBO
	// check to see if created fbo is the currently bound
	// i.e. glBindFramebuffer()...

	// name check
	if(getRenderBufferID(name)<=0) return;
	
	GLuint id = m_renderBufferNames[name];
	const RenderBufferFormat& bf = m_renderbuffers[id];
	
	GLenum attachmentType;
	switch(bf.bufferType)
	{
		case RBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case RBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case RBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, id);
	
}


void FrameBufferObject::detachRenderBuffer(std::string name)
{
	// name check
	if(getRenderBufferID(name)<=0) return;

	GLuint id = m_renderBufferNames[name];
	RenderBufferFormat& bf = m_renderbuffers[id];

	GLenum attachmentType;
	switch(bf.bufferType)
	{
		case RBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case RBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case RBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}

	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	glBindFramebuffer(target, m_Id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, 0);

	
}


void FrameBufferObject::deleteRenderBuffer(std::string name)
{
	// name check
	if(getRenderBufferID(name)<=0) return;
	GLuint id = m_renderBufferNames[name];
	glDeleteRenderbuffers(1, &id);
}


bool FrameBufferObject::isRenderBufferUsed(std::string name)
{
	// name check
	if(getRenderBufferID(name)<=0) return false;

	GLuint id = m_renderBufferNames[name];
	return GL_TRUE==glIsRenderbuffer(id) ? true : false;
}


void FrameBufferObject::attach1DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLint level)
{
	// create a texture object 
	GLuint textureid;
	glGenTextures(1, &textureid);
	m_attachedTextureNames[name] = textureid;
	
	TextureBufferFormat tbf;
	tbf.attachmentPoint=tbtype ;
	tbf.width = width;
	tbf.height = 0;
	tbf.type = TT_1D;

	m_texturebuffers[textureid] = tbf;

	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}


	GLenum attachmentType;
	switch(tbf.attachmentPoint)
	{
		case TBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case TBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case TBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}
	
	// set storage for the texture
	glBindTexture(GL_TEXTURE_1D, textureid);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, width, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );

	// attach to fbo
	glFramebufferTexture1D(target, attachmentType, GL_TEXTURE_1D, textureid, level);
	
}


void FrameBufferObject::attach2DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLsizei height, GLint level)
{
	
	// create a texture object 
	GLuint textureid;
	glGenTextures(1, &textureid);
	m_attachedTextureNames[name] = textureid;
	
	TextureBufferFormat tbf;
	tbf.attachmentPoint=tbtype ;
	tbf.width = width;
	tbf.height = height;
	tbf.type = TT_2D;

	m_texturebuffers[textureid] = tbf;

	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	GLenum attachmentType;
	switch(tbf.attachmentPoint)
	{
		case TBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case TBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case TBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}
	
	// set storage for the texture
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	// attach to fbo
	glFramebufferTexture2D(target, attachmentType, GL_TEXTURE_2D, textureid, level);

}

void FrameBufferObject::attach3DTexture(std::string name, TEXTURE_BUFFER_TYPE tbtype, GLsizei width, GLsizei height, GLsizei depth, GLint level, GLint layer)
{
	
	// create a texture object 
	GLuint textureid;
	glGenTextures(1, &textureid);
	m_attachedTextureNames[name] = textureid;

	TextureBufferFormat tbf;
	tbf.attachmentPoint=tbtype ;
	tbf.width = width;
	tbf.height = height;
	tbf.depth = depth;
	tbf.type = TT_3D;

	m_texturebuffers[textureid] = tbf;

	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	GLenum attachmentType;
	switch(tbf.attachmentPoint)
	{
		case TBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case TBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case TBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}
	
	// set storage for the texture
	glBindTexture(GL_TEXTURE_3D, textureid);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glFramebufferTexture3D(target, attachmentType, GL_TEXTURE_3D, textureid, level, layer);
	
}

void FrameBufferObject::detachTexture(std::string name)
{
	// name check
	if(getTextureBufferID(name)<=0) return;

	GLuint id = m_attachedTextureNames[name];
	TextureBufferFormat& tbf = m_texturebuffers[id];
	
	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	GLenum attachmentType;
	switch(tbf.attachmentPoint)
	{
		case TBT_COLOR: attachmentType=GL_COLOR_ATTACHMENT0;break;
		case TBT_DEPTH: attachmentType=GL_DEPTH_ATTACHMENT;break;
		case TBT_STENCIL: attachmentType=GL_STENCIL_ATTACHMENT;break;
	}

	switch(tbf.type)
	{
		case TT_1D: glFramebufferTexture1D(target, attachmentType, GL_TEXTURE_1D, 0, 0);break;
		case TT_2D: glFramebufferTexture2D(target, attachmentType, GL_TEXTURE_2D, 0, 0); break;
		case TT_3D: glFramebufferTexture3D(target, attachmentType, GL_TEXTURE_3D, 0, 0 ,0);break;
	}

	
}


GLuint FrameBufferObject::getID() const
{
	return m_Id;
}

BUFFER_TARGET_MODE FrameBufferObject::getBufferTargetMode()const
{
	return m_BufferTargetMode;
}

bool FrameBufferObject::isUsed()const
{
	return GL_TRUE == glIsFramebuffer(m_Id) ? true : false;
}


unsigned FrameBufferObject::getNumRenderbuffers() const
{
	return m_renderbuffers.size();
}


unsigned FrameBufferObject::getNumAttachedTexturebuffers() const
{
	return m_texturebuffers.size();
}


const GLuint FrameBufferObject::getRenderBufferID(std::string name)const 
{
	// name check
	std::map<std::string, GLuint>::const_iterator it = m_renderBufferNames.find(name);
	if(it==m_renderBufferNames.end())
	{
		std::cerr << "Error: " << name << " renderbuffer is not found...\n";
		return 0;
	}
	else
	{
		return it->second;
	}
}


const GLuint FrameBufferObject::getTextureBufferID(std::string name)const
{
	// name check
	std::map<std::string, GLuint>::const_iterator it = m_attachedTextureNames.find(name);
	if(it==m_attachedTextureNames.end())
	{
		std::cerr << "Error: " << name << " texturebuffer is not found...\n";
		return 0;
	}
	else
	{
		return it->second;
	}
}

void FrameBufferObject::switchToDefaultSystemBuffers()
{
	GLenum target;
	switch(m_BufferTargetMode)
	{
		case 0:  target = GL_READ_FRAMEBUFFER; break;
		case 1:  target = GL_DRAW_FRAMEBUFFER; break;
		case 2:  target = GL_FRAMEBUFFER; break;
		default: target = GL_FRAMEBUFFER; break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


