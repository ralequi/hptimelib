#ifndef __REALTIME__H__
#define __REALTIME__H__

#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define APROXCICLES (1024*16)
#define EXTRALEN 2.625

uint64_t realtime_timeofday_old;
uint64_t realtime_cicles_old;
uint64_t realtime_hpet_hz;

#define PRECCISIONu 	 10ull //Valor para obtener el tiempo en usec
#define PRECCISION (1000000ull * PRECCISIONu) // = 100ns
//2^64 = 	18.446.744.073.709.551.616
//       	36.450.791.397.000.000.000
//			xx.xxx.xxx.xxx.x00.000.000
//			18.446.744.073.7 ciclos ~ 76.8s @ 2.4hz
//			 3.647.989.712.700.000.000

typedef uint64_t realtime_t;

/**
 * Inicializa el modulo
 **/
inline void realtime_init(void);

/**
 * Sincroniza el reloj virtual con el reloj del sistema
 **/
inline void realtime_sync_real(void);

/**
 * Sincroniza el reloj virtual de forma simulada
 **/
inline void realtime_sync(void);

/**
 * Optiene el tiempo
 **/
inline realtime_t realtime_get(void);

/**
 * optiene una aproximacion del tiempo en funcion del tamanio del paquete.
 * cada APROXCICLES se corrijen los posibiles errores.
 **/
inline realtime_t realtime_getAprox(unsigned tam);

/**
 * Converts from realtime format to timespect format
 **/
struct timespec realtime_timespec(realtime_t u64);

/*** PRIVATE ***/
inline uint64_t realtime_get_slow(void);
#endif
