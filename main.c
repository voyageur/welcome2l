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
/*
 * September 1999, Robert Luberda's <robert@pingu.ii.uj.edu.pl> patch:
 * - added screens 4 and 5 (modified versions of 1 and 2);
 * - changed default screen to 4;
 * - added notime option;
 * - some other small changes.
 *
 * December 2002, Robert Luberda <robert@debian.org>
 * - added noblink option.
 *
 * February 2004, Robert Luberda <robert@debian.org>
 * - get memory info from /proc/meminfo
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <locale.h>
#include <time.h>
#include <ctype.h>

#include "version.h"
#include "ansi.h"
#include "ansi/welcome1.h"
#include "ansi/welcome2.h"
#include "ansi/xmas.h"


#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
#define NOTSEEN 2

#define NODENAME uname_buf.nodename
#define RELEASE uname_buf.release
#define MACHINE uname_buf.machine
#define TTY ttyname(0)+5



static struct utsname uname_buf;
static char MEMORY[15];
static char TIME[30];
static char WMESG[25];
static char CPUID[15];
static char CPU_INFO = FALSE;
static char want_getty_infos = FALSE;
static char THIS_SCREEN = 4;	/* default screen */
static char last_color = FALSE;
static char WANT_NEWLINE = TRUE;
static int NEW_YEAR = FALSE;
static char XMAS_AUTO = FALSE;
static char IS_WELCOME = TRUE;
static char NO_TIME = FALSE;
char NO_BLINK = FALSE;
char WSCREEN[10000];
char XMAS_SCREEN = 3;
#define MAX_RAND_SCREEN 4.0


/*
 * This function, added by Robert Luberda <robert@pingu.ii.uj.edu.pl>,
 * changes color of  'Welcome to:', 'CPU:', etc. texts.
 * It works only with screen 1 or screen 2.
 */
static void
change_text_colors (unsigned char *ansi_screen, int ansi_screen_length,
		     char color)
{
  register int i, f;
  int 	tab_val[4] = {1609, 2409, 2569, 2729};

  for (i = 0; i < 4; i ++)
  {
	if (tab_val[i] + 60 > ansi_screen_length) /* this should not happen */
		return;

	for (f = 0; f < 58; f += 2)
		ansi_screen[tab_val[i] + f] = color;
  }
}
/*
 * robert@debian.org, 2004
 * get memory info from /proc/meminfo, as stat-ing /proc/kcore
 * does not always give correct results (see Debian bug#230546)
 */
static void get_memory_info(void)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	long long int memsize = 0;
	char * units = NULL;

	if ((fp = fopen("/proc/meminfo", "r"))) {

		while ( getline(&line, &len, fp) > -1 ) {
			if (!strncmp(line, "MemTotal:", 9)) {
				memsize = strtoull(line + 9, NULL, 10);
				break; // while
			}
		}
		fclose(fp);
		if (line)
			free(line);
	}

	if (!memsize) {
		strncpy(MEMORY, "(unknown)", sizeof(MEMORY) - 1);
	        return;
	}

	if (memsize < 1000000) {
		units 	= "kB";
	} else if ((memsize = (memsize >> 10)) < 1000000) {
		units 	= "MB";
	} else if ((memsize = (memsize >> 10)) < 1000000) {
		units 	= "GB";
	} else {
		memsize = (memsize >> 10);
		units 	= "TB";
	}


	snprintf (MEMORY, sizeof(MEMORY), "%Ld %s", memsize, units);
}




static void
draw_ansi_screen (unsigned char *ansi_screen, int ansi_screen_length, int ansi_screen_width)
{
  register int f;
  int this_width = ansi_screen_width << 1;

  for (f = 0; f < ansi_screen_length; f += 2)
    {
      if (f)
	if ((f % this_width) == 0)
	  ANSI_do_char ('\n');
      ANSI_do_color (ansi_screen[f + 1]);
      ANSI_do_char (ansi_screen[f]);
    }
}



