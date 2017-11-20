#include <stdio.h>
#include <windows.h>

#define HELP_INFO "\
Tiny CurS Tool

Arguments:
/pos x y        Set Position(x,y) of Cursor
/crv n          Set Visible(0 or 1) of Cursor
/crh n          Set Height(1 to 100) of Cursor
/crx            Return X of Cursor Position
/cry            Return Y of Cursor Position

You can also use more command.

  >TCurS /pos 1 5 /crv 1 /crh 50

The ERRORLEVEL will be set by the last command.
"

HANDLE Hstdout;
CONSOLE_CURSOR_INFO CurInfo;
COORD mosPos={0,0};

int pos(int argc,char *argv[]);
int crv(int argc,char *argv[]);
int crh(int argc,char *argv[]);
int crx(int argc,char *argv[]);
int cry(int argc,char *argv[]);
void GetMosPos(COORD *out);
int atoi_s(char *in,int *out);

struct funcname {
 char *name;
 int (*func) (int argc,char *argv[]);
} func[5] = {
 "pos",&pos,
 "crv",&crv,
 "crh",&crh,
 "crx",&crx,
 "cry",&cry
};

char *errType[2]={
 "TCurS:无效参数 '%s'\n",
 "TCurS:参数不够\n"
};

#define ARG_ERROR errType[0]
#define ARG_LESS errType[1]

int main(int argc,char *argv[]){
 if(argc==2 && strcmp(argv[1],"/?") == 0){
  printf(HELP_INFO);
  return 0;
 }

 Hstdout= GetStdHandle(-11);

 int returnval=0;
 int (*Pfunc) (int argc,char *argv[]);

 int n=1,m,c;
 char **v;
 while(n<argc){
  if(argv[n][0] != '/'){
   fprintf(stderr,ARG_ERROR,argv[n]);
   n++;
   continue;
  } else {
   Pfunc=NULL;
   for(m=0;m<5;m++){
    if(stricmp(argv[n]+1,func[m].name) == 0){
     Pfunc=func[m].func;
   }}
   if(Pfunc==NULL){
    fprintf(stderr,ARG_ERROR,argv[n]);
    n++;
    continue;
  }}
  v=argv + n; /* &argv[n] */
  n++;
  c=1;
  while(n<argc && argv[n][0] != '/'){
   c++;n++;
  }
  returnval = (*Pfunc) (c,v);
 }
 return returnval;
}

int pos(int argc,char *argv[]){
 int Pos[2],n;

 if(argc<3){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( (n=aatoia(2,&argv[1],Pos)) != -1 ){
  fprintf(stderr,ARG_ERROR,argv[1+n]);
  return 255;
 }

 mosPos.X=Pos[0];
 mosPos.Y=Pos[1];
 return SetConsoleCursorPosition(Hstdout,mosPos);
}

int crv(int argc,char *argv[]){
 int bVisible;
 if(argc<2){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( atoi_s(argv[1],&bVisible)!=0 ){
  fprintf(stderr,ARG_ERROR,argv[1]);
  return 255;
 }
 GetConsoleCursorInfo(Hstdout,&CurInfo);
 CurInfo.bVisible = bVisible;
 return SetConsoleCursorInfo(Hstdout,&CurInfo);
}

int crh(int argc,char *argv[]){
 int alpha;
 if(argc<2){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( atoi_s(argv[1],&alpha)!=0 ){
  fprintf(stderr,ARG_ERROR,argv[1]);
  return 255;
 }
 GetConsoleCursorInfo(Hstdout,&CurInfo);
 CurInfo.dwSize = alpha;
 return SetConsoleCursorInfo(Hstdout,&CurInfo);
}

int crx(int argc,char *argv[]){
 COORD pos;
 GetMosPos(&pos);
 return pos.X;
}

int cry(int argc,char *argv[]){
 COORD pos;
 GetMosPos(&pos);
 return pos.Y;
}

void GetMosPos(COORD *out){
 CONSOLE_SCREEN_BUFFER_INFO csbi;
 GetConsoleScreenBufferInfo(Hstdout,&csbi);
 out->X=csbi.dwCursorPosition.X;
 out->Y=csbi.dwCursorPosition.Y;
}

int atoi_s(char *in,int *out){ // return 0:正常 1:不是数字
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

int aatoia(int n,char *in[],int out[]){// return 出错的个数(第几个,从0开始),-1:正常
 int i;
 for(i=0;i<n;i++){
  if(atoi_s(in[i],&out[i])!=0){
   return i;
 }}
 return -1;
}