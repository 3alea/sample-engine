#ifndef AEX_COLLISIONS_H_
#define AEX_COLLISIONS_H_

#include "AEXMath.h"

namespace AEX
{
	struct AEX_MATH_API AABB
	{
		AEX::AEVec2 p;
		AEX::AEVec2 size;
		bool Contains(const AEVec2 &pt);
	};
	struct AEX_MATH_API Line
	{
		AEVec2 start, end;
		inline AEVec2 GetNormal() { return (end - start).Perp().Normalize(); }
	};

	// ------------------------------------------------------------------------
	// COLLISION FUNCTIONS
	// ------------------------------------------------------------------------

	// collision functions
	bool AEX_MATH_API PointInRect(const AEVec2 &pt, const AEVec2 rect_pos, AEVec2 rect_size);

	// ------------------------------------------------------------------------
	// TestAABBToAABB
	bool AEX_MATH_API TestAABBToAABB(const AABB&a, const AABB&b);

	// ------------------------------------------------------------------------
	// TestPointToLine
	f32 AEX_MATH_API TestPointToLine(AEVec2 point, AEVec2 start_line, AEVec2 end_line);

	// ------------------------------------------------------------------------
	// TestAABBToLine
	bool AEX_MATH_API TestAABBToLine(AABB box, Line line);

} // namespace AEX

// ----------------------------------------------------------------------------
#endif