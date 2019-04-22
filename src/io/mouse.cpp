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
#include <SDL.h>

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

    *x = mouseX * video_state.surface->w / windowWidth;
    *y = mouseY * video_state.surface->h / windowHeight;
}
