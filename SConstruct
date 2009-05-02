import os
import sys

import os
import sys

import re


# Determine the platform
PLATFORM_WIN32 = 0
PLATFORM_LINUX = 1

platform = PLATFORM_WIN32

if sys.platform == 'win32':
	platform = PLATFORM_WIN32
else:
	platform = PLATFORM_LINUX





# Some helper functions
def prefixPaths(prefix, paths):
	return [ os.path.join( prefix, x )   for x in paths ]


def cppPrefixPaths(prefix, paths):
	return prefixPaths( os.path.join( 'cpp', prefix ), paths )


def shLibsForShLib(libs):
	if platform == PLATFORM_WIN32:
		return libs
	else:
		return []




cppCompGeometryFiles = cppPrefixPaths( 'CompGeometry', [ 'PolyTesselate.cpp' ] )

cppFileIOFiles = cppPrefixPaths( 'FileIO', [ 'fileio.cpp' ] )

cppGraphicsFiles = cppPrefixPaths( 'Graphics', [ 'BackfaceCull.cpp', 'Camera.cpp', 'CameraNavigation.cpp', 'CameraPosition.cpp', 'CameraProjection.cpp', 'Drawing.cpp', 'GLMatrix.cpp', 'Region.cpp' ] )
pyGraphicsFiles = cppPrefixPaths( 'Graphics', [ 'pyBackfaceCull.cpp', 'pyCamera.cpp', 'pyCameraNavigation.cpp', 'pyCameraPosition.cpp', 'pyCameraProjection.cpp', 'pyDrawing.cpp', 'pyGLMatrix.cpp',
										'pyRegion.cpp', 'pyGraphics.cpp' ] )

cppLogGridFiles = cppPrefixPaths( 'LogGrid', [ 'LogScale.cpp', 'LogMultiScale.cpp', 'Ruler.cpp', 'Grid.cpp', 'CGrid.cpp', 'ModellerGrid.cpp' ] )
pyLogGridFiles = cppPrefixPaths( 'LogGrid', [ 'pyModellerGrid.cpp', 'pyLogGrid.cpp' ] )

cppMathFiles = cppPrefixPaths( 'Math', [ 'Bezier2Util.cpp', 'ConvexHull2.cpp', 'IndexTriangle.cpp', 'Projection.cpp', 'Polygon2.cpp', 'Polygon3.cpp', 'Segment2.cpp', 'Segment3.cpp', 'Segment3f.cpp' ] )
pyMathFiles = cppPrefixPaths( 'Math', [ 'pyAxes2.cpp', 'pyAxes3.cpp', 'pyAxis.cpp', 'pyBBox2.cpp', 'pyBBox3.cpp', 'pyBBox3f.cpp', 'pyBezier2Util.cpp', 'pyCircle2.cpp', 'pyColour3f.cpp',
								'pyConvexHull2.cpp', 'pyEulerRotation.cpp',
								'pyFrustum.cpp', 'pyIndexTriangle.cpp', 'pyLine3.cpp', 'pyMatrix3.cpp', 'pyMatrix4.cpp', 'pyPlane.cpp', 'pyPoint2.cpp', 'pyPoint2f.cpp',
								'pyPoint2i.cpp', 'pyPoint3.cpp', 'pyPoint3f.cpp', 'pyPoint4.cpp', 'pyPolygon2.cpp', 'pyPolygon3.cpp', 'pyProjection.cpp', 'pyQuadratic.cpp',
								'pyQuaternion.cpp', 'pyRay3.cpp', 'pySegment2.cpp', 'pySegment3.cpp', 'pySegment3f.cpp', 'pySide.cpp', 'pyTriangle2.cpp', 'pyTriangle3.cpp',
								'pyVector2.cpp', 'pyVector2f.cpp', 'pyVector2i.cpp', 'pyVector3.cpp', 'pyVector3f.cpp', 'pyMath.cpp' ] )

