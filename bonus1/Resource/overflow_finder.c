#include <stdio.h>

int main(int argc, char **argv)
{
    int i;
    int result;

    for (i = 9; i >= -2147483647; i--)
    {
        result = i * 4;

        if (result == 44)
        {
            printf("result: %d | 0x%x\n", i, i);
            return 0;
        }
    }

    printf("nothing finded");

    return 0;
}