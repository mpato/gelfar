#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <stdio.h>
#include "graphics/map.h"
#include "map/map.h"
#define window_width  640
#define window_height 480
// Main loop
grx::Map *map;

enum { X_MINUS_PAN, X_PLUS_PAN,Y_MINUS_PAN, Y_PLUS_PAN,NORMAL} mode = NORMAL;

void motionPassive(int x, int y)
{
	if(x <= 10){
		mode = X_MINUS_PAN;
		return;
	}
	else if(x >= window_width - 10){
		mode = X_PLUS_PAN;
		return;
	}

	if(y <= 10){
		mode = Y_MINUS_PAN;
		return;
	}
	else if(y >= window_height - 10){
		mode = Y_PLUS_PAN;
		return;
	}

	mode = NORMAL;
}

void pan(){
	static int pace = 0;

	if(pace == 2){
		if(mode == X_MINUS_PAN && map->map->startx > -10)
			map->map->startx--;
		else if(mode == X_PLUS_PAN && map->map->startx+32 < map->map->sizex+10)
			map->map->startx++;

		if(mode == Y_MINUS_PAN && map->map->starty > -10)
			map->map->starty--;

		else if(mode == Y_PLUS_PAN && map->map->startx+23 < map->map->sizex+10)
			map->map->starty++;

		pace = 0;
	} else
		pace++;
}

void main_loop_function()
{
	// Z angle
	pan();
	map->map->update();
	static float angle;
	// Clear color (screen)
	// And depth (used internally to block obstructed objects)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Load identity matrix
	glLoadIdentity();
	// Multiply in translation matrix
	glTranslatef(-1.65,1.23, -3);
	// Multiply in rotation matrix
	glRotatef(-45, 1, 0, 0);
	// Render colored quad
	/*int i,j;

	for(i=0; i < 100; i++){
		for(j=0; j < 100; j++){
			glBegin(GL_QUADS);
			glColor3ub(255, 255, 255);
			glVertex2f(0.01,  -0.01);
			glVertex2f( 0.01,  -0.09);
			glVertex2f( 0.09, -0.09);
			glVertex2f(0.09, -0.01);
			glEnd();
			// Swap buffers (color buffers, makes previous render visible)

			glTranslatef(0.1,0, 0);
		}
		glTranslatef(-100*0.1,0, 0);
		glTranslatef(0,-0.1,0);
	}*/

	map->draw(32,23);

	// Increase angle to rotate
	angle += 0.25;
	glutSwapBuffers();
}

void list_hits(GLint hits, GLuint *names)
{
	int i;

	/*
 		For each hit in the buffer are allocated 4 bytes:
 		1. Number of hits selected (always one,
 									because when we draw each object
 									we use glLoadName, so we replace the
 									previous name in the stack)
 		2. Min Z
 		3. Max Z
 		4. Name of the hit (glLoadName)
	 */

	printf("%d hits:\n", hits);

	for (i = 0; i < hits; i+=4)
		printf(	"Number: %d\n"
				"Min Z: %d\n"
				"Max Z: %d\n"
				"Name on stack: %d\n",
				names[i],
				names[i + 1],
				names[i + 2],
				names[i + 3]
		);

	printf("\n");
}



int gl_select(int x, int y)
{
	GLuint buff[64] = {0};
	GLint hits, view[4];
	int id;

	/*
		This choose the buffer where store the values for the selection data
	 */
	glSelectBuffer(64, buff);

	/*
		This retrieves info about the viewport
	 */
	glGetIntegerv(GL_VIEWPORT, view);

	/*
		Switching in selecton mode
	 */
	glRenderMode(GL_SELECT);

	/*
		Clearing the names' stack
		This stack contains all the info about the objects
	 */
	glInitNames();

	/*
		Now fill the stack with one element (or glLoadName will generate an error)
	 */
	glPushName(0);

	/*
		Now modify the viewing volume, restricting selection area around the cursor
	 */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/*
			restrict the draw to an area around the cursor
	 */
	gluPickMatrix(x, y, 1, 1, view);
	gluPerspective( 45, (float)window_width/window_height, .1, 100 );

	/*
			Draw the objects onto the screen
	 */
	glMatrixMode(GL_MODELVIEW);

	/*
			draw only the names in the stack, and fill the array
	 */
	main_loop_function();

	/*
			Do you remeber? We do pushMatrix in PROJECTION mode
	 */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	/*
		get number of objects drawed in that area
		and return to render mode
	 */
	hits = glRenderMode(GL_RENDER);

	/*
		Print a list of the objects
	 */
	//list_hits(hits, buff);

	/*
		uncomment this to show the whole buffer
	 * /
	gl_selall(hits, buff);
	 */

	glMatrixMode(GL_MODELVIEW);

	if(hits > 0)
		return buff[3];
	else
		return -1;
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN){
			int name= gl_select(x,window_height-y);

			if(name >=0)
				map->select((name-100)%map->map->sizex,(name-100)/map->map->sizex);
			cout << "Right button lifted "
					<< "at (" << name-100 << "," << name-100
					<< ")" << endl;
		}


	}
}


// Initialze OpenGL perspective matrix
void GL_Setup(int width, int height)
{
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glEnable( GL_DEPTH_TEST );
	gluPerspective( 45, (float)width/height, .1, 100 );
	glMatrixMode( GL_MODELVIEW );
}
// Initialize GLUT and start main loop
int main(int argc, char** argv) {

	map = new grx::Map(new gmp::Map(100,100));

	glutInit(&argc, argv);
	glutInitWindowSize(window_width, window_height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Stratega");
	glutIdleFunc(main_loop_function);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(motionPassive);
	GL_Setup(window_width, window_height);
	glutMainLoop();
}
