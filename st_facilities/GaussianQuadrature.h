/**
 * @file GaussianQuadrature.h
 * @brief Wrapper for dgaus8.
 * @author J. Chiang
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/st_facilities/GaussianQuadrature.h,v 1.1 2007/02/10 17:45:01 jchiang Exp $
 */

#ifndef st_facilities_GaussianQuadrature_h
#define st_facilities_GaussianQuadrature_h

namespace st_facilities {

class GaussianQuadrature {

public:

   typedef double (*D_fp)(double*);

   static double integrate(D_fp func, double xmin, double xmax, 
                           double error, long & ier);

};

} // namespace st_facilities

#endif // st_facilities_GaussianQuadrature_h
