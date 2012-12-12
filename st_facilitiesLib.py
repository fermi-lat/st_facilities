#$Id: st_facilitiesLib.py,v 1.3 2012/12/11 21:18:41 jrb Exp $
def generate(env, **kw):
	if not kw.get('depsOnly',0):
	    env.Tool('addLibrary', library = ['st_facilities'])

	    if env['PLATFORM'] == 'win32'and env.get('CONTAINERNAME','')=='GlastRelease':
		env.Tool('findPkgPath', package = 'st_facilities') 


	env.Tool('astroLib')
	env.Tool('addLibrary', library = env['cfitsioLibs'])
	env.Tool('addLibrary', library = env['f2cLibs'])


def exists(env):
	return 1
