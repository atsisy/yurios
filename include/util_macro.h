#ifndef UTIL_MACRO_H
#define UTIL_MACRO_H

#include "value.h"

#define IS_FAILURE(x) ( x == -1 ? true : false )
#define IS_SUCCESS(x) ( x != -1 ? x : false )
#define IS_NULLPO(x) ( x ? true : false )

#endif
