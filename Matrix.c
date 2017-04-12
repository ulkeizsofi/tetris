#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shape.h"
#include "matrixDrv.h"
#include <unistd.h>
#include <assert.h>
#include <curses.h>

typedef enum {
	DOWN, RIGHT, LEFT, ROTATE
} Moves;

void merge2Matrixes(uint8_t* m1, uint8_t* m2, uint8_t** matrix) {

	int i;
	for (i = 0; i < 8; i++) {
		(*matrix)[i] = m1[i] | m2[i];
	}
}

int placeShapeToMatrix(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t x, int8_t y) {
	uint8_t *extended = (uint8_t*)malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);

	if (shapeToMatrix(&extended, shp, x, y) < 0){
		*resoult = NULL;
		return -1;
	}
	assert(extended);
	int i;
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		//Check if there is any conflict
		if (extended[i] & matrix[i]) {
			printf("Nem szabad\n");
			*resoult = NULL;
			return -1;
		}
	}
	merge2Matrixes(matrix, extended, resoult);
	free(extended);
	return 0;
}

void game_over() {
	printf("GAME OVER\n");
	exit(0);
}

int shapeLeft(uint8_t **resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x, int8_t shape_y) {
	(*shape_x)++;
	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0 ) {
		//Unchange
		(*shape_x)--;
		return -1;
	}
	return 0;
}

int shapeRight(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y) {
	(*shape_x)--;

	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0 || *shape_x < 0) {
		(*shape_x)++;
		*resoult = NULL;
		return -1;
	}
	return 0;
}

int shapeDown(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t shape_x, int8_t* shape_y) {
	printf("X: %d, Y:%d\n",shape_x, *shape_y);
	(*shape_y)++;
	if (placeShapeToMatrix(resoult, matrix, shp, shape_x, *shape_y) < 0){
		(*shape_y)--;
		*resoult = NULL;
		return -1;
	}
	return 0;
}

int shapeRotate(uint8_t** resoult, uint8_t* matrix, Shape** shp, int8_t shape_x,
		int8_t shape_y) {
	Shape* shpHelp;
	shpHelp = shapeMatrixRotate(*shp);

	if (placeShapeToMatrix(resoult, matrix, shpHelp, shape_x, shape_y) < 0) {
		*resoult = NULL;
		return -1;
	}
//	free((*shp)->shpMat);
//	free(*shp);
	copyShape(shpHelp, shp);
	free(shpHelp);
}

uint8_t tryMove(Moves move, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t *shape_y) {
	uint8_t* m = (uint8_t*)malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);
	printf("TRYMOVE: X: %d, Y:%d",*shape_x, *shape_y);
	switch (move) {
	case DOWN:
		shapeDown(&m, matrix, shp, *shape_x, shape_y);
		break;
	case RIGHT:
		shapeRight(&m, matrix, shp, shape_x, *shape_y);
		break;
	case LEFT:
		shapeLeft(&m, matrix, shp, shape_x, *shape_y);
		break;
	case ROTATE:
		shapeRotate(&m, matrix, &shp, *shape_x, *shape_y);

		break;
	default:
		break;
	}
	if (m != NULL) {
		return 1;
	} else {
		return 0;
	}
	free(m);
}

void createNewShape(Shape** shp, int8_t* shape_x, int8_t* shape_y) {

	*shp = shapeGenerator();
	*shape_x = 0;
	*shape_y = 0;
}

void main(int argc, char* argv[]) {
	int8_t shape_x = 0, shape_y = 0;
	initMatrix();
	createShapeVector();
	//uint8_t on[] = {0x01, 0x03}, off[] = {0xff, 0x0};

	//The map on which the shapes are
	uint8_t *map;
	map = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The temporar matrix
	uint8_t* m = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The shape we use in every step
	Shape* shp;
	createNewShape(&shp, &shape_x, &shape_y);
	sendMatrix(shp->shpMat);

	if (placeShapeToMatrix(&m, map, shp, shape_x, shape_y) < 0) {
		perror("Can't place to matrix\n");
		return;
	}
	char key;
	int i, r;
	while (1) {
		//If any key was entered
//		if (key = getch()) {
//			if (key == KEY_LEFT) {
//				if (!tryMove(LEFT, map, shp, &shape_x, shape_y))
//					printf("can't\n");
//			}
//			if (key == KEY_RIGHT) {
//				if (!tryMove(RIGHT, map, shp, &shape_x, shape_y))
//					printf("can't\n");
//			}
//		}
		printf("DOWN:");
		if (tryMove(DOWN, map, shp, &shape_x, &shape_y)){

			r = rand() % 3 + 1;
			printf("type %d:",r);
			if (tryMove((Moves)r, map, shp, &shape_x, &shape_y)){
				printf("OK\n");
			}
			else printf("NOK\n");

			placeShapeToMatrix(&m, map, shp, shape_x, shape_y);
			sendMatrix(m);

		} else {

			//try to undo the last fall
			placeShapeToMatrix(&map, map, shp, shape_x, shape_y);

			assert(m);
			sendMatrix(m);
			//free(shp);

			*map = *m;

			//free(m);
			createNewShape(&shp, &shape_x, &shape_y);
			//Try to place to the map
			//Realloc the m to be sure nothing remains
			m = memset(m, 0, MAX_MATRIX_DIM * sizeof(uint8_t));
			placeShapeToMatrix(&m, map, shp, shape_x, shape_y);

			if (m == NULL) {
				game_over();
			}
			sendMatrix(m);

		}
		sleep(1);
	}
}

