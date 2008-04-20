BUILDING gSculpt ON THE LINUX PLATFORM
======================================


To build gSculpt, at the command prompt type:
 > scons
The gSculpt Python extensions will now be built.

To install gSculpt, at the command prompt (with root access; use 'sudo' if necessary)
 > scons -f SConstruct-install -Q install



Note that on some distributions you will need to edit the Sconscript file to ensure that the system looks
for components such as the boost libraries in the correct location.
You will see a line that reads:
 "elif platform == PLATFORM_LINUX:"
Below this line is the section where you can configure paths and library names.
