
// ****************************************************************************
//
//                             PidiBoy - Screen Shot
//
// ****************************************************************************

#if USE_SCREENSHOT		// 1=use screen shot

#ifndef _PIDIBOY_SS_H
#define _PIDIBOY_SS_H

#ifdef __cplusplus
extern "C" {
#endif

// Do one screen shot (generates file SCRxxxxx.BMP in root of SD card)
void ScreenShot();

#ifdef __cplusplus
}
#endif

#endif // _PIDIBOY_SS_H

#endif // USE_SCREENSHOT
