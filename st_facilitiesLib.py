#$Id: st_facilitiesLib.py,v 1.5 2012/12/12 10:01:50 cohen Exp $
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['st_facilities'])
	if env['PLATFORM'] == 'win32'and env.get('CONTAINERNAME','')=='GlastRelease':
            env.Tool('findPkgPath', package = 'st_facilities') 
    env.Tool('astroLib')
    env.Tool('addLibrary', library = env['cfitsioLibs'])
    env.Tool('addLibrary', library = env['f2cLibs'])
    env.Tool('addLibrary', library = env['cppunitLibs'])
    env.Tool('addLibrary', library = env['gsllibs'])

def exists(env):
	return 1
