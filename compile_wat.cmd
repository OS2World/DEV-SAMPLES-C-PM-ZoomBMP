@echo off
SET INCLUDE=C:\watcom\h;C:\watcom\h\os2
SET LIB=C:\watcom\lib386;C:\watcom\lib386\os2
wmake -f makefile.wat clean
wmake -f makefile.wat 2>&1 | tee make_wat.out
