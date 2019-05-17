#ifndef AEX_MATH_DEFS_H_
#define AEX_MATH_DEFS_H_

typedef char				s8;
typedef unsigned char		u8;
typedef signed short		s16;
typedef unsigned short		u16;
typedef signed int			s32;
typedef unsigned int		u32;
typedef signed long long	s64;
typedef unsigned long long	u64;
typedef float				f32;
typedef double				f64;

#ifndef EPSILON
#define	EPSILON	0.00001f
#endif

#ifndef FLOAT_ZERO
#define FLOAT_ZERO(x) (x >-EPSILON && x < EPSILON)
#endif
#ifndef DOUBLE_EPSILON
#define	DOUBLE_EPSILON	0.00001
#endif

#ifndef DOUBLE_ZERO
#define DOUBLE_ZERO(x) (x >-DOUBLE_EPSILON && x < DOUBLE_EPSILON)
#endif

#ifndef PI
#define	PI		3.1415926535897932384626433832795f
#endif

#define	HALF_PI	(PI * 0.5f)
#define	TWO_PI	(PI * 2.0f)

#endif