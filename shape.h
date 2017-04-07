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

Shape* shapeInit(uint8_t* shpmat);
uint8_t* shapeToMatrix(Shape* shp,int8_t x,int8_t y);
Shape* shapeMatrixRotate(Shape* shp);
Shape* shapeGenerator();
void createShapeVector();

#endif
