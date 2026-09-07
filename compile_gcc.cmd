@echo off
SET EMXOMFLD_TYPE=WLINK
SET EMXOMFLD_LINKER=wl.exe
SET EMXOMFLD_PRELINK=0
make -f makefile.gcc clean
make -f makefile.gcc 2>&1 | tee make_gcc.out
