SHilScript 0.2.0
Copyright (C) 2002 Scott Hilbert
See license.txt for licensing details.

PACKAGE CONTENTS

license.txt:
 SHilScript license information.

readme.txt:
 The file you're reading right now :)

Main.cpp:
 Source for the standalone interpreter version of SHilScript.
 This is used as a simple example of hooking up SHilScript to
 an application via C++.

Other .cpp and .h files:
 Source files to SHilScript. Compile them directly into your
 project, make a static library with them, make a DLL, whatever,
 the source is at your fingertips :)

shilscript.dsw, shilscript.dsp, shilscript.opt:
 MSVC project files for SHilScript. These are the project files
 I used to make SHilScript. They're configured to run pascal.shil
 when you execute the program from MSVC.

Makefile:
 Makefile for GNU make. This should work with most versions of
 Make as well, and it also has support for cygwin :)
 To build the shilscript example, just type:
 $ make shilscript
 And it should properly create the shilscript executable. To test
 it, do this:
 $ shilscript pascal.shil
 And it will run the pascal.shil SHilScript example program.

