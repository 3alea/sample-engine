/// ---------------------------------------------------------------------------
/// \file			AEXTexture.cpp
/// \author			Thomas Komair
/// \brief			Implementation of the texture interface
/// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "AEXTexture.h"
#include "AEXGL.h"
#include "..\Debug\MyDebug.h"
#include "..\Debug\CheckGLError.h"

// image loading library
#define STB_IMAGE_IMPLEMENTATION
#include <Extern\STB\stb_image.h>

namespace AEX
{
	Texture::Texture()
		: mWidth(0)
		, mHeight(0)
		, mPixels(0)
		, mGLHandle(0)
	{
		CreateOpenGLTexture();
	}

	// constructor
	Texture::Texture(u32 width, u32 height, u8*pixels)
		: mWidth(width)
		, mHeight(height)
		, mPixels(pixels)
		, mGLHandle(0)
	{
		CreateOpenGLTexture();
	}
	Texture::Texture(const char * filename)
		: mWidth(0)
		, mHeight(0)
		, mPixels(NULL)
		, mGLHandle(0)
	{
		LoadFromFile(filename);
	}
	Texture::~Texture()
	{
		FreeData();
		if (mGLHandle)
		{
			glBindTexture(GL_TEXTURE_2D, mGLHandle); // bind
			glDeleteTextures(1, (GLuint*)(&mGLHandle)); // delete
			glBindTexture(GL_TEXTURE_2D, 0); // unbind
		}
	}

	void Texture::LoadFromFile(const char *filename)
	{
		if (!filename)
			return;

		// delete pixels if any
		FreeData();

		bool res = AEX::LoadImageFromFile(filename, mPixels, mWidth, mHeight);
		if (res)// success
		{
			// upload to GPU
			CreateOpenGLTexture();
			UploadToGPU();
		}
	}

	// Upload the texture data to openGL
	void Texture::UploadToGPU()
	{
		if (mGLHandle && (mWidth * mHeight) != 0)
		{
			// bind texture object
			Bind();

			// This will allocate memory for it
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(mPixels));

			// NOTE ON OPENGL: 
			// The line below will replace the data without allocating memory for it
			// This is more efficient to call when you know the texture size hasn't changed, only the contents of the texture. 
			//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(mPixels));

			// sanity check
			check_gl_error();

			// Free the data when done
			//FreeData();
		}
	}

	// Creates the texture in OpenGL
	void Texture::CreateOpenGLTexture()
	{
		if (mGLHandle) // already created
			return;

		// if we have a texture associated already -> kill it
		if (mGLHandle)
			glDeleteTextures(1, (GLuint*)&mGLHandle);

		// Create texture and bind it
		glGenTextures(1, (GLuint*)&mGLHandle);
		glBindTexture(GL_TEXTURE_2D, mGLHandle);
		check_gl_error();

		// set texture look-up parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		check_gl_error();
	}
	void Texture::FreeData()
	{
		if (mPixels) {
			delete[] mPixels;
			mPixels = NULL;
		}
	}
	void Texture::Bind()
	{
		if (mGLHandle)
			glBindTexture(GL_TEXTURE_2D, mGLHandle);
	}
	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// External Image loading // works with PNG, JPG and TGA
	bool LoadImageFromFile(const char * filename, u8 *& outPixels, u32 & outWidth, u32 & outHeight)
	{
		// Load the data
		int x, y, n, reqComp = 4;	// out parameters
		u8 * tempPixels = stbi_load(filename, &x, &y, &n, reqComp);	// 4 - forces output data to be of the form RGBA

		// Error
		if (tempPixels == NULL)
		{
			// TODO: provide error message
			return false;
		}

		//
		// Image loaded correctly

		// store width and height
		outWidth = (u32)x;
		outHeight = (u32)y;

		// allocate memory for our image
		outPixels = new u8[outWidth * outHeight * reqComp];

		// copy the data but flip the height to match openGL
		for (u32 i = 0; i < outHeight; ++i)
		{
			for (u32 j = 0; j < outWidth; ++j)
			{
				u32 op = reqComp * (i * outWidth + j);
				u32 ip = reqComp * ((outHeight - 1 - i) * outWidth + j);
				outPixels[op] = tempPixels[ip];
				outPixels[op + 1] = tempPixels[ip + 1];
				outPixels[op + 2] = tempPixels[ip + 2];
				outPixels[op + 3] = tempPixels[ip + 3];
			}
		}

		// free the image from stbi
		stbi_image_free(tempPixels);

		// success
		return true;
	}

}