#ifndef MYFIXPOINT_H_
#define MYFIXPOINT_H_


// Fixed point
#define SHIFT_AMOUNT 28 // 2^16 = 65536
#define SHIFT_MASK                                                             \
  ((1 << SHIFT_AMOUNT) - 1) // 65535 (all LSB set, all MSB clear)
#define Q16(a) (int32_t)((uint32_t)(a) << SHIFT_AMOUNT)
#define Q16_FRAC(num, den) (Q16(num) / den)
#define Q16_WHOLE(a) (int32_t)(((a)) >> SHIFT_AMOUNT)
#define Q16_FRACTIONAL(a) ((a) & SHIFT_MASK)
#define Q16_MUL(a, b) (((uint64_t)a * (uint64_t)b) / (1 << SHIFT_AMOUNT))

#define Q16_TO_float(n)(float)(Q16_WHOLE(n)+((float)(n&SHIFT_MASK)/(1<<SHIFT_AMOUNT)))

#endif /* MYFIXPOINT_H_ */
