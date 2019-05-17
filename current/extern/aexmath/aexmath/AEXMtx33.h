// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMtx33.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 3x3 matrix library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_MTX33_H
#define AE_MTX33_H

#include "AEXMathDLL.h"

// ---------------------------------------------------------------------------
// Defines

#define AEMtx33RowCol(pMtx, row, col) (pMtx)->m[(row)][(col)]

namespace AEX
{

	// ---------------------------------------------------------------------------
	// Struct/Class definitions
#pragma warning (disable:4201)

	// matrix is stored in row major (rows are stored consecutively in memory)
	struct AEX_MATH_API AEMtx33
	{
		union
		{
			f32	m[3][3];
			f32 v[9];
			struct {
				f32 m11, m12, m13,
					m21, m22, m23,
					m31, m32, m33;
			};
		};


		// ------------------------------------------------------------------------
		// AEMtx33(): Default Constructor - Sets the matrix to identity.
		AEMtx33();

		// ------------------------------------------------------------------------
		// AEMtx33(): Custom Constructor - Sets the elements as the one specified.
		AEMtx33(float a11, float a12, float a13,
			float a21, float a22, float a23,
			float a31, float a32, float a33);

		// ------------------------------------------------------------------------
		// SetIdentity() - Set this matrix to identity
		void SetIdentity();

		// ------------------------------------------------------------------------
		// Transpose - return the transpose version of the matrix
		AEMtx33 Transpose()const;

		// ------------------------------------------------------------------------
		// TransposeThis - Sets this matrix as its transpose. 
		void TransposeThis();

		// ------------------------------------------------------------------------
		// Concat - Concatenate this matrix with 'rhs' such that result = this * rhs
		AEMtx33 Concat(const AEMtx33 & rhs)const;

		// ------------------------------------------------------------------------
		// operator * (AEMtx33) - Same as above but in operator form. 
		AEMtx33 operator *(const AEMtx33 & rhs)const;

		// ------------------------------------------------------------------------
		// operator *=(AEMtx33) - Concatenates this with 'rhs' as above and stores
		// the result in this matrix. Returns this matrix
		const AEMtx33& operator *=(const AEMtx33 &rhs);

		// ------------------------------------------------------------------------
		// MultVec - Matrix to 2D Vector multiplication such that result = this * rhs
		AEVec2 MultVec(const AEVec2 & vec)const;

		// ------------------------------------------------------------------------
		// operator * - Matrix to 2D Vector multiplication such that result = this * rhs
		// same as above but in operator form.
		AEVec2 operator*(const AEVec2 & vec)const;

		// ------------------------------------------------------------------------
		// MultVecDir - Multiplies the vector by the matrix such that only the 
		// upper-left 2x2 matrix is considered (we assume we are multiplying a 
		// 2D vector and not a point.
		AEVec2 MultVecDir(const AEVec2 &vec)const;

		// ------------------------------------------------------------------------
		// Static Interface
		// ------------------------------------------------------------------------

		// ------------------------------------------------------------------------
		// AEMtx33::Identity - Returns a matrix as the identity
		static AEMtx33 Identity();

		// ------------------------------------------------------------------------
		// AEMtx33::Translate - Returns a translation matrix by a vector (x,y)
		static AEMtx33 Translate(float x, float y);

		// ------------------------------------------------------------------------
		// AEMtx33::Scale - Returns a scaling matrix by a factor (sx, sy)
		static AEMtx33 Scale(float sx, float sy);

		// ------------------------------------------------------------------------
		// AEMtx33::RotDeg - Returns a CCW rotation matrix by an angle specified 
		// in degrees. 
		static AEMtx33 RotDeg(float angle_deg);

		// ------------------------------------------------------------------------
		// AEMtx33::RotRad - Returns a CCW rotation matrix by an angle specified
		// in radians
		static AEMtx33 RotRad(float angle_rad);
	};

	// ---------------------------------------------------------------------------

	void 	AEX_MATH_API	AEMtx33Identity(AEMtx33* pResult);
			
	void 	AEX_MATH_API	AEMtx33Transpose(AEMtx33* pResult, AEMtx33* pMtx);
	f32		AEX_MATH_API	AEMtx33Inverse(AEMtx33* pResult, AEMtx33* pMtx);
	void 	AEX_MATH_API	AEMtx33InvTranspose(AEMtx33* pResult, AEMtx33* pMtx);
			
	void 	AEX_MATH_API	AEMtx33Concat(AEMtx33* pResult, AEMtx33* pMtx0, AEMtx33* pMtx1);
			
	f32		AEX_MATH_API	AEMtx33Determinant(AEMtx33* pMtx);

	// ---------------------------------------------------------------------------

	void 	AEX_MATH_API	AEMtx33SetCol(AEMtx33* pResult, u32 col, AEVec2* pVec);
	void 	AEX_MATH_API	AEMtx33SetRow(AEMtx33* pResult, u32 row, AEVec2* pVec);
	void 	AEX_MATH_API	AEMtx33GetCol(AEVec2*  pResult, u32 col, AEMtx33* pMtx);
	void	AEX_MATH_API	AEMtx33GetRow(AEVec2*  pResult, u32 row, AEMtx33* pMtx);

	// ---------------------------------------------------------------------------

	void 	AEX_MATH_API	AEMtx33Trans(AEMtx33* pResult, f32 x, f32 y);
	void 	AEX_MATH_API	AEMtx33TransApply(AEMtx33* pResult, AEMtx33* pMtx, f32 x, f32 y);

	void 	AEX_MATH_API	AEMtx33Scale(AEMtx33* pResult, f32 x, f32 y);
	void 	AEX_MATH_API	AEMtx33ScaleApply(AEMtx33* pResult, AEMtx33* pMtx, f32 x, f32 y);

	void 	AEX_MATH_API	AEMtx33Rot(AEMtx33* pResult, f32 angle);
	void 	AEX_MATH_API	AEMtx33RotDeg(AEMtx33* pResult, f32 angle);
	void	AEX_MATH_API	AEMtx33RotApply(AEMtx33* pResult, AEMtx33* pMtx, f32 angle);
	void	AEX_MATH_API	AEMtx33RotDegApply(AEMtx33* pResult, AEMtx33* pMtx, f32 angle);
	// ---------------------------------------------------------------------------

	void 	AEX_MATH_API	AEMtx33MultVec(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec);
	void 	AEX_MATH_API	AEMtx33MultVecArray(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec, u32 count);
	void 	AEX_MATH_API	AEMtx33MultVecSR(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec);
	void 	AEX_MATH_API	AEMtx33MultVecArraySR(AEVec2* pResult, AEMtx33* pMtx, AEVec2* pVec, u32 count);
}
// ---------------------------------------------------------------------------

#pragma warning (default:4201)

#endif // MTX_H