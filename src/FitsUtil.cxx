/** 
 * @file FitsUtil.cxx
 * @brief Implementation for utility class.
 * @author J. Chiang
 * 
 * $Header: /nfs/slac/g/glast/ground/cvs/st_facilities/src/FitsUtil.cxx,v 1.1.1.1 2004/08/25 04:55:03 jchiang Exp $
 */

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "tip/FileSummary.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/FitsUtil.h"

namespace st_facilities {

#include "fitsio.h"

void FitsUtil::getTableVector(const std::string & filename,
                              const std::string & extName,
                              const std::string & columnName, 
                              std::vector<double> & branchVector) {
   const tip::Table * my_tree 
      = tip::IFileSvc::instance().readTable(filename, extName);
   tip::Table::ConstIterator it = my_tree->begin();
   tip::ConstTableRecord & row = *it;
   int nrows = my_tree->getNumRecords();
   branchVector.resize(nrows);
   for (int i = 0; it != my_tree->end() && i < nrows; ++it, ++i) {
      row[columnName].get(branchVector[i]);
   }
   delete my_tree;
}

void FitsUtil::getRecordVector(const std::string & filename,
                               const std::string & extName,
                               const std::string & columnName,
                               std::vector<double> & tableVector,
                               int recordNum) {
   const tip::Table * my_table 
      = tip::IFileSvc::instance().readTable(filename, extName);
   tip::Table::ConstIterator it = my_table->begin();
   for (int i = 0; i < recordNum && my_table->end() != it; ++i, ++it);
   (*it)[columnName].get(tableVector);
   delete my_table;
}

void FitsUtil::getFitsHduName(const std::string &filename, int hdu,
                              std::string &hduName) {
   int status(0);
   fitsfile * fptr = 0;

   fits_open_file(&fptr, filename.c_str(), READONLY, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::getFitsHduName:\n cfitsio error.");
   }

   int hdutype = 0;
   fits_movabs_hdu(fptr, hdu, &hdutype, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::getFitsHduName:\n cfitsio error.");
   }
   
   char extname[20];
   char comment[72];
   fits_read_key_str(fptr, "EXTNAME", extname, comment, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::getFitsHduName:\n cfitsio error.");
   }

   hduName = extname;
   fits_close_file(fptr, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::getFitsHduName:\n cfitsio error.");
   }
}

void FitsUtil::getFitsColNames(const std::string & filename, int hdu,
                               std::vector<std::string> &columnNames) {
   std::string extName;
   getFitsHduName(filename, hdu, extName);
   const tip::Table * my_table =
      tip::IFileSvc::instance().readTable(filename, extName);
   columnNames = my_table->getValidFields();
}

void FitsUtil::writeChecksums(const std::string & filename) {
// Use tip to ascertain the number of HDUs
   tip::FileSummary summary;
   tip::IFileSvc::instance().getFileSummary(filename, summary);
   unsigned int nhdus = summary.size();

   int status(0);
   fitsfile * fptr = 0;

   fits_open_file(&fptr, filename.c_str(), READWRITE, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::writeChecksums: cfitsio error.");
   }

   int hdutype(0);
   for (unsigned int hdu = 1; hdu < nhdus+1; hdu++) {
      fits_movabs_hdu(fptr, hdu, &hdutype, &status);
      if (status != 0) {
         fits_report_error(stderr, status);
         throw 
            std::runtime_error("FitsUtil::writeChecksums: cfitsio error.");
      }
      
      fits_write_chksum(fptr, &status);
      if (status != 0) {
         fits_report_error(stderr, status);
         throw 
            std::runtime_error("FitsUtil::writeChecksums: cfitsio error.");
      }
   }

   fits_close_file(fptr, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw std::runtime_error("FitsUtil::writeChecksums: cfitsio error.");
   }
}   

} // namespace st_facilities
