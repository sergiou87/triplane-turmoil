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

#include "io/video.h"
#include "io/dksfile.h"
#include "util/wutil.h"
#include <SDL.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

struct video_state_t video_state = { 0, 0, NULL, NULL, NULL, NULL, 0 };

struct naytto ruutu;

int current_mode = VGA_MODE;
unsigned char *vircr;
int update_vircr_mode = 1;
int draw_with_vircr_mode = 0;
int wantfullscreen = 1;

SDL_Color curpal[256];

/**
 * Sets palette entries firstcolor to firstcolor+n-1
 * from pal[0] to pal[n-1].
 * @param pal the palette, specify NULL to set all colors to black=(0,0,0)
 * @param reverse = 1 to read colors in reverse order (pal[n-1] to pal[0])
 */
void setpal_range(const char pal[][3], int firstcolor, int n, int reverse) {
    SDL_Color *cc = (SDL_Color *) walloc(n * sizeof(SDL_Color));
    int i, from = (reverse ? n - 1 : 0);

    for (i = 0; i < n; i++) {
        if (pal == NULL) {
            cc[i].r = cc[i].g = cc[i].b = 0;
        } else {
            cc[i].r = 4 * pal[from][0];
            cc[i].g = 4 * pal[from][1];
            cc[i].b = 4 * pal[from][2];
        }
        if (reverse)
            from--;
        else
            from++;
    }

    SDL_SetPaletteColors(video_state.surface->format->palette, cc, firstcolor, n);
    // if (draw_with_vircr_mode) {
    //     SDL_SetPalette(video_state.surface, video_state.haverealpalette ? SDL_PHYSPAL : SDL_LOGPAL, cc, firstcolor, n);
    // } else {
    //     SDL_SetPalette(video_state.surface, SDL_PHYSPAL | SDL_LOGPAL, cc, firstcolor, n);
    // }
    memcpy(&curpal[firstcolor], cc, n * sizeof(SDL_Color));
    wfree(cc);
}

static Uint32 getcolor(unsigned char c) {
    // if (video_state.haverealpalette)
        return c;
    // else
    //     return SDL_MapRGB(video_state.surface->format, curpal[c].r, curpal[c].g, curpal[c].b);
}

void fillrect(int x, int y, int w, int h, int c) {
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_FillRect(video_state.surface, &r, getcolor(c));
}

void do_all(int do_retrace) {
    SDL_UpdateTexture(video_state.texture, NULL, video_state.surface->pixels, video_state.surface->w * sizeof (Uint32));

    SDL_RenderClear(video_state.renderer);

    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(video_state.renderer, &windowWidth, &windowHeight);
    float textureAspectRatio = (float)video_state.w / video_state.h;
    float screenAspectRatio = (float)windowWidth / windowHeight;

    SDL_Rect dstRect;

    if (textureAspectRatio > screenAspectRatio) {
        dstRect.x = 0;
        dstRect.w = windowWidth;
        dstRect.h = dstRect.w / textureAspectRatio;
        dstRect.y = (windowHeight - dstRect.h) >> 1;
    }
    else {
        dstRect.y = 0;
        dstRect.h = windowHeight;
        dstRect.w = dstRect.h * textureAspectRatio;
        dstRect.x = (windowWidth - dstRect.w) >> 1;
    }

    SDL_RenderCopy(video_state.renderer, video_state.texture, NULL, &dstRect);
    SDL_RenderPresent(video_state.renderer);
}

static void sigint_handler(int dummy) {
    _exit(1);
}

void init_video(void) {
    int ret;

    if (!video_state.init_done) {
        ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE);
        if (ret) {
            fprintf(stderr, "SDL_Init failed with %d. Is your DISPLAY environment variable set?\n", ret);
            exit(1);
        }
        signal(SIGINT, sigint_handler);
        atexit(SDL_Quit);
        video_state.init_done = 1;

        Uint32 mode_flags = /*SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWPALETTE |*/ SDL_WINDOW_OPENGL;

        // if (!draw_with_vircr_mode)
        //     mode_flags |= SDL_ANYFORMAT;
        // if (wantfullscreen)
        //     mode_flags |= SDL_WINDOW_FULLSCREEN;

        video_state.window = SDL_CreateWindow("Triplane Classic",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                1920,
                                1080,
                                mode_flags);
        assert(video_state.window);

        SDL_SetWindowResizable(video_state.window, SDL_TRUE);

        SDL_ShowCursor(SDL_DISABLE);

        video_state.renderer = SDL_CreateRenderer(video_state.window, -1, 0);
        assert(video_state.renderer);

        if (!draw_with_vircr_mode) {
            vircr = (unsigned char *) walloc(800 * 600);
        }
    }
}

static int init_mode(int new_mode, const char *paletname) {
    // const SDL_VideoInfo *vi;
    int las, las2;
    int w = (new_mode == SVGA_MODE) ? 800 : 320;
    int h = (new_mode == SVGA_MODE) ? 600 : 200;

    init_video();

    int width = w;
    int height = h;

    video_state.w = w;
    video_state.h = h;

    if (video_state.texture != NULL) {
        SDL_DestroyTexture(video_state.texture);
        video_state.texture = NULL;
    }

    if (video_state.surface != NULL) {
        SDL_FreeSurface(video_state.surface);
        video_state.surface = NULL;
    }

    video_state.texture = SDL_CreateTexture(video_state.renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, width, height);
    assert(video_state.texture);

    video_state.surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_ARGB32);

    dksopen(paletname);

    dksread(ruutu.normaalipaletti, sizeof(ruutu.normaalipaletti));
    for (las = 0; las < 256; las++)
        for (las2 = 0; las2 < 3; las2++)
            ruutu.paletti[las][las2] = ruutu.normaalipaletti[las][las2];

    dksclose();

    setpal_range(ruutu.paletti, 0, 256);
    all_bitmaps_refresh();

    current_mode = new_mode;
    return 1;
}

int init_vesa(const char *paletname) {
    return init_mode(SVGA_MODE, paletname);
}

void init_vga(const char *paletname) {
    init_mode(VGA_MODE, paletname);
}
