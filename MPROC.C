/*
 writen by Apirak J.
 Simple task schedule ".COM" file
 OS: DOS
 Compiler : Borland C++ v3.x
*/
#include <stdio.h>
#include <stdlib.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <alloc.h>
#include <conio.h>

struct processInfo
{
	int pid;
	struct 	processInfo far *next;

	/* loader info */
	unsigned c_ax,c_bx,c_cx,c_dx;
	unsigned c_si,c_di,c_bp,c_sp,c_ip;
	unsigned c_es,c_ds,c_ss,c_cs;
	unsigned c_flags;

	unsigned int 	imagesize;
	int 			num_paragraph;
	unsigned int 	base_addr;
};


struct processInfo far *pfReadyQ    = NULL;
struct processInfo far *pfRunningQ  = NULL;

#define  MAX_STK	(1024*6)
unsigned mystack[MAX_STK];
unsigned endstk;


struct processInfo far *pCurrentRunning=NULL;
struct processInfo far *pNextRunning=NULL;


void far interrupt  (*oldInt_8h)(void);

unsigned c_ax,c_bx,c_cx,c_dx;
unsigned c_si,c_di,c_bp,c_sp;
unsigned c_cs,c_es,c_ds,c_ss;
unsigned c_ip,c_flags;


//backup register
unsigned s_ax,s_bx,s_cx,s_dx;
unsigned s_si,s_di,s_bp,s_sp;
unsigned s_cs,s_es,s_ds,s_ss;
unsigned s_ip,s_flags;


unsigned RunApp[10];
int		 iNumRunApp=0;

int addRunApp(unsigned seg)
{
	RunApp[ iNumRunApp ] = seg;
	iNumRunApp++;
	return iNumRunApp;
}


struct processInfo far *newPCB(void)
{
	static int pidCount=0;
	struct processInfo far *p;
	p = (struct processInfo far *)malloc(sizeof(struct processInfo ));
	_fmemset( p, 0 , sizeof( struct processInfo ));
	p->next = NULL;
	p->pid  = pidCount++;
	return p;
}

void AddTailProcessList(struct processInfo far **pfHead ,
						struct processInfo far *p)
{
	struct processInfo far *pf= *pfHead;

	//add at tail
	if(pf==NULL) {
		*pfHead = p;
		(*pfHead)->next = NULL;
	}
	else
	{
		while(pf->next !=NULL )
		{
			pf = pf->next;
		}
		pf->next = p;
		p->next = NULL;
	}

}

struct processInfo far *getNextProcessRunning(void)
{
	struct processInfo far *pf= pfReadyQ;

	pfReadyQ = pfReadyQ->next;

	pf->next = NULL;

	return pf;
}


struct processInfo far * getRuningProcessPCB( unsigned int P_CS )
{
	struct processInfo far *pf= pfRunningQ;

	while(pf)
	{
		if( pf->base_addr == P_CS )
		{
			return pf;
		}
		pf = pf->next;
	}
	return NULL;
}


struct processInfo far * RemoveProcessQ(
		struct processInfo far **pfHead ,
		struct processInfo far * pCurrentRunning )
{
	struct processInfo far *p= *pfHead;
	struct processInfo far *prv = NULL;

	while(p)
	{
		if( p == pCurrentRunning)
		{
			if(prv == NULL)
			{	//remove header
				(*pfHead) = p->next;
			}
			else
			{
				prv->next = p->next;
			}
			pCurrentRunning->next = NULL;
			return pCurrentRunning;
		}
		prv = p;
		p = p->next;
	}
	return NULL;
}

#define _BP_	0
#define _DI_	2
#define _SI_	4
#define _DS_	6
#define _ES_	8
#define _DX_	10
#define _CX_	12
#define _BX_	14
#define _AX_	16
#define _IP_	18
#define _CS_	20
#define _FLAGS_	22


