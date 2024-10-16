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

#include <triplane.h>
#include <SDL.h>
#include <SDL_endian.h>
#include "io/joystick.h"
#include "io/sound.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


//\\ Keys

const Uint8 *key = SDL_GetKeyboardState(NULL);
struct keymap player_keys[4];

//\\ Rosterdata

struct rosteri roster[MAX_PLAYERS_IN_ROSTER];

//\\ Game config

struct configuration config;

/*
 * Find settings directory and create it if necessary. Directories are
 * tried in the following order:
 *
 * "$TRIPLANE_HOME/"
 * "$HOME/.triplane/" (created if missing)
 * "" (current directory)
 *
 * @param pointer to FILENAME_MAX bytes where settings directory path
 * can be written. Note that it conveniently includes trailing slash
 * when needed for easy concatenation.
 *
 */
static void find_settings_directory(char *dir) {
    char *home;

    home = getenv("TRIPLANE_HOME");
    if (home) {
        strncpy(dir, home, FILENAME_MAX - 1);
        strncat(dir, "/", FILENAME_MAX - 1);
        return;
    }

    home = getenv("HOME");
    if (home) {
        int ret;
        struct stat st;

        strncpy(dir, home, FILENAME_MAX - 1);
        strncat(dir, "/.triplane", FILENAME_MAX - 1);
        ret = stat(dir, &st);
        if (ret) {
#if (defined(_WIN32) || defined(__WIN32__))
            ret = mkdir(dir);
#else
            ret = mkdir(dir, 0755);
#endif
            if (ret) {
                fprintf(stderr, "Failed to create settings directory \"%s\".\n", dir);
                exit(1);
            }
        }
        strncat(dir, "/", FILENAME_MAX - 1);
        return;
    }

    strncpy(dir, "", FILENAME_MAX - 1);
}

/*
 * Open file in settings directory.
 *
 * @param filename settings file name relative to settings directory
 * @param mode mode to use for opening the file
 * @return return value of fopen
 */
FILE *settings_open(const char *filename, const char *mode) {
    char buf[FILENAME_MAX];
    FILE *fp;

    find_settings_directory(buf);
    strncat(buf, filename, FILENAME_MAX - 1);
    fp = fopen(buf, mode);
    return fp;
}

void wait_release(void) {
    int c = 0;

    while (c != SDL_NUM_SCANCODES) {
        update_key_state();
        for (c = 0; c < SDL_NUM_SCANCODES; c++)
            if (key[c] && c != SDL_SCANCODE_NUMLOCKCLEAR && c != SDL_SCANCODE_CAPSLOCK && c != SDL_SCANCODE_SCROLLLOCK)
                break;
    }

}

SDL_Scancode select_key(int player, SDL_Scancode old) {
    int c;
    int flag = 1;

    while (flag) {
        if (key[SDL_SCANCODE_ESCAPE])
            flag = 0;

        update_key_state();

        for (c = SDL_SCANCODE_UNKNOWN; c < SDL_NUM_SCANCODES; c++)
            if (key[c] && c != SDL_SCANCODE_NUMLOCKCLEAR && c != SDL_SCANCODE_CAPSLOCK && c != SDL_SCANCODE_SCROLLLOCK)
                break;

        if (c != SDL_NUM_SCANCODES)
            if ((c != SDL_SCANCODE_ESCAPE) && (c != SDL_SCANCODE_PAUSE)) {
                wait_release();
                return (SDL_Scancode) c;
            }
        if (player == 100)
            return (SDL_Scancode) 100;
    }

    wait_release();
    return old;

}

