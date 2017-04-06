#ifndef UTIL_MACRO_H
#define UTIL_MACRO_H

#include "value.h"

#define IS_SUCCESS(x) ( x == -1 ? false : x )
#define IS_NULLPO(x) ( x ? true : false )

#endif
