#include "AEXModel.h"

///--------------------------------------------------------------------
// OPENGL 
#include "..\Debug\CheckGLError.h"
#include "Extern\GL\glew.h"
#include "Extern\GL\wglew.h"
#include "Extern\GL\GL.h"

namespace AEX
{
	Model::Model()
		: mPrimitiveType(eTriangleList)
		, mDrawMode(eDM_Fill)
	{
		CreateOpenGLModel();
	}
	Model::Model(EPrimitiveType primType)
		: mPrimitiveType(primType)
		, mDrawMode(eDM_Fill)
	{
		CreateOpenGLModel();
	}
	Model::~Model()
	{
		Clear();
		DestroyOpenGLModel();
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn GetVertex (const)
	/// \brief Returns a pointer to the vertex  at the specified offset
	/// \param offset Zero based offset in the vertex array.
	const Vertex * Model::GetVertex(u32 offset) const
	{
		// Sanity Check
		if (offset >= GetVertexCount())
			return NULL;

		// return vertex
		return (mVertices.data() + offset);
	}


	// TODO
	///--------------------------------------------------------------------
	/// \fn GetVertex 
	/// \brief Returns a pointer to the vertex  at the specified offset
	/// \param offset Zero based offset in the vertex array.
	Vertex * Model::GetVertex(u32 offset)
	{
		// Sanity Check
		if (offset >= GetVertexCount())
			return NULL;

		// return vertex
		return (mVertices.data() + offset);
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexPos
	void Model::SetVertexPos(u32 offset, AEVec2 pos)
	{
		if (Vertex * vtx = GetVertex(offset))
			vtx->mPosition = pos;
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexPos
	void Model::SetVertexPos(u32 offset, f32 x, f32 y)
	{
		SetVertexPos(offset, AEVec2(x, y));
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexTex
	void Model::SetVertexTex(u32 offset, AEVec2 texCoord)
	{
		if (Vertex * vtx = GetVertex(offset))
			vtx->mTexCoord = texCoord;
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexTex
	void Model::SetVertexTex(u32 offset, f32 u, f32 v)
	{
		SetVertexTex(offset, AEVec2(u, v));
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexColor
	void Model::SetVertexColor(u32 offset, Color col)
	{
		if (Vertex * vtx = GetVertex(offset))
			vtx->mColor = col;
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn SetVertexColor
	void Model::SetVertexColor(u32 offset, f32 r, f32 g, f32 b, f32 a)
	{
		SetVertexColor(offset, Color(r, g, b, a));
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn GetVertexCount (const)
	/// \brief Returns the number of vertices in the model
	u32		Model::GetVertexCount() const
	{
		return mVertices.size();
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn		AddVertex
	/// \brief	Inserts the given vertex at the end of the vertex list. 
	/// \param	newVert the vertex to add
	void Model::AddVertex(const Vertex & newVert)
	{
		mVertices.push_back(newVert);
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn		Clear
	/// \brief	Clears all vertices
	void Model::Clear()
	{
		mVertices.clear();
	}

	// Create OpenGL 
	void Model::CreateOpenGLModel()
	{
		GLuint vao;
		GLuint vertexBuffer;
		GLuint indexBuffer;

		// Create vertex array object and bind
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Create vertex buffer
		glGenBuffers(1, &vertexBuffer);

		// Create index buffer
		glGenBuffers(1, &indexBuffer);


		mGLVAO = vao;
		mGLVertexBuffer = vertexBuffer;
		mGLIndexBuffer = indexBuffer;
	}
	void Model::DestroyOpenGLModel()
	{
		glDeleteBuffers(1, &mGLIndexBuffer);
		glDeleteBuffers(1, &mGLVertexBuffer);
		glDeleteVertexArrays(1, &mGLVAO);
	}

	///--------------------------------------------------------------------
	/// \fn		UploadVertexFormatToGPU
	void Model::UploadVertexFormatToGPU()
	{
		u32		vertexSize = sizeof(Vertex);
		// Specify how the data for the vertices is layed out
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, 0); // position
		check_gl_error();
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(AEVec2))); // texture coord
		check_gl_error();
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(sizeof(AEVec2) * 2)); // color
		check_gl_error();
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		check_gl_error();
	}

	///--------------------------------------------------------------------
	/// \fn		UploadToGPU
	/// \brief	
	void Model::UploadToGPU()
	{
		// Sanity Checks
		if (mGLVAO == 0)
			return;

		GLuint	vertexBuffer = (GLuint)(mGLVertexBuffer);
		GLuint	indexBuffer = (GLuint)(mGLIndexBuffer);

		// Bind
		Bind();

		// upload vertex data as is
		u32		vertexSize = sizeof(Vertex);
		u32		vertexBufferSize = vertexSize * mVertices.size();
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, reinterpret_cast<void*>(mVertices.data()), GL_STATIC_DRAW);
		check_gl_error();

		// Specify how the data for the vertices is layed out
		UploadVertexFormatToGPU();

		// compute index dataand upload to GL device
		u32	indexBufferSize = mVertices.size();
		u16 *indexBufferData = new u16[indexBufferSize];
		for (u32 i = 0; i < indexBufferSize; ++i)
			indexBufferData[i] = i;

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indexBufferSize, reinterpret_cast<void*>(indexBufferData), GL_STATIC_DRAW);
		check_gl_error();

		// delete the index buffer data now that we've sent it to the GPU
		delete[] indexBufferData;

		// Unbind
		Unbind();
	}

	void Model::ReloadToGPU(s32 start, s32 end)
	{
		// Sanity check
		if (end < start || start >= (int)GetVertexCount())
			return;

		// upload all
		if (start == -1 && end == -1)
			end = (int)GetVertexCount() - 1; // set end to last vertex

		// clamp to end
		if (end >= (int)GetVertexCount())
			end = (int)GetVertexCount() - 1;

		// count of vertices to upload
		u32 toUploadCount = end - start + 1;

		// get the vertex buffer handles
		GLuint	vertexBuffer = (GLuint)(mGLVertexBuffer);

		// Bind
		Bind();

		// upload vertex data as is
		u32		vertexSize = sizeof(Vertex);
		u32		vertexBufferSize = vertexSize * toUploadCount;
		u32		startOffset = start * vertexSize;
		glBufferSubData(GL_ARRAY_BUFFER, startOffset, vertexBufferSize, reinterpret_cast<void*>(mVertices.data() + startOffset));
		check_gl_error();
	}

	///--------------------------------------------------------------------
	/// \fn		Bind
	/// \brief	
	void Model::Bind()
	{
		if (mGLVAO){
			glBindVertexArray(mGLVAO);
			glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(mGLVertexBuffer));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)(mGLIndexBuffer));
		}
	}

	///--------------------------------------------------------------------
	/// \fn		Unbind
	/// \brief	
	void Model::Unbind()
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	///--------------------------------------------------------------------
	/// \fn		GetGLHandleVAO()
	/// \brief	Returns the VAO handle
	u32 Model::GetGLHandleVAO()
	{
		return mGLVAO;
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn		Draw
	/// \brief	Draws the model taken into account mPrimitiveType
	void Model::Draw(s32 startOffset, s32 endOffset)
	{

		// Sanity check
		if (endOffset < startOffset || startOffset >= (int)GetVertexCount())
			return;

		// draw all
		if (startOffset == -1 && endOffset == -1)
		{
			startOffset = 0;
			endOffset = (int)GetVertexCount() - 1; // set end to last vertex
		}

		// clamp to end
		if (endOffset >= (int)GetVertexCount())
			endOffset = (int)GetVertexCount() - 1;

		// count of vertices to draw
		u32 toDrawCount = endOffset - startOffset + 1;

		// Bind the vertex arrays
		Bind();
		check_gl_error();

		// Set the fill mode
		// TODO(Thomas): Prevent redundant calls. 
		switch (mDrawMode)
		{
		case eDM_Wireframe:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case eDM_Fill:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}

		// Draw

		switch (mPrimitiveType)
		{
		case eTriangleList:
			glDrawElementsBaseVertex(GL_TRIANGLES, toDrawCount, GL_UNSIGNED_SHORT, 0, startOffset);
			break;
		case eLineList:
			glDrawElementsBaseVertex(GL_LINES, toDrawCount, GL_UNSIGNED_SHORT, 0, startOffset);
			break;
		}
		check_gl_error();
	}

	///--------------------------------------------------------------------
	/// \fn		DrawInstanced
	void Model::DrawInstanced(u32 instanceCount, s32 startOffset, s32 endOffset)
	{
		// Sanity check
		if (0 == instanceCount && endOffset < startOffset || startOffset >= (int)GetVertexCount())
			return;

		// draw all
		if (startOffset == -1 && endOffset == -1)
		{
			startOffset = 0;
			endOffset = (int)GetVertexCount() - 1; // set end to last vertex
		}

		// clamp to end
		if (endOffset >= (int)GetVertexCount())
			endOffset = (int)GetVertexCount() - 1;

		// count of vertices to draw
		u32 toDrawCount = endOffset - startOffset + 1;

		// Bind the vertex arrays
		Bind();
		check_gl_error();

		// Set the fill mode
		// TODO(Thomas): Move this to graphics system 
		switch (mDrawMode)
		{
		case eDM_Wireframe:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case eDM_Fill:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}

		// Draw

		switch (mPrimitiveType)
		{
		case eTriangleList:
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, toDrawCount, GL_UNSIGNED_SHORT, 0, instanceCount, startOffset);
			break;
		case eLineList:
			glDrawElementsInstancedBaseVertex(GL_LINES, toDrawCount, GL_UNSIGNED_SHORT, 0, instanceCount, startOffset);
			break;
		}
		check_gl_error();
		
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn		Transform (const)
	/// \brief	Transforms the model by the given matrix
	/// \param	mtx Matrix to transform the model by.
	Model Model::Transform(AEMtx33 & mtx)const
	{
		// Create a transformed Model into view
		Model newModel;
		newModel.mPrimitiveType = mPrimitiveType;
		newModel.mDrawMode = mDrawMode;
		for (u32 i = 0; i < GetVertexCount(); ++i)
		{
			// get the current vertex
			const Vertex * pVertex = GetVertex(i);

			// transform the position of the vertex
			Vertex vPrime = *pVertex;
			AEMtx33MultVec(&vPrime.mPosition, &mtx, &vPrime.mPosition);

			// Add to the transformed model
			newModel.AddVertex(vPrime);
		}
		return newModel;
	}

	// TODO
	///--------------------------------------------------------------------
	/// \fn		Transform
	/// \brief	Transforms the model by the given matrix
	/// \param	mtx Matrix to transform the model by.
	Model Model::Transform(AEMtx33 & mtx)
	{
		// Create a transformed Model into view
		Model newModel;
		newModel.mPrimitiveType = mPrimitiveType;
		newModel.mDrawMode = mDrawMode;
		for (u32 i = 0; i < GetVertexCount(); ++i)
		{
			// get the current vertex
			const Vertex * pVertex = GetVertex(i);

			// transform the position of the vertex
			Vertex vPrime = *pVertex;
			AEMtx33MultVec(&vPrime.mPosition, &mtx, &vPrime.mPosition);

			// Add to the transformed model
			newModel.AddVertex(vPrime);
		}
		return newModel;
	}

}// namespace Rasterizer