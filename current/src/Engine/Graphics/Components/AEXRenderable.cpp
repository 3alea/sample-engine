#include "AEXRenderable.h"
#include "..\AEXGraphics.h"
#include "..\..\Composition\AEXGameObject.h"
#include "..\..\Scene\AEXTransformComp.h"
#include "..\..\Debug\MyDebug.h"
#include "..\AEXGL.h"

namespace AEX
{
	// ---------------------------------------------------------------------------
	// CLASS:	Renderable:
	// ---------------------------------------------------------------------------
	Renderable::Renderable()
		: mIsVisible(true)
		, pTransform(NULL)
		, pTransform3D(NULL)
		, pShaderRes(NULL)
		, pModelRes(NULL)
		, pTextureRes(NULL)
		, pSamplerRes(NULL)
	{}
	Renderable::~Renderable()
	{}
	void Renderable::Initialize()
	{
		if (GetOwner())
		{
			pTransform = GetOwner()->GetComp<TransformComp>();
			pTransform3D = GetOwner()->GetComp<TransformComp3D>();
		}
		// Add self to graphics system. TODO(Thomas): Implement this in the next lesson
		//Graphics::Instance()->AddRenderable(this);
	}
	void Renderable::Shutdown()
	{
		// Add self to graphics system. TODO(Thomas): Implement this in the next lesson
		//Graphics::Instance()->RemoveRenderable(this);
	}

	void Renderable::Render()
	{
		DebugAssert(pModelRes != NULL, "Renderable: Model resource is NULL");

		// Bind Resources
		{
			// Set Texture to current
			if (pTextureRes)
			{
				pTextureRes->Bind();

				// bind the sampler if one is set
				if (pSamplerRes)
					pSamplerRes->BindToTextureUnit(0);
			}

			// Send data to shader
			if (pShaderRes)
			{
				// Compute model and send to shader
				AEMtx44 mtxModel = AEMtx44::Identity();
				if (pTransform3D)
					mtxModel = pTransform3D->GetModelToWorld();
				else
					mtxModel = pTransform->GetModelToWorld4x4();

				pShaderRes->Bind();
				check_gl_error();
				pShaderRes->SetShaderUniform("mtxModel", &mtxModel);
				check_gl_error();

				// Texture unit
				int texUnit = 0;
				pShaderRes->SetShaderUniform("ts_diffuse", &texUnit);
				check_gl_error();
			}
		}

		// Draw Model
		pModelRes->Draw();
	}
	bool Renderable::IsVisible()
	{
		return mIsVisible;
	}
	void Renderable::SetVisible(bool visible)
	{
		mIsVisible = visible;
	}
	TransformComp * Renderable::GetTransform()
	{
		return pTransform;
	}
	TransformComp3D * Renderable::GetTransform3D()
	{
		return pTransform3D;
	}

}