#ifndef _MONITOR_H_
#define _MONITOR_H_

void monitor_init();
void monitor_arrive(struct cart_t *cart);
void monitor_cross(struct cart_t *cart);
void monitor_leave(struct cart_t *cart);
void monitor_shutdown();

#endif
