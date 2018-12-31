/* ********************************************************************** */
/*                                                                        */
/*   ZoomBMP  Main Module                                                 */
/*                                                                        */
/*   This quick and dirty sample demostrates one method for displaying,   */
/*   zooming in, and zooming out of a bitmap. Clicking in the window      */
/*   with the left mouse button causes a zoom in. Clicking in the window  */
/*   with the right mouse button causes a zoom out.                       */
/*                                                                        */
/*   This is a very simplistic program that is meant only to demonstrate  */
/*   a method of calculating source and target rectangles when doing      */
/*   bitmap "zooming". It is not meant as a demonstration of good,        */
/*   general (PM) programing (i.e. not a lot of time was spent on it's    */
/*   development). It is meant more to demonstrate ideas rather than      */
/*   actual code implementation.                                          */
/*                                                                        */
/*   My apologies if the original choice for display bitmap offends       */
/*   anyone, as that was never the intention.                             */
/*                                                                        */
/*    Standard legalities:                                                */
/*    DISCLAIMER OF WARRANTIES.  The following [enclosed] code is         */
/*    sample code created by IBM Corporation. This sample code is not     */
/*    part of any standard or IBM product and is provided to you solely   */
/*    for  the purpose of assisting you in the development of your        */
/*    applications.  The code is provided "AS IS", without                */
/*    warranty of any kind.  IBM shall not be liable for any damages      */
/*    arising out of your use of the sample code, even if they have been  */
/*    advised of the possibility of such damages.                         */
/*                                                                        */
/*    John Webb                                    (c) IBM Corp, 1993     */
/* ********************************************************************** */


#define INCL_WIN
#define INCL_GPI
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include "ZoomBMP.h"

      /* ------ defines for bit blit points array indices ------ */
#define  TGT_BL  0      //  lower-left corner of target rect
#define  TGT_TR  1      //  upper-right corner of target rect
#define  SRC_BL  2      //  lower-left corner of source rect
#define  SRC_TR  3      //  upper-right corner of source rect

#define  NOMINAL_REZ 100   // 100% image size

HAB     hab;
HMQ     hmq;
HWND    hwndClient;
HWND    hwndFrame;
HWND    hwndVScroll ;
HBITMAP hbmBitmap ;

QMSG    qmsg;

ULONG   ulBitmapCX ;
ULONG   ulBitmapCY ;

    /* ---- These scale factors will be used to zoom in and out ----- */
    /*
     *   The Source Scale Factor will be used for zooming in from the
     *   normal size bitmap, and the Target Scale Factor will be used
     *   for zooming out from the normal sized bitmap. Note that only
     *   one of these factors will ever be greater than 1 at any given
     *   time. By using two scaling factors, we can get away without having
     *   to use floating point (e.g. we will never need a factor of 0.5),
     *   and we can make sure we are always working will the minimum number
     *   of points necessary. Also note that zooming in and out will be done
     *   by a fixed factor of increment of X2 ( i.e. *4, *2, *1, *1/2, *1/4)
     */
ULONG   ulSourceScaleFactor = 1 ;
ULONG   ulTargetScaleFactor = 1 ;

PSZ    szClassName  = "ZoomBMPClass" ;
PSZ    szMainTitle  = "ZoomBMP" ;
PSZ    szErrorTitle = "ZoomBMP Error" ;

        /* ----------------  Prototypes  ------------------------ */
MRESULT EXPENTRY MainWindowProc( HWND, USHORT, MPARAM, MPARAM );
VOID             CalcTransformedPoints( HWND, PPOINTL );
VOID             SetTitlePercentageIndicator( ULONG );
VOID             ShowErrorWindow( PSZ, BOOL );



/* ********************************************************************** */
/*                                                                        */
/*   Main                                                                 */
/*                                                                        */
/*      Standard Main window creation and message loop. Boring.           */
/*                                                                        */
/* ********************************************************************** */

