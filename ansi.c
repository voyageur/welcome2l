/* Welcome2L - Welcome to Linux, ANSI boot logo
 * Copyright (c)1998-1999 Jean-Marc Jacquet <jm@littleigloo.org>  
 * Little Igloo Org  http://www.LittleIgloo.org
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <string.h>

char WSCREEN[10000];
static int old_bg, old_fg, old_attr;

void ANSI_screen_init (void) {
  strcpy (WSCREEN, "\033[m\033]R\033]R\033[H\033[J\033[0;10;1;11m");
  old_bg = old_fg = old_attr = -1;
}


void ANSI_screen_close (void) {
  strcat (WSCREEN, "\033[10m\033[m");
}

int ANSI_col_convert (int col) {
  col = col & 7;
  switch (col)
    {
    case 1:
      col = 4;
      break;
    case 3:
      col = 6;
      break;
    case 4:
      col = 1;
      break;
    case 6:
      col = 3;
      break;
    }
  return col;
}

void ANSI_do_color (unsigned char col) {
  extern char NO_BLINK;
  int this_bg, this_fg, this_attr;
  char buf[25];

  this_fg = ANSI_col_convert (col);
  this_bg = ANSI_col_convert (col >> 4);
  if ((col & 15) > 7)
    this_attr = 1;
  else
    this_attr = 0;

  if (((col >> 4) & 15) > 7) {
   if (NO_BLINK)
    this_attr = 1;
   else
    this_attr = 5;
  }

  if ((this_attr == old_attr) && (this_bg == old_bg) && (this_fg == old_fg))
    return;

  if (this_attr == old_attr)
    {
      if ((this_bg != old_bg) && (this_fg != old_fg))
  sprintf (buf, "\033[3%i;4%im", this_fg, this_bg);
      else
  {
    if (this_bg != old_bg)
      sprintf (buf, "\033[4%im", this_bg);
    else
      sprintf (buf, "\033[3%im", this_fg);
  }
    }
  else
    {
      if (this_attr)
  {
    if (old_attr != 5)
      sprintf (buf, "\033[%i", this_attr);
    else
      sprintf (buf, "\033[0;1");

    if (this_attr == 5)
      if ((col & 15) > 7)
        sprintf (buf, "\033[1;5");

    if (this_fg != old_fg)
      sprintf (buf + strlen(buf), ";3%i", this_fg);
    if (this_bg != old_bg)
      sprintf (buf + strlen(buf), ";4%i", this_bg);
    strcat (buf, "m");
  }
      else
  sprintf (buf, "\033[0;3%i;4%im", this_fg, this_bg);
    }

  strcat (WSCREEN, buf);

  old_attr = this_attr;
  old_bg = this_bg;
  old_fg = this_fg;
}

void ANSI_do_char (unsigned char c) {
  switch (c)
    {
    case 22:
      c = 254;
      break;
    case 0x0F:
      c = 14;
    }
  const char buf[] = { c, '\0' };
  strcat (WSCREEN, buf);
}

void ANSI_pos (int x, int y) {
  sprintf (WSCREEN + strlen(WSCREEN), "\033[%i;%iH", y, x);
}

void ANSI_save_pos (void) {
  strcat (WSCREEN, "\033[s");
}

void ANSI_restore_pos (void) {
  strcat (WSCREEN, "\033[u");
}

/* EOF */
/* vim: set tabstop=2 softtabstop=0 shiftwidth=2 expandtab: */
