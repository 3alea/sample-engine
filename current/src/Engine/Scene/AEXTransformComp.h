#ifndef AEX_TRANSFORM_COMP
#define AEX_TRANSFORM_COMP

#include <aexmath\AEXMath.h>
#include "..\Composition\AEXComponent.h"

namespace AEX
{
	class TransformComp : public IComp
	{
		AEX_RTTI_DECL(TransformComp, IComp);

	public:
		TransformComp();
		virtual ~TransformComp();

		f32 GetRotationAngle();
		AEVec2 GetDirection();
		AEVec2 GetPosition();
		AEVec3 GetPosition3D();
		AEVec2 GetScale();
		AEMtx33 GetModelToWorld();
		AEMtx33 GetWorldToModel();
		AEMtx44 GetModelToWorld4x4();
		AEMtx44 GetWorldToModel4x4();

		void SetDirection(AEVec2 dir);
		void SetRotationAngle(f32 angle);
		void SetPosition(const AEVec2 & pos);
		void SetPosition3D(const AEVec3 & posZorder);
		void SetScale(const AEVec2 & scale);
		
		// Data
	public:
		Transform mLocal;
	};


	class TransformComp3D : public IComp
	{
		AEX_RTTI_DECL(TransformComp3D, IComp);

	public:
		TransformComp3D();
		virtual ~TransformComp3D();

		AEVec3	GetRotationXYZ();
		AEMtx33 GetRotationMtx33();
		AEMtx44 GetRotationMtx44();
		AEVec3	GetPosition();
		AEVec3	GetScale();
		AEMtx44 GetModelToWorld();
		AEMtx44 GetWorldToModel();

		void SetRotationXYZRad(f32 xRad, f32 yRad, f32 zRad);
		void SetRotationXYZDeg(f32 xDeg, f32 yDeg, f32 zDeg);
		void SetPosition(const AEVec3 & pos);
		void SetScale(const AEVec3 & scale);
		void SetScale(f32 sx, f32 sy, f32 sz);
		void SetScale(f32 sc);
		
		// Data
	public:
		Transform3D mLocal;
	};
}

#endif