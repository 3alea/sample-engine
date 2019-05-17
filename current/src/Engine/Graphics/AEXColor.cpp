#include "AEXColor.h"
#include <aexmath\AEXMath.h>
namespace AEX
{

	// ------------------------------------------------------------------------
	/// \fn		Default & custom
	/// \brief	
	Color::Color(f32 rr, f32 gg, f32 bb, f32 aa)
		: r(rr)
		, g(gg)
		, b(bb)
		, a(aa)
	{}

	// ------------------------------------------------------------------------
	/// \fn		custom constructor
	/// \brief
	Color::Color(u32 color) // custom constructor
	{
		u32 rr, gg, bb, aa;
		// decompose

		aa = color & (255 << 24);
		rr = color & (255 << 16);
		gg = color & (255 << 8);
		bb = color & 255;

		// float
		r = (f32)rr / 255.0f;
		g = (f32)gg / 255.0f;
		b = (f32)bb / 255.0f;
		a = (f32)aa / 255.0f;
	}

	// ------------------------------------------------------------------------
	/// \fn		conversion operator
	/// \brief
	Color::operator u32()	// conversion operator
	{
		u32 aa = (a < 0.0f) ? (0) : ((a > 1.0f) ? (255) : (u32)(255.0f * a));
		u32 rr = (r < 0.0f) ? (0) : ((r > 1.0f) ? (255) : (u32)(255.0f * r));
		u32 gg = (g < 0.0f) ? (0) : ((g > 1.0f) ? (255) : (u32)(255.0f * g));
		u32 bb = (b < 0.0f) ? (0) : ((b > 1.0f) ? (255) : (u32)(255.0f * b));

		return ((aa << 24) | (rr << 16) | (gg << 8) | bb);
	}

	Color Color::operator *(const Color & rhs)
	{
		return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}
	Color Color::operator *=(const Color & rhs)
	{
		return *this = Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}
	Color Color::operator *(const f32 & sc)
	{
		return Color(r * sc, g * sc, b * sc, a * sc);
	}
	Color Color::operator *=(const f32 & sc)
	{
		return *this = Color(r * sc, g * sc, b * sc, a * sc);
	}
	Color Color::operator +(const Color & rhs)
	{
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}
	Color& Color::operator += (const Color & rhs)
	{
		return *this = Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}
	Color Color::operator - (const Color & rhs)
	{
		return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}
	Color& Color::operator -= (const Color & rhs)
	{
		return *this = Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}
	Color Color::operator!()
	{
		return Color(1.0f - r, 1.0f - g, 1.0f - b, 1.0f - a);
	}
	Color Color::Clamped()
	{
		return Color(Clamp(r, 0.5f, 1.0f),
			Clamp(g, 0.5f, 1.0f),
			Clamp(b, 0.5f, 1.0f),
			Clamp(a, 0.5f, 1.0f));
	}

}// namespace AEX