static void
draw_welcome_screen (unsigned char *screen, int screen_length, int screen_width,
		     char color1, char color2, char color3,
		     char xnode, char ynode, char xtime, char ytime,
		     char xtty, char ytty,
		     char xwmsg, char ywmsg, char xrel, char yrel,
		     char xmach, char ymach, char xmem, char ymem)
{
  ANSI_screen_init ();

  draw_ansi_screen (screen, screen_length, screen_width);
  ANSI_save_pos ();



  ANSI_do_color (color1);
  ANSI_printxy (xnode, ynode, want_getty_infos == 2 ? "@S" : NODENAME);


  ANSI_do_color (color2);
  if (xtty + ytty)
    {
      if (want_getty_infos || isatty (0))
	{
	  ANSI_printxy (xtty, ytty, "Login on ");

          ANSI_do_color (color1);
	  if (want_getty_infos)
	    ANSI_print (want_getty_infos == 1 ? "\\l" : "@L");
	  else
	    ANSI_print (TTY);
	}
    }


  ANSI_do_color (color2);
  if (xwmsg + ywmsg)
    {
      ANSI_printxy (xwmsg, ywmsg, WMESG);
    }

  if (want_getty_infos)
    {
      if (xtime + ytime)
	{
	  ANSI_do_color (color2);
	  if (NO_TIME == FALSE)
	  {
	     ANSI_printxy (xtime, ytime, want_getty_infos == 1 ? " \\d - \\t" : " @D - @T");
	  }
	  else
	  {
	     ANSI_printxy (xtime, ytime, want_getty_infos == 1 ? " \\d" : " @D");
	  }
	}
      if (xrel + yrel)
	{
	  ANSI_do_color (color3);
	  ANSI_printxy (xrel, yrel, want_getty_infos == 1 ? "\\r" : "@V");
	}
      if (xmach + ymach)
	{
	  ANSI_do_color (color3);
	  ANSI_printxy (xmach, ymach, CPU_INFO ? CPUID : (want_getty_infos == 1) ? "\\m" : MACHINE);
	}
    }
  else
    {
      if (xtime + ytime)
	{
	  ANSI_do_color (color2);
	  ANSI_printxy (xtime, ytime, TIME);
	}
      if (xrel + yrel)
	{
	  ANSI_do_color (color3);
	  ANSI_printxy (xrel, yrel, RELEASE);
	}
      if (xmach + ymach)
	{
	  ANSI_do_color (color3);
	  ANSI_printxy (xmach, ymach, CPU_INFO ? CPUID : MACHINE);
	}
    }

  if (xmem + ymem)
    ANSI_printxy (xmem, ymem, MEMORY);

}



