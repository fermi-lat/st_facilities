/**
 * @file PowerLaw.h
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/src/test/PowerLaw.h,v 1.1.1.1 2004/08/25 04:55:03 jchiang Exp $
 */

#include <cmath>
#include <cstdlib>

class PowerLaw {

public:

   PowerLaw(double prefactor, double index) : m_prefactor(prefactor),
                                              m_index(index) {}

   double operator()(double x) const {
      return m_prefactor*std::pow(x, m_index);
   }

   double index() const {
      return m_index;
   }

   double prefactor() const {
      return m_prefactor;
   }

private:

   double m_prefactor;
   double m_index;

};
