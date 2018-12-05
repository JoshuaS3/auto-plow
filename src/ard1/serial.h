#ifndef __SERIAL_H__
#define __SERIAL_H__ 1

typedef void (* callback)(String);
void serial(void);
void input(callback);
void input_update(void);

#endif
