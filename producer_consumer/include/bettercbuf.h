#ifndef _BETTERCBUF_H_
#define _BETTERCBUF_H_

#define CBUF_CAPACITY 4
#define ENTRYMAX 32

extern void cbuf_init();
extern void cbuf_terminate();
extern void cbuf_print();
extern int cbuf_data_is_available();
extern int cbuf_space_available();
extern int cbuf_copy_in(char *);
extern int cbuf_copy_out(char *);

#endif
