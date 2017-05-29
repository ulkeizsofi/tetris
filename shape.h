#ifndef HEADER_SHAPE
#define HEADER_SHAPE

#define  MAX_SHAPE_MATRIX_DIM 4
#define MAX_MATRIX_DIM 8
#define MAX_SHAPE_NO 10

#include <stdint.h>

//we will use just the 4x4 multidim. vector
typedef struct _Shape{
	uint8_t width;
	uint8_t height;
	uint8_t* shpMat; 
}Shape;

void shapeInit(uint8_t* shpmat,Shape* shp);
int shapeToMatrix(uint8_t** extended, Shape* shp,int8_t x,int8_t y);
void shapeMatrixRotate(Shape* shp, Shape* newShape);
Shape* shapeGenerator();
void createShapeVector();
void copyShape(Shape* from, Shape **to);
void freeShapeVector();

#endif
