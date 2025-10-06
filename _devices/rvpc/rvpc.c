
#include "../../includes.h"

#include "rvpc_vga.c"
#include "rvpc_draw.c"
#include "rvpc_key.c"
#include "rvpc_snd.c"
#include "rvpc_init.c"

#if FONT_CHAR_COUNT == 64
#include "fonts/chars64/text_mode_font_80.c"
#include "fonts/chars64/text_mode_font_81.c"
#include "fonts/chars64/text_mode_font_thin_8x8_64.c"
#include "fonts/chars64/text_mode_font_bold_8x8_64.c"
#endif

#if FONT_CHAR_COUNT == 128
#include "fonts/chars128/text_mode_font_zx.c"
#include "fonts/chars128/text_mode_font_rvpc.c"
#include "fonts/chars128/text_mode_font_thin_8x8.c"
#include "fonts/chars128/text_mode_font_bold_8x8.c"
#endif

