#include "type.h"

u16 swaponle_u16(u16 val) {
	#ifdef BE
		return val;
	#endif
	return (val << 8) | (val >> 8 );
}

u32 swaponle_u32(u32 val) {
	#ifdef BE
		return val;
	#endif
	val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
	return (val << 16) | (val >> 16);
}

/*u64 swaponle_u64(u64 val) {
	#ifdef BE
		return val;
	#endif
	val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
	val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
	return (val << 32) | (val >> 32);
}*/

