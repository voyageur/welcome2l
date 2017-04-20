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


#define ANSI_print(x) strcat(WSCREEN,x)
#define ANSI_printxy(x,y,s) ANSI_pos(x,y);ANSI_print(s)

extern char WSCREEN[10000];

void ANSI_screen_init (void);
void ANSI_screen_close (void);
int ANSI_col_convert (int col);
void ANSI_do_color (unsigned char col);
void ANSI_do_char (unsigned char c);
void ANSI_pos (int x, int y);
void ANSI_save_pos (void);
void ANSI_restore_pos (void);

/* vim: set tabstop=2 softtabstop=0 shiftwidth=2 expandtab: */
