#include <stdio.h>

int main()
{
    unsigned int x = 0x12345678;
    unsigned char *p = (unsigned char*)&x;
    printf("%0x %0x %0x %0x\n",p[0],p[1],p[2],p[3]);
    return 0;
}
