// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMtx33.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 3x3 matrix library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AEX_VEC3_H
#define AEX_VEC3_H
#include "AEXMathDLL.h"

#pragma warning (disable:4201) // nameless struct warning

namespace AEX
{
	struct AEX_MATH_API AEVec3
	{
		union
		{
			struct
			{
				f32	x, y, z;
			};
			f32		v[3];
		};

		AEVec3();
		AEVec3(const AEVec3& rhs);
		AEVec3(const AEVec2 &rhs2D, f32 zz = 0.f);
		AEVec3(f32 xx, f32 yy, f32 zz);
		explicit	AEVec3(f32 xx);
		;
		// add or substract										;
		AEVec3		operator+		(const AEVec3& rhs) const;
		AEVec3&		operator+=		(const AEVec3& rhs);
		AEVec3		operator-		(const AEVec3& rhs) const;
		AEVec3&		operator-=		(const AEVec3& rhs);
		;
		// multiply or divide by a scalar						;
		AEVec3		operator*		(f32 s) const;
		AEVec3&		operator*=		(f32 s);
		AEVec3		operator/		(f32 s) const;
		AEVec3&		operator/=		(f32 s);
		;
		// get vector length									;
		f32			Length() const;
		;
		// calculate normalized vector							;
		AEVec3		Normalize() const;
		const AEVec3&	NormalizeThis();
		;
		// dot product											;
		f32			operator*		(const AEVec3& rhs) const;
		;
		// cross product										;
		AEVec3		Cross(const AEVec3& rhs) const;
	};

	// ---------------------------------------------------------------------------

	template <typename T>
	AEVec3 operator*(T s, const AEVec3& rhs)
	{
		return (rhs * s);
	}

	struct AEInteger4
	{
		union
		{
			struct
			{
				int x, y, z, w;
			};
			int		v[4];
		};
	};
}
#pragma warning (default:4201) // nameless struct warning

// ---------------------------------------------------------------------------
#endif