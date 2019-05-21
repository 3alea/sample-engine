#ifndef CS200_CAMERA2D_H_
#define CS200_CAMERA2D_H_

#include "..\..\Core\AEXDataTypes.h"
#include "..\..\Composition\AEXComponent.h"
#include <aexmath\AEXMath.h>
#include "AEXViewport.h"


namespace AEX
{
	class TransformComp3D;

	class  Camera: public IComp
	{
		AEX_RTTI_DECL(Camera, IComp);

	public:
		enum EProjectionType {ePT_Orthographic, ePT_Perspective};

	private:
		TransformComp3D * mTransform;

		// Projection type
		EProjectionType mProjectionType;

		// 3D projection data
		f32		mFOVy;			// vertical field of view
		f32		mNear;			// near plane
		f32		mFar;			// far plane

		// 2D view rectangle - used for orthographic and for determining the
		// aspect ratio in perspective projection.
		AEVec2 mViewRect;	// View window rectangle (x = width, y = height)
	

		// viewport data
		Viewport mViewport; // Viewport data
		bool	mbClearBG;	// Whether the camera should clear the viewport before

	public:

		// ------------------------------------------------------------------------
		/// \fn		Camera (default constructor)
		Camera();

		// ------------------------------------------------------------------------
		/// \fn		Camera (default destructor)
		virtual ~Camera();

		// ------------------------------------------------------------------------
		/// \fn		Initialize()
		virtual void Initialize();

		// ------------------------------------------------------------------------
		/// \fn		Udpate
		virtual void Update();

		// ------------------------------------------------------------------------
		/// \fn		Shutdown		
		virtual void Shutdown();

		// ------------------------------------------------------------------------
		/// \fn		ComputeViewMatrix 
		AEMtx44 ComputeViewMatrix();

		// ------------------------------------------------------------------------
		/// \fn		ComputeInvViewMatrix
		AEMtx44 ComputeInvViewMatrix();

		// ------------------------------------------------------------------------
		/// \fn		ComputeProjectionMatrix
		AEMtx44	ComputeProjectionMatrix();

		// ------------------------------------------------------------------------
		/// \fn		ComputeViewProjectionMatrix
		AEMtx44 ComputeViewProjectionMatrix();
		
		// ------------------------------------------------------------------------
		#pragma region // GETTERS & SETTERS

		// Get/Set View Window Rect
		void SetViewRect(const AEVec2 & dimensions)					{		mViewRect = dimensions;		}
		AEVec2 GetViewRect()										{		return mViewRect;			}

		// Get/Set width only
		void SetViewRectWidth(float width)							{		mViewRect.x = width;		}
		f32	 GetViewRectWidth()										{		return mViewRect.x;			}

		// Get/Set height only
		void SetViewRectHeight(float height)						{		mViewRect.y = height;		}
		f32  GetViewRectHeight()									{		return mViewRect.y;			}
		
		// Get/Set View Window Rect
		void SetViewport(const Viewport & viewport)					{ mViewport = viewport; }
		void SetViewport(f32 t, f32 l, f32 b, f32 r)				{ mViewport.mPosition = AEVec2(l, b); mViewport.mDimensions = AEVec2(r-l, t-b);}
		Viewport& GetViewport()										{ return mViewport; }

		// Whether we should clear or not
		bool ClearBG()												{ return mbClearBG; }
		void SetClearBG(bool clear)									{ mbClearBG = clear; }

		void SetFOV(f32 fov_y)										{ mFOVy = fov_y; }
		f32	 GetFOV()												{ return mFOVy; }

		void SetProjectiontype(EProjectionType projType)			{ mProjectionType = projType; }
		EProjectionType		GetProjectiontype()						{ return mProjectionType; }

		#pragma endregion
	};
}

#endif