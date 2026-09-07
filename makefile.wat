# makefile.wat - wmake / OpenWatcom build for DEV-SAMPLES-C-PM-ZoomBMP
#
# Tools used:
#   Compile: wcc386
#   Link:    wlink
#   Resource: wrc
#   Make:    wmake

SRC = src
BIN = bin-wat

all : $(BIN)\zoombmp.exe

$(BIN)\zoombmp.exe : $(BIN)\zoombmp.obj $(BIN)\zoombmp.res $(SRC)\zoombmp.lnk
	wlink system os2v2_pm name $(BIN)\zoombmp.exe op q op maxe=25 &
	    file { $(BIN)\zoombmp.obj } @$(SRC)\zoombmp.lnk
	wrc -q $(BIN)\zoombmp.res $(BIN)\zoombmp.exe

$(BIN)\zoombmp.obj : $(SRC)\zoombmp.c $(SRC)\zoombmp.h
	if not exist $(BIN) md $(BIN)
	wcc386 -bt=os2 -q -Ot -wx -I$(SRC) $(SRC)\zoombmp.c -fo=$(BIN)\zoombmp.obj

$(BIN)\zoombmp.res : $(SRC)\zoombmp.rc $(SRC)\zoombmp.h $(SRC)\zoombmp.ico $(SRC)\mandy.bmp
	if not exist $(BIN) md $(BIN)
	wrc -r -I$(SRC) $(SRC)\zoombmp.rc -fo=$(BIN)\zoombmp.res

clean : .SYMBOLIC
	if exist $(BIN)\zoombmp.obj del $(BIN)\zoombmp.obj
	if exist $(BIN)\zoombmp.res del $(BIN)\zoombmp.res
	if exist $(BIN)\zoombmp.exe del $(BIN)\zoombmp.exe