void swap_roster_endianes(void) {
    int l, l2, l3;

    for (l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
        roster[l].solo_mis_flown = SDL_SwapLE32(roster[l].solo_mis_flown);
        roster[l].solo_mis_success = SDL_SwapLE32(roster[l].solo_mis_success);
        roster[l].solo_mis_drops = SDL_SwapLE32(roster[l].solo_mis_drops);
        roster[l].solo_mis_shotsf = SDL_SwapLE32(roster[l].solo_mis_shotsf);
        roster[l].solo_mis_shotshit = SDL_SwapLE32(roster[l].solo_mis_shotshit);
        roster[l].solo_mis_bombs = SDL_SwapLE32(roster[l].solo_mis_bombs);
        roster[l].solo_mis_bombshit = SDL_SwapLE32(roster[l].solo_mis_bombshit);
        roster[l].solo_mis_totals = SDL_SwapLE32(roster[l].solo_mis_totals);
        roster[l].solo_mis_dropped = SDL_SwapLE32(roster[l].solo_mis_dropped);

        for (l2 = 0; l2 < 4; l2++)
            for (l3 = 0; l3 < 7; l3++)
                roster[l].solo_mis_scores[l2][l3] = SDL_SwapLE32(roster[l].solo_mis_scores[l2][l3]);

        roster[l].multi_mis_flown = SDL_SwapLE32(roster[l].multi_mis_flown);
        roster[l].multi_mis_success = SDL_SwapLE32(roster[l].multi_mis_success);
        roster[l].multi_mis_drops = SDL_SwapLE32(roster[l].multi_mis_drops);
        roster[l].multi_mis_shotsf = SDL_SwapLE32(roster[l].multi_mis_shotsf);
        roster[l].multi_mis_shotshit = SDL_SwapLE32(roster[l].multi_mis_shotshit);
        roster[l].multi_mis_bombs = SDL_SwapLE32(roster[l].multi_mis_bombs);
        roster[l].multi_mis_bombshit = SDL_SwapLE32(roster[l].multi_mis_bombshit);
        roster[l].multi_mis_totals = SDL_SwapLE32(roster[l].multi_mis_totals);
        roster[l].multi_mis_dropped = SDL_SwapLE32(roster[l].multi_mis_dropped);

        roster[l].up = SDL_SwapLE32(roster[l].up);
        roster[l].down = SDL_SwapLE32(roster[l].down);
        roster[l].roll = SDL_SwapLE32(roster[l].roll);
        roster[l].power = SDL_SwapLE32(roster[l].power);
        roster[l].guns = SDL_SwapLE32(roster[l].guns);
        roster[l].bombs = SDL_SwapLE32(roster[l].bombs);
    }
}

void convert_dos_roster(struct dos_roster *droster) {
    int l, l2, l3;

    for (l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
        strncpy(roster[l].pilotname, droster[l].pilotname, 31);
        roster[l].pilotname[31] = 0;

        roster[l].solo_mis_flown = droster[l].solo_mis_flown;
        roster[l].solo_mis_success = droster[l].solo_mis_success;
        roster[l].solo_mis_drops = droster[l].solo_mis_drops;
        roster[l].solo_mis_shotsf = droster[l].solo_mis_shotsf;
        roster[l].solo_mis_shotshit = droster[l].solo_mis_shotshit;
        roster[l].solo_mis_bombs = droster[l].solo_mis_bombs;
        roster[l].solo_mis_bombshit = droster[l].solo_mis_bombshit;
        roster[l].solo_mis_totals = droster[l].solo_mis_totals;
        roster[l].solo_mis_dropped = droster[l].solo_mis_dropped;

        for (l2 = 0; l2 < 4; l2++)
            for (l3 = 0; l3 < 7; l3++)
                roster[l].solo_mis_scores[l2][l3] = droster[l].solo_mis_scores[l2][l3];

        roster[l].multi_mis_flown = droster[l].multi_mis_flown;
        roster[l].multi_mis_success = droster[l].multi_mis_success;
        roster[l].multi_mis_drops = droster[l].multi_mis_drops;
        roster[l].multi_mis_shotsf = droster[l].multi_mis_shotsf;
        roster[l].multi_mis_shotshit = droster[l].multi_mis_shotshit;
        roster[l].multi_mis_bombs = droster[l].multi_mis_bombs;
        roster[l].multi_mis_bombshit = droster[l].multi_mis_bombshit;
        roster[l].multi_mis_totals = droster[l].multi_mis_totals;
        roster[l].multi_mis_dropped = droster[l].multi_mis_dropped;

        roster[l].up = SDL_SCANCODE_X;
        roster[l].down = SDL_SCANCODE_W;
        roster[l].roll = SDL_SCANCODE_S;
        roster[l].power = SDL_SCANCODE_TAB;
        roster[l].guns = SDL_SCANCODE_2;
        roster[l].bombs = SDL_SCANCODE_1;
    }
}

