#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shape.h"
#include "matrixDrv.h"
#include <unistd.h>
#include <assert.h>

uint8_t* merge2Matrixes(uint8_t* m1, uint8_t* m2){
	
	uint8_t* matrix;
	matrix = (uint8_t*)malloc(8*sizeof(uint8_t));
	int i;
	for(i = 0; i<8; i++){
	matrix[i] = m1[i] | m2[i];
	}
	return matrix;
}

uint8_t* placeShapeToMatrix(uint8_t* matrix, Shape* shp, int8_t x, int8_t y){
	uint8_t *extended = shapeToMatrix(shp, x, y);
	if (extended == NULL)
		return NULL;
	assert(extended);
	int i;
	for (i = 0; i < MAX_MATRIX_DIM; i++){
		//Check if there is any conflict
		if (extended[i] & matrix[i]){
			printf("Nem szabad\n");
			return NULL;
		}
	}
	uint8_t* help = merge2Matrixes(matrix, extended);
	free(extended);
	return help;
}

void game_over(){
	printf("GAME OVER\n");
	exit(0);
}

uint8_t* shapeRight(uint8_t* matrix, Shape* shp, int8_t* shape_x, int8_t shape_y){
	(*shape_x)++;
	uint8_t* right = placeShapeToMatrix(matrix, shp, *shape_x, shape_y);
	if (right == NULL)
		return NULL;
	return right;
}

uint8_t* shapeDown(uint8_t* matrix, Shape* shp, int8_t shape_x, int8_t* shape_y){
	(*shape_y)++;
	uint8_t* down = placeShapeToMatrix(matrix, shp, shape_x, *shape_y);
	if (down == NULL)
		return NULL;
	return down;
}

void main(int argc, char* argv[]){
	int8_t shape_x = -1, shape_y = -1;
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

	uint8_t off[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	sendMatrix(off);
	uint8_t *map;
	uint8_t mapBuff[8] = {0};
	map = (uint8_t*)malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);
	uint8_t matr[8] = {0};
	//sendMatrix(matr);
	Shape* shp = shapeInit(shpmat);
	
	uint8_t* m;
	
//	m = merge2Shapes(m, matrix2);

	int i;
	while(1){
	m = shapeDown(map, shp, shape_x,&shape_y);
	if (m!=NULL){	
		printf("%d\n",shape_x);
		m = shapeRight(map, shp, &shape_x, shape_y);
		if (m != NULL) 
			sendMatrix(m);
		else{
			m = placeShapeToMatrix(map, shp, shape_x - 1, shape_y);
			sendMatrix(m);
			shape_x -= 1;
		}
		free(m);
	}
	else{
		if (shape_y == 0)
			game_over();
		//try to undo the last fall
		m = placeShapeToMatrix(map, shp, shape_x,shape_y - 1);
		assert(m);
		sendMatrix(m);
		shape_y = -1;
		shape_x = -1;

		free(map);
		map = m;
	}
//		if (shp->y <= 8)
//			m = shapeFall(shp);
		sleep(1);
	}
}

