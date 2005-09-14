/**
 * @file Util.h
 * @brief Some basic utility functions.
 * @author J. Chiang
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/st_facilities/Util.h,v 1.3 2005/05/25 23:12:15 jchiang Exp $
 */

#ifndef st_facilities_Util_h
#define st_facilities_Util_h

#include <string>
#include <vector>

#include "facilities/Util.h"

namespace st_facilities {

/**
 * @class Util
 * @brief Various static functions of general use for Science Tools 
 * applications.
 *
 * @author J. Chiang
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/st_facilities/Util.h,v 1.3 2005/05/25 23:12:15 jchiang Exp $
 */

class Util {

public:

   /// @return true if the file exists.  Environment variables are 
   ///         *not* expanded.
   static bool fileExists(const std::string & filename);

   /// This expands any environment variables in filename and checks
   /// if the file exists.  If it doesn't, a runtime_error exception
   /// is thrown.  Otherwise, it does nothing.
   static void file_ok(std::string filename);

   /// @brief Read lines, separated by "\n", from a file.  Blank lines
   ///        are skipped.
   /// @param inputFile file to be read; environment variables are expanded
   /// @param lines On return, this vector is filled with each line read 
   ///        from the file.
   /// @param skip The comment string. Lines beginning with this string are
   ///        not put into lines.
   /// @param cleanLines Flag to remove spurious carriage return characters
   ///        introduced by Windows formatting of ascii files.
   static void readLines(std::string inputFile, 
                         std::vector<std::string> &lines,
                         const std::string &skip = "#",
                         bool cleanLines=false);

   /// @brief Remove carriage return characters from Windows formatted
   ///        text files.
   /// @param line The string to be cleaned.
   static void cleanLine(std::string & line);

   /// @brief Determine if a file is a FITS file by looking for the "SIMPLE"
   ///        keyword as the first six characters of the file.  If it is
   ///        not a FITS file, then it is assumed to be a list if FITS files.
   /// @param filename The name of the candidate file; enviroment 
   ///        variables are expanded.
   /// @param files If filename is a FITS file, this is filled with
   ///        the name of that file; otherwise, it is filled with the 
   ///        "\n"-separated lines in the file.
   static void resolve_fits_files(std::string filename, 
                                  std::vector<std::string> &files);

   /// @return true if the filename ends in ".xml" extension
   static bool isXmlFile(std::string filename);

   /// Linear interpolation.
   static double interpolate(const std::vector<double> &x,
                             const std::vector<double> &y,
                             double xx);

   /// A bilinear interpolater.
   static double bilinear(const std::vector<double> &xx, double x,
                          const std::vector<double> &yy, double y, 
                          const std::vector<double> &z);

   /// A bilinear interpolater operating on a vector of vectors
   static double bilinear(const std::vector<double> &xx, double x,
                          const std::vector<double> &yy, double y, 
                          const std::vector< std::vector<double> > &z);

   /// @return true if eObj.what() contains the targetMessage
   ///         as a substring.
   static bool expectedException(const std::exception & eObj,
                                 const std::string & targetMessage);

};

} // namespace st_facilities

#endif // st_facilities_Util_h
