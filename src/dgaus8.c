/* dgaus8.f -- translated by f2c (version 20160102).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c/f2c.h"

/* Table of constant values */

static integer c__14 = 14;
static integer c__5 = 5;
static doublereal c_b6 = 1.;
static doublereal c_b8 = 2.;
static integer c__4 = 4;
static integer c__3 = 3;
static integer c__1 = 1;
static integer c_n1 = -1;

/* DECK DGAUS8 */
/* Subroutine */ int dgaus8_(D_fp fun, doublereal *a, doublereal *b, 
	doublereal *err, doublereal *ans, integer *ierr)
{
    /* Initialized data */

    static doublereal x1 = .183434642495649805;
    static integer nlmn = 1;
    static integer kmx = 5000;
    static integer kml = 6;
    static doublereal x2 = .525532409916328986;
    static doublereal x3 = .79666647741362674;
    static doublereal x4 = .960289856497536232;
    static doublereal w1 = .362683783378361983;
    static doublereal w2 = .313706645877887287;
    static doublereal w3 = .222381034453374471;
    static doublereal w4 = .101228536290376259;
    static doublereal sq2 = 1.41421356;

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), log(doublereal), pow_di(
	    doublereal *, integer *), sqrt(doublereal);

    /* Local variables */
    static doublereal c__;
    static integer k, l;
    static doublereal aa[60], ae, ce, ee, ef, hh[60], gl, gr[60];
    static integer lr[60];
    static doublereal vl[60], vr;
    static integer nib;
    static doublereal glr;
    static integer lmn;
    static doublereal eps, est, tol;
    static integer lmx, mxl;
    static doublereal area, anib;
    static integer nlmx, nbits;
    extern doublereal d1mach_(integer *);
    extern integer i1mach_(integer *);
    extern /* Subroutine */ int xermsg_(char *, char *, char *, integer *, 
	    integer *, ftnlen, ftnlen, ftnlen);

/* ***BEGIN PROLOGUE  DGAUS8 */
/* ***PURPOSE  Integrate a real function of one variable over a finite */
/*            interval using an adaptive 8-point Legendre-Gauss */
/*            algorithm.  Intended primarily for high accuracy */
/*            integration or integration of smooth functions. */
/* ***LIBRARY   SLATEC */
/* ***CATEGORY  H2A1A1 */
/* ***TYPE      DOUBLE PRECISION (GAUS8-S, DGAUS8-D) */
/* ***KEYWORDS  ADAPTIVE QUADRATURE, AUTOMATIC INTEGRATOR, */
/*             GAUSS QUADRATURE, NUMERICAL INTEGRATION */
/* ***AUTHOR  Jones, R. E., (SNLA) */
/* ***DESCRIPTION */

/*     Abstract  *** a DOUBLE PRECISION routine *** */
/*        DGAUS8 integrates real functions of one variable over finite */
/*        intervals using an adaptive 8-point Legendre-Gauss algorithm. */
/*        DGAUS8 is intended primarily for high accuracy integration */
/*        or integration of smooth functions. */

/*        The maximum number of significant digits obtainable in ANS */
/*        is the smaller of 18 and the number of digits carried in */
/*        double precision arithmetic. */

/*     Description of Arguments */

/*        Input--* FUN, A, B, ERR are DOUBLE PRECISION * */
/*        FUN - name of external function to be integrated.  This name */
/*              must be in an EXTERNAL statement in the calling program. */
/*              FUN must be a DOUBLE PRECISION function of one DOUBLE */
/*              PRECISION argument.  The value of the argument to FUN */
/*              is the variable of integration which ranges from A to B. */
/*        A   - lower limit of integration */
/*        B   - upper limit of integration (may be less than A) */
/*        ERR - is a requested pseudorelative error tolerance.  Normally */
/*              pick a value of ABS(ERR) so that DTOL .LT. ABS(ERR) .LE. */
/*              1.0D-3 where DTOL is the larger of 1.0D-18 and the */
/*              double precision unit roundoff D1MACH(4).  ANS will */
/*              normally have no more error than ABS(ERR) times the */
/*              integral of the absolute value of FUN(X).  Usually, */
/*              smaller values of ERR yield more accuracy and require */
/*              more function evaluations. */

