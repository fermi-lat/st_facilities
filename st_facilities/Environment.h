/**
 * @file Environment.h
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
 * $Header: /nfs/slac/g/glast/ground/cvs/ScienceTools-scons/st_facilities/st_facilities/Environment.h,v 1.1 2012/11/10 07:22:51 jchiang Exp $
 */

#ifndef st_facilities_Environment_h
#define st_facilities_Environment_h

#include <string>

namespace st_facilities {

class Environment {

public:

   static Environment & instance();

   static std::string dataPath(const std::string & package);

   static std::string packagePath(const std::string & package);

protected:

   Environment();

private:

   static Environment * s_instance;

};

} // namespace st_facilities

#endif // st_facilities_Environment_h
