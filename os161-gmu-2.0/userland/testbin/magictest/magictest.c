#include<stdio.h>
#include<unistd.h>
int main()
{
//char *argv[5];
char *argv[5];
argv[0] = "yes";
argv[1]="how";
//argv[2]=NULL;
//argv[1]="no";
argv[2]="quick";
argv[3]="what";
argv[4] = NULL;
execv("testbin/argtest", argv);
}
