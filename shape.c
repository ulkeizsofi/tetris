#include "shape.h"
#include <stdlib.h>
#include <stdio.h>
#include "matrixDrv.h"
#include <math.h>

Shape** vShapes;
uint8_t noShapes = 0;

//uint8_t addShapeToVect(uint8_t* shpMat){
//	if (noShapes < MAX_SHAPE_NO){
//		vShapes[noShapes] = shpMat;
//		return 0;
//	}
//	return -1;
//}

void createShapeVector() {
	uint8_t** shpmat;
	Shape ** shpVect;
	noShapes = 4;
	int i;
	shpVect = (Shape**) malloc(noShapes * sizeof(Shape*));
	shpmat = (uint8_t**) malloc(noShapes * sizeof(uint8_t*));
	for (i = 0; i < noShapes; i++) {
		shpmat[i] = (uint8_t*) calloc( MAX_MATRIX_DIM, sizeof(uint8_t));
		if (shpmat[i] == NULL) {
			perror("Error at malloc\n");
			return;
		}
	}
	shpmat[0][0] = 0x07;
	shpmat[0][1] = 0x02;
	shpmat[0][2] = 0x00;
	shpmat[0][3] = 0x00;
	shpmat[1][0] = 0x07;
	shpmat[1][1] = 0x00;
	shpmat[1][2] = 0x00;
	shpmat[1][3] = 0x00;
	shpmat[2][0] = 0x03;
	shpmat[2][1] = 0x03;
	shpmat[2][2] = 0x00;
	shpmat[2][3] = 0x00;
	shpmat[3][0] = 0x06;
	shpmat[3][1] = 0x03;
	shpmat[3][2] = 0x00;
	shpmat[3][3] = 0x00;

	for (i = 0; i < noShapes; i++) {

		shpVect[i] = shapeInit(shpmat[i]);
		if (shpVect[i] == NULL) {
			perror("Error at malloc\n");
			return;
		}
	}
	vShapes = shpVect;
}

int calculateWidth(uint8_t* matr) {
	uint8_t width = 0;
	uint8_t rowOr = 0;
	uint8_t i;
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		rowOr |= matr[i];
	}
	//Count the 1s in help
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		width += (rowOr >> i) & 1;
	}
	return width;
}

int calculateHeigth(uint8_t* matr) {
	uint8_t height = 0;
	uint8_t colOr = 0;
	uint8_t i, j;
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		height += (matr[i] != 0);
	}
	return height;
}

Shape* shapeInit(uint8_t* shpmat) {
	int i;
	Shape* shp;
	shp = (Shape*) malloc(sizeof(Shape));
	if (shp == NULL) {
		perror("Error at malloc\n");
		return NULL;
	}
	if (shpmat == NULL) {
		shp->width = 0;
		shp->height = 0;
		shp->shpMat = calloc(MAX_MATRIX_DIM, sizeof(uint8_t));
		return shp;
	}
	shp->width = calculateWidth(shpmat);
	shp->height = calculateHeigth(shpmat);
	printf("WIDTH: %d HEIGHT: %d\n", shp->width, shp->height);
	shp->shpMat = calloc(MAX_MATRIX_DIM, sizeof(uint8_t));
	for (i = 0; i < shp->height; i++) {
		shp->shpMat[i] = shpmat[i];
	}

	return shp;
}

Shape* shapeGenerator() {
	if (noShapes == 0) {
		printf("EXITTED\n");
		exit(1);
	}
	uint8_t random = rand() % noShapes;
	printf("GEN: %d\n", random);
	return vShapes[random];
}

int shapeToMatrix(uint8_t** extended, Shape* shp, int8_t x, int8_t y) {
	int i;
	//if it exceeds the dimensions of a matrix
	if (x <= 0)
		x = 0;
	if (x + shp->width > MAX_MATRIX_DIM)
		x = MAX_MATRIX_DIM - shp->width;
	if (y + shp->height > MAX_MATRIX_DIM) {
		printf("%d + %d\n", y, shp->height);
		return -1; //y = MAX_MATRIX_DIM - shp->height;
	}
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		if (i >= y && i < y + shp->height)
			(*extended)[i] = shp->shpMat[i - y] << x;
		else {
			(*extended)[i] = 0x0;
		}
	}
	return 0;
}

Shape* shapeMatrixRotate(Shape* shp) {
	int i, j;
	uint8_t helpNo = 0;
	Shape* newShape = shapeInit(NULL);
	for (i = shp->width - 1; i >= 0; i--) {
		for (j = shp->height - 1; j >= 0; j--) {
			//The i-th bit of the shpMat[j]
			//From a coloumn of shpMat we construct a number and place in the appropriate line of newShapeMat
			newShape->shpMat[i] |= ((shp->shpMat[j] >> i) & 1)
					<< (shp->height - j);
		}
//		newShape->shpMat[i] = helpNo;
//		helpNo = 0;
	}

	//intercgange width and height

	newShape->height = shp->width;
	newShape->width = shp->height;
	return newShape;
}

void copyShape(Shape* from, Shape **to) {
	(*to)->height = from->height;
	(*to)->width = from->width;
	int i;
	for (i = 0; i < from->height; i++) {
		(*to)->shpMat[i] = from->shpMat[i];
	}
}
