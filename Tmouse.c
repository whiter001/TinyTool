#include <stdio.h>
#include <windows.h>

#define HELP_INFO "\
Tiny mouse Tool

�÷�:
Tmouse c t l
Tmouse c t l X Y
Tmouse c t l Ret

c        : ��갴��(0-��,1-��,2-��,3-��)
t        : ʱ������ (0-������)
l        : ��������ڴ�����(0-��,����-��)
Ret      : ��ɺ��귵��
[X Y]    : ��ɺ�����ת��(X,Y)
"

const char* errType1="Tmouse: ��Ч���� '%s'\n";

HANDLE Hstdin,Hstdout;
HANDLE HTtimeout=0,HTclipmos=0;

int atoi_s(char *in,int *out);
void TmosEND(int ret);
void GetMosPos(COORD *out);
void timeout(void *arg);
void clipmos(void *Nothing);

int oldConMode;
CONSOLE_CURSOR_INFO oldCurInfo;
CONSOLE_CURSOR_INFO newCurInfo={100,1};

COORD RetmosPos={-1,-1};
COORD mosPos={0,0};
int mosPosval;
int temp;

int mosKey,timer,IsclipMos;

/* ***************************************************** */

int main(int argc,char* argv[]){
 Hstdin  = GetStdHandle(-10);
 Hstdout = GetStdHandle(-11);

 int n;
 INPUT_RECORD ir;

 if(argc==2 && strcmp(argv[1],"/?") == 0){
  printf(HELP_INFO);
  return 0;
 }

 if(argc<4){
  fprintf(stderr,"Tmouse: ��������. ����'Tmouse /?'�Ի�ȡ����.\n");
  return 255;
 }

 if(strlen(argv[1])!=1 || argv[1][0] < '0' || argv[1][0] > '3'){
  fprintf(stderr,errType1,argv[1]);
  return 255;
 } else {
  mosKey = argv[1][0] - '0';
 }

 if(atoi_s(argv[2],&timer)!=0){
  fprintf(stderr,errType1,argv[2]);
  return 255;
 }

 IsclipMos = strcmp(argv[3],"0")==0 ? 0 : 1;

 if(argc>=5 && stricmp(argv[4],"Ret") == 0){
  GetMosPos(&RetmosPos);
 } else {
  if(argc>=6){
   if(atoi_s(argv[4],&temp)!=0){
    fprintf(stderr,errType1,argv[4]);
    return 255;
   }
   RetmosPos.X=(short)temp;

   if(atoi_s(argv[5],&temp)!=0){
    fprintf(stderr,errType1,argv[5]);
    return 255;
   }
   RetmosPos.Y=(short)temp;
 }}

 GetConsoleMode(Hstdin,&oldConMode); // ����
 GetConsoleCursorInfo(Hstdout,&oldCurInfo);
 SetConsoleMode(Hstdin,oldConMode | ENABLE_MOUSE_INPUT);
 SetConsoleCursorInfo(Hstdout,&newCurInfo);

 if(timer>=0){
  HTtimeout=(HANDLE)_beginthreadex(NULL,0,(unsigned(__stdcall *)(void *))timeout,&timer,0,NULL);
 }
 if(IsclipMos==0){
  HTclipmos=(HANDLE)_beginthreadex(NULL,0,(unsigned(__stdcall *)(void *))clipmos,NULL,0,NULL);
 }

 while(1){
  ReadConsoleInput(Hstdin,&ir,1,&temp); //temp != NULL
  if(ir.EventType == MOUSE_EVENT){
   mosPos.X=ir.Event.MouseEvent.dwMousePosition.X;
   mosPos.Y=ir.Event.MouseEvent.dwMousePosition.Y;
   SetConsoleCursorPosition(Hstdout,mosPos);
   mosPosval = (mosPos.X+1) * 1000 + (mosPos.Y+1); // ����ֵ

   if(ir.Event.MouseEvent.dwEventFlags == 0){
    temp=ir.Event.MouseEvent.dwButtonState;
    switch(mosKey){
     case 0:
     if((temp & FROM_LEFT_1ST_BUTTON_PRESSED) != 0)goto key_arrived;
     break;

     case 1:
     if((temp & RIGHTMOST_BUTTON_PRESSED) != 0)goto key_arrived;
     break;

     case 2:
     if((temp & FROM_LEFT_2ND_BUTTON_PRESSED) != 0)goto key_arrived;
 }}}}

 key_arrived:
 TmosEND(mosPosval);
}

void TmosEND(int ret){
 if(HTtimeout!=0)CloseHandle(HTtimeout);
 if(HTclipmos!=0)CloseHandle(HTclipmos);

 SetConsoleMode(Hstdin,oldConMode); // ��ԭ
 SetConsoleCursorInfo(Hstdout,&oldCurInfo);

 if(RetmosPos.X>=0 && RetmosPos.X>=0){
  SetConsoleCursorPosition(Hstdout,RetmosPos);
 }
 exit(ret);
}

void GetMosPos(COORD *out){
 CONSOLE_SCREEN_BUFFER_INFO csbi;
 GetConsoleScreenBufferInfo(Hstdout,&csbi);
 out->X=csbi.dwCursorPosition.X;
 out->Y=csbi.dwCursorPosition.Y;
}

void timeout(void *arg){
 sleep(*(int*)arg * 1000);
 TmosEND(-mosPosval); // ȡ�෴��
}

void clipmos(void *Nothing){ //��(Hou)��(Yan)��(Wu)Ч(Chi)��ClipMos�߳�
 RECT WinRect;
 HWND Hcons=GetConsoleWindow();
 POINT mosPos;
 char change='N'; // �������λ��
 while(1){
  sleep(1);
  GetWindowRect(Hcons,&WinRect);
  GetCursorPos(&mosPos);
  if(mosPos.x < WinRect.left){ // �������ڷ�Χ?
   mosPos.x = WinRect.left;
   change='Y';
  }
  if(mosPos.x > WinRect.right){
   mosPos.x = WinRect.right;
   change='Y';
  }
  if(mosPos.y < WinRect.top){
   mosPos.y = WinRect.top;
   change='Y';
  }
  if(mosPos.y > WinRect.bottom){
   mosPos.y = WinRect.bottom;
   change='Y';
  }
  if(change=='Y'){
   SetCursorPos(mosPos.x,mosPos.y);
   change='N';
}}}

int atoi_s(char *in,int *out){ // return 0:���� 1:��������
 char negative=1;
 *out = 0;
 if (*in == '-') {negative=-1;in++;}
 if (*in == '\0') return 1;
 while(*in != '\0' && '0' <= *in && *in <= '9'){
  *out = *out * 10 + (*in - '0');
  in++;
 }
 while(*in == ' ') in++;
 if (*in != '\0') return 1;
 *out *= negative;
 return 0;
}

int aatoia(int n,char *in[],int *out[]){// return ����ĸ���(�ڼ���,��0��ʼ),-1:����
 int i;
 for(i=0;i<n;i++){
  if(atoi_s(in[i],out[i])!=0){
   return i;
 }}
 return -1;
}