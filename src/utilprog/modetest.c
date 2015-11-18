/**
 ** modetest.c ---- test all available graphics modes
 **
 ** Copyright (c) 1995 Csaba Biegl, 820 Stirrup Dr, Nashville, TN 37221
 ** [e-mail: csaba@vuse.vanderbilt.edu]
 **
 ** This file is part of the GRX graphics library.
 **
 ** The GRX graphics library is free software; you can redistribute it
 ** and/or modify it under some conditions; see the "copying.grx" file
 ** for details.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 **
 **/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "grx-3.0.h"
#include "grxkeys.h"

#include "../../test/drawing.h"

static void PrintInfo(void)
{
    char aux[81];
    int x, y;

    sprintf(aux, " Mode: %dx%d %d bpp ", grx_get_current_video_mode()->width,
    grx_get_current_video_mode()->height, grx_get_current_video_mode()->bpp);
    x = (GrMaxX() -
        GrFontStringWidth(&GrDefaultFont, aux, strlen(aux), GR_BYTE_TEXT)) / 2;
    y = (GrMaxY() -
        GrFontStringHeight(&GrDefaultFont, aux, strlen(aux), GR_BYTE_TEXT)) / 2;
    GrTextXY(x, y, aux, GrWhite(), GrBlack());
}

typedef struct {
    int  w,h,bpp;
} gvmode;

gvmode grmodes[200];
int  n_modes = 0;

gvmode *collectmodes(const GrxVideoDriver *drv,gvmode *gp)
{
        GrxFrameMode fm;
        const GrxVideoMode *mp;
        for(fm =GRX_FRAME_MODE_FIRST_GRAPHICS;
              fm <= GRX_FRAME_MODE_LAST_GRAPHICS; fm++) {
            for(mp = grx_get_first_video_mode(fm); mp; mp = grx_get_next_video_mode(mp)) {
                gp->w   = mp->width;
                gp->h   = mp->height;
                gp->bpp = mp->bpp;
                gp++;
            }
        }
        return(gp);
}

int vmcmp(const void *m1,const void *m2)
{
        gvmode *md1 = (gvmode *)m1;
        gvmode *md2 = (gvmode *)m2;
        if(md1->bpp != md2->bpp) return(md1->bpp - md2->bpp);
        if(md1->w   != md2->w  ) return(md1->w   - md2->w  );
        if(md1->h   != md2->h  ) return(md1->h   - md2->h  );
        return(0);
}

#define LINES   18
#define COLUMNS 80
void ModeText(int i, int shrt,char *mdtxt) {
        switch (shrt) {
          case 2 : sprintf(mdtxt,"%2d) %dx%d ", i+1, grmodes[i].w, grmodes[i].h);
                   break;
          case 1 : sprintf(mdtxt,"%2d) %4dx%-4d ", i+1, grmodes[i].w, grmodes[i].h);
                   break;
          default: sprintf(mdtxt,"  %2d)  %4dx%-4d ", i+1, grmodes[i].w, grmodes[i].h);
                   break;
        }
        mdtxt += strlen(mdtxt);

        if (grmodes[i].bpp > 20)
          sprintf(mdtxt, "%ldM", 1L << (grmodes[i].bpp-20));
        else  if (grmodes[i].bpp > 10)
          sprintf(mdtxt, "%ldk", 1L << (grmodes[i].bpp-10));
        else
          sprintf(mdtxt, "%ld", 1L << grmodes[i].bpp);
        switch (shrt) {
          case 2 : break;
          case 1 : strcat(mdtxt, " col"); break;
          default: strcat(mdtxt, " colors"); break;
        }
}

int ColsCheck(int cols, int ml, int sep) {
  int len;

  len = ml * cols + (cols-1) * sep + 1;
  return len <= COLUMNS;
}

