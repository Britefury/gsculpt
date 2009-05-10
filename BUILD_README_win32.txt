BUILDING gSculpt ON THE WIN32 PLATFORM
======================================


Install Microsoft Visual C++ Express Edition. The 2008 version should work.

Install the Microsoft Windows Platform SDK.
	Set the environment variable 'MSSDK' to the install path of the Microsoft Windows Platform SDK. (e.g. 'C:\Program Files\Microsoft Platform SDK')

Install the Boost libraries. (http://www.boost.org)
	Set the environment variable 'BOOST' to the install path of the Boost libraries. (e.g. 'C:\boost')

Download and install the GTK+ developer libraries; v2.16.1 or later. You can download a bundle which includes header files, lib files, etc.
  Currently at: http://www.gtk.org -> Download -> Windows (32-bit) -> Bundle

Install Wix. (http://wix.sourceforge.net)

Install the Microsoft Visual C++ v9.0 redistributable DLLs.

Find the Boost::python Visual Studio project and build it.

Edit the line of win32build\makerunlight.py that reads:
 "_lineTemplate = 'light -out gSculpt.msi.......' "
  And change the last two paths (which start with C:\Software\Wix\) to point to the location of your Wix installation.
  
Edit the lines of the SConstruct file that read:
	_gthreadIncPaths = [ 'C:\\packages\\gtk+-2.16.1\\include\\glib-2.0', 'C:\\packages\\gtk+-2.16.1\\lib\\glib-2.0\\include' ]
	_gtkLibPaths = [ 'C:\\packages\\gtk+-2.16.1\\lib' ]
  Modify the first part of the paths (c:\packages\gtk+-2.16.1), so that they point to your GTK developer files (the bundle you
  downloaded, above) remembering to use double backslashes.






1. Go into the gSculpt directory (called gSculpt-<version>).
2. At the prompt type:
        > scons
   The gSculpt Python extensions will now be built.
3. At the prompt type:
        > win32insertmanifest
   This will use the manifest tool to insert the generated manifest files into the DLLs.
   This has to be done, as unfortunately, Scons does not seem to deal with the manifest files created by Visual C++ v8.
4. Copy the boost_python.dll built by the Boost::python project into the gSculpt directory.
5. Copy libgdkglext-win32-1.0-0.dll and libgtkglext-win32-1.0-0.dll from the GdkGLExt package into the gSculpt directory.
6. Copy msvcr90.dll, msvcp90.dll, and msvcm90.dll from the MS redistributable package into the gSculpt directory.
6. At the prompt type:
	> buildwin32msi
   This will build the gSculpt MSI installer, which will be placed in the win32build subdirectory.



If the build step ('scons') does not work, then:
Edit the gSculpt SConscript file, and look at the section below the line
 "if sys.platform == 'win32':"
and change the include and library paths for visual studio, the platform SDK, and boost.

