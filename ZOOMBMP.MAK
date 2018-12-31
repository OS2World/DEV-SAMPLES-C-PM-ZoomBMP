# ======================================================================== #
#                                                                          #
#   Makefile for ZoomBMP                                                   #
#                                                                          #
# ======================================================================== #


ZoomBMP.exe : ZoomBMP.obj  ZoomBMP.def ZoomBMP.res
     link386  /NOE /NOI /DE ZoomBMP,,,, ZoomBMP.def
     rc ZoomBMP.res  ZoomBMP.exe

ZoomBMP.obj : ZoomBMP.c ZoomBMP.h
     icc /C /Ti /Ss ZoomBMP.c

ZoomBMP.res : ZoomBMP.rc ZoomBMP.h
     rc -r ZoomBMP
