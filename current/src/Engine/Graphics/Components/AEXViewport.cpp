#include <aexmath\AEXMath.h>
#include "AEXViewport.h"
namespace AEX
{
	// PROVIDED
	// ------------------------------------------------------------------------
	/// \fn		Viewport (default constructor)
	/// \brief	Constructs a viewport at (0,0), size of the screen and black color
	Viewport::Viewport()
		: mPosition(0.0f, 0.0f)
		, mDimensions(0.0f, 0.0f)
	{}

	// PROVIDED
	// ------------------------------------------------------------------------
	/// \fn		Viewport (custom constructor)
	/// \brief	Constructs a viewport at the specified position and dimensions.
	/// \param	position	position of the viewport (bottom left) in the framebuffer 
	/// \param	dimensions	dimensions of the viewport in the frame buffer.
	Viewport::Viewport(AEVec2 position, AEVec2 dimensions)
		: mPosition(position)
		, mDimensions(dimensions)
	{}

	// PROVIDED
	// ------------------------------------------------------------------------
	/// \fn		Viewport (custom constructor)
	/// \brief	Constructs a viewport at the specified position and dimensions.
	/// \param	position	position of the viewport (bottom left) in the framebuffer 
	/// \param	dimensions	dimensions of the viewport in the frame buffer.
	Viewport::Viewport(f32 t, f32 l, f32 b, f32 r)
	{
		mDimensions.x = r - l;
		mDimensions.y = t - b;
		mPosition.x = l;
		mPosition.y = b;
	}

	// TODO
	// ------------------------------------------------------------------------
	/// \fn		ComputeMatrix
	/// \brief	Computes the camera to viewport matrix using the method seen in
	///			class. After this transform, the points should be in frame-buffer
	///			coordinates and ready to be sent to the Rasterizer. 
	AEMtx33 Viewport::ComputeMatrix()
	{
		// Step 1: Compute origin translate
		AEMtx33 originTr;
		AEMtx33Trans(&originTr, 0.5f, 0.5f);

		// Step 3: Compute Scale matrix
		AEMtx33 scMtx;
		AEMtx33Scale(&scMtx, mDimensions.x, mDimensions.y);
				
		// Step 4: Compute Translate
		AEMtx33 trMtx;
		AEMtx33Trans(&trMtx, mPosition.x, mPosition.y);
		
		// Final Step: Concat
		AEMtx33 vpMtx;
		AEMtx33Concat(&vpMtx, &scMtx, &originTr);
		AEMtx33Concat(&vpMtx, &trMtx, &vpMtx);

		// return 
		return vpMtx;
	}

	// TODO
	// ------------------------------------------------------------------------
	/// \fn		ComputeInvMatrix
	/// \brief	Computes the viewport to camera  matrix. After this transform, 
	///			the points should be in NDC coordinates. You cannot use 
	///			AEMtx33Inverse to compute the matrix.
	AEMtx33 Viewport::ComputeInvMatrix()
	{
		// Step 1: Compute inverse origin translate
		AEMtx33 originTr;
		AEMtx33Trans(&originTr, -0.5f, -0.5f);

		// Step 3: Compute inverse Scale matrix
		AEMtx33 scMtx;
		if (!FLOAT_ZERO(mDimensions.x) && !FLOAT_ZERO(mDimensions.y))
			AEMtx33Scale(&scMtx, 1.0f / mDimensions.x, 1.0f / mDimensions.y);

		// Step 4: Compute inverse Translate
		AEMtx33 trMtx;
		AEMtx33Trans(&trMtx, -mPosition.x, -mPosition.y);

		// Final Step: Concat in reverse order
		AEMtx33 vpMtx;
		AEMtx33Concat(&vpMtx, &scMtx, &trMtx);
		AEMtx33Concat(&vpMtx, &originTr, &vpMtx);

		// return 
		return vpMtx;
	}

}// namespace Rasterizer