int int8hActive=0;
void far interrupt active_workInt8h(
		unsigned BP_8, //[bp]
		unsigned DI_8, //[bp+2]
		unsigned SI_8, //[bp+4]
		unsigned DS_8, //[bp+6]
		unsigned ES_8, //[bp+8]
		unsigned DX_8, //[bp+10]
		unsigned CX_8, //[bp+12]
		unsigned BX_8, //[bp+14]
		unsigned AX_8, //[bp+16]
		unsigned IP_8, //[bp+18]
		unsigned CS_8, //[bp+20]
		unsigned FLAGS_8 //[bp+22]
)
{
	_asm cli
	_asm cmp word ptr cs:int8hActive,1
	_asm je  L01
	_asm jmp L02
	//if( (int8hActive==1) )
	{
  L01:;

		//call orignal function
		asm pop bp
		asm pop di
		asm	pop	si
		asm pop ds
		asm pop es
		asm	pop dx
		asm pop cx
		asm pop bx
		asm pop ax
		asm jmp dword ptr cs:cs:oldInt_8h
		return ;
	}

L02:;

	 asm mov  word ptr cs:int8hActive,1

	_asm  mov	ax,word ptr ss:[bp + _FLAGS_]
	_asm  mov	word ptr cs:s_flags,ax

  /* 
	for( iCntRun = 0 ; iCntRun < iNumRunApp ;iCntRun++)
	{
		if( RunApp[iCntRun] == CS_8)
		{
			goto L05;
		}
	}
	asm mov  cs:int8hActive,0
	goto L03;
	L05:;
  */

	_asm mov	cx,0
L10450:;
	_asm mov	bx,cx
	_asm shl	bx,1

	_asm mov	ax,word ptr cs:RunApp[bx]
							
	_asm cmp	ax,word ptr ss:[bp+_CS_]
	_asm jne	short L10451;

	_asm jmp	short L05

L10451:;
	_asm inc	cx
	_asm cmp	cx,word ptr cs:iNumRunApp
	_asm jl		short L10450

	//case the entrance process is not in mylist
	_asm mov	word ptr cs:int8hActive,0
	_asm jmp	short L01

L05:;

	_asm mov cs:s_ss,ss
	_asm mov cs:s_sp,bp
	_asm mov ax,word ptr ss:[bp+_BP_]
	_asm mov cs:s_bp,ax

	_asm mov ax,word ptr ss:[bp+_DI_]
	_asm mov cs:s_di,ax

	_asm mov ax,word ptr ss:[bp+_SI_]
	_asm mov cs:s_si,ax

	_asm mov ax,word ptr ss:[bp+_DS_]
	_asm mov cs:s_ds,ax
	_asm mov ax,word ptr ss:[bp+_ES_]
	_asm mov cs:s_es,es

	_asm mov ax,word ptr ss:[bp+_DX_]
	_asm mov cs:s_dx,ax

	_asm mov ax,word ptr ss:[bp+_CX_]
	_asm mov cs:s_cx,ax

	_asm mov ax,word ptr ss:[bp+_BX_]
	_asm mov cs:s_bx,ax

	_asm mov ax,word ptr ss:[bp+_AX_]
	_asm mov cs:s_ax,ax

	_asm mov ax,word ptr ss:[bp+_IP_]
	_asm mov cs:s_ip,ax
	_asm mov ax,word ptr ss:[bp+_CS_]
	_asm mov cs:s_cs,ax


	//set DS,ES,SS to CS
	_asm push cs
	_asm pop  es

	_asm push cs
	_asm pop  ds

	//--
	_asm push cs
	_asm pop  ss
	_asm mov  sp,cs:endstk
	_asm mov  bp,sp
	//--

	(*oldInt_8h)();

	_asm cli

  #define _CLEAR_C_STACK_
  #ifdef _CLEAR_C_STACK_
	//clear C stack 
	//_asm cli
	_asm mov ss, cs:s_ss
   //	_asm mov bp, cs:s_bp
	_asm mov sp, cs:s_sp

	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax
	_asm pop 	ax

	_asm pop 	ax	//pop ip
	_asm pop 	ax 	//pop cs
	_asm pop 	ax  //popf

	_asm mov ax, sp
   //	_asm mov cs:s_bp,ax
	_asm mov cs:s_sp,ax
	//------------- end clear c stack
#endif

	//set stack
	_asm push cs
	_asm pop  es

	_asm push cs
	_asm pop  ds

	_asm push cs
	_asm pop  ss

	_asm mov  sp,cs:endstk
	_asm mov  bp,sp


	pCurrentRunning = getRuningProcessPCB( s_cs  );
	
	//remove current process from running quence
	RemoveProcessQ( &pfRunningQ, pCurrentRunning );

		//my process in list  (backup register values)
		pCurrentRunning->c_bp = s_bp;
		pCurrentRunning->c_sp = s_sp;

		pCurrentRunning->c_di = s_di,
		pCurrentRunning->c_si = s_si,

		pCurrentRunning->c_ds = s_ds;
		pCurrentRunning->c_es = s_es;
		pCurrentRunning->c_cs = s_cs;
		pCurrentRunning->c_ss = s_ss;

		pCurrentRunning->c_dx = s_dx;
		pCurrentRunning->c_cx = s_cx;
		pCurrentRunning->c_bx = s_bx;
		pCurrentRunning->c_ax = s_ax;

		pCurrentRunning->c_ip = s_ip;
		pCurrentRunning->c_cs = s_cs;

		pCurrentRunning->c_flags = s_flags;

	
		AddTailProcessList( &pfReadyQ, pCurrentRunning  );

		pNextRunning = getNextProcessRunning();

		AddTailProcessList( &pfRunningQ, pNextRunning   );

		//restore register values
		c_bp = pNextRunning->c_bp;
		c_sp = pNextRunning->c_sp;
		c_ss = pNextRunning->c_ss;

		c_di = pNextRunning->c_di;
		c_si = pNextRunning->c_si;

		c_ax = pNextRunning->c_ax;
		c_bx = pNextRunning->c_bx;
		c_cx = pNextRunning->c_cx;
		c_dx = pNextRunning->c_dx;

		c_es = pNextRunning->c_es;
		c_ds = pNextRunning->c_ds;

		c_cs = pNextRunning->c_cs;
		c_ip = pNextRunning->c_ip;

		c_flags = pNextRunning->c_flags;


		//switch address
		_asm mov ss, cs:c_ss
		_asm mov bp, cs:c_bp
		_asm mov sp, cs:c_sp

		_asm mov si, cs:c_si
		_asm mov di, cs:c_di

		_asm mov es, cs:c_es
		_asm mov ds, cs:c_ds

		_asm mov ax,cs:c_flags
		_asm push ax
		_asm mov ax,cs:c_cs
		_asm push ax
		_asm mov ax,cs:c_ip
		_asm push ax

		_asm mov ax, cs:c_ax
		_asm mov bx, cs:c_bx
		_asm mov cx, cs:c_cx
		_asm mov dx, cs:c_dx

		_asm mov word ptr cs:int8hActive,0;

		_asm sti
		_asm iret


}