cppMeshFiles = cppPrefixPaths( 'Mesh', [ 'GSProductMesh.cpp', 'MeshPainter.cpp', 'MeshPrimitives.cpp' ]  +
								prefixPaths( 'MMesh', [ 'MeshBuild.cpp', 'MRenderMesh.cpp', 'MVertex.cpp', 'MEdge.cpp', 'MFace.cpp', 'MMesh.cpp', 'MImportMesh.cpp', 'MBBTree.cpp' ] ) )
pyMeshFiles = cppPrefixPaths( 'Mesh', [ 'pyGSProductMesh.cpp', 'pyMeshPainter.cpp', 'pyMeshPrimitives.cpp', 'pyMesh.cpp', 'pyMImportMesh.cpp' ] )

cppModelFiles = cppPrefixPaths( 'Model', [ 'GSProductModelBlank.cpp', 'GSProductModel.cpp', 'ModelDraw.cpp', 'ModelPainter.cpp', 'MarkPredicate.cpp', 'VisualPlane.cpp' ] )
pyModelFiles = cppPrefixPaths( 'Model', [ 'pyGSProductModelBlank.cpp', 'pyGSProductModel.cpp', 'pyModelPainter.cpp', 'pyMPick.cpp', 'pyModel.cpp', 'pyMarkPredicate.cpp', 'pyVisualPlane.cpp', 'pyModelDraw.cpp' ] )

cppGroupFiles = cppPrefixPaths( 'Group', ['GSProductGroup.cpp', 'GroupPainter.cpp' ] )
pyGroupFiles = cppPrefixPaths( 'Group', [ 'pyGSProductGroup.cpp', 'pyGroupPainter.cpp', 'pyGroup.cpp' ] )

cppPainterFiles = cppPrefixPaths( 'Painter', [ 'ObjectPainter.cpp', 'ObjectPainterFactory.cpp' ] )
pyPainterFiles = cppPrefixPaths( 'Painter', [ 'pyObjectPainter.cpp', 'pyObjectPainterFactory.cpp', 'pyPainter.cpp' ] )

cppPolyBlendFiles = cppPrefixPaths( 'PolyBlend', [ 'BlendingPolyline.cpp', 'PolyBlend.cpp' ] )
pyPolyBlendFiles = cppPrefixPaths( 'PolyBlend', [ 'pyPolyBlend.cpp', 'pyPolyBlendModule.cpp' ] )

cppPolylineFiles = cppPrefixPaths( 'Polyline', [ 'PVertex.cpp', 'Polyline.cpp', 'PolylinePath.cpp' ] )

cppProductFiles = cppPrefixPaths( 'Product', [ 'GSProduct.cpp' ] )
pyProductFiles = cppPrefixPaths( 'Product', [ 'pyGSProduct.cpp', 'pyProduct.cpp' ] )

cppRTTypeFiles = cppPrefixPaths( 'RTType', [ 'RTType.cpp' ] )
pyRTTypeFiles = cppPrefixPaths( 'RTType', [ 'pyRTObject.cpp', 'pyRTType.cpp' ] )

cppTransformationFiles = cppPrefixPaths( 'Transformation', [ 'Transformation.cpp', 'BendTransformation.cpp', 'DentTransformation.cpp', 'LinearTwistTransformation.cpp',
												'MatrixTransformation.cpp', 'RadialTwistTransformation.cpp', 'Radius2Transformation.cpp', 'ShrinkTransformation.cpp',
												'TaperTransformation.cpp' ] )
pyTransformationFiles = cppPrefixPaths( 'Transformation', [ 'pyTransformationModule.cpp', 'pyTransformation.cpp', 'pyBendTransformation.cpp', 'pyDentTransformation.cpp',
												'pyLinearTwistTransformation.cpp', 'pyMatrixTransformation.cpp', 'pyRadialTwistTransformation.cpp', 'pyRadius2Transformation.cpp',
												'pyShrinkTransformation.cpp', 'pyTaperTransformation.cpp' ] )

cppUtilFiles = cppPrefixPaths( 'Util', [ 'SETree.cpp', 'Timer.cpp', 'FPUPrecision.cpp' ] )
pyUtilFiles = cppPrefixPaths( 'Util', [ 'pySETree.cpp', 'pyFPUPrecision.cpp', 'pyUtil.cpp' ] )

