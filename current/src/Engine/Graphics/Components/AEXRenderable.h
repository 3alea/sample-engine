// ----------------------------------------------------------------------------
#ifndef AEX_RENDERABLE_H_
#define AEX_RENDERABLE_H_
// ----------------------------------------------------------------------------

#include "..\..\Composition\AEXComponent.h"
namespace AEX
{
	class TransformComp;
	class TransformComp3D;
	class ShaderProgram;
	class Model;
	class Texture;
	class TextureSampler;

	// ---------------------------------------------------------------------------
	// CLASS:	Renderable:
	// PURPOSE:	A renderable is any object that can be processed by the rendering
	//			pipeline. Most importantly, it can be culled by the camera.
	//			Note: The renderable needs to have a transform
	class Renderable : IComp
	{
		AEX_RTTI_DECL(Renderable, IComp);
	public:

	public:
		Renderable();
		virtual ~Renderable();
		virtual void Initialize();
		virtual void Shutdown();
		virtual void Render();

		bool IsVisible();
		void SetVisible(bool visible);
		TransformComp * GetTransform();
		TransformComp3D * GetTransform3D();

		ShaderProgram	*pShaderRes;
		Model			*pModelRes;
		Texture			*pTextureRes;
		TextureSampler	*pSamplerRes;

	protected:
		friend class Graphics;
		bool			mIsVisible;
		TransformComp	*pTransform;
		TransformComp3D	*pTransform3D;
	};
}

// ----------------------------------------------------------------------------
#endif