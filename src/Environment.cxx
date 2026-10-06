/**
 * @file Environment.cxx
 * @brief Yet another environment interface.  This provides access to
 * facilities::commonUtilities functions that rely on environment
 * variables.  By implementing as a Singleton and providing access to
 * the underlying functions only via the Singleton object, this class
 * ensures that the facilities::commonUtilities::setupEnvironment()
 * function is called without having to burden the clients with this
 * task.
 *
 * @author J. Chiang
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/ScienceTools-scons/st_facilities/src/Environment.cxx,v 1.2 2012/11/11 00:18:22 jchiang Exp $
 */

#include "facilities/commonUtilities.h"

#include "st_facilities/Environment.h"

namespace st_facilities {

Environment * Environment::s_instance(nullptr);

auto Environment::instance() -> Environment & {
   if (s_instance == nullptr) {
      s_instance = new Environment();
   }
   return *s_instance;
}

Environment::Environment() {
   facilities::commonUtilities::setupEnvironment();
}

auto Environment::dataPath(const std::string & package) -> std::string {
   instance();
   return facilities::commonUtilities::getDataPath(package);
}

auto Environment::getEnv(const std::string & envvar) -> std::string {
   instance();
   return facilities::commonUtilities::getEnvironment(envvar);
}

auto Environment::packagePath(const std::string & package) -> std::string {
   instance();
   return facilities::commonUtilities::getPackagePath(package);
}

auto Environment::xmlPath(const std::string & package) -> std::string {
   instance();
   return facilities::commonUtilities::getXmlPath(package);
}

} // namespace st_facilities
