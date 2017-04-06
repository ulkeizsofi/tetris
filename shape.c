#include "shape.h"
#include <stdlib.h>
#include <stdio.h>
#include "matrixDrv.h"
#include <math.h>

//Shape* vShapes[MAX_SHAPE_NO];
//uint8_t noShapes = 0;
//
//uint8_t addShapeToVect(uint8_t* shpMat){
//	if (noShapes < MAX_SHAPE_NO){
//		vShapes[noShapes] = shpMat;
//		return 0;
//	}
//	return -1;
//}

Shape* shapeInit(uint8_t* shpmat){
	int i;
	Shape* shp;
	shp = (Shape*)malloc(sizeof(Shape));
	shp->width = 3;
        shp->height = 2;
	shp->shpMat = calloc(MAX_MATRIX_DIM, sizeof(uint8_t));
	for (i = 0; i < shp->height; i++){
		shp->shpMat[i] = shpmat[i];
	}
	
	return shp;
}

//Shape* shapeGenerator(){
//
//}

uint8_t* shapeToMatrix(Shape* shp, int8_t x, int8_t y){
	uint8_t* extended = (uint8_t*)malloc(sizeof(uint8_t)*MAX_MATRIX_DIM);
	int i;
	//if it exceeds the dimensions of a matrix
	if (x <= 0) x = 0;
	if (x + shp->width > MAX_MATRIX_DIM) x = MAX_MATRIX_DIM - shp->width;
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

Shape* shapeMatrixRotate(Shape* shp){
	int i,j;
	uint8_t helpNo = 0;
	uint8_t* newShapeMat = calloc(MAX_MATRIX_DIM, sizeof(uint8_t));
	for (i = shp->width - 1; i >= 0; i--){
		for(j = shp->height - 1; j >= 0; j--){
			//The i-th bit of the shpMat[j]
			//From a coloumn of shpMat we construct a number and place in the appropriate line of newShapeMat
			helpNo += ((shp->shpMat[j] >> i) & 1) * pow(2, j);
		}
		newShapeMat[i] = helpNo;
		helpNo = 0;
	}
	free(shp->shpMat);
	shp->shpMat = newShapeMat;
	//intercgange width and height
	helpNo = shp->height;
	shp->height = shp->width;
	shp->width = helpNo;
	return shp;
}
