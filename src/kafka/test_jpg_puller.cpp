#include "common.h"
#include "pull_jpg2ram.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
    JpgPuller jp;
    jp.initialize();
    if(jp.get_snapshot(argv[1]))
    {
        printf("OK, jpg size:\t%d", (int)jp.jpg_size);
    }else{
        printf("Pull jpg error");
    }
    
    return 0;
}
