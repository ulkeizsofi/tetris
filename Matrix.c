#include <stdio.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shape.h"

void sendMatrix(uint8_t* matrix){
	uint8_t buff[2];
	int i;
	for (i = 0; i < 8; i++){
		buff[1] = matrix[i];
		buff[0] = i + 1;
		wiringPiSPIDataRW(0, buff, 2);
	}
}

uint8_t* merge2Matrix(uint8_t* m1, uint8_t* m2){
	
	uint8_t* matrix;
	matrix = (uint8_t*)malloc(8*sizeof(uint8_t));
	int i;
	for(i = 0; i<8; i++){
	matrix[i] = m1[i] | m2[i];
	}
	return matrix;
}

void main(int argc, char* argv[]){
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
	//uint8_t on[] = {0x01, 0x03}, off[] = {0xff, 0x0};
	uint8_t shpmat[4]={0x07, 0x02, 0x00, 0x00}; 
	//The matrix we want to send
	uint8_t matrix[8]={
		0xFF,
		0x7F,
		0x3F,
		0x1F,
		0x0F,
		0x07,
		0x03,
		0x01
	};

	uint8_t matrix2[8]={
                0xFF,
                0x7F,
                0x3F,
                0x1F,
                0x0F,
                0x07,
                0x03,
                0x80
        };

	uint8_t matr[8] = {0};
	sendMatrix(matr);
	Shape* shp = shapeInit(shpmat, 2);
	
	uint8_t* m;
	m = shapeExtend(shp);
	
//	m = merge2Matrix(m, matrix2);

	int i;
	while(1){
		
		sendMatrix(m);
		if (shp->y <= 8)
			m = shapeFall(shp);
		sleep(1);
	}
}

