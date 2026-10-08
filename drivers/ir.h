/*
 * ir.h - IR sensor for parking slot
 * Author: Ponmudi
 */

#ifndef IR_H
#define IR_H

/* set IR pin as input */
void ir_init(void);

/* returns 1 if slot is occupied, 0 if free */
unsigned char ir_is_occupied(void);

#endif
