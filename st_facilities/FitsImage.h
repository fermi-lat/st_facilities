/** 
 * @file FitsImage.h
 * @brief Declaration of FitsImage class
 * @authors J. Chiang
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/st_facilities/FitsImage.h,v 1.1.1.1 2004/08/25 04:55:03 jchiang Exp $
 *
 */

#ifndef st_facilities_FitsImage_h
#define st_facilities_FitsImage_h

#include <sstream>
#include <string>
#include <vector>

namespace st_facilities {

/** 
 * @class FitsImage
 *
 * @brief A class for accessing FITS image data.
 *
 * @author J. Chiang
 *    
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/st_facilities/FitsImage.h,v 1.1.1.1 2004/08/25 04:55:03 jchiang Exp $
 *
 */

class FitsImage {
    
public:

   FitsImage() {}

   FitsImage(const std::string &fitsfile);

   virtual ~FitsImage() {}

   /// A vector of the image axes dimensions
   virtual void getAxisDims(std::vector<int> &axisDims);

   /// The names (CTYPEs) of the image axes
   virtual void getAxisNames(std::vector<std::string> &axisNames);

   /// Get a vector filled with axis abscissa points for the naxis-th
   /// coordinate.
   virtual void getAxisVector(unsigned int naxis, 
                              std::vector<double> &axisVector);

   /// This method computes arrays of longitude and latitude obtained
   /// by traversing the image plane by column number then row.
   virtual void getCelestialArrays(std::vector<double> &lonArray,
                                   std::vector<double> &latArray);
   
   /// Get the pixel values.  They will be indexed by column, row,
   /// then plane, i.e., indx = i + j*NAXIS1 + k*NAXIS1*NAXIS2.  Note
   /// that each image plane is indexed starting at the lower left
   /// (South-East) corner.
   virtual void getImageData(std::vector<double> &imageData);

   /// This returns the pixel solid angles.  Use of this method assumes
   /// that m_axis[0] represents a longitudinal coordinate and that
   /// m_axis[1] represents a latitudinal coordinate.  The pixel values
   /// will be indexed by column then row, indx = i + j*NAXIS1.
   virtual void getSolidAngles(std::vector<double> &solidAngles);

protected:

/** 
 * @class AxisParams
 * @brief Nested n-tuple class to represent FITS image axis information
 */
   class AxisParams {
   public:
      AxisParams() {}
      ~AxisParams() {}
      int size;
      float refVal;
      float step;
      float refPixel;
      std::string axisType;
      std::string comment;
      bool logScale;

      /// Returns a vector of abscissa values based on the axis parameters.
      void computeAxisVector(std::vector<double> &axisVector);
   };

   /// Interface to cfitsio routines.
   void read_fits_image(std::string &filename, std::vector<AxisParams> &axes,
                        std::vector<double> &image);

   /// FITS file name.
   std::string m_filename;

   /// Descriptions for each image axis.
   std::vector<AxisParams> m_axes;

   /// Vectors of abscissa values for each axis.
   std::vector< std::vector<double> > m_axisVectors;

   /// The FITS image data
   std::vector<double> m_image;

};

} // namespace st_facilities

#endif // st_facilities_FitsImage.h
