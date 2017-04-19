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
#include <time.h>
#include <signal.h>
#include <sys/time.h>

typedef enum {
	DOWN, RIGHT, LEFT, ROTATE
} Moves;

int8_t x = 0, y = 0;
uint8_t *map;
uint8_t* m;
Shape* shape;

void merge2Matrixes(uint8_t* m1, uint8_t* m2, uint8_t** matrix) {

	int i;
	for (i = 0; i < 8; i++) {
		(*matrix)[i] = m1[i] | m2[i];
	}
}

int placeShapeToMatrix(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t x,
		int8_t y) {
	uint8_t *extended = (uint8_t*) malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);

	if (shapeToMatrix(&extended, shp, x, y) < 0) {
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

int shapeLeft(uint8_t **resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y) {
	(*shape_x)++;
	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0) {
		//Unchange
		(*shape_x)--;
		return -1;
	}
	return 0;
}

int shapeRight(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y) {
	(*shape_x)--;

	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0
			|| *shape_x < 0) {
		(*shape_x)++;
		*resoult = NULL;
		return -1;
	}
	return 0;
}

int shapeDown(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t shape_x,
		int8_t* shape_y) {
	printf("X: %d, Y:%d\n", shape_x, *shape_y);
	(*shape_y)++;
	if (placeShapeToMatrix(resoult, matrix, shp, shape_x, *shape_y) < 0) {
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
	uint8_t* m = (uint8_t*) malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);
	printf("TRYMOVE: X: %d, Y:%d", *shape_x, *shape_y);
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

void removeFullRows(uint8_t** matrix) {
	int i;
	for (i = MAX_MATRIX_DIM - 1; i > 0; i--) {
		if (((*matrix)[i] & 0xFF) == 0xFF) {
			(*matrix)[i] = (*matrix)[i - 1];
		}
	}
	if (((*matrix)[0] & 0xFF) == 0xFF)
		(*matrix)[0] = 0;
}

void shapeDownHandler(int sgn) {
	printf("DOWN:");
	if (tryMove(DOWN, map, shape, &x, &y)) {

		placeShapeToMatrix(&m, map, shape, x, y);
		sendMatrix(m);

	} else {

		//try to undo the last fall
		placeShapeToMatrix(&map, map, shape, x, y);

		assert(m);
		sendMatrix(m);
		//free(shp);

		*map = *m;

		//free(m);
		createNewShape(&shape, &x, &y);
		//Try to place to the map
		//Realloc the m to be sure nothing remains
		m = memset(m, 0, MAX_MATRIX_DIM * sizeof(uint8_t));
		placeShapeToMatrix(&m, map, shape, x, y);
		removeFullRows(&map);

		if (m == NULL) {
			game_over();
		}
		sendMatrix(m);

	}
	struct itimerval timer;
	/* Configure the timer to expire after 250 msec... */
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	/* ... and every 250 msec after that. */
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_usec = 0;
	/* Start a virtual timer. It counts down whenever this process is
	 executing. */
	setitimer(ITIMER_REAL, &timer, NULL);
}

void main(int argc, char* argv[]) {
	initMatrix();
	srand(time(NULL));
	createShapeVector();

	//Set signal handler
//	const struct sigaction sgAct;
//	sgAct.sa_sigaction = shapeDownHandler;
//	if (signal(SIGALRM, shapeDownHandler) == SIG_ERR) {
//		perror("Error setting sigaction\n");
//		return;
//	}

	//The map on which the shapes are
	map = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The temporar matrix
	m = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The shape we use in every step
	createNewShape(&shape, &x, &y);
	sendMatrix(shape->shpMat);

	if (placeShapeToMatrix(&m, map, shape, x, y) < 0) {
		perror("Can't place to matrix\n");
		return;
	}

	//Settimer
	struct sigaction sa;
	struct itimerval timer;

	/* Install timer_handler as the signal handler for SIGVTALRM. */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &shapeDownHandler;
	sigaction(SIGALRM, &sa, NULL);

	/* Configure the timer to expire after 250 msec... */
	timer.it_value.tv_sec = 2;
	timer.it_value.tv_usec = 0;
	/* ... and every 250 msec after that. */
	timer.it_interval.tv_sec = 2;
	timer.it_interval.tv_usec = 0;
	/* Start a virtual timer. It counts down whenever this process is
	 executing. */
	setitimer(ITIMER_REAL, &timer, NULL);

	char key;
	int i, r;
	while (1) {
		//setitimer(ITIMER_REAL, &tmval, NULL);
		//If any key was entered
		if (key = (char)getchar()) {
			if (key == 'a') {
				if (!tryMove(LEFT, map, shape, &x, &y))
					printf("can't\n");
			}
			if (key == 's') {
				if (!tryMove(RIGHT, map, shape, &x, &y))
					printf("can't\n");
			}
			if (key == 'w') {
				if (!tryMove(ROTATE, map, shape, &x, &y))
					printf("can't\n");
			}
		}
	}
}