VOID main()
{

  if ( (hab = WinInitialize( 0L )) == (HAB) NULL ){
     printf( "ZoomBMP Error:  WinInitialize failed \n" );
     return;
  }
  else {
     if ( (hmq = WinCreateMsgQueue( hab, 0 )) == (HMQ) NULL ){
        printf( "ZoomBMP Error:  WinCreateMsgQueue failed \n" );
        return;
     }
     else {

       ULONG fulCreate= FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER |
                        FCF_MINMAX | FCF_SHELLPOSITION | FCF_ICON  ;

        WinSetPointer( HWND_DESKTOP,
                       WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,TRUE));

        WinRegisterClass(hab, szClassName, (PFNWP)MainWindowProc, CS_SIZEREDRAW, 0);

        hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                       0L,
                                       (PULONG)&fulCreate,
                                       szClassName ,
                                       "",
                                       0L,
                                       (HMODULE)NULL,
                                       ID_MAIN_WIN,
                                       &hwndClient);
        if ( hwndFrame == NULLHANDLE ) {
           ShowErrorWindow( "Error creating Main window !", TRUE );
        }
        else {
           PID     pid ;
           SWCNTRL swCntrl;
           HSWITCH hSwitch ;

             /* ------  set the title percentage indicator  ------ */
           SetTitlePercentageIndicator( NOMINAL_REZ );
             /* ---------  set window size and pos  -------------- */
           WinSetWindowPos( hwndFrame,
                            HWND_TOP,
                            0, 0, 2*ulBitmapCX, ulBitmapCY,
                            SWP_SHOW | SWP_SIZE | SWP_ACTIVATE );

            /* ----------- add program to tasklist  --------------- */
           WinQueryWindowProcess( hwndFrame, &pid, NULL );
           swCntrl.hwnd = hwndFrame ;
           swCntrl.hwndIcon = (HWND) NULL ;
           swCntrl.hprog = (HPROGRAM) NULL ;
           swCntrl.idProcess = pid ;
           swCntrl.idSession = (LONG) NULL ;
           swCntrl.uchVisibility = SWL_VISIBLE ;
           swCntrl.fbJump = SWL_JUMPABLE ;
           sprintf( swCntrl.szSwtitle, szMainTitle );
           hSwitch = WinCreateSwitchEntry( hab, (PSWCNTRL)&swCntrl);


           WinSetPointer(HWND_DESKTOP,
                         WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));

              /* ---------- start the main processing loop ----------- */
           while (WinGetMsg(hab, &qmsg,NULLHANDLE,0,0)){
               WinDispatchMsg(hab, &qmsg);
           }

           WinRemoveSwitchEntry( hSwitch );
           WinDestroyWindow(hwndFrame);
        }  /* end of else */

        WinSetPointer(HWND_DESKTOP,
                      WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));
        WinDestroyMsgQueue(hmq);
     }  /* end of else ( ...WinCreateMsgQueue() */

   WinTerminate(hab);
   }  /* end of else (...WinInitialize(NULL) */
}  /*  end of main() */

/* ********************************************************************** */
/*                                                                        */
/*   MainWindowProc                                                       */
/*                                                                        */
/* ********************************************************************** */

