/*
 * matrixDrv.c
 *
 *  Created on: Apr 3, 2017
 *      Author: zsofi
 */

#ifdef __PIBUILD__
#include <wiringPiSPI.h>
#else
#define wiringPiSPISetup(...) (0)
#define wiringPiSPIDataRW(...) (0)
#endif
#include <stdint.h>
#include "matrixDrv.h"
#include <stdio.h>
void sendMatrix(uint8_t* matrix){
	uint8_t buff[2];
	int i;
	for (i = 0; i < 8; i++){
		buff[1] = matrix[i];
		buff[0] = i + 1;
		wiringPiSPIDataRW(0, buff, 2);
	}
}

void initMatrix(){
	int fd;
	if (fd = wiringPiSPISetup(0,1000000)<0)
		printf("Setup failed\n");
	printf("FD: %d", fd);
	uint8_t buff[2];
	//Set the BCD disabled
	buff[0] = 0x9;
	buff[1] = 0;
	wiringPiSPIDataRW(0, buff, 2);
	//set intensity
	buff[0] = 0xA;
	buff[1] = 0x8;
	wiringPiSPIDataRW(0, buff, 2);
	///scan limit regitser
	buff[0] = 0xB;
	buff[1] = 0xF;
	wiringPiSPIDataRW(0,buff , 2);
	//set shutdown to normal
	//buff[0] = 0xC;
	//buff[1] = 0x1;
}