void swap_keyset_endianes(void) {
    int i;

    for (i = 0; i < 4; i++) {
        player_keys[i].up = SDL_SwapLE32(player_keys[i].up);
        player_keys[i].down = SDL_SwapLE32(player_keys[i].down);
        player_keys[i].roll = SDL_SwapLE32(player_keys[i].roll);
        player_keys[i].power = SDL_SwapLE32(player_keys[i].power);
        player_keys[i].guns = SDL_SwapLE32(player_keys[i].guns);
        player_keys[i].bombs = SDL_SwapLE32(player_keys[i].bombs);
    }
}

void save_keyset(void) {
    FILE *faili;

    faili = settings_open(KEYSET_FILENAME, "wb");
    if (faili == NULL) {
        printf("Unable to write keyboard data\n");
    } else {
        swap_keyset_endianes();
        fwrite(player_keys, sizeof(player_keys), 1, faili);
        fclose(faili);
        swap_keyset_endianes();
    }
}

void load_keyset(void) {
    FILE *faili;

    if ((faili = settings_open(KEYSET_FILENAME, "rb")) == NULL) {
        player_keys[0].up = SDL_SCANCODE_X;
        player_keys[0].down = SDL_SCANCODE_W;
        player_keys[0].roll = SDL_SCANCODE_S;
        player_keys[0].power = SDL_SCANCODE_TAB;
        player_keys[0].guns = SDL_SCANCODE_2;
        player_keys[0].bombs = SDL_SCANCODE_1;

        player_keys[2].up = SDL_SCANCODE_V;
        player_keys[2].down = SDL_SCANCODE_T;
        player_keys[2].roll = SDL_SCANCODE_G;
        player_keys[2].power = SDL_SCANCODE_J;
        player_keys[2].guns = SDL_SCANCODE_H;
        player_keys[2].bombs = SDL_SCANCODE_Y;

        player_keys[3].up = SDL_SCANCODE_COMMA;
        player_keys[3].down = SDL_SCANCODE_O;
        player_keys[3].roll = SDL_SCANCODE_L;
        player_keys[3].power = SDL_SCANCODE_APOSTROPHE;
        player_keys[3].guns = SDL_SCANCODE_PERIOD;
        player_keys[3].bombs = SDL_SCANCODE_P;

        player_keys[1].up = SDL_SCANCODE_DOWN;
        player_keys[1].down = SDL_SCANCODE_UP;
        player_keys[1].roll = SDL_SCANCODE_KP_5;
        player_keys[1].power = SDL_SCANCODE_KP_PLUS;
        player_keys[1].guns = SDL_SCANCODE_HOME;
        player_keys[1].bombs = SDL_SCANCODE_LEFT;

        save_keyset();
    } else {
        fread(player_keys, sizeof(player_keys), 1, faili);
        fclose(faili);
        swap_keyset_endianes();
    }
}

