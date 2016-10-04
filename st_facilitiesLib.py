#$Id: st_facilitiesLib.py,v 1.6 2016/09/29 18:55:12 mdwood Exp $
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['st_facilities'])
        if env['PLATFORM'] == 'win32'and env.get('CONTAINERNAME','')=='GlastRelease':
            env.Tool('findPkgPath', package = 'st_facilities')
    env.Tool('astroLib')
    env.Tool('addLibrary', library = env['cfitsioLibs'])
    env.Tool('addLibrary', library = env['f2cLibs'])
    env.Tool('addLibrary', library = env['cppunitLibs'])
    if env.get['CONTAINERNAME', '') == 'ScienceTools':
        env.Tool('addLibrary', library = env['gsllibs'])

def exists(env):
    return 1
