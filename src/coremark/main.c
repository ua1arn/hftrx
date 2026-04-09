#include <stdio.h>
#include "hardware.h"

int coremark_main(void);

//int main (void)
//{
//  puts("\e[36mAllwinner F1C100S Coremark\e[0m");
//  while(1)
//  {
//    printf("Benchmark started\r");
//    coremark_main();
//    printf("Press any key\r");
//    while(!kbhit()) dev_enable(state_switch() && state_vsys() > 3000 ? 1 : 0);
//    getchar();
//  }
//}
