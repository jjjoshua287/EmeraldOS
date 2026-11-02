#ifndef EMERALD_MIN_MAX_H
#define EMERALD_MIN_MAX_H

#define min(a, b) ((a < b) ? (a) : (b))
#define max(a, b) ((a > b) ? (a) : (b))

#define max_t(type, x, y) ({	\
	type __max1 = (x); 	\
	type __max2 = (y);	\
	(__max1 > __max2) ? __max1 : __max2; })

#endif