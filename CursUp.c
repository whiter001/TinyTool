#include <windows.h>
HANDLE h;
CONSOLE_CURSOR_INFO CurInfo={1,0};
COORD mosPos={0,0};

void GetMosPos(COORD *out){
 CONSOLE_SCREEN_BUFFER_INFO csbi;
 GetConsoleScreenBufferInfo(h,&csbi);
 out->X=csbi.dwCursorPosition.X;
 out->Y=csbi.dwCursorPosition.Y;
}

void Hide(COORD pos){
 int n;short m;
 ReadConsoleOutputAttribute(h,&m,1,pos,&n);
 FillConsoleOutputAttribute(h,m&0xF7FF,1,pos,&n);
}

void Show(COORD pos){
 int n;short m;
 ReadConsoleOutputAttribute(h,&m,1,pos,&n);
 FillConsoleOutputAttribute(h,m|0x0800,1,pos,&n);
}

int main(int argc,char *argv[]){
 int c;
 h=GetStdHandle(-11);
 while(1){
  SetConsoleCursorInfo(h,&CurInfo);
  sleep(1);
  Hide(mosPos);
  GetMosPos(&mosPos);
  if(c>5){
   Show(mosPos);
  }
  c=c<10?c+1:0;
  sleep(1);
}}