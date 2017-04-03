#include "shape.h"
#include <stdlib.h>
#include <stdio.h>

Shape* shapeInit(uint8_t* shpmat, uint8_t line){
	int i;
	Shape* shp;
	shp = (Shape*)malloc(sizeof(Shape));
	shp->shpMat = (uint8_t*)malloc(sizeof(uint8_t)*MAX_MATRIX_DIM);
	for (i = 0; i < line; i++){
		shp->shpMat[i] = shpmat[i];
	}
	shp->y = line;
	shp->x = 0;
	shp->width = 4;
	shp->height = 4;
	return shp;
}

uint8_t* shapeExtend(Shape* shp){
	uint8_t* extended = (uint8_t*)malloc(sizeof(uint8_t)*MAX_MATRIX_DIM);
	int i;
	for (i = 0; i < MAX_MATRIX_DIM; i++){
		if (i <= shp->y && i > shp->y - MAX_SHAPE_MATRIX_DIM)
			extended[i] = shp->shpMat[i-shp->y+1];
		else{
			extended[i] = 0x0;
		}
	}
	return extended;
}

uint8_t* shapeFall(Shape* shp){
	uint8_t* fall;
	int i;
	shp->y++;
	fall = shapeExtend(shp);
	return fall;
}
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
