#include <stdio.h>
#include <windows.h>

#define HELP_INFO "\
Tiny Key Tool

�÷�:
TKey t k1 [k2 k3 k4 ...]
t        : ʱ������ (0-������)
k1,k2... : ѡ�񰴼������ⰴ���� (Virtual key code)

ע��: ʱ�����Ƴ�ʱ�᷵��-1
"

const char* errType1="TKey: ��Ч���� '%s'\n";

int temp;
int atoi_s(char *in,int *out);
void timeout(void *arg);

int main(int argc,char* argv[]){
 int n,timer,*keyList;
 HANDLE Hstdin = GetStdHandle(-10);
 HANDLE Htimeout;
 INPUT_RECORD ir;

 if(argc==2 && strcmp(argv[1],"/?") == 0){
  printf(HELP_INFO);
  return 0;
 }

 if(argc<3){
  fprintf(stderr,"TKey: ��������. ����'TKey /?'�Ի�ȡ����.\n");
  return 255;
 }

 if(atoi_s(argv[1],&timer)!=0){
  fprintf(stderr,errType1,argv[1]);
  return 255;
 }

 keyList = (int*) malloc((argc-2) *sizeof(int));
 for(n=0;n<argc-2;n++){
  if(atoi_s(argv[n+2],&keyList[n])!=0){
   fprintf(stderr,errType1,argv[n+2]);
   return 255;
 }}

 fflush(stdin);
 if(timer>0){
  Htimeout=(HANDLE)_beginthreadex(NULL,0,(unsigned(__stdcall *)(void *))timeout,&timer,0,NULL);
 }
 while(1){
  ReadConsoleInput(Hstdin,&ir,1,&temp); //temp != NULL
  if(ir.EventType == KEY_EVENT){
   if(ir.Event.KeyEvent.bKeyDown){
    temp=ir.Event.KeyEvent.wVirtualKeyCode;
    for(n=0;n<argc-2;n++){
     if(temp == keyList[n]){
      free(keyList);
      if(Htimeout!=0)CloseHandle(Htimeout);
      return n+1;
 }}}}}
 return 0;
}

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

void timeout(void *arg){
 sleep(*(int*)arg * 1000);
 exit(-1);
}