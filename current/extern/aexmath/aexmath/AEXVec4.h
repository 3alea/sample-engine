#ifndef AEX_VEC4_H_
#define AEX_VEC4_H_
#include "AEXMathDLL.h"

#pragma warning (disable:4201) // nameless struct warning
namespace AEX
{
	struct AEX_MATH_API AEVec4
	{
		union
		{
			struct
			{

				f32 x, y, z, w;
			};
			f32 v[4];
		};
		AEVec4(f32 rr = 0.0f, f32 gg = 0.0f, f32 bb = 0.0f, f32 aa = 1.0f);
		AEVec4(u32 color); // custom constructor
		operator u32();	// conversion operator

		AEVec4 operator *(const AEVec4 & rhs);
		AEVec4 operator *=(const AEVec4 & rhs);
		AEVec4 operator *(const f32 & sc);
		AEVec4 operator *=(const f32 & sc);
		AEVec4 operator +(const AEVec4 & rhs);
		AEVec4& operator +=(const AEVec4 & rhs);
		AEVec4 operator -(const AEVec4 & rhs);
		AEVec4& operator -=(const AEVec4 & rhs);
		AEVec4 operator!();
		AEVec4 Clamped();
	};
}
#pragma warning (default:4201) // nameless struct warning


#endif