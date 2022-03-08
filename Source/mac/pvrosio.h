/* pvrosio.h
 *
 * inline functions for building endian swapped lists
 * and for writing to HW regs
 */
 

/* Write Int to list
 */
inline void IW( void *base, long offset, long val)
{
	__stwbrx( val, (void*)base, ((offset)<<2));
}

/* Read Int from list.
 */
inline long IR( void *base, long offset)
{
	return __lwbrx( (void*)base, ((offset)<<2));
}

/* Read int from list, OR with val, and write back
 */
inline void IORW( void *base, long offset, long val)
{
	long	temp;
	
	temp = __lwbrx( (void*)base, ((offset)<<2));
	temp |= val;
	__stwbrx( temp, (void*)base, ((offset)<<2));
}

/* Write Float to list
 */
#if 0
/* inline version */
/* Unfortunaltly with the inline version the MetroWerks compiler forces
 * the val arg into an fpu register causeing an unecc write/read cycle
 * to the stack.
 */
inline void FW( void *base, long offset, float val)
{
	__stwbrx( *((long*)(&val)), (void*)base, ((offset)<<2));
}

#else
/* Macro version */
#define FW( x, y, z) __stwbrx( *((long*)(&z)), (void*)x, ((y)<<2));

#endif

/* Write int to HW reg
 */
inline void IWREG( void *base, long offset, long val)
{
	__stwbrx( val, (void*)base, ((offset)<<2));
}