void restore_default_roster(FILE *faili) {

    for (int l = 0; l < MAX_PLAYERS_IN_ROSTER; l++) {
        roster[l].pilotname[0] = 0;
        roster[l].solo_mis_flown = 0;
        roster[l].solo_mis_success = 0;
        roster[l].solo_mis_drops = 0;
        roster[l].solo_mis_shotsf = 0;
        roster[l].solo_mis_shotshit = 0;
        roster[l].solo_mis_bombs = 0;
        roster[l].solo_mis_bombshit = 0;
        roster[l].solo_mis_totals = 0;
        roster[l].solo_mis_dropped = 0;

        for (int l2 = 0; l2 < 4; l2++)
            for (int l3 = 0; l3 < 7; l3++)
                roster[l].solo_mis_scores[l2][l3] = 0;

        roster[l].multi_mis_flown = 0;
        roster[l].multi_mis_success = 0;
        roster[l].multi_mis_drops = 0;
        roster[l].multi_mis_shotsf = 0;
        roster[l].multi_mis_shotshit = 0;
        roster[l].multi_mis_bombs = 0;
        roster[l].multi_mis_bombshit = 0;
        roster[l].multi_mis_totals = 0;
        roster[l].multi_mis_dropped = 0;

        roster[l].up = SDL_SCANCODE_X;
        roster[l].down = SDL_SCANCODE_W;
        roster[l].roll = SDL_SCANCODE_S;
        roster[l].power = SDL_SCANCODE_TAB;
        roster[l].guns = SDL_SCANCODE_2;
        roster[l].bombs = SDL_SCANCODE_1;

    }

// Since text input is a pain in Switch, initialize some names
#ifdef __SWITCH__
    for (int idx = 0; idx < 4; idx++) {
        sprintf(roster[idx].pilotname, "Player %d", idx + 1);
    }
#endif

    swap_roster_endianes();
    fwrite(&roster, sizeof(roster), 1, faili);
    fclose(faili);
    swap_roster_endianes();
}

void load_roster(void) {
    FILE *faili;

    if ((faili = settings_open(ROSTER_FILENAME, "rb")) == NULL) {
        if ((faili = settings_open(ROSTER_FILENAME, "wb")) == NULL) {
            printf("\n\nError writing file: %s\n", ROSTER_FILENAME);
            exit(1);
        }

        restore_default_roster(faili);

    } else {
        struct stat sb;
        struct dos_roster droster[MAX_PLAYERS_IN_ROSTER];
        fstat(fileno(faili), &sb);

        if (sb.st_size != sizeof(droster)) {
            struct rosteri_header hdr;
            int ret;

            ret = fread(&hdr, sizeof(hdr), 1, faili);
            if (ret != 1) {
                printf("failed to read roster.dta header\n");
                restore_default_roster(faili);
                return;
            }
            hdr.magic = SDL_SwapLE32(hdr.magic);
            hdr.version = SDL_SwapLE32(hdr.version);
            if (hdr.magic != ROSTERI_MAGIC) {
                printf("invalid roster.dta magic %08x, expected %08x\n", hdr.magic, ROSTERI_MAGIC);
                restore_default_roster(faili);
                return;
            }
            if (hdr.version != ROSTERI_VERSION) {
                printf("unsupported roster.dta version %08x\n", hdr.version);
                restore_default_roster(faili);
                return;
            }
            fread(&roster, sizeof(roster), 1, faili);
        } else {
            fread(&droster, sizeof(droster), 1, faili);
            printf("Converting roster from DOS format.\n");
            convert_dos_roster((struct dos_roster *) &droster);
        }
        fclose(faili);
        swap_roster_endianes();
    }
}

void save_roster(void) {
    FILE *faili;

    if ((faili = settings_open(ROSTER_FILENAME, "wb")) == NULL) {
        printf("\n\nError writing file: %s\n", ROSTER_FILENAME);
        exit(1);
    }

    {
        struct rosteri_header hdr;
        hdr.magic = SDL_SwapLE32(ROSTERI_MAGIC);
        hdr.version = SDL_SwapLE32(ROSTERI_VERSION);
        fwrite(&hdr, sizeof(hdr), 1, faili);
    }
    swap_roster_endianes();
    fwrite(&roster, sizeof(roster), 1, faili);
    fclose(faili);
    swap_roster_endianes();
}

