// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEMtx33.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	header file for the 3x3 matrix library
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AEX_MTX44_H
#define AEX_MTX44_H

#include "AEXMathDLL.h"

#pragma warning (disable:4201) // nameless struct warning

namespace AEX
{
	// ---------------------------------------------------------------------------
	// a simple 4x4 matrix class

	struct AEX_MATH_API AEMtx44
	{
		union
		{
			// matrix data is stored row major
			f32	v[16];
			f32	m[4][4];
			struct
			{
				f32		m00, m01, m02, m03,
					m10, m11, m12, m13,
					m20, m21, m22, m23,
					m30, m31, m32, m33;
			};
		};

		// constructor
		AEMtx44();
		AEMtx44(const f32* mm);
		AEMtx44(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33);

		AEMtx44(const AEMtx44& rhs);
		AEMtx44(const AEMtx33&rhs);
		AEMtx44 & operator=(const AEMtx33&rhs);

		// ---------------------------------------------------------------------------
		// accessors

		f32&			RowCol(u32 row, u32 col)			{ return v[row * 4 + col]; }
		f32				RowCol(u32 row, u32 col) const	{ return v[row * 4 + col]; }
		f32&			operator()		(u32 row, u32 col)			{ return RowCol(row, col); }
		f32				operator()		(u32 row, u32 col) const	{ return RowCol(row, col); }


		// ---------------------------------------------------------------------------
		// conversion to mtx33
		operator AEMtx33() const;


		// ---------------------------------------------------------------------------
		// operator overload

		// accessors
		AEMtx44			operator+		(const AEMtx44& rhs) const;

		AEMtx44			operator-		(const AEMtx44& rhs) const;

		AEMtx44			operator*		(f32 s) const;
		AEMtx44			operator*		(const AEMtx44& rhs) const;

		AEMtx44			operator/		(f32 s) const;

		// multiply with a 3D vector
		// * '*' assume w = 1.0
		// * '/' assume w = 0.0
		AEVec3			operator*		(const AEVec3& v) const;
		AEVec3			operator/		(const AEVec3& v) const;

		// ---------------------------------------------------------------------------
		// member functions
		void			Print() const;

		// scalar multiplication
		AEMtx44			Mult(f32 s) const;
		AEMtx44&		MultThis(f32 s);

		// matrix concatenation
		AEMtx44			Mult(const AEMtx44& rhs) const;
		AEMtx44&		MultThis(const AEMtx44& rhs);

		// vector multiplication
		AEVec3			MultVec(const AEVec3& v) const;
		AEVec3			MultVecSR(const AEVec3& v) const;

		// Transpose
		AEMtx44			Transpose() const;
		AEMtx44&		TransposeThis();

		// Invert
		AEMtx44			Inverse() const;
		AEMtx44&		InverseThis();

		// Extract the angles from the matrix
		void GetEulerAnglesXYZ(float & x, float & y, float & z) const;
		void GetEulerAnglesZYX(float & x, float & y, float & z) const;

		// the following functions will be applied to 'this' matrix
		AEMtx44&	ZeroThis();
		AEMtx44&	IdentityThis();
		AEMtx44&	ScaleThis(f32 x, f32 y, f32 z);
		AEMtx44&	RotateThis(f32 x, f32 y, f32 z);
		AEMtx44&	RotateThis(f32 x, f32 y, f32 z, f32 angle);
		AEMtx44&	TranslateThis(f32 x, f32 y, f32 z);

		// the following functions construct a matrix
		static AEMtx44	Zero();
		static AEMtx44	Identity();
		static AEMtx44	Scale(f32 x, f32 y, f32 z);
		static AEMtx44	Rotate(f32 x, f32 y, f32 z); // sames as default: Z-Y-X Euler
		static AEMtx44	RotateZYX(f32 x, f32 y, f32 z); // Z-Y-X Euler
		static AEMtx44	RotateXYZ(f32 x, f32 y, f32 z); // X-Y-Z Euler
		static AEMtx44	Rotate(f32 x, f32 y, f32 z, f32 angle);
		static AEMtx44	Translate(f32 x, f32 y, f32 z);

		// calculate a look at matrix
		// * camera is looking along -z axis and y is up
		static AEMtx44	LookAt(f32 posX, f32 posY, f32 posZ, f32 targetX, f32 targetY, f32 targetZ, f32 upX, f32 upY, f32 upZ);
		static AEMtx44	LookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up);
		static AEMtx44	InvLookAt(const AEVec3& pos, const AEVec3& target, const AEVec3& up);
		static AEMtx44	OrthoProj(f32 width, f32 height, f32 near, f32 far);
		static AEMtx44	OrthoProjGL(f32 width, f32 height, f32 near, f32 far);
		static AEMtx44	PerspProj(f32 fovY, f32 ratio, f32 near, f32 far);
		static AEMtx44	PerspProj(f32 fovY, f32 width, f32 height, f32 near, f32 far);
	};

	typedef AEMtx44 AEMtx44;

	// ---------------------------------------------------------------------------

	template <typename T>
	AEMtx44 operator*(T s, const AEMtx44& rhs)
	{
		return (rhs * s);
	}
}
#pragma warning (default:4201)
// ---------------------------------------------------------------------------
#endif