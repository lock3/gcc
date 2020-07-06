/* { dg-final { check-function-bodies "**" "" "-DCHECK_ASM" { target { ! ilp32 } } } } */

#include "test_sve_acle.h"

/*
** ldnf1_s64_base:
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_s64_base, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0),
	   z0 = svldnf1 (p0, x0))

/*
** ldnf1_s64_index:
**	add	(x[0-9]+), x0, x1, lsl 3
**	ldnf1d	z0\.d, p0/z, \[\1\]
**	ret
*/
TEST_LOAD (ldnf1_s64_index, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 + x1),
	   z0 = svldnf1 (p0, x0 + x1))

/*
** ldnf1_s64_1:
**	ldnf1d	z0\.d, p0/z, \[x0, #1, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_s64_1, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 + svcntd ()),
	   z0 = svldnf1 (p0, x0 + svcntd ()))

/*
** ldnf1_s64_7:
**	ldnf1d	z0\.d, p0/z, \[x0, #7, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_s64_7, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 + svcntd () * 7),
	   z0 = svldnf1 (p0, x0 + svcntd () * 7))

/*
** ldnf1_s64_8:
**	incb	x0, all, mul #8
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_s64_8, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 + svcntd () * 8),
	   z0 = svldnf1 (p0, x0 + svcntd () * 8))

/*
** ldnf1_s64_m1:
**	ldnf1d	z0\.d, p0/z, \[x0, #-1, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_s64_m1, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 - svcntd ()),
	   z0 = svldnf1 (p0, x0 - svcntd ()))

/*
** ldnf1_s64_m8:
**	ldnf1d	z0\.d, p0/z, \[x0, #-8, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_s64_m8, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 - svcntd () * 8),
	   z0 = svldnf1 (p0, x0 - svcntd () * 8))

/*
** ldnf1_s64_m9:
**	decb	x0, all, mul #9
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_s64_m9, svint64_t, int64_t,
	   z0 = svldnf1_s64 (p0, x0 - svcntd () * 9),
	   z0 = svldnf1 (p0, x0 - svcntd () * 9))

/*
** ldnf1_vnum_s64_0:
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_0, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, 0),
	   z0 = svldnf1_vnum (p0, x0, 0))

/*
** ldnf1_vnum_s64_1:
**	ldnf1d	z0\.d, p0/z, \[x0, #1, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_1, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, 1),
	   z0 = svldnf1_vnum (p0, x0, 1))

/*
** ldnf1_vnum_s64_7:
**	ldnf1d	z0\.d, p0/z, \[x0, #7, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_7, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, 7),
	   z0 = svldnf1_vnum (p0, x0, 7))

/*
** ldnf1_vnum_s64_8:
**	incb	x0, all, mul #8
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_8, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, 8),
	   z0 = svldnf1_vnum (p0, x0, 8))

/*
** ldnf1_vnum_s64_m1:
**	ldnf1d	z0\.d, p0/z, \[x0, #-1, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_m1, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, -1),
	   z0 = svldnf1_vnum (p0, x0, -1))

/*
** ldnf1_vnum_s64_m8:
**	ldnf1d	z0\.d, p0/z, \[x0, #-8, mul vl\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_m8, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, -8),
	   z0 = svldnf1_vnum (p0, x0, -8))

/*
** ldnf1_vnum_s64_m9:
**	decb	x0, all, mul #9
**	ldnf1d	z0\.d, p0/z, \[x0\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_m9, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, -9),
	   z0 = svldnf1_vnum (p0, x0, -9))

/*
** ldnf1_vnum_s64_x1:
**	cntb	(x[0-9]+)
**	madd	(x[0-9]+), (?:x1, \1|\1, x1), x0
**	ldnf1d	z0\.d, p0/z, \[\2\]
**	ret
*/
TEST_LOAD (ldnf1_vnum_s64_x1, svint64_t, int64_t,
	   z0 = svldnf1_vnum_s64 (p0, x0, x1),
	   z0 = svldnf1_vnum (p0, x0, x1))
