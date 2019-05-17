/// ---------------------------------------------------------------------------
/// \file			AEXTexture.h
/// \author			Thomas Komair
/// \brief			Declares the interface for a texture object
/// ---------------------------------------------------------------------------
#ifndef AEX_TEXTURE_H_
#define AEX_TEXTURE_H_
// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "..\Core\AEXCore.h"

namespace AEX
{
	class  Texture : public IBase
	{
		AEX_RTTI_DECL(Texture, IBase);

		// --------------------------------------------------------------------
		// Data memebers
	private:
		friend class Graphics;
		u32 mWidth;
		u32 mHeight;
		u8* mPixels; // pixels in RGBA format
		u32	mGLHandle;// openGL handle

		// Creates the texture in OpenGL
		void CreateOpenGLTexture();
		void FreeData();

		// --------------------------------------------------------------------
		// Methods
	public:

		// constructor
		Texture();
		Texture(u32 width, u32 height, u8*pixels = NULL);
		Texture(const char * filename);

		// Destructor
		virtual ~Texture();

		// Load from file
		void LoadFromFile(const char *filename);

		// Upload the texture data to openGL
		void UploadToGPU();
		void Bind();
		void Unbind();

		// getters and setters
		u32 GetWidth() { return mWidth; }
		u32 GetHeight() { return mHeight; }
		u8* GetPixels(u32 x = 0, u32 y = 0) { return mPixels + ((mWidth * y + x) * 4); }
		u32 GetGLHandle() { return mGLHandle; }

	};


	// External Image loading // works with PNG, JPG and TGA
	bool LoadImageFromFile(const char * filename, u8 *& outPixels, u32 & outWidth, u32 & outHeight);
}
//-----------------------------------------------------------------------------
#endif