/*
 * ir.h - IR obstacle sensor driver, any pin
 * Author: Ponmudi
 */

#ifndef IR_H
#define IR_H

#define IR_MAX          4     /* max sensors */
#define IR_NONE         255   /* ir_init failed */
#define IR_ACTIVE_LOW   1     /* output 0 when object seen (most modules) */
#define IR_ACTIVE_HIGH  0     /* output 1 when object seen */

/* add a sensor, returns id or IR_NONE */
unsigned char ir_init(char port, unsigned char pin, unsigned char active_low);

/* pin level now, 0 or 1 (0 if wrong id) */
unsigned char ir_read_raw(unsigned char id);

/* 1 if object seen in 3 reads (~1 ms apart), else 0 */
unsigned char ir_is_detected(unsigned char id);

#endif