static void
draw_this_screen (void)
{
  int last_line;

  switch (THIS_SCREEN)
    {
    case 1:
      {
	char buf[80];
	draw_welcome_screen (WELCOME1, WELCOME1_LENGTH, WELCOME1_WIDTH,
			     0x08, 0x08, 0x08,
			     18, 11, 7, 12, 7, 13,
			     7, 15, 16, 16, 16, 17, 16, 18);
	ANSI_do_color (0x19);
	sprintf (buf, "%s V%s (c)LittleIgloo.Org", PROG_NAME, PROG_VERSION);
	ANSI_printxy (0, last_line = 22, buf);
      }
      break;

    case 2:
      {
	char buf[80];
	draw_welcome_screen (WELCOME2, WELCOME2_LENGTH, WELCOME2_WIDTH,
			     0x09, 0x01, 0x09,
			     18, 11, 7, 12, 7, 13,
			     7, 15, 16, 16, 16, 17, 16, 18);
	ANSI_do_color (0x19);
	sprintf (buf, "%s %s (c)LittleIgloo.Org", PROG_NAME, PROG_VERSION);
	ANSI_printxy (0, last_line = 22, buf);
      }
      break;

    default:
    case 4:	/* R.L. */
      {
	char buf[80];
	change_text_colors(WELCOME1, WELCOME1_LENGTH, 0x2);
	draw_welcome_screen (WELCOME1, WELCOME1_LENGTH, WELCOME1_WIDTH,
			     0x0E, 0x0C, 0x09,
			     18, 11, 7, 12, 7, 13,
			     7, 15, 16, 16, 16, 17, 16, 18);
	ANSI_do_color (0x19);
	sprintf (buf, "%s V%s (c)LittleIgloo.Org", PROG_NAME, PROG_VERSION);
	ANSI_printxy (0, last_line = 22, buf);
      }
      break;

    case 5:	/* R.L. */
      {
	char buf[80];
	change_text_colors(WELCOME2, WELCOME2_LENGTH, 0x2);
	draw_welcome_screen (WELCOME2, WELCOME2_LENGTH, WELCOME2_WIDTH,
			     0x0E, 0x0C, 0x09,
			     18, 11, 7, 12, 7, 13,
			     7, 15, 16, 16, 16, 17, 16, 18);
	ANSI_do_color (0x19);
	sprintf (buf, "%s %s (c)LittleIgloo.Org", PROG_NAME, PROG_VERSION);
	ANSI_printxy (0, last_line = 22, buf);
      }
      break;


    case 3:
      draw_welcome_screen (XMAS, XMAS_LENGTH, XMAS_WIDTH,
			   0x04, 0x04, 0x04,
			   15, 12, 0, 0, 0, 0,
			   0, 0, 24, 14, 22, 15, 23, 16);
      ANSI_do_color (0x04);
      ANSI_printxy (18, 18, "We wish you a");

      if (!NEW_YEAR)
	{
	  ANSI_do_color (0x8F);
	  ANSI_printxy (16, 19, "MERRY CHRISTMAS");
	}
      else
	{
	  char what_year[50];
	  sprintf (what_year, "HAPPY NEW YEAR %i", NEW_YEAR);
/*	  ANSI_do_char ('n'); */
	  ANSI_do_color (0x8F);
	  ANSI_printxy (13, 19, what_year);
	}
      last_line = 23;
      break;
    }


  if (!IS_WELCOME)
    {
      ANSI_do_color (0x4E);
      ANSI_printxy (0, last_line, " *** Unauthorized access is PROHIBITED ** Unauthorized access is PROHIBITED *** ");
    }


  ANSI_restore_pos ();
  ANSI_screen_close ();

  printf ("%s", WSCREEN);
}



static void
prog_infos ()
{
  printf ("\033[H\033[J");
  printf ("%s%s V%s By %s\n%sCopyright (c)%s %s\n%sCompiled on %s %s by %s@%s (%s)\n%s\n",
	  "\033[1;33m", PROG_NAME, PROG_VERSION, PROG_AUTHOR,
	  "\033[36m", VERSION_DATE, LITTLEIGLOO,
	  "\033[0;36m", COMPILE_DATE, COMPILE_TIME, COMPILE_BY, COMPILE_HOST, CC_VERSION,
	  "\033[0;37mDistributed under the terms of the GNU General Public License.\033[m\n");
}



static void
help_usage (void)
{
  prog_infos ();
  printf ("ERROR: Unrecognized Argument.\nType : %s -help for usage information.\n\n", PROG_NAME);
  exit (1);
}


static void
print_option (char *this_option, char *this_option_arg, char *this_message)
{
  printf ("\033[0;36m  \033[1m-%s\033[1;37m%s\033[0;36m : %s\n",
	  this_option, this_option_arg, this_message);
}

static void
usage (void)
{
  prog_infos ();
  printf ("\033[1;36m");

  printf ("OPTIONS:\n\033[0;36m");
  print_option ("scr", "xx", "Display the screen number xx");
  print_option ("msg", "\"text of message\"", "Show the msg given as the distribution info");
  print_option ("getty", "", "Display more accurate information when launched at boot time\n  (via an init script).");
  print_option ("nolf", "", "Do not add an empty line to the ansi screen produced.");
  print_option ("lcol", "xx", "Change the prompt color to the color number xx");


  printf ("\033[0;37m            COLOR NUMBERS ARE: %s\n", "\033[0;34m1 \033[32m2 \033[36m3 \033[31m4 \033[35m5 \033[33m6 \033[37m7 \033[1;30m8 \033[34m9 \033[32m10 \033[36m11 \033[31m12 \033[35m13 \033[33m14 \033[37m15");
  print_option ("cpu", "\"string\"", "Specifies an alternate CPU description");
  print_option ("private", "", "Add an \"Unauthorized access is PROHIBITED\" line");
  print_option ("notime", "", "Do not dispaly current time");
  print_option ("noblink", "", "Turn blinking off");
  printf ("\n\033[1;36mEXAMPLE:\n\033[0;36m");
  printf ("  ./Welcome2L -getty -msg\"Gentoo Linux\" -lcol14");


  printf ("\033[m\n\n");
  exit (0);
}



