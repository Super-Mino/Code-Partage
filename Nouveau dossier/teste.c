#include <stdio.h>

int g(void)
{
    printf("test reussi !\n"); 
    return 1;
}

int main()
{
    g(); 
    return 0;
}