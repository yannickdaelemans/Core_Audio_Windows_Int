# Core_Audio_Windows_Int
make an interface to get audio from windows peripherals to JUCE

# Compiler_Needed
Since this application is made in Windows, we need a GCC/G++ which can work
together with the Windows SDK. The choice has fallen on:
Winlibs: http://winlibs.com/

When compiling, put the tag "-lole32" behind it for the Windows Linker
