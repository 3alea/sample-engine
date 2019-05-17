// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	Math.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01//31
// Purpose			:	Implementations of the math library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------
#ifndef AEX_MATH_INL
#define AEX_MATH_INL

namespace AEX
{
	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 DegToRad(f32 x)
	{
		return x * (PI / 180.0f);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 RadToDeg(f32 x)
	{
		return x * (180.0f / PI);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Sin(f32 x)
	{
		return sinf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Cos(f32 x)
	{
		return cosf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Tan(f32 x)
	{
		return tanf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ASin(f32 x)
	{
		return asinf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ACos(f32 x)
	{
		return acosf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ATan(f32 x)
	{
		return atanf(x);
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 SinDeg(f32 x)
	{
		return Sin(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 CosDeg(f32 x)
	{
		return Cos(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 TanDeg(f32 x)
	{
		return Tan(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ASinDeg(f32 x)
	{
		return ASin(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ACosDeg(f32 x)
	{
		return ACos(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 ATanDeg(f32 x)
	{
		return ATan(DegToRad(x));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API u32 IsPowOf2(u32 x)
	{
		return ((x) && (((x)& ((x)-1)) == 0));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API u32 NextPowOf2(u32 x)
	{
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		return x + 1;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API u32 LogBase2(u32 x)
	{
		u32 i;

		for (x = x >> 1, i = 0; x >> i; i++)
			;

		return i;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Wrap(f32 x, f32 x0, f32 x1)
	{
		if (x < x0)
			return x + (x1 - x0);
		if (x > x1)
			return x - (x1 - x0);
		return x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Min(f32 x, f32 y)
	{
		return (x < y) ? x : y;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Max(f32 x, f32 y)
	{
		return (x < y) ? y : x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API s32 Min(s32 x, s32 y)
	{
		return (x < y) ? x : y;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API s32 Max(s32 x, s32 y)
	{
		return (x < y) ? y : x;
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API f32 Clamp(f32 x, f32 x0, f32 x1)
	{
		return Max(x0, Min(x, x1));
	}

	// ---------------------------------------------------------------------------

	inline AEX_MATH_API s32 Clamp(s32 x, s32 x0, s32 x1)
	{
		return Max(x0, Min(x, x1));
	}

	// ---------------------------------------------------------------------------

	inline bool AEX_MATH_API InRange(f32 x, f32 x0, f32 x1)
	{
		if ((x0 <= x) && (x <= x1))
			return true;

		return false;
	}

	// ---------------------------------------------------------------------------
	
	inline f32 AEX_MATH_API RandFloat(f32 min, f32 max)
	{
		int r = rand();
		return min + ((f32)r/(f32)RAND_MAX) * (max - min);
	}

	inline f32 AEX_MATH_API RandFloat01()
	{
		int r = rand();
		return ((f32)r / (f32)RAND_MAX);
	}

	template <typename T>
	T  Lerp(T start, T end, f32 tn)
	{
		return start + (end - start)*tn;
	}
}

// ---------------------------------------------------------------------------
#endif