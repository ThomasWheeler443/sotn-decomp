// SPDX-License-Identifier: AGPL-3.0-or-later
//! O=1
#include "common.h"
#include "game.h"

#define HD_MAIN_GAME_INIT 9

extern void (*g_MainGame)();

static s32 main_fd;
static CdlFILE main_cdlFile;
static char main_cdlMode[4];

void main(void) {
    ResetCallback();
    InitHeap((void*)0x801F0000, 0x7800);
    CdInit();

    for (g_GameState = HD_MAIN_GAME_INIT; g_GameState != -1; g_GameState--) {
        main_fd = open("sim:c:\\bin\\dra.bin", 1);
        if (main_fd != -1) {
            break;
        }
    }
    if (g_GameState != Game_Init) {
        read(main_fd, (void*)DRA_PRG_PTR, 0x120000);
        close(main_fd);
        g_UseDisk = 0;
    } else {
    main_search_loop_1:
        while (!CdSearchFile(&main_cdlFile, "\\DRA.BIN;1")) {
        }

        *main_cdlMode = CdlModeSpeed | CdlModeSpeedNormal;

        CdControl(CdlSetmode, main_cdlMode, NULL);
        CdControl(CdlSetloc, &main_cdlFile, NULL);
        CdRead(0x240, (u32*)DRA_PRG_PTR, CdlModeSpeed | CdlModeSpeedNormal);

        // block until read is complete
        main_fd = CdReadSync(CdlSync, NULL);
        if (main_fd < 0) {
            goto main_search_loop_1;
        }

        g_UseDisk = 1;
    }
    g_MainGame();
}
