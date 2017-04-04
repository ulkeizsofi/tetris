#ifndef HEADER_SHAPE
#define HEADER_SHAPE

#define  MAX_SHAPE_MATRIX_DIM 4
#define MAX_MATRIX_DIM 8

#include <stdint.h>

//we will use just the 4x4 multidim. vector
typedef struct _Shape{
	uint8_t width;
	uint8_t height;
	uint8_t* shpMat; 
}Shape;

Shape* shapeInit(uint8_t* shpmat, uint8_t line);
uint8_t* shapeToMatrix(Shape* shp);
uint8_t* shapeFall(Shape* shp);
uint8_t* shapeToMatrix(Shape* shp, uint8_t x, uint8_t y);

#endif
