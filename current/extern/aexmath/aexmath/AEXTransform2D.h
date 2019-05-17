//-----------------------------------------------------------------------------
#ifndef AEX_TRANSFORM_H_
#define AEX_TRANSFORM_H_
//-----------------------------------------------------------------------------
#include "AEXMathDLL.h"

namespace AEX
{
	struct AEX_MATH_API Transform
	{
		Transform();
		Transform(const AEVec2 & pos, const AEVec2 & scale, f32 rot);

		AEVec2	mScale;				// X-Y Scale
		AEVec2	mTranslation;		// Position
		AEVec3	mTranslationZ;		// Position + z Order. Used in Rendering(TODO(Thomas): Just make translation a 3D vector
		f32	mOrientation;			// Angle of Rotation

		// wrapper functions around globals;
		AEMtx33 GetMatrix();
		AEMtx33 GetInvMatrix();
		Transform Concat(const Transform &rhs);
		Transform operator *(const Transform &rhs);
		AEVec2 MultVec(const AEVec2 &rhs);
		AEVec2 operator *(const AEVec2 & rhs);
	};

	/*-----------------------------------------------------------------------------
	 * TransformMultVec
	 * Computes the transformation matrix of the specified transform and multiplies pVec.
	 * and stores the result in pResult.
	 */
	void AEX_MATH_API TransformMultVec(AEVec2 * pResult, Transform * transform, AEVec2 * pVec);

	/*-----------------------------------------------------------------------------
	 * TransformGetMatrix
	 * Computes and stores the transfomation matrix in the passed matrix
	 */
	void AEX_MATH_API TransformGetMatrix(AEMtx33 * pResult, Transform * transform);

	/*-----------------------------------------------------------------------------
	* TransformGetInvMatrix
	* Computes and stores the inverse transform matrix in the passed matrix
	*/
	void AEX_MATH_API TransformGetInvMatrix(AEMtx33 * pResult, Transform * transform);

	/*-----------------------------------------------------------------------------
	 * TransformConcatenate
	 * Purpose:
	 *	Concatenates two transform t1 and t0 by applying the method of concatenating a series of
	 *	scale, rotates and translates as demonstrated in class and stores the resulting transformation
	 *	in pResult. Where pResult represents a trasnformation like so: T1R1S1T0R0S0
	 * Parameters:
	 *	- t1: left hand side tranform.
	 *	- t0: right hand side transform.
	 */
	void AEX_MATH_API TransformConcatenate(Transform * pResult, Transform * t1, Transform * t0);


	/*-----------------------------------------------------------------------------
	 * !TODO
	 * TransformVertices_Vector
	 * Transforms an array of vertices using vector arithmetic
	 */
	void AEX_MATH_API TransformVertices_Vector(AEVec2 * oVertex, AEVec2 * wVertex, int size, Transform * transform);

	/*-----------------------------------------------------------------------------
	 * !TODO
	 * TransformVertices_Matrix
	 * Transforms an array of vertices using matrix arithmetic
	 */
	void AEX_MATH_API TransformVertices_Matrix(AEVec2 * oVertex, AEVec2 * wVertex, int size, Transform * transform);
}
#endif // TRANSFORM_H_