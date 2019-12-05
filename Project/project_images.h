// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __HW3_IMAGES_H__
#define __HW3_IMAGES_H__

#include "main.h"

// Bitmap sizes for g
extern const uint8_t game_overWidthPixels;
extern const uint8_t game_overHeightPixels;
extern const uint8_t game_overBitmaps[];

// Bitmap sizes for soldier
extern const uint8_t soldierWidthPixels;
extern const uint8_t soldierHeightPixels;
extern const uint8_t soldierBitmaps[];

// Bitmap sizes for tank
extern const uint8_t tankWidthPixels;
extern const uint8_t tankHeightPixels;
extern const uint8_t tankBitmaps[];

// Bitmap sizes for fighter
extern const uint8_t fighterWidthPixels;
extern const uint8_t fighterHeightPixels;
extern const uint8_t fighterBitmaps[];

// Bitmap sizes for war
extern const uint8_t warWidthPixels;
extern const uint8_t warHeightPixels;
extern const uint8_t warBitmaps[];

// Bitmap sizes for select
extern const uint8_t selectWidthPixels;
extern const uint8_t selectHeightPixels;
extern const uint8_t selectBitmaps[];

// Bitmap info for enemy_32_32
extern const uint8_t enemy_32_32WidthPixels;
extern const uint8_t enemy_32_32HeightPixels;
extern const uint8_t enemy_32_32Bitmaps[];

// Bitmap info for user_fighter
extern const uint8_t user_fighterWidthPixels;
extern const uint8_t user_fighterHeightPixels;
extern const uint8_t user_fighterBitmaps[];

// Bitmap sizes for usertank
extern const uint8_t user_tankWidthPixels;
extern const uint8_t user_tankHeightPixels;
extern const uint8_t user_tankBitmaps[];

// Bitmap sizes for projectile
extern const uint8_t projectileWidthPixels;
extern const uint8_t projectileHeightPixels;
extern const uint8_t projectileBitmaps[];

// Bitmap sizes for wall
extern const uint8_t wallWidthPixels;
extern const uint8_t wallHeightPixels;
extern const uint8_t wallBitmaps[];


#define SCREEN_X            240
#define SCREEN_Y            320

#define SCREEN_CENTER_X    ((SCREEN_X/2)-1)
#define SCREEN_CENTER_Y    ((SCREEN_Y/2)-1)

#define LINE_WIDTH          11
#define LINE_LENGTH         (SCREEN_X - 1)

#define SQUARE_SIZE         64

#define PADDING             4

#define LEFT_X                      (SCREEN_CENTER_X - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH)
#define CENTER_X                    (SCREEN_CENTER_X)
#define RIGHT_X                     (SCREEN_CENTER_X + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH)

#define UPPER_Y                     (SCREEN_CENTER_Y - SQUARE_SIZE - (2*PADDING) - LINE_WIDTH)
#define CENTER_Y                    (SCREEN_CENTER_Y)
#define LOWER_Y                     (SCREEN_CENTER_Y + SQUARE_SIZE + (2*PADDING) + LINE_WIDTH)

#define UPPER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_Y - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define LOWER_HORIZONTAL_LINE_Y     (SCREEN_CENTER_Y + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)

#define LEFT_VERTICAL_LINE_X      (SCREEN_CENTER_X - (SQUARE_SIZE/2) - PADDING - LINE_WIDTH/2)
#define RIGHT_VERTICAL_LINE_X     (SCREEN_CENTER_X + (SQUARE_SIZE/2) + PADDING + LINE_WIDTH/2)


#endif
