/*
 * ir.h - IR sensor for parking slot
 * Author: Ponmudi
 */

#ifndef IR_H
#define IR_H

/* set IR pin as input with pull-up */
void ir_init(void);

/* pin level now, 0 or 1 */
unsigned char ir_read_raw(void);

/* 1 if occupied in 3 reads (~1 ms apart), else 0 */
unsigned char ir_is_occupied(void);

#endif
