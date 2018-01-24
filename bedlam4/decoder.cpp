#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char cube[4][4][4];

void fill_cube(int name, long exp)
{
  for (int x=0; x<4; x++)
    for (int y=0; y<4; y++)
      for (int z=0; z<4; z++)
	if (exp & (1L<<(x*16+y*4+z)))
	  cube[x][y][z] = name;
}

void printspaces(int n)
{ for (int i=0; i<n; i++) printf(" "); }

int main(int arv, char** argv)
{
  long exps[13]=
  {
    0x0000000000110003,
    0x00600c4000000000,
    0x000080008c008000,
    0x088c008000000000,
    0x00000008004c0004,
    0x3100010001000000,
    0x0011003000200000,
    0x0600020022000000,
    0x00000000008008c8,
    0x0000100010003100,
    0x0000000000004630,
    0xc000600040000000,
    0x0002000700020000
  };

  for (int i=0; i<13; i++)
    fill_cube(i+1, exps[i]);

  for (short x=0; x<4; x++)
  {
    printspaces(x*6);
    printf("layer %d%s\n", x, x==0?" (front)":x==3?" (back)":"");
    for (short y=0; y<4; y++)
    {
      printspaces(x*6);
      for (short z=0; z<4; z++)
	printf("% 3d", (int) (cube[x][y][z]));
      printf("\n");
    }
    printf("\n");
  }

  return 0;
}
