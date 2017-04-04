#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shape.h"
#include "matrixDrv.h"
#include <unistd.h>

uint8_t* merge2Matrixes(uint8_t* m1, uint8_t* m2){
	
	uint8_t* matrix;
	matrix = (uint8_t*)malloc(8*sizeof(uint8_t));
	int i;
	for(i = 0; i<8; i++){
	matrix[i] = m1[i] | m2[i];
	}
	return matrix;
}

uint8_t* placeShapeToMatrix(uint8_t* matrix, Shape shp, uint8_t x, uint8_t y){
	uint8_t *extended = shapeToMatrix(shp, x, y);

	int i;
	for (i = 0; i < MAX_MATRIX_DIM; i++){
		//Check if there is any conflict
		if (extended[i] & matrix[i])
			return NULL;
	}
	return merge2Matrixes(matrix, extended);
}

void main(int argc, char* argv[]){
	int shape_x = 5, shape_y = 0;
	initMatrix();
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
	m = placeShapeToMatrix(matr, shp, 2,2);
	
//	m = merge2Shapes(m, matrix2);

	int i;
	while(1){
		
		sendMatrix(m);
//		if (shp->y <= 8)
//			m = shapeFall(shp);
		sleep(1);
	}
}

