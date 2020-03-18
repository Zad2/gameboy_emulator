#include <stdio.h>
#include <stdint.h>   // for uint8_t and uint16_t types
#include <inttypes.h> // for PRIx8, etc.
#include "bit.h"

// ======================================================================
/**
 * @brief a type to represent 1 single bit.
 */
/* Nous vous fournission ici un type à n'utiliser QUE lorsque vous
 * voulez représenter UN SEUL bit ; p.ex. :
 *     bit_t un_bit_tout_seul = 1;
 */
typedef uint8_t bit_t;

// ======================================================================
/**
 * @brief clamp a value to be a bit index between 0 and 7
 */
/* Nous vous fournission ici une macro (les macros seront présentées dans
 *     le cours, bien plus tard dans le semestre) permettant de forcer
 * une valeur entre 0 et 7.
 * Par exemple :
 *     i = CLAMP07(j);
 * fera que la variable i contiendra :
 *     + la même valeur que celle de j si celle-ci est comprise entre 0 et 7 ;
 *     + 0 si la valeur de j est inférieure ou égale à 0 ;
 *     + 0 si la valeur de j est supérieure ou égale à 8.
 */
// #define CLAMP07(x) (((x) < 0) || ((x) > 7) ? 0 : (x))


// ======================================================================
/* TODO: check if useful
#define size(T) (sizeof T / sizeof T[0])
#define FMT8 "0x%02" PRIx8
#define FMT16 "0x%04" PRIx16
#define show(M, S1) printf(#M "(" FMT ## S1 ") = " FMT8 "\n", value, result)
#define show2(M, V) printf("bit_" #M "(" FMT8 ", %d) = " FMT8 "\n", V, k, value2)
*/

uint8_t lsb4(uint8_t value){
	return value & 0x0f;
}

uint8_t msb4(uint8_t value){
	return (value & 0xf0) >> 4;
}

uint8_t lsb8(uint16_t value){
	return value & 0x00ff;
}

uint8_t msb8(uint16_t value){
	return (value & 0xff00) >> 8;
}

uint16_t merge8(uint8_t v1, uint8_t v2){
	return v1 | v2 << 8;
}

uint8_t merge4(uint8_t v1, uint8_t v2){
	return lsb4(v1) | (lsb4(v2) << 4);
}

bit_t bit_get(uint8_t value, int index){
	int k = CLAMP07(index);
	return (value & (1 << k)) >> k;
}

void bit_set(uint8_t* value, int index){
	int k = CLAMP07(index);
	*value = (1 << k) | *value;
}

void bit_unset(uint8_t* value, int index){
	int k = CLAMP07(index);
	*value = ~ (1 << k) & *value;
}

void bit_rotate(uint8_t* value, rot_dir_t dir, int d){
	int k = CLAMP07(d);
	uint8_t temp = 1;
#define SIZE_BYTE 8
	if (dir == LEFT){
		temp = *value >> (SIZE_BYTE - d);
		*value = (*value << d) | temp;
	} else if (dir == RIGHT){
		temp = *value << (SIZE_BYTE - d) >> (SIZE_BYTE - d);
		
	}
}