#include "include/asm.h"
#include "include/regdef.h"
#include "include/cp0regdef.h"

.section .text.entry
.set noreorder

LEAF(_start)  
	j	main
END(_start)
