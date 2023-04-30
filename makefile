# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: nmake or GNU make
all : zoombmp.exe

zoombmp.exe : zoombmp.obj zoombmp.res zoombmp.def
	gcc -Zomf zoombmp.obj zoombmp.res zoombmp.def -o zoombmp.exe
	wrc zoombmp.res

zoombmp.obj : zoombmp.c zoombmp.h
	gcc -Wall -Zomf -O2 -c zoombmp.c -o zoombmp.obj

zoombmp.res : zoombmp.rc zoombmp.h zoombmp.ico
	wrc -r zoombmp.rc

clean :
	rm -rf *exe *RES *obj