cppUVMapFiles = cppPrefixPaths( 'UVMap', [ 'UVMapper.cpp' ] )

cppViewFiles = cppPrefixPaths( 'View', [ 'ViewSettings.cpp', 'ViewportSettings.cpp', 'Viewport3dSettings.cpp', 'ViewportUVSettings.cpp' ] )
pyViewFiles = cppPrefixPaths( 'View', [ 'pyPaintLayer.cpp', 'pyViewSettings.cpp', 'pyViewportSettings.cpp', 'pyViewport3dSettings.cpp', 'pyViewportUVSettings.cpp', 'pyView.cpp' ] )

cppGraphViewHelperFiles = cppPrefixPaths( 'GraphViewHelper', [ 'GraphViewWidgetBoxTable.cpp', 'GraphViewLinkCurveTable.cpp' ] )
pyGraphViewHelperFiles = cppPrefixPaths( 'GraphViewHelper', [ 'pyGraphViewWidgetBoxTable.cpp', 'pyGraphViewLinkCurveTable.cpp', 'pyGraphViewHelper.cpp' ] )

cppBrushFiles = cppPrefixPaths( 'Brush', [ 'Brush.cpp', 'SphericalBoundaryBrush.cpp', 'GaussianBrush.cpp', 'SphericalBrush.cpp', 'LinearBrush.cpp', 'FlatBrush.cpp', 'CubicSCurveBrush.cpp' ] )
pyBrushFiles = cppPrefixPaths( 'Brush', [ 'pyBrushModule.cpp', 'pyBrush.cpp', 'pySphericalBoundaryBrush.cpp', 'pyGaussianBrush.cpp', 'pySphericalBrush.cpp', 'pyLinearBrush.cpp', 'pyFlatBrush.cpp', 'pyCubicSCurveBrush.cpp' ] )

cppBackgroundModelFiles = cppPrefixPaths( 'Background', prefixPaths( 'Model', [ 'BackgroundMesh.cpp' ] ) )
pyBackgroundModelFiles = cppPrefixPaths( 'Background', prefixPaths( 'Model', [ 'PyBackgroundModel.cpp', 'PyBackgroundMesh.cpp' ] ) )

cppImportExportFilterObjImportFiles = cppPrefixPaths( 'ImportExportFilter', prefixPaths( 'ObjImport', [ 'ObjStringUtils.cpp', 'ObjLayout.cpp', 'ObjData.cpp', 'LineReader.cpp', 'ObjImport.cpp' ] ) )
pyImportExportFilterObjImportFiles = cppPrefixPaths( 'ImportExportFilter', prefixPaths( 'ObjImport', [ 'PyObjImport.cpp' ] ) )

cppPlatformSpecificFiles = cppPrefixPaths( 'PlatformSpecific', [ 'GLExtensions.cpp' ] )
pyPlatformSpecificFiles = cppPrefixPaths( 'PlatformSpecific', [ 'PyPlatformSpecific.cpp' ] )


cppGreenletFiles = cppPrefixPaths( os.path.join( 'extlibs', 'greenlet' ), [ 'greenlet.c' ] )

pyGLWrapperFiles = cppPrefixPaths( 'GLWrapper', [ 'wrap_GL.cpp' ] )
pyGLUWrapperFiles = cppPrefixPaths( 'GLWrapper', [ 'wrap_GLU.cpp' ] )

pyBugWorkaroundFiles = cppPrefixPaths( 'bugworkarounds', [ 'pyGLDeleteTextures.cpp', 'pybugworkarounds.cpp' ] )




