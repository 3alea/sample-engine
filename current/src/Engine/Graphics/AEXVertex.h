#ifndef AEX_VERTEX_H_
#define AEX_VERTEX_H_

#include "..\Core\AEXCore.h"
#include <aexmath\AEXMath.h>

#pragma warning (disable:4251) // dll and STL

namespace AEX
{
	/// \class	Vertex
	///	\brief	Represents a vertex with position, texture coordinates and color
	struct  Vertex
	{
		Vertex()
			:mPosition(AEVec2(0)), mTexCoord(AEVec2(0)), mColor(Color(0, 0, 0)){}
		Vertex(AEVec2 pos, AEVec2 tex, Color col)
			:mPosition(pos), mTexCoord(tex), mColor(col){}
		AEVec2 mPosition;	//! x,y
		AEVec2 mTexCoord;	//! u,v
		Color  mColor;		//! r,g,b,a
	};
}

#endif