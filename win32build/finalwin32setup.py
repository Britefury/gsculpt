##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os
import sys

import _winreg

_reg_gtk2 = _winreg.OpenKey( _winreg.HKEY_LOCAL_MACHINE, 'Software\\GTK\\2.0' )



distDir = 'dist'
#gtkDir = 'C:\\Program Files\\Common Files\\GTK\\2.0'
gtkDir = _winreg.QueryValueEx( _reg_gtk2, 'Path' )[0]
gtkResources = [ 'etc', 'lib', 'share' ]
gsimagesDir = 'gsimages'
examplesDir = 'examples'
localeDir = 'locale'
docDir = 'doc'


print 'Found GTK 2.0 at: %s'  %  ( gtkDir, )



def _filterFilename(filename, exts, excludeExts):
    base, ext = os.path.splitext( filename )
    ext = ext.lower()

    if exts is not None:
        if ext  not in  [ e.lower()  for e in exts ]:
            return False

    if ext  in  [ e.lower()  for e in excludeExts ]:
        return False

    return True


def copyFile(srcPath, destPath):
    f = open( srcPath, 'rb' )
    destF = open( destPath, 'wb' )
    destF.write( f.read() )


def copyTree(srcPath, destPath, bRecursive=True, exts=None, excludeExts=[], excludeDirs=[]):
    # Check to ensure that this path is not excluded
    for exc in excludeDirs:
        if srcPath.lower().endswith( exc.lower() ):
            return

    if not os.path.exists( destPath ):
        os.mkdir( destPath )

    entries = [ os.path.join( srcPath, fn )   for fn in os.listdir( srcPath ) ]
    files = [ fn   for fn in entries   if os.path.isfile( fn )  and  _filterFilename( fn, exts, excludeExts ) ]
    dirs = [ dn   for dn in entries   if os.path.isdir( dn ) ]

    # Copy the files
    for fn in files:
        base = os.path.split( fn )[1]
        copyFile( fn, os.path.join( destPath, base ) )

    if bRecursive:
        for dir in dirs:
            base = os.path.split( dir )[1]
            copyTree( dir, os.path.join( destPath, base ), bRecursive, exts, excludeExts, excludeDirs)




def deltree(path):
    if path != ''  and  not os.path.isabs( path )   and   os.path.exists( path ):
        for root, dirs, files in os.walk( path, topdown=False ):
            for name in files:
                os.remove( os.path.join( root, name ) )
            for name in dirs:
                os.rmdir( os.path.join( root, name ) )
        os.rmdir( path )

# Copy the GTK resources
for rsc in gtkResources:
    deltree( os.path.join( distDir, rsc ) )
    copyTree( os.path.join( gtkDir, rsc ), os.path.join( distDir, rsc ) )



# Copy gsimages, examples, and locale
deltree( os.path.join( distDir, gsimagesDir ) )
copyTree( gsimagesDir, os.path.join( distDir, gsimagesDir ), False, [ '.png' ] )
deltree( os.path.join( distDir, examplesDir ) )
copyTree( examplesDir, os.path.join( distDir, examplesDir ), False, [ '.gpx' ] )
deltree( os.path.join( distDir, localeDir ) )
copyTree( localeDir, os.path.join( distDir, localeDir ), True, [ '.mo' ] )
deltree( os.path.join( distDir, docDir ) )
copyTree( docDir, os.path.join( distDir, docDir ), False, [ '.txt', '.html', '.css' ] )


copyFile( 'msvcr90.dll', os.path.join( 'dist', 'msvcr90.dll' ) )
copyFile( 'msvcp90.dll', os.path.join( 'dist', 'msvcp90.dll' ) )
copyFile( 'msvcm90.dll', os.path.join( 'dist', 'msvcm90.dll' ) )
