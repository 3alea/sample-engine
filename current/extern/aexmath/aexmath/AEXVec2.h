// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEVec2.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 2D vector library
// History			:
// - 2008/01/31		:	- initial implementation
// - 2012/02/15		:	- added a point2d typedef
// ---------------------------------------------------------------------------
#ifndef AE_VEC2_H
#define AE_VEC2_H

#include "AEXMathDLL.h"

// ---------------------------------------------------------------------------

#pragma warning (disable:4201) // nameless struct warning
namespace AEX
{
	struct AEX_MATH_API AEVec2
	{
		union
		{
			struct
			{
				f32 x, y;
			};
			f32 v[2];
		};

		// ------------------------------------------------------------------------
		AEVec2();
		AEVec2(const AEVec2& rhs);
		AEVec2(f32 xx, f32 yy);
		explicit	AEVec2(f32 xx);

		// add or substract
		AEVec2		operator+	(const AEVec2& rhs) const;
		AEVec2		operator+=	(const AEVec2& rhs);
		AEVec2		operator-	(const AEVec2& rhs) const;
		AEVec2		operator-=	(const AEVec2& rhs);

		// multiply or divide by a scalar
		AEVec2		operator*	(f32 s) const;
		AEVec2		operator*=	(f32 s);
		AEVec2		operator/	(f32 s) const;
		AEVec2		operator/=	(f32 s);

		// unitary negation
		AEVec2		operator-() const;

		// perpendicualr
		AEVec2		Perp() const;

		// vector length
		f32			Length() const;
		f32			LengthSq() const;

		// Distance
		f32			Distance(const AEVec2 &rhs);
		f32			DistanceSq(const AEVec2 &rhs);

		// Normalize
		AEVec2		Normalize() const;
		const AEVec2 &NormalizeThis();

		// dot product
		f32			Dot(const AEVec2& rhs) const;
		f32			operator*		(const AEVec2& rhs) const;

		// projection
		AEVec2		Project(const AEVec2& rhs);
		AEVec2		ProjectPerp(const AEVec2& rhs);

		// cross product magnitude
		f32			CrossMag(const AEVec2& rhs);

		// To and From Angle
		f32			GetAngle();
		void		FromAngle(f32 angle);

		// Random
		static AEVec2 Random(AEVec2 min = AEVec2(0, 0), AEVec2 max = AEVec2(1, 1));
		static AEVec2 Lerp(AEVec2 from, AEVec2 to, f32 tn);

	};

	// typedef for point2D
	typedef AEVec2 AEPoint2;
}


// ---------------------------------------------------------------------------

#endif // VEC2_H