if platform == PLATFORM_WIN32:
	_pythonPath = sys.prefix
	_pythonIncPath = os.path.join( _pythonPath, 'include' )
	_pythonLibPath = os.path.join( _pythonPath, 'libs' )

	assert os.path.exists( _pythonIncPath ), 'Could not get path for python include files, tried %s'  %  ( _pythonIncPath, )
	assert os.path.exists( _pythonLibPath ), 'Could not get path for python library files, tried %s'  %  ( _pythonLibPath, )

	_vc8RootPath = os.environ['VCINSTALLDIR']
	_vc8IncPaths = os.environ['INCLUDE'].split( ';' )
	_vc8LibPaths = os.environ['LIB'].split( ';' )


	# Attempt to get the Platform SDK path
	try:
		_platformSDKRootPath = os.environ['MSSDK']
	except KeyError:
		_vs8RootPath = os.environ['VSINSTALLDIR']
		_platformSDKRootPath = os.path.join( _vs8RootPath, 'Microsoft Platform SDK' )

	assert os.path.exists( _platformSDKRootPath ), 'Could not get path for Microsoft Platform SDK, tried %s, you can set the environment variable \'MSSDK\' to the SDK install path'  %  ( _platformSDKRootPath, )

	_platformSDKIncPath = os.path.join( _platformSDKRootPath, 'include' )
	_platformSDKLibPath = os.path.join( _platformSDKRootPath, 'lib' )

	assert os.path.exists( _platformSDKIncPath ), 'Could not get path for Microsoft Platform SDK include files, tried %s'  %  ( _platformSDKIncPath, )
	assert os.path.exists( _platformSDKLibPath ), 'Could not get path for Microsoft Platform SDK include files, tried %s'  %  ( _platformSDKLibPath, )


	# Attempt to get the boost path
	try:
		_boostRootPath = os.environ['BOOST']
	except KeyError:
		_drive = os.path.splitdrive( sys.prefix )[0]
		_boostRootPath = os.path.join( _drive, '\\boost' )

	assert os.path.exists( _boostRootPath ), 'Could not get path for Boost, tried %s, you can set the environment variable \'BOOST\' to the boost install path'  %  ( _boostRootPath, )

	_boostIncPath = _boostRootPath
	_boostLibPath = os.path.join( _boostRootPath, 'libs\\python\\build\\bin-stage' )

	assert os.path.exists( _boostIncPath ), 'Could not get path for Boost include files, tried %s'  %  ( _boostIncPath, )
	assert os.path.exists( _boostLibPath ), 'Could not get path for Boost include files, tried %s'  %  ( _boostLibPath, )


	localIncPaths = [ 'cpp' ]
	pyIncPaths = [ _pythonIncPath ]
	boostPyIncPaths = [ _boostIncPath ]
	standardIncPaths = _vc8IncPaths  +  [ _platformSDKIncPath ]
	localLibPaths = [ '.' ]
	pyLibPaths = [ _pythonLibPath ]
	boostPyLibPaths = [ _boostLibPath ]
	standardLibPaths = _vc8LibPaths  +  [ _platformSDKLibPath ]

	pyLibs = [ 'python%s%s'  %  ( sys.version_info[0], sys.version_info[1] ) ]
	boostPyLibs = [ 'boost_python' ]
	glLibs = [ 'OpenGL32', 'GLU32' ]
	standardLibs = []
	ccFlags = [ '/nologo', '/EHsc', '/DLL', '/MD', '"/DGS_DllExport=__declspec(dllexport)"', '/DGSCULPT_PLATFORM_WIN32', '/DGSCULPT_FPU_X86' ]
	linkFlags = [ '/NOLOGO' ]

	envPath = os.environ['PATH'].split( ';' )

	pathSplitChar = ';'

	pyExtSuffix = '.pyd'
