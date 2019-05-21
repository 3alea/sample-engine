#include "AEXCamera.h"
#include "AEXViewport.h"
#include "..\AEXGraphics.h"
#include "..\..\Composition\AEXGameObject.h"
#include "..\..\Scene\AEXTransformComp.h"
#include <aexmath\AEXMath.h>
namespace AEX
{
	// ------------------------------------------------------------------------
	/// \fn		Camera (default constructor)
	/// \brief	Constructs a camera at (0,0), (1,0), (0,1), size of the screen
	Camera::Camera()
		: mTransform(NULL)
		, mProjectionType(ePT_Orthographic)
		, mNear(.01f)
		, mFar(1000.0f)
		, mViewRect(1920,1080)
		, mFOVy(DegToRad(60.0f))
	{
	}

	// ------------------------------------------------------------------------
	/// \fn		Camera (default destructor)
	Camera::~Camera()
	{
		// remove from graphics
		Shutdown();
	}

	// ------------------------------------------------------------------------
	void Camera::Initialize()
	{
		// add self to graphics - TODO(Thomas): Implement this for next lesson
		// aexGraphics->AddCamera(this);

		// get the transform component
		mTransform = GetOwner()->GetComp<TransformComp3D>();
	}

	// ------------------------------------------------------------------------
	void Camera::Shutdown()
	{
		// Remove selef from graphics - TODO(Thomas): Implement this for next lesson
		//aexGraphics->RemoveCamera(this);
	}

	// ------------------------------------------------------------------------
	void Camera::Update()
	{}

	// ------------------------------------------------------------------------
	/// \fn		ComputeViewMatrix
	/// \brief	Computes the world to camera matrix using the orthogonal method 
	///			seen in class. Additionally, the matrix needs to transform 
	///			the world points in NDC space.
	AEMtx44 Camera::ComputeViewMatrix()
	{
		AEMtx44 viewMtx = AEMtx44::Identity();

		// use the transform to compute the camera matrix
		if (mTransform)
		{
			AEVec3 rot = mTransform->GetRotationXYZ();
			AEVec3 pos = mTransform->GetPosition();
			viewMtx = AEMtx44::RotateZYX(-rot.x, -rot.y, -rot.z) * AEMtx44::Translate(-pos.x, -pos.y, -pos.z);
		}
		return viewMtx;
	}

	// ------------------------------------------------------------------------
	/// \fn		ComputeInvViewMatrix
	/// \brief	Computes the camera to world matrix. You cannot use AEMtx33Inverse
	///			to compute the matrix
	AEMtx44 Camera::ComputeInvViewMatrix()
	{
		AEMtx44 invViewMtx = AEMtx44::Identity();

		// use the transform to compute the camera matrix
		if (mTransform)
		{
			AEVec3 rot = mTransform->GetRotationXYZ();
			AEVec3 pos = mTransform->GetPosition();
			invViewMtx = AEMtx44::Translate(pos.x, pos.y, pos.z) * AEMtx44::RotateXYZ(rot.x, rot.y, rot.z);
		}
		return invViewMtx;
	}

	// ------------------------------------------------------------------------
	/// \fn		ComputeProjectionMatrix
	/// \brief	Computes the projection matrix based on the 
	AEMtx44	Camera::ComputeProjectionMatrix()
	{
		switch (mProjectionType)
		{
		case ePT_Orthographic:
			return AEMtx44::OrthoProjGL(mViewRect.x,	// Projection Volume width
				mViewRect.y,							// Projection Volume height
				mNear,									// Projection near plane distance
				mFar);
		case ePT_Perspective:
			return AEMtx44::PerspProj(mFOVy, mViewRect.x, mViewRect.y, mNear, mFar);
		}

		return AEMtx44::Identity();
	}
	// ------------------------------------------------------------------------
	/// \fn		ComputeViewProjectionMatrix
	AEMtx44 Camera::ComputeViewProjectionMatrix()
	{
		return ComputeProjectionMatrix() * ComputeViewMatrix();
	}

}// namespace AEX