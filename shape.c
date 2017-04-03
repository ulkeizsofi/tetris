#include "shape.h"
#include <stdlib.h>
#include <stdio.h>

Shape* shapeInit(uint8_t* shpmat, uint8_t line){
	int i;
	Shape* shp;
	shp = (Shape*)malloc(sizeof(Shape));
	shp->shpMat = (uint8_t*)malloc(sizeof(uint8_t)*8);
	for (i = 0; i < line; i++){
		//place in the central
		shp->shpMat[i] = shpmat[i];
	}
	shp->line = line;
	return shp;
}

uint8_t* shapeExtend(Shape* shp){
	uint8_t* extended = (uint8_t*)malloc(sizeof(uint8_t)*8);
	int i;
	for (i = 0; i < 8; i++){
		if (i <= shp->line && i > shp->line - MAX_SHAPE_MATRIX_DIM)
			extended[i] = shp->shpMat[i-shp->line+1];
		else{
			extended[i] = 0x0;
		}
	}
	return extended;
}

uint8_t* shapeFall(Shape* shp){
	uint8_t* fall;
	int i;
	shp->line++;
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
