#include "include/mm.h"
#include "include/pmap.h"

struct sector sectors[SEC_NUM]; 


struct sector * pa2sector(unsigned int pa ){
	return &sectors [(pa - KSEG0_START)/SEC_SIZE];
}


void memmory_init(){
	bzero(sectors, sizeof(sectors));

}
