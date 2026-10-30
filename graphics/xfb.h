#ifndef XFB_H
#define XFB_H

#include "../global/type.h"

void xfb_generatelut(void);
u8 xfb_lutlookupr(int y, int cr);
u8 xfb_lutlookupb(int y, int cb);
u8 xfb_lutlookupgcb(int cb);
u8 xfb_lutlookupg(int y, signed int cr, int cb);
void xfb_freelut(void);

#endif/* XFB */