/*************** COST 0 ***************/
int no_loops(int a, int b) // 0
{
    int c = a + b;
    if (c >= 0)
    {
        return c;
    }
    else
    {
        return -c;
    }
};

/*************** COST linear ***************/
// Simple while
void simple_while_1(int n) // n
{
    while (n > 0) n--;
}

void simple_while_2(int n) // n - 100
{
    while (n > 100) n--;
}

void simple_while_3(int n) // n
{
    int bottom = 0;
    while (n > bottom) n--;
}

void simple_while_4(int n) // n - 100
{
    int bottom = 100;
    while (n > bottom) n--;
}

void simple_while_5(int n, int m) // n-m
{
    while (n > m) n--;
}

void simple_while_6(int n, int m) // n-m
{
    while (n > m) m++;
}

void simple_while_7(int n) // n
{
    int i = 0;
    while (i < n) i++;
}

// Simple for loop
void simple_for_1(int n, int m) // n
{
    for (int i=0;i<n;i++){}
}
void simple_for_2(int n, int m) // n-m
{
    for (int i=m;i<n;i++){}
}
void simple_for_3(int n, int m) // m-n
{
    for (int i=m;i>n;i--){}
}


/*************** COST quadratic ***************/
// Nested while
void nested_while(int n, int m) // n
{
    int difference = n - m;
    while (n > m)
    {
        m++;
        int i = difference;
        while (i > 0)
        {
            i--;
        }
    }
}

// Nested for
void nested_for(int n, int m) // n
{
    for (int i=m;i<n;i++){
        for (int j=n;j>m;j--)
        {
            
        }
    }
}

/*************** COST - increment sum ***************/

/*************** COST - reset sum ***************/