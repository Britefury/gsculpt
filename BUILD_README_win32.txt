BUILDING gSculpt ON THE WIN32 PLATFORM
======================================


Install Microsoft Visual C++ Express Edition.

Install the Microsoft Windows Platform SDK.
	Set the environment variable 'MSSDK' to the install path of the Microsoft Windows Platform SDK. (e.g. 'C:\Program Files\Microsoft Platform SDK')

Install the Boost libraries. (http://www.boost.org)
	Set the environment variable 'BOOST' to the install path of the Boost libraries. (e.g. 'C:\boost')

Install Wix. (http://wix.sourceforge.net)

Install the Microsoft Visual C++ v8.0 redistributable DLLs.

Find the Boost::python Visual Studio project and build it.

Edit the line of buildwin32msi.bat that reads:
 "light -out gSculpt.msi".......
  And change the last two paths (which start with \wix\) to point to the location of your Wix installation.




1. Go into the gSculpt directory (called gSculpt-<version>).
2. At the prompt type:
        > scons
   The gSculpt Python extensions will now be built.
3. At the prompt type:
        > python win32build\insertmanifest.py
   This will use the manifest tool to insert the generated manifest files into the DLLs.
   This has to be done, as unfortunately, Scons 0.96.1 does not deal with the manifest files created by Visual C++ v8.
4. Copy the boost_python.dll built by the Boost::python project into the gSculpt directory.
5. Copy libgdkglext-win32-1.0-0.dll and libgtkglext-win32-1.0-0.dll from the GdkGLExt package into the gSculpt directory.
6. Copy msvcr80.dll, msvcp80.dll, and msvcm80.dll from the MS redistributable package into the gSculpt directory.
6. At the prompt type:
	> buildwin32msi
   This will build the gSculpt MSI installer, which will be placed in the win32build subdirectory.



If the build step ('scons') does not work, then:
Edit the gSculpt SConscript file, and look at the section below the line
 "if sys.platform == 'win32':"
and change the include and library paths for visual studio, the platform SDK, and boost.

