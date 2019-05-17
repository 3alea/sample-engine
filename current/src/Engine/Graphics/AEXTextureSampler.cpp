#include "AEXTextureSampler.h"

#include "Extern\GL\glew.h"
#include "Extern\GL\wglew.h"
#include "Extern\GL\GL.h"
#include "..\Debug\CheckGLError.h"
namespace AEX
{
	TextureSampler::TextureSampler(EBorderMode borderMode, Color borderColor)
		: mGLHandle(0)
	{
		CreateOpenGLSampler();
		SetBorderMode(eBS_WidthAndHeight, borderMode);
		SetBorderColor(borderColor);
	}

	TextureSampler::~TextureSampler()
	{
		if (mGLHandle)
		{
			glDeleteSamplers(1, (GLuint *)&mGLHandle);
		}
	}

	// Set Border mode
	void TextureSampler::SetBorderMode(EBorderSide borderSide, EBorderMode borderMode)
	{
		// Sanity Check
		if (mGLHandle == 0)
			return;

		// convert from engine enum to openGL
		GLint param;
		switch (borderMode)
		{
		case eBM_Clamp:
			param = GL_CLAMP_TO_EDGE;
			break;
		case eBM_Color:
			param = GL_CLAMP_TO_BORDER;
			break;
		case eBM_Repeat:
			param = GL_REPEAT;
			break;
		case eBM_Mirror:
			param = GL_MIRRORED_REPEAT;
			break;
		}
		check_gl_error();
		// set the border mode on the proper side
		switch (borderSide)
		{
		case eBS_Width:
			glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_S, param);
			check_gl_error();
			mBorderMode[0] = borderMode; // save border mode
			break;
		case eBS_Height:
			check_gl_error();
			glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_T, param);
			mBorderMode[1] = borderMode; // save border mode
			break;
		case eBS_WidthAndHeight:
			glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_S, param);
			check_gl_error();
			glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_T, param);
			check_gl_error();
			mBorderMode[0] = borderMode; // save border mode
			mBorderMode[1] = borderMode; // save border mode
			break;
		}
	}

	TextureSampler::EBorderMode TextureSampler::GetBorderMode(EBorderSide which)
	{
		u32 side = which % eBS_WidthAndHeight; // cannot return both
		return mBorderMode[side];
	}

	// Set Border Color
	void TextureSampler::SetBorderColor(Color borderColor)
	{ 
		mBorderColor = borderColor; 
		glSamplerParameterfv(mGLHandle, GL_TEXTURE_BORDER_COLOR, borderColor.v);
	}
	Color TextureSampler::GetBorderColor()
	{ 
		return mBorderColor; 
	}

	void TextureSampler::CreateOpenGLSampler()
	{
		if (0 != mGLHandle)
			return;

		check_gl_error();
		glGenSamplers(1, (GLuint *)&mGLHandle);
		check_gl_error();
		glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
		check_gl_error();
		glSamplerParameteri(mGLHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
		check_gl_error();
		glSamplerParameteri(mGLHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		check_gl_error();
		glSamplerParameteri(mGLHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		check_gl_error();
	}
	// Bind/UnBind to Renderer
	void TextureSampler::BindToTextureUnit(u32 textureUnit)
	{
		check_gl_error();
		if (mGLHandle)
			glBindSampler(textureUnit, (GLuint)mGLHandle);

		check_gl_error();
	}
	void TextureSampler::UnBindFromTextureUnit(u32 textureUnit)
	{
		glBindSampler(textureUnit, 0);
	}
}