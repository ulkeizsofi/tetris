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
#include <wiringPi.h>

typedef enum {
	DOWN, RIGHT, LEFT, ROTATE
} Moves;

int8_t x = 0, y = 0;
uint8_t *map;
uint8_t* m;
Shape* shape;
struct itimerval timer;

void setTimer(int sec);

void merge2Matrixes(uint8_t* m1, uint8_t* m2, uint8_t** matrix);
int placeShapeToMatrix(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t x,
		int8_t y);
void game_over();
int shapeLeft(uint8_t **resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y);
int shapeRight(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y);
int shapeDown(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t shape_x,
		int8_t* shape_y);
int shapeRotate(uint8_t** resoult, uint8_t* matrix, Shape** shp, int8_t shape_x,
		int8_t shape_y);
uint8_t tryMove(Moves move, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t *shape_y);
void createNewShape(Shape** shp, int8_t* shape_x, int8_t* shape_y);
void removeFullRows(uint8_t** matrix);
void shapeDownHandler(int sgn);
volatile char button = '0';
void leftButtonHandler(void){
	button = 'a';
}
void rightButtonHandler(void){
	button = 's';
}
void rotateButtonHandler(void){
	button = 'w';
}

void main(int argc, char* argv[]) {
	wiringPiSetup();
while(1){
	wiringPiISR(24, INT_EDGE_FALLING, leftButtonHandler);
	        waitForInterrupt(24,-1);

	wiringPiISR(25, INT_EDGE_FALLING, rightButtonHandler);
	wiringPiISR(27, INT_EDGE_FALLING, rotateButtonHandler);
	pullUpDnControl(24, PUD_UP);
	pullUpDnControl(25, PUD_UP);
	pullUpDnControl(27, PUD_UP);
	setPadDrive(0,0);
	setPadDrive(1,0);
	setPadDrive(2,0);
	
	initMatrix();
	srand(time(NULL));
	createShapeVector();

	//The map on which the shapes are
	map = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The temporar matrix
	m = (uint8_t*) calloc(MAX_MATRIX_DIM, sizeof(uint8_t));

	//The shape we use in every step
	createNewShape(&shape, &x, &y);

	//Display the first shape
	sendMatrix(shape->shpMat);

	if (placeShapeToMatrix(&m, map, shape, x, y) < 0) {
		perror("Can't place to matrix\n");
		return;
	}

	//Settimer
	struct sigaction sa;

	/* Install timer_handler as the signal handler for SIGVTALRM. */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &shapeDownHandler;
	sigaction(SIGALRM, &sa, NULL);

	setTimer(2);

	char key;
	int i, r;
	while (1) {
		//If any key was entered
		
			if (button == 'a') {
				if (!tryMove(LEFT, map, shape, &x, &y))
					printf("can't\n");
				usleep(300000);
				button = '0';

			}


			if (button == 's') {
				printf("right\n");
				if (!tryMove(RIGHT, map, shape, &x, &y))
					printf("can't\n");
				usleep(300000);
				button = '0';

			}
			if (button == 'w') {
				if (!tryMove(ROTATE, map, shape, &x, &y))
					printf("can't\n");
				usleep(300000);
				button = '0';

			}
		
	}
	setTimer(-1);
}
}
void setTimer(int sec) {
	/* Configure the timer to expire after sec... */
	timer.it_value.tv_sec = sec;
	timer.it_value.tv_usec = 0;
	/* ... and every sec after that. */
	timer.it_interval.tv_sec = 1000;
	timer.it_interval.tv_usec = 0;
	/* Start timer. It counts down whenever this process is
	 executing. */
	setitimer(ITIMER_REAL, &timer, NULL);
}

void merge2Matrixes(uint8_t* m1, uint8_t* m2, uint8_t** matrix) {

	int i;
	for (i = 0; i < 8; i++) {
		(*matrix)[i] = m1[i] | m2[i];
	}
}

int placeShapeToMatrix(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t x,
		int8_t y) {
	uint8_t *extended = (uint8_t*) malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);

	//Extend the shape
	if (shapeToMatrix(&extended, shp, x, y) < 0) {
		*resoult = NULL;
		return -1;
	}
	assert(extended);
	int i;
	for (i = 0; i < MAX_MATRIX_DIM; i++) {
		//Check if there is any conflict
		if (extended[i] & matrix[i]) {
			*resoult = NULL;
			return -1;
		}
	}
	//Merge the extended matrix with the map
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
	//Move the shape left
	(*shape_x)++;
	//Check if there is any conflict
	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0) {
		//Unchange
		(*shape_x)--;
		return -1;
	}
	return 0;
}

int shapeRight(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t shape_y) {
	//Move the shape right
	(*shape_x)--;
	//Check if it is OK
	if (placeShapeToMatrix(resoult, matrix, shp, *shape_x, shape_y) < 0
			|| *shape_x < 0) {
		//Make it unchanged
		(*shape_x)++;
		*resoult = NULL;
		return -1;
	}
	return 0;
}

int shapeDown(uint8_t** resoult, uint8_t* matrix, Shape* shp, int8_t shape_x,
		int8_t* shape_y) {
	//Move it down
	(*shape_y)++;
	//Check if OK
	if (placeShapeToMatrix(resoult, matrix, shp, shape_x, *shape_y) < 0) {
		//Unmove
		(*shape_y)--;
		*resoult = NULL;
		return -1;
	}
	return 0;
}

int shapeRotate(uint8_t** resoult, uint8_t* matrix, Shape** shp, int8_t shape_x,
		int8_t shape_y) {
	Shape* shpHelp;
	//Rotate the matrix of the shape
	shpHelp = shapeMatrixRotate(*shp);

	//Check if it fits to the map
	if (placeShapeToMatrix(resoult, matrix, shpHelp, shape_x, shape_y) < 0) {
		*resoult = NULL;
		free(shpHelp);
		return -1;
	}

	//Make the same change on the original shape
	copyShape(shpHelp, shp);
	free(shpHelp);
}

uint8_t tryMove(Moves move, uint8_t* matrix, Shape* shp, int8_t* shape_x,
		int8_t *shape_y) {
	uint8_t* m = (uint8_t*) malloc(sizeof(uint8_t) * MAX_MATRIX_DIM);

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

	//If there was any problem
	if (m != NULL) {
		sendMatrix(m);
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
	int i, j;
	for (i = MAX_MATRIX_DIM - 1; i > 0; i--) {
		//If it is full
		if (((*matrix)[i] & 0xFF) == 0xFF) {
			{
				printf("Remove row: %d\n",i);
				//Move each row above
				for (j = i; j > 0; j--)
					(*matrix)[j] = (*matrix)[j - 1];
				i++;

			}
		}
	}
	//If the top row is full
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
		sendMatrix(m);

		//Replace the map with the temporary matrix
		*map = *m;

		//Check if there are full rows
		removeFullRows(&map);

		createNewShape(&shape, &x, &y);
		//Empty the m to be sure nothing remains
		m = memset(m, 0, MAX_MATRIX_DIM * sizeof(uint8_t));

		//Try to place to the map
		placeShapeToMatrix(&m, map, shape, x, y);
		//If it fails then the game is over
		if (m == NULL) {
			game_over();
		}
		sendMatrix(m);

	}
	setTimer(1);
}