int loadApp( char *appfname, int appimagesize ,unsigned char far *appimage )
{
	int res;
	unsigned segProcessAppAddr;
	char far *ptrProcessAppAddr;
	char far *firstInstrProcessApp;

	struct processInfo far *pf = (struct processInfo far *) newPCB();
  /*
	allocate memory for app
   unsigned _dos_allocmem(unsigned size, unsigned *segp);

	size ³ The number of 16-byte paragraphs requested
	segp ³ Pointer to a word that will be assigned the

   segment address of the newly allocated block
   On success,
	 _dos_allocmem returns 0
  */

	int num_para = (appimagesize+15+0x100)/16;
	//printf("\nallocate memory for app  size = %d paragraph (%d byte)" ,num_para,appimagesize);

	res = _dos_allocmem( num_para , &segProcessAppAddr );
	if(res != 0)
	{
	printf("\ncannot allocate memory for run app  error : %d",res);
	}

//	printf("\nload app into memory at segment no. : %x", segProcessAppAddr);
//	printf("\n\n\n");

	firstInstrProcessApp = (char far*) MK_FP(segProcessAppAddr, 0x0100);
	_fmemcpy( (char far*)firstInstrProcessApp ,
			(char far*)appimage,
			appimagesize);

	pf->imagesize = appimagesize;
	pf->num_paragraph = num_para;
	pf->base_addr = segProcessAppAddr;
	pf->next = NULL;


	pf->c_bp = 0x0000; //0xFFFE;
	pf->c_sp = 0xFFFE;
	pf->c_si = 0x0;
	pf->c_di = 0x0;

	pf->c_ds = pf->base_addr;
	pf->c_es = pf->base_addr;
	pf->c_cs = pf->base_addr;
	pf->c_ss = pf->base_addr;

	pf->c_ax = 0;
	pf->c_bx = 0;
	pf->c_cx = 0;
	pf->c_dx = 0;

	pf->c_ip = 0x100;
	pf->c_flags = 0x00; //0x02;

	AddTailProcessList( &pfReadyQ, pf );
	addRunApp( pf->base_addr );

	return 0;
}