void PrintModes(void) {
        char mdtxt[100];
        unsigned int maxlen;
        int i, n, shrt, c, cols;

        cols = (n_modes+LINES-1) / LINES;
        do {
          for (shrt = 0; shrt <= 2; ++shrt) {
            maxlen = 0;
            for (i = 0; i < n_modes; ++i) {
              ModeText(i,shrt,mdtxt);
              if (strlen(mdtxt) > maxlen) maxlen = strlen(mdtxt);
            }
            n = 2;
            if (cols>1 || shrt<2) {
              if (!ColsCheck(cols, maxlen, n)) continue;
              while (ColsCheck(cols, maxlen, n+1) && n < 4) ++n;
            }
            c = 0;
            for (i = 0; i < n_modes; ++i) {
              if (++c == cols) c = 0;
              ModeText(i,shrt,mdtxt);
              printf("%*s%s", (c ? -((int)(maxlen+n)) : -((int)maxlen)), mdtxt, (c ? "" : "\n") );
            }
            if (!c) printf("\n");
            return;
          }
          --cols;
        } while (1);
}

int main(void)
{
        static int firstgr = 1;
        grx_set_driver(NULL);
        if(grx_get_current_video_driver() == NULL) {
            printf("No graphics driver found\n");
            exit(1);
        }
        for( ; ; ) {
            int  i,w,h,px,py;
            char m1[41];
            n_modes = (int)(collectmodes(grx_get_current_video_driver(),grmodes) - grmodes);
            grx_set_mode(GRX_GRAPHICS_MODE_TEXT_DEFAULT);
            if(n_modes == 0) {
                printf("No graphics modes found\n");
                exit(1);
            }
            qsort(grmodes,n_modes,sizeof(grmodes[0]),vmcmp);
            printf(
                "Graphics driver: \"%s\"\n"
                "  graphics defaults: %dx%d %ld colors\n"
                "  text defaults: %dx%d %ld colors\n\n",
                grx_get_current_video_driver()->name,
                GrDriverInfo->defgw,
                GrDriverInfo->defgh,
                (long)GrDriverInfo->defgc,
                GrDriverInfo->deftw,
                GrDriverInfo->defth,
                (long)GrDriverInfo->deftc
            );
            PrintModes();
            printf("\nEnter choice #, or anything else to quit> ");
            fflush(stdout);
            if(!fgets(m1,40,stdin) || 
            (sscanf(m1,"%d",&i) != 1) || (i < 1) || (i > n_modes)) {
                exit(0);
            }
            if(firstgr) {
                printf(
                    "When in graphics mode, press any key to return to menu.\n"
                    "Now press <CR> to continue..."
                );
                fflush(stdout);
                fgets(m1,40,stdin);
                firstgr = 0;
            }
            i--;
            grx_set_mode(
                GRX_GRAPHICS_MODE_GRAPHICS_WIDTH_HEIGHT_BPP,
                grmodes[i].w,
                grmodes[i].h,
                grmodes[i].bpp
            );
            if(grmodes[i].bpp<15) {
                w = grx_get_screen_x() >> 1;
                h = grx_get_screen_y() >> 1;
                px = w + 5;
                py = h + 5;
                w -= 10;
                h -= 10;
                drawing(
                    5,5,w,h,
                    GrBlack(),
                    GrWhite()
                );
                drawing(
                    px,5,w,h,
                    GrAllocColor(255,0,0),
                    GrAllocColor(0,255,0)
                );
                drawing(
                    5,py,w,h,
                    GrAllocColor(0,0,255),
                    GrAllocColor(255,255,0)
                );
                drawing(
                    px,py,w,h,
                    GrAllocColor(255,0,255),
                    GrAllocColor(0,255,255)
                );
            } else {
                int y,sx;
                sx=grx_get_screen_x()>>2;
                for(y=0;y<grx_get_screen_y();y++) {
                    int yy = y & 255;
                    GrHLine(0,sx-1,y,GrBuildRGBcolorT(yy,0,0));
                    GrHLine(sx,2*sx-1,y,GrBuildRGBcolorT(0,yy,0));
                    GrHLine(2*sx,3*sx-1,y,GrBuildRGBcolorT(0,0,yy));
                    GrHLine(3*sx,4*sx-1,y,GrBuildRGBcolorT(yy,yy,yy));
                }
            }
            PrintInfo();
            GrKeyRead();
        }
        return 0;
}