MRESULT EXPENTRY
MainWindowProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{

  switch (msg) {

    case WM_CREATE:
        {
          HPS                 hps;
          BITMAPINFOHEADER2   bmp2Data ;

               /* ---- get temp PS for the window  --------- */
           hps = WinGetPS( hwnd );

               /* ----- load bitmap out of resource  ------- */
           hbmBitmap = GpiLoadBitmap( hps, NULLHANDLE, ID_BMP, 0L, 0L );

               /* ------- get actual bitmap size  ---------- */
           bmp2Data.cbFix = 16 ;
           GpiQueryBitmapInfoHeader( hbmBitmap, &bmp2Data );
           ulBitmapCX = bmp2Data.cx ;
           ulBitmapCY = bmp2Data.cy ;

           WinReleasePS( hps );
        }
        break;

    case WM_BUTTON1UP :
        {
          ULONG   ulPercentScale ;
           /* -----------------  ZOOM IN  ------------------ */
           /*
            * First check if we are zoomed out any. If so then we need to
            * remove some of the "zoom out" factor. Otherwise, bump up the
            * "zoom in" factor.
            */
        if ( ulTargetScaleFactor > 1 ){
           ulTargetScaleFactor /= 2 ;
           ulPercentScale = NOMINAL_REZ / ulTargetScaleFactor ;
        }
        else {
           ulSourceScaleFactor *= 2 ;
           ulPercentScale = NOMINAL_REZ * ulSourceScaleFactor ;
        }
           /* ------ adjust title bar size indicator  ----------- */
        SetTitlePercentageIndicator( ulPercentScale );
           /* ------------- trigger window repaint -------------- */
        WinInvalidateRect( hwnd, (PRECTL)NULL, FALSE );  
        }
        break;

    case WM_BUTTON2UP :
        {
          ULONG   ulPercentScale ;
           /* -----------------  ZOOM OUT ----------------- */
           /*
            * First check if we are zoomed in any. If so then we need to
            * remove some of the "zoom in" factor. Otherwise, bump up the
            * "zoom out" factor.
            */
        if ( ulSourceScaleFactor > 1 ){
          ulSourceScaleFactor /= 2 ;
          ulPercentScale = NOMINAL_REZ * ulSourceScaleFactor ;
        }
        else {
           ulTargetScaleFactor *= 2 ;
           ulPercentScale = NOMINAL_REZ / ulTargetScaleFactor ;
        }
           /* ------ adjust title bar size indicator  ----------- */
        SetTitlePercentageIndicator( ulPercentScale );
           /* ------------- trigger window repaint -------------- */
        WinInvalidateRect( hwnd, (PRECTL)NULL, FALSE );  
        }
        break;

    case WM_PAINT:
       {
         RECTL   rectl ;
         HPS     hps;
         POINTL  aptlPoints[4] ;

         hps = WinBeginPaint( hwnd, (HPS) NULL, &rectl );

             /* ------- calc source and target boundry points  ------- */
         CalcTransformedPoints( hwnd, aptlPoints );

             /* ----------------- wait pointer on  ---------------- */
         WinSetPointer( HWND_DESKTOP,
                        WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,TRUE));

             /* ------ if zoomed out, fill in black around image ----- */
         if ( ulTargetScaleFactor > 1 ) {
            WinFillRect( hps, &rectl, CLR_BLACK );
         }

             /* ------  blit the bitmap into the window  --------- */
         GpiWCBitBlt( hps,
                      hbmBitmap,
                      4L,
                      aptlPoints,
                      ROP_SRCCOPY,
                      BBO_IGNORE );

              /* ------------ restore normal pointer  -------------- */
          WinSetPointer(HWND_DESKTOP,
                        WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,TRUE));

         WinEndPaint( hps );
      }
      break;

    case WM_DESTROY :
      GpiDeleteBitmap( hbmBitmap );
      return WinDefWindowProc(hwnd,msg,mp1,mp2);

    default:
      return WinDefWindowProc(hwnd,msg,mp1,mp2);

  } /*  end of switch () */
  return( FALSE );

} /*  end of MainWindowProc */
/* ********************************************************************** */
/*                                                                        */
/*   CalcTransformedPoints()                                              */
/*                                                                        */
/*      This is were the fun starts...                                    */
/*                                                                        */
/*      Note: There are four points that need to be calculated in the     */
/*            aptlPoints array:                                           */
/*          aptlPoints[TGT_BL] = lower-left corner of target rect         */
/*          aptlPoints[TGT_TR] = upper-right corner of target rect        */
/*          aptlPoints[SRC_BL] = lower-left corner of source rect         */
/*          aptlPoints[SRC_TR] = upper-right corner of source rect        */
/*                                                                        */
/*                                                                        */
/* ********************************************************************** */
VOID
CalcTransformedPoints( HWND hwndTarget, PPOINTL aptlPoints )
{
  POINTL  ptlWindowCenter,      // center point of window
          ptlBmpCenter,         // center point of actual bitmap
          ptlSourceCenter,      // center point of source rectangle
          ptlTargetCenter ;     // center point of target rectangle

  RECTL   rectl;

     /* ------ get current window size and window center pt  ----- */
  WinQueryWindowRect( hwndTarget, &rectl );
  ptlWindowCenter.x = (rectl.xRight - rectl.xLeft) / 2 ;
  ptlWindowCenter.y = (rectl.yTop - rectl.yBottom) / 2 ;

      /* -------- set "normal" bitmap to get aspect ratio -------- */
      /*
       *  First set source rectangle and target rectangle to be
       *  equal to the window rectangle. If drawn now, the bitmap
       *  would appear justified in the lower left corner ( i.e. it's
       *  0,0 origin at the 0,0 origin of the window). Note that if
       *  the actual bitmap rect is smaller than the window rect, the
       *  extra area will be filled in with black. This step might be
       *  able to be skipped, but for demostration purposes it's good
       *  to start out at a "known" normal sized point
       */
  aptlPoints[TGT_BL].x = aptlPoints[SRC_BL].x = rectl.xLeft ;
  aptlPoints[TGT_BL].y = aptlPoints[SRC_BL].y = rectl.yBottom ;
  aptlPoints[TGT_TR].x = aptlPoints[SRC_TR].x = rectl.xRight ;
  aptlPoints[TGT_TR].y = aptlPoints[SRC_TR].y = rectl.yTop ;

      /* -----------  apply source scaling  ----------------- */
      /*
       *  The trick for good performance is to make sure we are only
       *  manipulating the smallest number of points necessary (i.e.
       *  only those that will fit with in the window rect). The Source
       *  Scaling Factor is used for zooming IN. However, we are going
       *  to use the factor to reduce the source rectangle rather than
       *  enlarge the target. For example, assume our Source Scaling
       *  factor is 2. We want to see everything twice as large as it
       *  was; however, given the same window size, we will only be
       *  seeing half the number of original bitmap points. Therefore,
       *  instead of multipling the target rectangle by the source factor
       *  to create a target twice as large (of which half the points won't
       *  be seen), we will divide the source rectangle by the source factor
       *  to create a source that is half as big and will be stretched to
       *  fit the window doubling it's size in the process. This way we only
       *  manipulate the minimum number of points necessary.
       *
       *  Note: we are making an assumtion that Source bottom left (SRC_BL)
       *      is at 0,0 and Source top right (SRC_TR) is equal to source size
       */
  aptlPoints[SRC_TR].x /= ulSourceScaleFactor ;
  aptlPoints[SRC_TR].y /= ulSourceScaleFactor ;

      /* ------------ get center point of scaled source  ---------- */
  ptlSourceCenter.x = (aptlPoints[SRC_TR].x - aptlPoints[SRC_BL].x) / 2 ;
  ptlSourceCenter.y = (aptlPoints[SRC_TR].y - aptlPoints[SRC_BL].y) / 2 ;

      /* -----------  center Bitmap in Source rect --------------- */
      /*
       *  We want to "slide" the source rect around (without changing
       *  it's size!) to get the center of the actual bitmap aligned with
       *  the center of the source rectangle
       */
  ptlBmpCenter.x = ulBitmapCX / 2 ;
  ptlBmpCenter.y = ulBitmapCY / 2 ;
  aptlPoints[SRC_BL].x += ptlBmpCenter.x - ptlSourceCenter.x ;
  aptlPoints[SRC_BL].y += ptlBmpCenter.y - ptlSourceCenter.y ;
  aptlPoints[SRC_TR].x += ptlBmpCenter.x - ptlSourceCenter.x ;
  aptlPoints[SRC_TR].y += ptlBmpCenter.y - ptlSourceCenter.y ;

      /* --------- apply target scaling  ---------- */
      /*
       *  See description above about source scaling. Target scaling
       *  is done to "zoom out". The target rect is divided by the
       *  scaling factor to decrease the size and cause the original
       *  bitmap to be compressed into the rect.
       */
  aptlPoints[TGT_TR].x /= ulTargetScaleFactor ;
  aptlPoints[TGT_TR].y /= ulTargetScaleFactor ;

      /* --------- adjust target so always centered --------- */
      /*
       *  Slide Target rectangle so that it is centered in
       *  the window rectangle
       */
  ptlTargetCenter.x = (aptlPoints[TGT_TR].x - aptlPoints[TGT_BL].x) / 2 ;
  ptlTargetCenter.y = (aptlPoints[TGT_TR].y - aptlPoints[TGT_BL].y) / 2 ;

  aptlPoints[TGT_BL].x += ptlWindowCenter.x - ptlTargetCenter.x ;
  aptlPoints[TGT_BL].y += ptlWindowCenter.y - ptlTargetCenter.y ;
  aptlPoints[TGT_TR].x += ptlWindowCenter.x - ptlTargetCenter.x ;
  aptlPoints[TGT_TR].y += ptlWindowCenter.y - ptlTargetCenter.y ;

} // end of CalcTransformedPoints
/* ********************************************************************** */
/*                                                                        */
/*   SetTitlePercentageIndicator()                                        */
/*                                                                        */
/* ********************************************************************** */
VOID
SetTitlePercentageIndicator( ULONG ulPercent )
{
  CHAR  acString[64];

  sprintf( acString, " %s - %d%%", szMainTitle, ulPercent );
  WinSetWindowText( hwndFrame, acString );

}  // end of SetTitlePercentageIndicator()
/* ********************************************************************** */
/*                                                                        */
/*   ShowErrorWindow                                                      */
/*                                                                        */
/* ********************************************************************** */
VOID
ShowErrorWindow( PSZ  pszErrorMsg, BOOL bUseLastError )
{
  CHAR      acErrorBuffer[256] ;

  if ( bUseLastError ) {
      ERRORID   errorID = WinGetLastError( hab );

      sprintf( acErrorBuffer,
               "%s \n(code = 0x%lX)",
               pszErrorMsg,
               (ULONG) errorID );
      pszErrorMsg = (PSZ) acErrorBuffer ;
  }  /* end of if ( bUseLastError ) */

  WinMessageBox( HWND_DESKTOP,
                 HWND_DESKTOP,
                 pszErrorMsg ,
                 szErrorTitle ,
                 0,
                 MB_CUACRITICAL | MB_OK );

}


