#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include <dos.h>
#include <conio.h>


void main()
{
 unsigned int i = 0;
 int j = 0;
 int x,y;
 char ch;
// clrscr();
 while(1)
 {
  if(kbhit())
  {
	 ch = getch();
	if(ch == 'x')
	{
		printf("\n\napp 3 terminate");
		return ;
	}
  }

   x = wherex();
   y = wherey();

   gotoxy( 1,12);
   for(j=0; j < 10 ; j++)
   {
	 gotoxy( 1,12+j );
	 //textattr( rand() );
	 printf( "Run App 3 x (%x) :(%d)", _CS, rand()%1000 );
   }

   gotoxy(x,y);

   for(j=0; j < 9999;j++);
   //delay(100);
   i++;
  }
}

