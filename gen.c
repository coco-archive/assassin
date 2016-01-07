/********************************
 *				*
 *   Assassin - 6809 assembler	*
 *				*
 *     Brian Windrim 6/11/88	*
 *				*
 *    gen.c - function module	*
 *				*
 ********************************/

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

#include "const.h"
#include "typedefs.h"
#include "err.h"
#include "globals.h"
#include "gen.h"

byte genbuffer [GEN_BUFF_SIZE];
byte_ptr	genptr;

static FILE    *gen_file;


void	gen_init(address)
unsigned	address;
{
  load_addr =
  exe_addr  =
  genaddr   = address;
  genptr    = genbuffer;
}

void	gen_org (address)
unsigned	address;
{
  if (genaddr == load_addr)
    {
      load_addr =
      exe_addr  = address;
    }

  genaddr = address;
  genptr  = genbuffer;
}

void gen_exec (address)
unsigned address;
{
    error = noerror;

    if (load_addr == exe_addr)
	exe_addr = address;
    else
	error = exeadderr;
}

void	genbyte (data)
int	data;
{
  if (data <= 255 && data >= -128)
    *genptr++ = data;
  else
    error = byterrstr;
}

void	genword (data)
int	data;
{
  *genptr++ = data >> 8;
  *genptr++ = data;
}

void	genbytebyte (data1, data2)
byte	data1, data2;
{
  genbyte (data1);
  genbyte (data2);
}

void	genbyteword (data1, data2)
byte	data1;
int	data2;
{
  genbyte (data1);
  genword (data2);
}

void	genwordword (data1, data2)
int	data1, data2;
{
  genword (data1);
  genword (data2);
}

void	genstring (ptr)
	char	*ptr;
{
  if (NULL == ptr)
    error = badstrerr;
  else
    while (*ptr)
      *genptr++ = *ptr++;
}

void	dumpcode ()
{
  static int length;

  if (length = (genptr - genbuffer))
    {
      if (!gen)
        code_length += length;
      else
	if (-1 == fwrite (genbuffer, 1, length, gen_file))
          error = outerrstr;

      genaddr += length;
      genptr = genbuffer;
    }
}

void	flushcode ()
{
  if (1 < pass &&
      (symerrstr == error ||
       deferrstr == error))
    {
      if (!gen)
        code_length += genptr - genbuffer;

      genaddr += genptr - genbuffer;
    }

  genptr = genbuffer;
}

void	fputmw (fp, word)
FILE	*fp;
unsigned	word;
{
  putc (word >> 8, fp);
  putc (word & 0xff, fp);
}

void	gen_open (filename)
char	*filename;
{
  char	 buffer [20];
  static char	*ptr;

  ptr = buffer;

  while (*filename != '.' && *filename != NUL)
    *ptr++ = *filename++;

  *ptr = NUL;

  strcat (buffer, ".ex9");

  if (NULL == (gen_file = fopen (buffer, "wb")))
    {
      fprintf (stderr, filerrstr, buffer, "writing");
      exit (1);
    }

  fputmw (gen_file, load_addr);
  fputmw (gen_file, code_length);
}

void gen_close ()
{
    fputmw (gen_file, exe_addr);
    fclose (gen_file);
}

void	dumpblock ()
{

  if (!gen)
    {
      code_length += nextaddr - genaddr;
      genaddr = nextaddr;
    }
  else
    while (genaddr < nextaddr)
      {
	putc (0, gen_file);
	++genaddr;
      }
}
