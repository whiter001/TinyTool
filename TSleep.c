int main(int argc,char *argv[]){
 if(argc==2 && strcmp(argv[1],"/?") == 0){
  printf("Usage: TSleep <millisecond>");
  return 0;
 }
 sleep(argc==1?0:atoi(argv[1]));
}