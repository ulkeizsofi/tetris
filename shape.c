#include "shape.h"
#include <stdlib.h>
#include <stdio.h>

Shape* shapeInit(uint8_t* shpmat){
	int i;
	Shape* shp;
	shp = (Shape*)malloc(sizeof(Shape));
	shp->width = 3;
        shp->height = 2;
	shp->shpMat = (uint8_t*)malloc(sizeof(uint8_t)*MAX_MATRIX_DIM);
	for (i = 0; i < shp->height; i++){
		shp->shpMat[i] = shpmat[i];
	}
	
	return shp;
}

uint8_t* shapeToMatrix(Shape* shp, int8_t x, int8_t y){
	uint8_t* extended = (uint8_t*)malloc(sizeof(uint8_t)*MAX_MATRIX_DIM);
	int i;
	//if it exceeds the dimensions of a matrix
	if (x <= 0) x = 0;
	if (x + shp->width < MAX_MATRIX_DIM) x = MAX_MATRIX_DIM - shp->width;
	if (y + shp->height > MAX_MATRIX_DIM) return NULL;//y = MAX_MATRIX_DIM - shp->height;
	for (i = 0; i < MAX_MATRIX_DIM; i++){
		if (i >= y && i < y + shp->height)
			extended[i] = shp->shpMat[i-y] << x;
		else{
			extended[i] = 0x0;
		}
	}
	return extended;
}

//uint8_t* shapeFall(Shape* shp){
//	uint8_t* fall;
//	int i;
//	shp->y++;
//	fall = shapeToMatrix(shp);
//	return fall;
//}
/*
uint8_t* shapeRotate(Shape* shp){
	int i, j;
	uint8_t* transpose = (uint8_t*)malloc(sizeof(uint8_t) * 8);
	for (i = 0; i < MAX_SHAPE_MATRIX_DIM; i++){
		for (j = 0; j < MAX_SHAPE_MATRIX_DIM; j++){
			transpose = ;
		}
	}
	transpose = shapeExtend(shp);
	return transpose;
}
*/
