#ifndef AEX_COLOR_H_
#define AEX_COLOR_H_


#include "..\Core\AEXDataTypes.h"

#pragma warning (disable:4201) // nameless struct warning
namespace AEX
{
	struct  Color
	{
		union
		{
			struct
			{

				f32 r, g, b, a;
			};
			struct
			{

				f32 x, y, z, w;
			};
			f32 v[4];
		};
		Color(f32 rr = 0.0f, f32 gg = 0.0f, f32 bb = 0.0f, f32 aa = 1.0f);
		Color(u32 color); // custom constructor
		operator u32();	// conversion operator

		Color operator *(const Color & rhs);
		Color operator *=(const Color & rhs);
		Color operator *(const f32 & sc);
		Color operator *=(const f32 & sc);
		Color operator +(const Color & rhs);
		Color& operator +=(const Color & rhs);
		Color operator -(const Color & rhs);
		Color& operator -=(const Color & rhs);
		Color operator!();
		Color Clamped();
	};
}
#pragma warning (default:4201) // nameless struct warning


#endif