int
main (int argc, char *argv[])
{
/*  struct stat stat_buf; */
  time_t timep;
  struct tm *timeptr;
  char SEE_ARG = FALSE;

  WMESG[0] = 0;

  if (argc > 1)
    SEE_ARG = NOTSEEN;
  while (argc--)
    {
      if (strstr (argv[argc], "-help"))
	usage ();

      if (strstr (argv[argc], "-msg"))
	{
	  strncpy (WMESG, argv[argc] + 4, sizeof (WMESG));
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-lcol"))
	{
	  last_color = atoi (argv[argc] + 5);
	  if (last_color > 15)
	    last_color = 15;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-nolf"))
	{
	  WANT_NEWLINE = FALSE;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-getty"))
	{
	  want_getty_infos = 1;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-gettps"))
	{
	  want_getty_infos = 2;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-scr"))
	{
	  char *ptr = (char *) argv[argc] + 4;
	  srand (time (0));
	  THIS_SCREEN = isdigit (*ptr) ? atoi (ptr) : (1 + (int) (MAX_RAND_SCREEN * rand () / (RAND_MAX + 1.0)));
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-xmasauto"))
	{
	  XMAS_AUTO = TRUE;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-xmas") && !XMAS_AUTO)
	{
/*	  printf ("here\n"); */
	  THIS_SCREEN = XMAS_SCREEN;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-cpu"))
	{
	  strncpy (CPUID, argv[argc] + 4, sizeof (CPUID));
	  CPU_INFO = TRUE;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-private"))
	{
	  IS_WELCOME = FALSE;
	  SEE_ARG = TRUE;
	}

      if (strstr (argv[argc], "-notime"))
	{
	  NO_TIME = TRUE;
	  SEE_ARG = TRUE;
	}
      if (strstr (argv[argc], "-noblink"))
	{
	  NO_BLINK = TRUE;
	  SEE_ARG = TRUE;
	}
    }
  if (SEE_ARG == NOTSEEN)
    help_usage ();


  setlocale (LC_ALL, "");

  uname (&uname_buf);

  if (strlen (NODENAME) > 15)
    *(strchr (NODENAME, '.')) = 0;

  timep = time (NULL);
  timeptr = localtime (&timep);
  if (NO_TIME == FALSE)
	strftime (TIME, sizeof (TIME), " %a %d %b %Y - %X", timeptr);
  else
	strftime (TIME, sizeof (TIME), " %a %d %b %Y", timeptr);


  if ((timeptr->tm_mon < 2) || (timeptr->tm_mon == 11 && timeptr->tm_mday > 25))
    NEW_YEAR = timeptr->tm_year + 1900 + (timeptr->tm_mon == 11 && timeptr->tm_mday > 25);

  if ((XMAS_AUTO) && ((timeptr->tm_mon == 0) || (timeptr->tm_mon == 11)))
    THIS_SCREEN = XMAS_SCREEN;		/* Ross Younger's patch (revised) - no Xmas decorations outside of Jan/Dec. (enable it with -xmasauto) */

/*
  stat ("/proc/kcore", &stat_buf);
  sprintf (MEMORY, "%ld kB", stat_buf.st_size >> 10);
*/
  get_memory_info();


  draw_this_screen ();

  if (last_color)
    {
      char attr = 0;
      if (last_color > 8)
	{
	  attr++;
	  last_color -= 8;
	}
      printf ("\033[%i;3%im", attr, ANSI_col_convert (last_color));
    }

  if (WANT_NEWLINE)
    printf ("\n");

  return 0;
}
