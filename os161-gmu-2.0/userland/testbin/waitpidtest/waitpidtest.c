#include<stdio.h>
int main()
{
int parent_pid=getpid();
printf("\nParent PID is: %d",parent_pid);
fork();
if(
