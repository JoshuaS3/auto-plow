#ifndef __UTIL_H__
#define __UTIL_H__

int ticks_to_ms(int ticks);
int ms_to_ticks(int ms);
void delay(int ms);
int map(int input, int min_in, int max_in, int min_out, int max_out);

extern unsigned int xthal_get_ccount();

#endif
