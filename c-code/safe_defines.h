#ifndef SAFE_H
#define SAFE_H

#define FABINT_BASE			0x40050000

#define SERVO_LOCK			0
#define SERVO_UNLOCK		1
#define SERVO_LOCK_POS		180000
#define SERVO_UNLOCK_POS	75000
#define SERVO_ADDR			(FABINT_BASE + 4)

#endif // SAFE_H
