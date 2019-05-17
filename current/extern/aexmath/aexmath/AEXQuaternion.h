#ifndef AEX_QUATERNION_H_
#define AEX_QUATERNION_H_
// ----------------------------------------------------------------------------
#include "AEXMathDLL.h"

#pragma warning (disable:4201) // nameless struct warning
namespace AEX
{
	struct AEX_MATH_API Quaternion
	{
		union
		{
			struct
			{

				f32 x, y, z, w;
			};
			f32 v[4];
		};

		Quaternion();
		Quaternion(f32 xx, f32 yy, f32 zz, f32 ww);
		Quaternion(AEVec3 vec, f32 ww);
		Quaternion(AEVec3);


		// operators
		Quaternion operator+ (const Quaternion & rhs) const;
		Quaternion operator- (const Quaternion & rhs) const;
		Quaternion operator*(f32 scalar)const;
		Quaternion operator/(f32 scalar) const;
		Quaternion& operator+= (const Quaternion & rhs);
		Quaternion& operator-=(const Quaternion &rhs);
		Quaternion& operator*=(f32 scalar);
		Quaternion& operator/=(f32 scalar);

		// Left to right concatenation (q = this * rhs).
		Quaternion operator *(const Quaternion&rhs) const;
		Quaternion& operator*=(const Quaternion&rhs);

		// vector rotation
		AEVec3 operator*(const AEVec3 & rhs) const;

		// common operations
		void SetIdentity();
		void NormalizeThis();
		f32 Length()const ;
		Quaternion Normalize() const;
		f32 Dot(const Quaternion &rhs) const;
		Quaternion Inverse() const;

		// Format Conversion
		AEMtx33 ToMtx33() const;
		AEMtx44 ToMtx44() const;
		AEVec3	ToEulerXYZ() const;
		Quaternion& FromMtx33(const AEMtx33& mtx33);
		Quaternion& FromMtx44(const AEMtx44& mtx44);
		Quaternion& FromAxisAngle(const AEVec3 & axis, f32 angle_rad);
		Quaternion& FromEulerXYZ(f32 x_rad, f32 y_rad, f32 z_rad);

		// Static Quaternion Creation
		static Quaternion MakeAxisAngle(const AEVec3 &axis, f32 angle_rad);
		static Quaternion MakeEulerXYZ(f32 x_rad, f32 y_rad, f32 z_rad);
		static Quaternion MakeMatrix33(const AEMtx33 & rot33);
	};
}
#pragma warning (default:4201) // nameless struct warning

// ----------------------------------------------------------------------------
#endif