#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include <dos.h>
#include <conio.h>

/*
void main()
{
 unsigned int i = 0;
 int x,y;
 char ch;
 clrscr();
 while(1)
 {
  if(kbhit())
  {
	 ch = getch();
	if(ch == 'x')
	{
		printf("\n\napp 4 terminate");
		return ;
	}
  }

   x = wherex();
   y = wherey();

   gotoxy( 20,2);
   if( (i%2) == 0 )
   {
	 cprintf( " Run App-4 (x) " );
   }
   else
   {
	 cprintf( "          " );
   }

   gotoxy(x,y);

   delay(100);
   i++;
  }
}

*/

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
	if(ch == 'k')
	{
		printf("\n\napp 4 terminate");
		return ;
	}
  }

   x = wherex();
   y = wherey();

   gotoxy( 1,12);
   for(j=0; j < 10 ; j++)
   {
	 gotoxy( 50,12+j );
	 //textattr( rand() );
	 printf( "Run App 4 (k) (%x) : %d", _CS, rand()%1000 );
   }

   gotoxy(x,y);

   //delay(100);
   for(j=0; j < 9999;j++);

   i++;
  }
}


