#ifndef CS200_MODEL_H_
#define CS200_MODEL_H_

#include "..\Core\AEXCore.h"
#include <aexmath\AEXMath.h>
#include "AEXColor.h"
#include "AEXVertex.h"
#pragma warning (disable:4251) // dll and STL
namespace AEX
{
	/// \class	Model
	/// \brief	Model class use to represent a list of primitives. Lists that are accepted are
	///			pointlist, linelist, triangle list and circle and ellipse list. 
	class  Model : public IBase
	{
		AEX_RTTI_DECL(Model, IBase);
		AEX_CONTAINERS(Model);
	public:
		enum EPrimitiveType
		{
			ePointList,
			eLineList,
			eTriangleList,
		};

		enum EDrawMode
		{
			eDM_Fill = 0,
			eDM_Wireframe,
			eDM_Count
		};

		// PROVIDED: Dflt and only Constructor
		// The vertices should be added using the functions below
		Model();
		Model(EPrimitiveType primType);
		virtual ~Model();

		#pragma region// Getters/Setters
		EPrimitiveType GetPrimitiveType() const			{ return mPrimitiveType; }
		EPrimitiveType GetPrimitiveType()				{ return mPrimitiveType; }
		void SetPrimitiveType(EPrimitiveType primType)	{ mPrimitiveType = primType; }
		EDrawMode GetDrawMode()	const					{ return mDrawMode; }
		EDrawMode GetDrawMode()							{ return mDrawMode; }
		void SetDrawMode(EDrawMode drawMode)			{ mDrawMode = drawMode; }
		#pragma endregion

		///--------------------------------------------------------------------
		/// \fn GetVertex (const)
		const Vertex * GetVertex(u32 offset = 0) const;

		///--------------------------------------------------------------------
		/// \fn GetVertex 
		Vertex * GetVertex(u32 offset = 0);	

		///--------------------------------------------------------------------
		/// \fn SetVertexPos
		void SetVertexPos(u32 offset, AEVec2 pos);

		///--------------------------------------------------------------------
		/// \fn SetVertexPos
		void SetVertexPos(u32 offset, f32 x, f32 y);

		///--------------------------------------------------------------------
		/// \fn SetVertexTex
		void SetVertexTex(u32 offset, AEVec2 texCoord);

		///--------------------------------------------------------------------
		/// \fn SetVertexTex
		void SetVertexTex(u32 offset, f32 u, f32 v);

		///--------------------------------------------------------------------
		/// \fn SetVertexColor
		void SetVertexColor(u32 offset, Color col);

		///--------------------------------------------------------------------
		/// \fn SetVertexColor
		void SetVertexColor(u32 offset, f32 r, f32 g, f32 b, f32 a);


		///--------------------------------------------------------------------
		/// \fn GetVertexCount (const)
		u32		GetVertexCount() const;

		///--------------------------------------------------------------------
		/// \fn		AddVertex
		void AddVertex(const Vertex & newVert);

		///--------------------------------------------------------------------
		/// \fn		Clear
		void Clear();

		///--------------------------------------------------------------------
		/// \fn		Draw
		void Draw(s32 startOffset = -1, s32 endOffset = -1);

		///--------------------------------------------------------------------
		/// \fn		DrawInstanced
		void DrawInstanced(u32 instanceCount, s32 startOffset = -1, s32 endOffset = -1);

		///--------------------------------------------------------------------
		/// \fn		Transform (const)
		Model Transform(AEMtx33 & mtx)const;

		///--------------------------------------------------------------------
		/// \fn		Transform
		Model Transform(AEMtx33 & mtx);	

		///--------------------------------------------------------------------
		// \brief Device Interface functions

		///--------------------------------------------------------------------
		/// \fn		UploadVertexFormatToGPU
		void UploadVertexFormatToGPU();

		///--------------------------------------------------------------------
		/// \fn		UploadToGPU
		void UploadToGPU();

		///--------------------------------------------------------------------
		/// \fn		ReloadToGPU
		void ReloadToGPU(s32 start = -1, s32 end = -1);

		///--------------------------------------------------------------------
		/// \fn		Bind
		void Bind();

		///--------------------------------------------------------------------
		/// \fn		Unbind
		void Unbind();

		///--------------------------------------------------------------------
		/// \fn		GetGLHandleVAO()
		/// \brief	Returns the VAO handle
		u32 GetGLHandleVAO();

	private:
		// Primitive type
		EPrimitiveType		mPrimitiveType;

		// Vertex Buffer - Interleaved
		// Note(Thomas): The format of the vertex is set. 
		// Unless your game requires mesh manipulation 
		// or advanced shading. This Vertex is more than enough.
		std::vector<Vertex> mVertices;

		// Draw mode
		EDrawMode				mDrawMode;

		// OpenGL data
		u32	mGLVAO;				// vertex array object
		u32 mGLVertexBuffer;	// vertex buffer object
		u32 mGLIndexBuffer;		// index buffer object

		// Create OpenGL 
		void CreateOpenGLModel();
		void DestroyOpenGLModel();
	};
}
#pragma warning (default:4251) // dll and STL

#endif