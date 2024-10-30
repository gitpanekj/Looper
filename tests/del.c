#include <stdbool.h>

int add(int a, int b){return a+b;}
int sub(int a, int b){return a-b;}
int mul(int a, int b){return a*b;}
int div(int a, int b){return a/b;}



void while_loop_1(int n)
{
    int i = 0;
    while(i < n)
    {
        i++;
    }
}

void while_loop_2(int n)
{
    while(n > 0)
    {
        n--;
    }
}

void for_loop_1(int n)
{
    for (int i=0;i<n;i++){

    }
}

void for_loop_2(int n)
{
    for (int i=n;i>0;i--){
        
    }
}

void nested_loop(int n)
{
    int i,j;
    i = n;
    while(i > 0)
    {
        j = n;
        while (j > 0)
        {
            j--;
        }
        i--;
    }
}

void until_simple(int n)
{
    while (true)
    {
        if (n <= 0) break;
        n--;
    }
}

void if_simple(int n)
{
    if (n > 100){
        while (n > 0)
        {
            n--;
        }
    }
    else 
    {
        while(n > 0)
        {
            n-=2;
        }
    }
}


void until_nested(int n)
{
    int i,j;
    i = n;

    while (true)
    {
        if (i <= 0) break;

        j = n;
        while (true)
        {
            if (j <= 0) break;
            j--;
        }

        i--;
    }
}