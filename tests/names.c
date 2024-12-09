#include <stdio.h>

void basicblock1(){
    int a = 58;
    int b = 64;
    int result = a + b;
    printf("%d",  result);
}


void basicblock2(){
    int a = 2024;
    int b = 58;
    int result = 2*(b) + a*b;
    printf("%d",  result);
}

void basicblock3(int n, int m){
    int a = 2024;
    int result = m + n + a + 50;
    printf("%d",  result);
}


void basicblock4(int n, int m){
    int a = 2024;
    int b = m + n;
    int result = 2*(b) + a*b;
    printf("%d",  result);
}