int startRun( char *appfname )
{
	FILE *fp;
	unsigned char *appimage=NULL;
	int appimagesize;
	fp = fopen( appfname , "rb");
	if(fp==NULL) {
		printf("\ncannot open file : %s",appfname);
		return -1;
	}
	appimage = (unsigned char far*) malloc(1024*20);
	appimagesize = fread(	(unsigned char*)appimage,
							sizeof(unsigned char),
							1024*20,
							fp);
	//printf( " \nread app file : %s  (size %d)",appfname,appimagesize );
	fclose(fp);

	//printf( " \nstart  app file : %s ",appfname );

	//
	loadApp( appfname, appimagesize , (unsigned char far*)appimage );

	free(appimage);
	return 0;
}

void createParentProcess( void )
{
	struct processInfo far *pf = (struct processInfo far *) newPCB();
	
	_asm push cs
	_asm pop  ax
	_asm mov  cs:c_cs,ax;
	
	pf->base_addr = c_cs;
	AddTailProcessList( &pfRunningQ, pf   );
	addRunApp( c_cs );
	
	endstk = (unsigned ) &mystack[MAX_STK];
}


#define INT_8H		0x08

void init( void )
{
	oldInt_8h = getvect( INT_8H ); 			/* save old keyboard interrupt */
	setvect( INT_8H , active_workInt8h); 	/* set new interrupt  */
}


void deinit(void)
{
	setvect( INT_8H ,oldInt_8h); /* set new interrupt  */
}


int main(int argc,char *argv[])
{
	char c;
	int x,y;
	int i,j;

	clrscr();
	printf( " \nstart  app file (input 'z' to terminate: \n");

	
	createParentProcess();

	startRun( "app3.com" );
	startRun( "app4.com" );
	
	init();

	while(1)
	{
		if(kbhit())
		{
			c = getch();
			if(c == 'j')
			{
				printf("\n\napp maltask1 terminate");
				goto app_exit;
			}
		}

		x = wherex();
		y = wherey();

		gotoxy( 1,2);
		for(j=0; j < 10 ; j++)
		{
			gotoxy( 25,1+j );
			printf("Run multask (j) : %d", rand()%1000 );
		}

		gotoxy(x,y);

		//delay
		for(j = 0 ; j < 9999 ; j++);

		i++;
  }

app_exit:;
	deinit();

	return 0;
}