elif platform == PLATFORM_LINUX:
	def pkgConfig(args):
		return os.popen( 'pkg-config ' + args ).read().strip().split()
	_pythonVersion = '%d.%d'  %  ( sys.version_info[0], sys.version_info[1] )

	localIncPaths = [ 'cpp' ]
	pyIncPaths = [ os.path.join( sys.prefix, 'include', 'python%s'  %  ( _pythonVersion, ) ) ]
	boostPyIncPaths = []
	standardIncPaths = []
	localLibPaths = [ '.' ]
	pyLibPaths = []
	boostPyLibPaths = []
	standardLibPaths = []

	pyLibs = [ 'python%s'  %  ( _pythonVersion, ) ]
	boostPyLibs = [ 'boost_python' ]
	glLibs = [ 'GL', 'GLU' ]
	standardLibs = []
	ccFlags = [ '-Wall', '-Werror', '-ffast-math', '-g', '-DGS_DllExport=', '-DGSCULPT_PLATFORM_POSIX', '-DGSCULPT_FPU_X86' ]  +  pkgConfig( '--cflags glib-2.0' )  +  pkgConfig( '--cflags gthread-2.0' )
	linkFlags = [ '-g' ]  +  pkgConfig( '--libs glib-2.0' )  +  pkgConfig( '--libs gthread-2.0' )

	envPath = None

	pathSplitChar = ':'

	pyExtSuffix = '.so'

incPaths = localIncPaths + pyIncPaths + boostPyIncPaths + standardIncPaths
libPaths = localLibPaths + pyLibPaths + boostPyLibPaths + standardLibPaths

extLibs = pyLibs + boostPyLibs + glLibs + standardLibs



env = Environment( ENV=os.environ )

# Remove optimisation flags
s = re.compile( '(-O2|-O3|-Os|-O)' )
for key in [ 'CXXFLAGS', 'CCFLAGS' ]:
	if env['ENV'].has_key( key ):
		os.environ[key] = s.sub('', env['ENV'][key])

# Get CC, CXX, and CCACHE_DIR from the OS environment
for key in [ 'CC', 'CXX', 'CCACHE_DIR' ]:
  if os.environ.has_key(key):
    env.Replace( **{key: os.environ[key]})

# Append the values of CCFLAGS, CXXFLAGS, CPPPATH, LINKFLAGS, LIBPATH in the OS environment to @env
for key in [ 'CCFLAGS', 'CXXFLAGS', 'LINKFLAGS' ]:
  if os.environ.has_key(key):
    env.Append( **{key: os.environ[key].split(' ')} )

for key in [ 'CPPPATH', 'LIBPATH' ]:
  if os.environ.has_key(key):
    env.Append( **{key: os.environ[key].split( pathSplitChar )} )

# Append @incPaths to CPPPATH, @ccFlags to CCFLAGS
env.Append(CPPPATH = incPaths)
env.Append(CCFLAGS = ccFlags)

# Append @libPaths to LIBPATH
if env.has_key( 'LIBPATH' ):
	libPaths = libPaths + env['LIBPATH']

# Set the path
if envPath is not None:
	env['ENV']['PATH'] = envPath




