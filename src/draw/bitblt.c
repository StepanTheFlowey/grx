/**
 ** bitblt.c ---- the pixel transfer routine
 **
 ** Copyright (c) 1995 Csaba Biegl, 820 Stirrup Dr, Nashville, TN 37221
 ** [e-mail: csaba@vuse.vanderbilt.edu].
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

#include "libgrx.h"
#include "clipping.h"

void grx_bit_blt(GrxContext *dst,int dx,int dy,
              GrxContext *src,int x1,int y1,int x2,int y2,GrxColor oper)
{
        int  oldx1,oldy1;
        int  oldx2,oldy2;
        int  dstx2,dsty2;
        void (*bltfun)(GrxFrame*,int,int,GrxFrame*,int,int,int,int,GrxColor);
        if(dst == NULL) dst = CURC;
        if(src == NULL) src = CURC;
        isort(x1,x2); oldx1 = x1;
        isort(y1,y2); oldy1 = y1;
        cxclip_ordbox(src,x1,y1,x2,y2);
        oldx1 = (dx  += (x1 - oldx1));
        oldy1 = (dy  += (y1 - oldy1));
        oldx2 = dstx2 = dx + x2 - x1;
        oldy2 = dsty2 = dy + y2 - y1;
        clip_ordbox(dst,dx,dy,dstx2,dsty2);
        x1 += (dx - oldx1);
        y1 += (dy - oldy1);
        x2 -= (oldx2 - dstx2);
        y2 -= (oldy2 - dsty2);
        if(src->gc_driver == dst->gc_driver)
            bltfun = src->gc_driver->bitblt;
        else if(src->gc_driver->mode == dst->gc_driver->rmode)
            bltfun = dst->gc_driver->bltr2v;
        else if(src->gc_driver->rmode == dst->gc_driver->mode)
            bltfun = src->gc_driver->bltv2r;
        else return;
        mouse_block(src,x1,y1,x2,y2);
        mouse_addblock(dst,dx,dy,dstx2,dsty2);
        (*bltfun)(
            &dst->frame,(dx + dst->x_offset),(dy + dst->y_offset),
            &src->frame,(x1 + src->x_offset),(y1 + src->y_offset),
            (x2 - x1 + 1),
            (y2 - y1 + 1),
            oper
        );
        mouse_unblock();
}

