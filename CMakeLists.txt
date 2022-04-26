add_library(dgaus8 OBJECT src/dgaus8.c)
target_link_libraries(dgaus8 PUBLIC PkgConfig::f2c m)

add_library(
  st_facilities STATIC
  src/Bilinear.cxx
  src/Env.cxx
  src/Environment.cxx
  src/FileSys.cxx
  src/FitsImage.cxx
  src/FitsTable.cxx
  src/FitsUtil.cxx
  src/GaussianQuadrature.cxx
  src/RootFinder.cxx
  src/Util.cxx
)

target_include_directories(
  st_facilities PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:>
)

target_link_libraries(
  st_facilities
  PRIVATE dgaus8 PkgConfig::cfitsio tip
  PUBLIC astro PkgConfig::gsl
)

target_compile_definitions(st_facilities PUBLIC ScienceTools)

add_executable(test_st_facilities src/test/test.cxx)
target_link_libraries(test_st_facilities PRIVATE st_facilities PkgConfig::cppunit)

###############################################################
# Installation
###############################################################
install(DIRECTORY st_facilities DESTINATION ${FERMI_INSTALL_INCLUDEDIR})
install(DIRECTORY data/ DESTINATION ${FERMI_INSTALL_DATADIR})

install(
  TARGETS st_facilities test_st_facilities
  # EXPORT fermiTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
