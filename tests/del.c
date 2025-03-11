// #include <stdbool.h>
// #include <stdio.h>


// void empty(){};

// void if_else(int n){
//     int r = 0;
//     if (n > 0){
//         n++;
//         r = (5 * (n - 1)) + 2 * r;
//     }
//     else {
//         n--;
//     }

//     printf("%d", r);
// }

// void if_elseif_else(int n){
//     if (n > 0){
//         n++;
//     }
//     else if (n < 0){
//         n--;
//     }
//     else {
//         n = 100;
//     }
// }


// void switch_case(int n){
//     switch(n){
//         case 0:
//         case 1:
//         case 2:
//         case 3:
//             n++;
//             break;
//         case 4:
//             n--;
//             break;
//         case 5:
//             n = 6;
//             break;
//         case 6 ... 10:
//             n = 11;
//             break;
//         default:
//             n--;
//     }
// }


// int multiple_return_simple(int n)
// {
//     if (n > 0){
//         n++;
//         return n;
//     }
//     else if (n < 0){
//         n--;
//         return n;
//     }
//     else {
//         n = 100;
//         return n;
//     }
// }

// void foo(int n){
//     int i;
    
//     if (n < 10){
//         i = n;
//         i ++;
//     }
//     else {
//         i = 2*n;
//         i--;
//     }
// }


// void multiple_return_loop(int n)
// {
//     if (n < 0) return;

//     while (n > 0){
//         n--;
//         if (n == 10) return;
//     }

//     return;
// }





// void while_loop(int n)
// {
//     int i = 0;
//     while(i < n)
//     {
//         i++;
//     }
// }


// void for_loop(int n)
// {
//     for (int i=0;i<n;i++){

//     }
// }


void nested_loop(int n, int m) // 2*n^2 + n
{
    int i,j,k;
    i = n;
    
    while(i > 0) // n^2
    {
        j = n;
        k = m;
        while (k > j)
        {
            j++;
        }  
        i--;
    }

    // while (k > 5){ k--; } // n^2
}

void nop(){}

void for_loops(int n, int m)
{
    int counter = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            counter++;
        }
    }

    while (counter > 0)
    {
        counter--;
    }

}



// void until_simple(int n)
// {
//     while (true)
//     {
//         if (n <= 0) break;
//         n--;
//     }
// }


// void until_nested(int n)
// {
//     int i,j;
//     i = n;

//     while (true)
//     {
//         if (i <= 0) break;

//         j = n;
//         while (true)
//         {
//             if (j <= 0) break;
//             j--;
//         }

//         i--;
//     }
// }