cppMathLib = env.SharedLibrary( 'Math', cppMathFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Util' ] ) )
cppCompGeometryLib = env.SharedLibrary( 'CompGeometry', cppCompGeometryFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppFileIOLib = env.SharedLibrary( 'FileIO', cppFileIOFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [] ) )
cppGraphicsLib = env.SharedLibrary( 'Graphics', cppGraphicsFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'RTType', 'Util' ] ) )
cppLogGridLib = env.SharedLibrary( 'LogGrid', cppLogGridFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Graphics', 'RTType', 'Util' ] ) )
cppMeshLib = env.SharedLibrary( 'Mesh', cppMeshFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Polyline', 'UVMap', 'RTType', 'Util', 'PolyBlend', 'Transformation', 'View', 'Graphics', 'Painter', 'Model', 'Product', 'CompGeometry' ] ) )
cppModelLib = env.SharedLibrary( 'Model', cppModelFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'RTType', 'Util', 'Product', 'Painter', 'Transformation', 'View', 'Graphics', 'FileIO' ] ) )
cppGroupLib = env.SharedLibrary( 'Group', cppGroupFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'RTType', 'Util', 'Product', 'Painter', 'Transformation', 'View', 'Graphics', 'FileIO', 'Model' ] ) )
cppPainterLib = env.SharedLibrary( 'Painter', cppPainterFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'RTType', 'Util', 'Product', 'Graphics' ] ) )
cppPolyBlendLib = env.SharedLibrary( 'PolyBlend', cppPolyBlendFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppPolylineLib = env.SharedLibrary( 'Polyline', cppPolylineFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'FileIO', 'Math', 'Util', 'Transformation', 'RTType', 'PolyBlend' ] ) )
cppProductLib = env.SharedLibrary( 'Product', cppProductFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util', 'RTType' ] ) )
cppRTTypeLib = env.SharedLibrary( 'RTType', cppRTTypeFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Util', 'FileIO' ] ) )
cppTransformationLib = env.SharedLibrary( 'Transformation', cppTransformationFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppUtilLib = env.SharedLibrary( 'Util', cppUtilFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [] ) )
cppUVMapLib = env.SharedLibrary( 'UVMap', cppUVMapFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppViewLib = env.SharedLibrary( 'View', cppViewFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util', 'RTType', 'Graphics' ] ) )
cppGraphViewHelperLib = env.SharedLibrary( 'GraphViewHelper', cppGraphViewHelperFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppBrushLib = env.SharedLibrary( 'Brush', cppBrushFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util', 'RTType' ] ) )
cppBackgroundModelLib = env.SharedLibrary( 'BackgroundModel', cppBackgroundModelFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppImportExportFilterObjImportLib = env.SharedLibrary( 'ImportExportFilterObjImport', cppImportExportFilterObjImportFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [ 'Math', 'Util' ] ) )
cppPlatformSpecificLib = env.SharedLibrary( 'PlatformSpecific', cppPlatformSpecificFiles, LIBPATH=libPaths, LINKFLAGS=linkFlags,
							LIBS=extLibs + shLibsForShLib( [] ) )

cppLibs = [ 'Math', 'CompGeometry', 'FileIO', 'Graphics', 'LogGrid', 'Mesh', 'Model', 'Painter', 'PolyBlend', 'Polyline', 'Product', 'RTType', 'Transformation', 'Util', 'UVMap', 'View', 'GraphViewHelper', 'Group', 'Brush', 'BackgroundModel',
		'ImportExportFilterObjImport', 'PlatformSpecific' ]


env.SharedLibrary( os.path.join( 'Britefury', 'Graphics', 'Graphics' ), pyGraphicsFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )
env.SharedLibrary( os.path.join( 'Britefury', 'Math', 'Math' ), pyMathFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'LogGrid', 'LogGrid' ), pyLogGridFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )

env.SharedLibrary( os.path.join( 'Britefury', 'Painter', 'Painter' ), pyPainterFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'View', 'View' ), pyViewFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'GraphView', 'GraphViewHelper' ), pyGraphViewHelperFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'Kernel', 'RTType' ), pyRTTypeFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'Util', 'Util' ), pyUtilFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )

env.SharedLibrary( os.path.join( 'Britefury', 'ProceduralCore', 'Product' ), pyProductFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'Model', 'Model' ), pyModelFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'Group', 'Group' ), pyGroupFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )
env.SharedLibrary( os.path.join( 'Britefury', 'Mesh', 'Mesh' ), pyMeshFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix  )

env.SharedLibrary( os.path.join( 'Britefury', 'PolyBlend', 'PolyBlend' ), pyPolyBlendFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'Transformation', 'Transformation' ), pyTransformationFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'Brush', 'Brush' ), pyBrushFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'Background', 'Model' ), pyBackgroundModelFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'ImportExportFilter', 'Obj', 'ObjImport' ), pyImportExportFilterObjImportFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'PlatformSpecific', 'PlatformSpecific' ), pyPlatformSpecificFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'extlibs', 'greenlet', 'greenlet' ), cppGreenletFiles, LIBS=pyLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'bugworkarounds', 'bugworkarounds' ), pyBugWorkaroundFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )

env.SharedLibrary( os.path.join( 'Britefury', 'GL', '_GL' ), pyGLWrapperFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )
env.SharedLibrary( os.path.join( 'Britefury', 'GL', '_GLU' ), pyGLUWrapperFiles, LIBS=extLibs + cppLibs, LINKFLAGS=linkFlags, LIBPATH=libPaths, SHLIBPREFIX='', SHLIBSUFFIX=pyExtSuffix )






