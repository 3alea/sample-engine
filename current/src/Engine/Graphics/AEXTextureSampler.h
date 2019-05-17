#ifndef CS200_TEXTURE_SAMPLER_H_
#define CS200_TEXTURE_SAMPLER_H_
#include "..\Core\AEXCore.h"
#include "AEXColor.h"
namespace AEX
{
	class  TextureSampler : public IBase
	{
		AEX_RTTI_DECL(TextureSampler, IBase);

		// --------------------------------------------------------------------
		// Public Definitions
	public:
		enum EBorderSide	{ eBS_Width, eBS_Height, eBS_WidthAndHeight, eBS_Count };
		enum EBorderMode	{ eBM_Repeat, eBM_Clamp, eBM_Color, eBM_Mirror, eBM_Count };

		// --------------------------------------------------------------------
		// Data memebers
	private:
		friend class Graphics;
		EBorderMode		mBorderMode[2];
		Color			mBorderColor;

		// OpenGL data
		u32	mGLHandle;// openGL handle
		// Creates the texture in OpenGL
		void CreateOpenGLSampler();

		// --------------------------------------------------------------------
		// Public Methods
	public: 
		
		// Constructor
		TextureSampler(EBorderMode borderMode = eBM_Clamp, Color borderColor = Color());

		// Destructor
		virtual ~TextureSampler();

		// Set Border mode
		void SetBorderMode(EBorderSide borderSide, EBorderMode borderMode);

		EBorderMode GetBorderMode(EBorderSide which);

		// Set Border Color
		void SetBorderColor(Color borderColor);
		Color GetBorderColor();
		
		// Bind/UnBind to Renderer
		void BindToTextureUnit(u32 textureUnit);
		void UnBindFromTextureUnit(u32 textureUnit);
	};
}

#endif