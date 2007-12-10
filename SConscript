import glob,os,platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if platform.system() == 'Linux':
        progEnv.Append(CPPDEFINES = 'TRAP_FPE')

st_facilitiesLib = libEnv.StaticLibrary('st_facilities', listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('st_facilitiesLib')
progEnv.Tool('addLibrary', library = progEnv['cppunitLibs'])
test_st_facilitiesBin = progEnv.Program('test_st_facilities', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'st_facilities', libraries = [st_facilitiesLib], testApps = [test_st_facilitiesBin], includes = listFiles(['st_facilities/*.h']))