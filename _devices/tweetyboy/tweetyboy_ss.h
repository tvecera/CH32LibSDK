
// ****************************************************************************
//
//                             TweetyBoy - Screen Shot
//
// ****************************************************************************

#if USE_SCREENSHOT		// 1=use screen shot

#ifndef _TWEETYBOY_SS_H
#define _TWEETYBOY_SS_H

#ifdef __cplusplus
extern "C" {
#endif

// Do one screen shot (generates file SCRxxxxx.BMP in root of SD card)
void ScreenShot();

#ifdef __cplusplus
}
#endif

#endif // _TWEETYBOY_SS_H

#endif // USE_SCREENSHOT
