#pragma once

/*
The bitshift operator '<<' makes it easy to make bitmasks
by shifting bit 1 to the left.
*/



#define CMASK_ENEMY 	(1 << 1)
#define CMASK_EDGES 	(1 << 2)
#define CMASK_PLAYER	(1 << 3)
#define CMASK_WALL		(1 << 4)
#define CMASK_MATTRESS	(1 << 5)
#define CMASK_LEVELWIN	(1 << 6)
