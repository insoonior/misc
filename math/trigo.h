/**
 * @file trig.h
 * Basic trigonometrical integer functions
 */

#ifndef TRIGO_H
#define TRIGO_H

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_TRIGO != 0

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
int16_t trigo_sin(int16_t angle); 

/**********************
 *      MACROS
 **********************/

#endif

#endif