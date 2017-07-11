#include <stdio.h>
#include <stdlib.h>

int main()
{
    for(int i=0; i<256;) {
        int c = rand() % (' ' - '~') + ' ';
        if(c!='"' && c!='\\') {
            putchar(c);
            ++i;
        }
    }
    puts("");
    return 0;
}
