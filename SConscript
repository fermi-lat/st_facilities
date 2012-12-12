# -*- python -*-
# $Id: SConscript,v 1.26 2012/12/11 09:01:23 cohen Exp $
# Authors: James Chiang <jchiang@slac.stanford.edu>
# Version: st_facilities-00-20-00

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()
if baseEnv['PLATFORM'] == "posix":
    libEnv.Append(CPPDEFINES = 'TRAP_FPE')

libEnv.Tool('addLinkDeps', package="st_facilities", toBuild="shared") 
st_facilitiesLib = libEnv.SharedLibrary('st_facilities',
                                        listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('st_facilitiesLib')

if baseEnv['PLATFORM'] == "posix":
    progEnv.Append(CPPDEFINES = 'TRAP_FPE')

progEnv.Tool('addLibrary', library = progEnv['cppunitLibs'])
test_st_facilitiesBin = progEnv.Program('test_st_facilities', 
                                        listFiles(['src/test/*.cxx']))

#progEnv.Tool('registerObjects', package = 'st_facilities', libraries = [st_facilitiesLib], testApps = [test_st_facilitiesBin], includes = listFiles(['st_facilities/*.h']),
#             data = listFiles(['data/*'], recursive = True))

progEnv.Tool('registerTargets', package = 'st_facilities',
             staticLibraryCxts = [[st_facilitiesLib, libEnv]],
             testAppCxts = [[test_st_facilitiesBin, progEnv]],
             includes = listFiles(['st_facilities/*.h']),
             data = listFiles(['data/*'], recursive = True))
