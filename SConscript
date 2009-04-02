# -*- python -*-
# $Id: SConscript,v 1.5 2008/10/02 23:30:50 glastrm Exp $
# Authors: James Chiang <jchiang@slac.stanford.edu>
# Version: st_facilities-00-14-00

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if baseEnv['PLATFORM'] != "win32":
        progEnv.AppendUnique(CPPDEFINES = ['TRAP_FPE'])

libEnv.Tool('st_facilitiesLib', depsOnly = 1)
st_facilitiesLib = libEnv.StaticLibrary('st_facilities', listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('st_facilitiesLib')
progEnv.Tool('addLibrary', library = progEnv['cppunitLibs'])
test_st_facilitiesBin = progEnv.Program('test_st_facilities', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'st_facilities', libraries = [st_facilitiesLib], testApps = [test_st_facilitiesBin], includes = listFiles(['st_facilities/*.h']),
             data = listFiles(['data/*'], recursive = True))
