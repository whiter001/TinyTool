#include <stdio.h>
#include <windows.h>

#define HELP_INFO "\
Tiny Color Tool

Arguments:
/line X Y L F B S       Color Text
/rect X Y W H F B S     Color Rect
/color F B S            Change Color

Parameters:
X :	Position X
Y :	Position Y
L :	Text Length
W :	Rect Width
H :	Rect Height
F :	Foreground Color
B :	Background Color
S :	Special Color (Format Attribute)

Colors(F&B):
0=Black   8=Grey
1=Blue    9=LBlue
2=Green   10=LGreen
3=GBlue   11=LGBlue
4=Red     12=LRed
5=Purple  13=LPurple
6=Yellow  14=LYellow
7=White   15=LWhite

Colors(S):
0=None
1=Horizontal Line
2=Left Vertical Line
4=Right Vertical Line
32=Underscore Line

2 + 32 = 34 = Left Vertical Line & Underscore Line.

You can also use more command.

  >TCol /rect 2 2 6 3 7 10 0 /rect 16 8 6 3 5 10 0

The ERRORLEVEL will be set by the last command.
"

#define ARGS_TO_COLOR (short)(args.F&0xF)+((args.B&0xF)<<4)+((args.S&0x3F)<<10)

int line(int argc,char *argv[]);
int rect(int argc,char *argv[]);
int color(int argc,char *argv[]);
int atoi_s(char *in,int out[]);

struct funcname {
 char *name;
 int (*func) (int argc,char *argv[]);
} func[3] = {
 "line",&line,
 "rect",&rect,
 "color",&color
};

char *errType[2]={
 "TCol :无效参数 '%s'\n",
 "TCol :参数不够\n"
};

#define ARG_ERROR errType[0]
#define ARG_LESS errType[1]

HANDLE Hstdout;

int main(int argc,char *argv[]){
 if(argc==2 && strcmp(argv[1],"/?") == 0){
  printf(HELP_INFO);
  return 0;
 }

 Hstdout= GetStdHandle(-11);

 int returnval=0;
 int (*Pfunc) (int argc,char *argv[]);

 int n=1,c,m;
 char **v;
 while(n<argc){
  if(argv[n][0] != '/'){
   fprintf(stderr,ARG_ERROR,argv[n]);
   n++;
   continue;
  } else {
   Pfunc=NULL;
   for(m=0;m<3;m++){
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

int line(int argc,char *argv[]){
 COORD drawPos;
 struct {
  int X,Y,L,F,B,S;
 } args;
 int n;

 if(argc<7){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( (n=aatoia(6,&argv[1],&args)) != -1 ){
  fprintf(stderr,ARG_ERROR,argv[1+n]);
  return 255;
 }

 drawPos.X=args.X;
 drawPos.Y=args.Y;
 FillConsoleOutputAttribute(Hstdout,ARGS_TO_COLOR,args.L,drawPos,&n); // &n不能为NULL
 return 0;
}

int rect(int argc,char *argv[]){
 COORD drawPos;
 struct {
  int X,Y,W,H,F,B,S;
 } args;
 int n,m;

 if(argc<8){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( (n=aatoia(7,&argv[1],&args)) != -1 ){
  fprintf(stderr,ARG_ERROR,argv[1+n]);
  return 255;
 }

 drawPos.X=args.X;
 drawPos.Y=args.Y;
 for(n=0;n<args.H;n++){
  FillConsoleOutputAttribute(Hstdout,ARGS_TO_COLOR,args.W,drawPos,&m); // &m不能为NULL
  drawPos.Y++;
 }
 return 0;
}

int color(int argc,char *argv[]){
 struct {
  int F,B,S;
 } args;
 int n;

 if(argc<4){
  fprintf(stderr,ARG_LESS);
  return 255;
 }
 if( (n=aatoia(3,&argv[1],&args)) != -1 ){
  fprintf(stderr,ARG_ERROR,argv[1+n]);
  return 255;
 }

 SetConsoleTextAttribute(Hstdout,ARGS_TO_COLOR);
 return 0;
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