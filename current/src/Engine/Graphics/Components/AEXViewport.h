#pragma once

#include <aexmath\AEXMath.h>
#include "..\AEXColor.h"
namespace AEX
{
	class Viewport
	{
		friend class Camera;
	private:
		AEVec2	mPosition;		// position in the framebuffer (this will eventually be reduced to an int)
		AEVec2	mDimensions;	// Width and height of the viewport in the frame buffer
		Color	mColor;			// Color used when clearing the frame buffer
	public:

		// ------------------------------------------------------------------------
		/// \fn		Viewport (default constructor)
		/// \brief	Constructs a viewport at (0,0), size of the screen and black color
		Viewport();

		// ------------------------------------------------------------------------
		/// \fn		Viewport (custom constructor)
		/// \brief	Constructs a viewport at the specified position and dimensions.
		/// \param	position	position of the viewport (bottom left) in the framebuffer 
		/// \param	dimensions	dimensions of the viewport in the frame buffer.
		Viewport(AEVec2 position, AEVec2 dimensions);

		// ------------------------------------------------------------------------
		/// \fn		Viewport (custom constructor)
		/// \brief	Constructs a viewport at the specified position and dimensions.
		/// \param	position	position of the viewport (bottom left) in the framebuffer 
		/// \param	dimensions	dimensions of the viewport in the frame buffer.
		Viewport(f32 t, f32 l, f32 b, f32 r);

		// ------------------------------------------------------------------------
		/// \fn		ComputeMatrix
		/// \brief	Computes the camera to viewport matrix using the method seen in
		///			class. After this transform, the points should be in frame-buffer
		///			coordinates and ready to be sent to the Rasterizer. 
		AEMtx33 ComputeMatrix();

		// ------------------------------------------------------------------------
		/// \fn		ComputeInvMatrix
		/// \brief	Computes the viewport to camera  matrix. After this transform, 
		///			the points should be in NDC coordinates. You cannot use 
		///			AEMtx33Inverse to compute the matrix.
		AEMtx33 ComputeInvMatrix();

		// ------------------------------------------------------------------------
		#pragma region // GETTERS & SETTERS (PROVIDED)
		
		void SetPosition(const AEVec2 & newPos)	{ mPosition = newPos;	}
		AEVec2 GetPosition()					{ return mPosition;		}
		void SetRect(const AEVec2 & rect)		{ mDimensions = rect;	}
		AEVec2 GetRect()						{ return mDimensions;	}
		void SetColor(const Color & c)			{ mColor = c;			}
		Color GetColor()						{ return mColor;		}

		#pragma endregion

	};
}