void swap_config_endianes(void) {
    int i;
    config.current_multilevel = SDL_SwapLE32(config.current_multilevel);
    for (i = 0; i < 4; i++) {
        config.player_type[i] = SDL_SwapLE32(config.player_type[i]);
        config.player_number[i] = SDL_SwapLE32(config.player_number[i]);
    }

    // Graphics
    config.shots_visible = SDL_SwapLE32(config.shots_visible);
    config.it_shots_visible = SDL_SwapLE32(config.it_shots_visible);
    config.aa_mg_shots_visible = SDL_SwapLE32(config.aa_mg_shots_visible);
    config.flags = SDL_SwapLE32(config.flags);
    config.flames = SDL_SwapLE32(config.flames);
    config.structure_smoke = SDL_SwapLE32(config.structure_smoke);
    config.svga = SDL_SwapLE32(config.svga);

    // Audio
    config.sound_on = SDL_SwapLE32(config.sound_on);
    config.music_on = SDL_SwapLE32(config.music_on);
    config.sfx_on = SDL_SwapLE32(config.sfx_on);
    config.explosion_sounds = SDL_SwapLE32(config.explosion_sounds);
    config.gunshot_sounds = SDL_SwapLE32(config.gunshot_sounds);
    config.ground_i_sounds = SDL_SwapLE32(config.ground_i_sounds);
    config.explosion_sounds = SDL_SwapLE32(config.explosion_sounds);
    config.splash = SDL_SwapLE32(config.splash);
    config.infantry_sounds = SDL_SwapLE32(config.infantry_sounds);

    // General Flying
    config.poweronoff = SDL_SwapLE32(config.poweronoff);
    config.powerrev = SDL_SwapLE32(config.powerrev);

    // Multiplayer Game
    config.all_planes_are = SDL_SwapLE32(config.all_planes_are);
    config.nocollision = SDL_SwapLE32(config.nocollision);
    config.partcollision = SDL_SwapLE32(config.partcollision);
    config.stop = SDL_SwapLE32(config.stop);
    config.alliance = SDL_SwapLE32(config.alliance);
    config.aa_mgs = SDL_SwapLE32(config.aa_mgs);
    config.it_guns = SDL_SwapLE32(config.it_guns);
    config.infantry = SDL_SwapLE32(config.infantry);
    config.unlimited_ammo = SDL_SwapLE32(config.unlimited_ammo);
    config.unlimited_gas = SDL_SwapLE32(config.unlimited_gas);

    for (int idx = 0; idx < MAX_JOYSTICK_COUNT; idx++) {
        config.joystick[idx] = SDL_SwapLE32(config.joystick[idx]);
        config.joystick_calibrated[idx] = SDL_SwapLE32(config.joystick_calibrated[idx]);
    }
}

void load_config(void) {
    int laskuri;
    FILE *faili;

    config.current_multilevel = 0;

    for (laskuri = 0; laskuri < 4; laskuri++) {
        config.player_type[laskuri] = 0;
        config.player_number[laskuri] = -1;

    }

    // Graphics
    config.shots_visible = 1;
    config.it_shots_visible = 1;
    config.aa_mg_shots_visible = 1;
    config.flags = 1;
    config.flames = 1;
    config.structure_smoke = 0;
    config.svga = 1;

    // Audio
    config.sound_on = 1;
    config.music_on = 1;
    config.sfx_on = 1;
    config.explosion_sounds = 1;
    config.gunshot_sounds = 1;
    config.ground_i_sounds = 1;
    config.explosion_sounds = 1;
    config.splash = 1;
    config.infantry_sounds = 1;

    // General Flying
    config.poweronoff = 0;
    config.powerrev = 0;

    // Multiplayer Game
    config.all_planes_are = 0;
    config.nocollision = 0;
    config.partcollision = 0;
    config.stop = 0;
    config.alliance = 0;
    config.aa_mgs = 1;
    config.it_guns = 1;
    config.infantry = 1;
    config.unlimited_ammo = 0;
    config.unlimited_gas = 0;

    for (int idx = 0; idx < MAX_JOYSTICK_COUNT; idx++) {
        config.joystick[idx] = idx;
        config.joystick_calibrated[idx] = 1;
    }

    faili = settings_open(CONFIGURATION_FILENAME, "rb");

    if (faili != NULL) {
        fread(&config, sizeof(config), 1, faili);
        fclose(faili);
        swap_config_endianes();
    }

    is_there_sound = config.sound_on;
}

void save_config(void) {
    FILE *faili;

    swap_config_endianes();
    faili = settings_open(CONFIGURATION_FILENAME, "wb");
    fwrite(&config, sizeof(config), 1, faili);
    fclose(faili);
    swap_config_endianes();
}
