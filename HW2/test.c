#include <stdio.h>
int main()
{
    char str_name[256];
    printf("Enter your name:");
    scanf("%s", str_name);
    printf("\nHello, %s\n", str_name);
    return 0;
}
