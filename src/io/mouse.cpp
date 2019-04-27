/* 
 * Triplane Classic - a side-scrolling dogfighting game.
 * Copyright (C) 1996,1997,2009  Dodekaedron Software Creations Oy
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * tjt@users.sourceforge.net
 */

/*******************************************************************************

   Purpose: 
   	Mouse handling part of Wsystem 2.0 for DJGPP v.2.0�5

*******************************************************************************/

#include "io/mouse.h"
#include "io/video.h"
#include "io/joystick.h"
#include <SDL.h>

#define    MIN(a,b)    (((a)<(b))?(a):(b))
#define    MAX(a,b)    (((a)>(b))?(a):(b))

bool read_touch_screen(float *x, float *y) {
    int count = SDL_GetNumTouchDevices();

    for (int idx = 0; idx < count; idx++) {
        SDL_TouchID touchID = SDL_GetTouchDevice(idx);

        int numTouches = SDL_GetNumTouchFingers(touchID);

        if (numTouches > 0) {
            SDL_Finger *finger = SDL_GetTouchFinger(touchID, 0);

            *x = finger->x;
            *y = finger->y;
            
            return true;
        }
    }

    return false;
}

void hiiri_to(int x, int y) {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(video_state.window, &windowWidth, &windowHeight);

    int mouseX = x * windowWidth / video_state.surface->w;
    int mouseY = y * windowHeight / video_state.surface->h;

    SDL_WarpMouseInWindow(video_state.window, mouseX, mouseY);
}

void koords(int *x, int *y, int *n1, int *n2) {
    Uint8 ret;

    int mouseX, mouseY;

    SDL_PumpEvents();
    ret = SDL_GetMouseState(&mouseX, &mouseY);
    *n1 = !!(ret & SDL_BUTTON(1));
    *n2 = !!(ret & SDL_BUTTON(3));

    int windowWidth, windowHeight;
    SDL_GetWindowSize(video_state.window, &windowWidth, &windowHeight);

#ifdef __SWITCH__
    // Use the first joystick to emulate a mouse (for devices without mouse
    // and with joysticks, like Nintendo Switch)
    //
    if (get_joysticks_count() > 0) {
        float joyX = 0, joyY = 0;
        int joyN1 = 0, joyN2 = 0;
        joystick_emulate_mouse(&joyX, &joyY, &joyN1, &joyN2);

        *n1 = (*n1 || joyN1);
        *n2 = (*n2 || joyN2);

        int speed = windowWidth * 3 / 1280;

        mouseX += speed * joyX;
        mouseY += speed * joyY;

        mouseX = MAX(0, MIN(mouseX, windowWidth));
        mouseY = MAX(0, MIN(mouseY, windowHeight));

        // Correct mouse position for future events
        SDL_WarpMouseInWindow(video_state.window, mouseX, mouseY);
    }

    // Read touch screen where available
    float touchScreenX, touchScreenY;
    if (read_touch_screen(&touchScreenX, &touchScreenY)) {
        mouseX = touchScreenX * windowWidth;
        mouseY = touchScreenY * windowHeight;

        *n1 = 1;

        // Correct mouse position for future events
        SDL_WarpMouseInWindow(video_state.window, mouseX, mouseY);
    }
#endif

    *x = (mouseX - video_state.viewportX) * video_state.surface->w / video_state.viewportWidth;
    *y = (mouseY - video_state.viewportY) * video_state.surface->h / video_state.viewportHeight;
}
