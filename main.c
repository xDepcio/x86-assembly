#include <stdio.h>
#include "swapNumbers.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Arg. missing\n");
        return;
    }
    char* res = swapNumbers(argv[1]);
    printf(res);
    printf("\n");
    return 0;
}