/*              A negative value for ERR causes an estimate of the */
/*              absolute error in ANS to be returned in ERR.  Note that */
/*              ERR must be a variable (not a constant) in this case. */
/*              Note also that the user must reset the value of ERR */
/*              before making any more calls that use the variable ERR. */

/*        Output--* ERR,ANS are double precision * */
/*        ERR - will be an estimate of the absolute error in ANS if the */
/*              input value of ERR was negative.  (ERR is unchanged if */
/*              the input value of ERR was non-negative.)  The estimated */
/*              error is solely for information to the user and should */
/*              not be used as a correction to the computed integral. */
/*        ANS - computed value of integral */
/*        IERR- a status code */
/*            --Normal codes */
/*               1 ANS most likely meets requested error tolerance, */
/*                 or A=B. */
/*              -1 A and B are too nearly equal to allow normal */
/*                 integration.  ANS is set to zero. */
/*            --Abnormal code */
/*               2 ANS probably does not meet requested error tolerance. */

/* ***REFERENCES  (NONE) */
/* ***ROUTINES CALLED  D1MACH, I1MACH, XERMSG */
/* ***REVISION HISTORY  (YYMMDD) */
/*   810223  DATE WRITTEN */
/*   890531  Changed all specific intrinsics to generic.  (WRB) */
/*   890911  Removed unnecessary intrinsics.  (WRB) */
/*   890911  REVISION DATE from Version 3.2 */
/*   891214  Prologue converted to Version 4.0 format.  (BAB) */
/*   900315  CALLs to XERROR changed to CALLs to XERMSG.  (THJ) */
/*   900326  Removed duplicate information from DESCRIPTION section. */
/*           (WRB) */
/* ***END PROLOGUE  DGAUS8 */
/* ***FIRST EXECUTABLE STATEMENT  DGAUS8 */

/*     Initialize */

    k = i1mach_(&c__14);
    anib = d1mach_(&c__5) * k / .30102;
    nbits = (integer) anib;
/* Computing MIN */
    i__1 = 60, i__2 = nbits * 5 / 8;
    nlmx = min(i__1,i__2);
    *ans = 0.;
    *ierr = 1;
    ce = 0.;
    if (*a == *b) {
	goto L140;
    }
    lmx = nlmx;
    lmn = nlmn;
    if (*b == 0.) {
	goto L10;
    }
    if (d_sign(&c_b6, b) * *a <= 0.) {
	goto L10;
    }
    c__ = (d__1 = 1. - *a / *b, abs(d__1));
    if (c__ > .1) {
	goto L10;
    }
    if (c__ <= 0.) {
	goto L140;
    }
    anib = .5 - log(c__) / .69314718;
    nib = (integer) anib;
/* Computing MIN */
    i__1 = nlmx, i__2 = nbits - nib - 7;
    lmx = min(i__1,i__2);
    if (lmx < 1) {
	goto L130;
    }
    lmn = min(lmn,lmx);
L10:
/* Computing MAX */
    i__1 = 5 - nbits;
    d__1 = abs(*err), d__2 = pow_di(&c_b8, &i__1);
    tol = max(d__1,d__2) / 2.;
    if (*err == 0.) {
	tol = sqrt(d1mach_(&c__4));
    }
    eps = tol;
    hh[0] = (*b - *a) / 4.;
    aa[0] = *a;
    lr[0] = 1;
    l = 1;
    d__1 = aa[l - 1] + hh[l - 1] * 2.;
    d__2 = hh[l - 1] * 2.;
    d__3 = d__1 - x1 * d__2;
    d__4 = d__1 + x1 * d__2;
    d__5 = d__1 - x2 * d__2;
    d__6 = d__1 + x2 * d__2;
    d__7 = d__1 - x3 * d__2;
    d__8 = d__1 + x3 * d__2;
    d__9 = d__1 - x4 * d__2;
    d__10 = d__1 + x4 * d__2;
    est = d__2 * (w1 * ((*fun)(&d__3) + (*fun)(&d__4)) + w2 * ((*fun)(&d__5) 
	    + (*fun)(&d__6)) + (w3 * ((*fun)(&d__7) + (*fun)(&d__8)) + w4 * ((
	    *fun)(&d__9) + (*fun)(&d__10))));
    k = 8;
    area = abs(est);
    ef = .5;
    mxl = 0;

/*     Compute refined estimates, estimate the error, etc. */

