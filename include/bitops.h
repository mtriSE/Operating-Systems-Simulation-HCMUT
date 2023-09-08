#ifdef CONFIG_64BIT
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif /* CONFIG_64BIT */

#define BITS_PER_BYTE           8
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define BIT(nr)                 (1U << (nr)) 	/* RETURN 2^(nr) in 32 bit */
#define BIT_ULL(nr)             (1ULL << (nr))	/* RETURN 2^(nr) in 64 bit */
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG)) 	/* mask bit nr-th to 1, other are 0, example BIT_MASK(4) = 0x0010*/
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)

#define BITS_TO_LONGS(nr)       DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define BIT_ULL_MASK(nr)        (1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)        ((nr) / BITS_PER_LONG_LONG)


/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
	(((~0U) << (l)) & (~0U >> (BITS_PER_LONG  - (h) - 1)))

#define NBITS2(n) ((n&2)?1:0)
#define NBITS4(n) ((n&(0xC))?(2+NBITS2(n>>2)):(NBITS2(n)))				// C = 1100(2)
#define NBITS8(n) ((n&0xF0)?(4+NBITS4(n>>4)):(NBITS4(n)))				// NBITS4(n)
#define NBITS16(n) ((n&0xFF00)?(8+NBITS8(n>>8)):(NBITS8(n))) 			//8 + NBITS8(n>>8) with n>>8 = 0x00000001
#define NBITS32(n) ((n&0xFFFF0000)?(16+NBITS16(n>>16)):(NBITS16(n))) 	//NBITS16(n)
#define NBITS(n) (n==0?0:NBITS32(n)) 									// n = 0x00000100

#define EXTRACT_NBITS(nr, h, l) ((nr&GENMASK(h,l)) >> l)
