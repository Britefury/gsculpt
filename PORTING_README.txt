PORTING gSculpt TO OTHER PLATFORMS
======================================

There are some platform dependent components to gSculpt which will need to be specialised for new platforms and compilers.



The following functions need to have implementations for your target platform:


cpp/Util/Timer.cpp:
	static double getCurrentTime()
It should return a time value, in seconds, as a double precision float. The synchronisation point (boot-up time, epoch, etc) is not important.



cpp/Util/FPUPrecision.cpp:
	void setFPUPrecision(FPUPrecision precision)
It should set/limit the precision of the FPU to single (32-bit float), double (64-bit float), or extended (80-bit float, if supported) dependent on
the value of the @precision parameter.






Platform macros:

Currently, the build script (SConstruct) will define the following macros via the command line. You may wish to add more for other platforms.


GSCULPT_PLATFORM_WIN32				Defined in a 32-bit Windows environment
GSCULPT_PLATFORM_POSIX				Defined in a Posix environment

GSCULPT_FPU_X86						Defined when compiling for an x86 architecture FPU