L20:
    d__1 = aa[l - 1] + hh[l - 1];
    d__2 = d__1 - x1 * hh[l - 1];
    d__3 = d__1 + x1 * hh[l - 1];
    d__4 = d__1 - x2 * hh[l - 1];
    d__5 = d__1 + x2 * hh[l - 1];
    d__6 = d__1 - x3 * hh[l - 1];
    d__7 = d__1 + x3 * hh[l - 1];
    d__8 = d__1 - x4 * hh[l - 1];
    d__9 = d__1 + x4 * hh[l - 1];
    gl = hh[l - 1] * (w1 * ((*fun)(&d__2) + (*fun)(&d__3)) + w2 * ((*fun)(&
	    d__4) + (*fun)(&d__5)) + (w3 * ((*fun)(&d__6) + (*fun)(&d__7)) + 
	    w4 * ((*fun)(&d__8) + (*fun)(&d__9))));
    d__1 = aa[l - 1] + hh[l - 1] * 3.;
    d__2 = d__1 - x1 * hh[l - 1];
    d__3 = d__1 + x1 * hh[l - 1];
    d__4 = d__1 - x2 * hh[l - 1];
    d__5 = d__1 + x2 * hh[l - 1];
    d__6 = d__1 - x3 * hh[l - 1];
    d__7 = d__1 + x3 * hh[l - 1];
    d__8 = d__1 - x4 * hh[l - 1];
    d__9 = d__1 + x4 * hh[l - 1];
    gr[l - 1] = hh[l - 1] * (w1 * ((*fun)(&d__2) + (*fun)(&d__3)) + w2 * ((*
	    fun)(&d__4) + (*fun)(&d__5)) + (w3 * ((*fun)(&d__6) + (*fun)(&
	    d__7)) + w4 * ((*fun)(&d__8) + (*fun)(&d__9))));
    k += 16;
    area += abs(gl) + (d__1 = gr[l - 1], abs(d__1)) - abs(est);
/*     IF (L .LT .LMN) GO TO 11 */
    glr = gl + gr[l - 1];
    ee = (d__1 = est - glr, abs(d__1)) * ef;
/* Computing MAX */
    d__1 = eps * area, d__2 = tol * abs(glr);
    ae = max(d__1,d__2);
    if (ee - ae <= 0.) {
	goto L40;
    } else {
	goto L50;
    }
L30:
    mxl = 1;
L40:
    ce += est - glr;
    if (lr[l - 1] <= 0) {
	goto L60;
    } else {
	goto L80;
    }

/*     Consider the left half of this level */

L50:
    if (k > kmx) {
	lmx = kml;
    }
    if (l >= lmx) {
	goto L30;
    }
    ++l;
    eps *= .5;
    ef /= sq2;
    hh[l - 1] = hh[l - 2] * .5;
    lr[l - 1] = -1;
    aa[l - 1] = aa[l - 2];
    est = gl;
    goto L20;

/*     Proceed to right half at this level */

L60:
    vl[l - 1] = glr;
L70:
    est = gr[l - 2];
    lr[l - 1] = 1;
    aa[l - 1] += hh[l - 1] * 4.;
    goto L20;

/*     Return one level */

L80:
    vr = glr;
L90:
    if (l <= 1) {
	goto L120;
    }
    --l;
    eps *= 2.;
    ef *= sq2;
    if (lr[l - 1] <= 0) {
	goto L100;
    } else {
	goto L110;
    }
L100:
    vl[l - 1] = vl[l] + vr;
    goto L70;
L110:
    vr = vl[l] + vr;
    goto L90;

/*     Exit */

L120:
    *ans = vr;
    if (mxl == 0 || abs(ce) <= tol * 2. * area) {
	goto L140;
    }
    *ierr = 2;
    xermsg_("SLATEC", "DGAUS8", "ANS is probably insufficiently accurate.", &
	    c__3, &c__1, (ftnlen)6, (ftnlen)6, (ftnlen)40);
    goto L140;
L130:
    *ierr = -1;
    xermsg_("SLATEC", "DGAUS8", "A and B are too nearly equal to allow norma"
	    "l integration. $$ANS is set to zero and IERR to -1.", &c__1, &
	    c_n1, (ftnlen)6, (ftnlen)6, (ftnlen)94);
L140:
    if (*err < 0.) {
	*err = ce;
    }
    return 0;
} /* dgaus8_ */

