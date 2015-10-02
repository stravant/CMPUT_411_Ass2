
//  AUTHOR:  Herb Yang
//  University of Alberta
//  September 20, 2015
//  CMPUT 411/511 Assignment 2

#include "cissoid.h"

// Grid coord -> Screen coord translation.
// Does not do any bounds checking, is a low level function
// that simply transforms coordinates. The user may want to
// is it to translate something like offscreen endpoints for
// a line which is on screen.
void gridToPixel(int gx, int gy, int& px, int& py) {
	// Calculate the center and step for the grid
	float xStep = (_px - 2*BORDER) / (float)(2*_nx);
	float yStep = (_py - 2*BORDER) / (float)(2*_ny);
	float xMid = _px / 2;
	float yMid = _py / 2;

	// Result
	px = xMid + ((float)gx)*xStep;
	py = yMid + ((float)gy)*yStep;
}

void drawGrid() {
	// Draw the grid one line at a time
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
		// Draw vertical lines at each x coordinate
		for (int xc = -_nx; xc <= _nx; ++xc) {
			int xPixel, yPixel;
			gridToPixel(xc, _ny, xPixel, yPixel);
			glVertex2f(xPixel, yPixel);
			gridToPixel(xc, -_ny, xPixel, yPixel);
			glVertex2f(xPixel, yPixel);
		}

		// Draw horizontal lines at each y coordinate
		for (int yc = -_ny; yc <= _ny; ++yc) {
			int xPixel, yPixel;
			gridToPixel(-_nx, yc, xPixel, yPixel);
			glVertex2f(xPixel, yPixel);
			gridToPixel(_nx, yc, xPixel, yPixel);
			glVertex2f(xPixel, yPixel);
		}
	glEnd();
}
void removeDots() {
	// Delete the display lists and then clear the dots array
	for (GLuint i: dots) {
		glDeleteLists(i, 1);
	}
	dots.clear();
}
void drawCircularDot(int x, int y) {
	// Translate the dot to screen coords
	int xPixel, yPixel;
	gridToPixel(x, y, xPixel, yPixel);

	// // Draw the dot as a display list
	// GLuint index = glGenLists(1);
	// glNewList(index, GL_COMPILE);
	// 	glPointSize()
	// 	glBegin(GL_POINTS);

	// 	glEnd();
	// glEndList();

	// // Insert the display list
	// dots.push_back(index);

	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
		float delTheta = 2*PI/(float)16;
		for (int i = 0; i < 16; ++i) {
			float theta = delTheta*i;
			glVertex2f(xPixel + DOT_SIZE*cos(theta), yPixel + DOT_SIZE*sin(theta));
		}
	glEnd();
}
void cissoid() {
	// f(x,y) = x^3 +(x-2k)y^2 = 0
	drawCircularDot(0, 0);

	/* Midpoint method with:
	 *       M2   (x+1,y+1)
	 *   D---*---C 
	 *   |       |
	 *   |       * M1
	 *   |       |
	 *   A-------B
	 * (x,y)_i  (x+1,y) 
	 */

	// d_1i: expand 
	//   ((x+1+dx)^3 + (x+1+dx-2k)(y+dy+1/2)^2) - ((x+1)^3 + (x+1-2k)(y+1/2)^2)
	//   for (dx,dy) = B(1,0) and C(1,1) 
	// =>
	// Case B: 12*x*x + 36*x + 4*y*y + 4*y + 29
	// Case C: -16*k*y - 16*k + 12*x*x + 8*x*y + 44*x + 4*y*y + 20*y + 45

	// d_2i: expand 
	//   ((x+1/2+dx)^3 + (x+1/2+dx-2k)(y+1+dy)^2) - ((x+1/2)^3 + (x+1/2-2k)(y+1)^2)
	//   for (dx,dy) = B(1,0), C(1,1), and D(0,1) 
	// =>
	// Case B: 24*x*x + 48*x + 8*y*y + 16*y + 34
	// Case C: -32*k*y - 48*k + 24*x*x + 16*x*y + 72*x + 8*y*y + 40*y + 70
	// Case D: -32*k*y - 48*k + 16*x*y + 24*x + 8*y + 12

	// Initial values for incremental draw.
	int x = 0;
	int y = 0;
	int d_1i = 5 - 2*k;
	int d_2i = 5 - 16*k;

	// While the second decision function does is not deciding on point D,
	// loop over points drawn choosing between B and C. Also stop if the path
	// drawn reaches the edge of the plot region.
	while (d_2i < 0 && x < _nx && y < _ny) {
		// Incrementally advance
		if (d_1i < 0) {
			// Choose B
			d_1i += 12*x*x + 36*x + 4*y*y + 4*y + 29;
			d_2i += 24*x*x + 48*x + 8*y*y + 16*y + 34;
			++x;
		} else {
			// Choose C
			d_1i += -16*k*y - 16*k + 12*x*x + 8*x*y + 44*x + 4*y*y + 20*y + 45;
			d_2i += -32*k*y - 48*k + 24*x*x + 16*x*y + 72*x + 8*y*y + 40*y + 70;
			++x;
			++y;
		}

		// Draw the point(s) out
		drawCircularDot(x, y);
		if (y != 0)
			drawCircularDot(x, -y); // Reflected figure
	}

	// While the line is still within the plot region, add the "vertical"
	// points, deciding between C and D.
	while (x < _nx && y < _ny) {
		// Incementally advance
		if (d_2i > 0) {
			// Choose D:
			d_2i += -32*k*y - 48*k + 16*x*y + 24*x + 8*y + 12;
			++y;
		} else {
			// Choose C
			d_2i += -32*k*y - 48*k + 24*x*x + 16*x*y + 72*x + 8*y*y + 40*y + 70;
			++y;
			++x;
		}

		// Draw the point(s) out
		drawCircularDot(x, y);
		if (y != 0)
			drawCircularDot(x, -y); // Reflected figure
	}
}

void draw() {  
	cissoid(); // draw a cissoid
}

//----------- Display:  The main display module ---------------------------- 
void display(void) { 
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawGrid(); 
	draw(); // draw parabola
	for(int i = 0; i < (int)dots.size(); i++) {
		glCallList(dots[i]);
	}
	glutSwapBuffers();
	glFlush(); 
} 

//--------- Mouse handler - not used ------------------------------------
void mouse(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glutPostRedisplay();
	}
}

void keyPressed (unsigned char key, int x, int y) {
	// Check the key
	bool isLowerCase = key & 0x20;
	key = key & ~0x20;

	if (key == 'G') {
		if (isLowerCase) {
			_nx = max(_nx-1, 4);
			_ny = max(_ny-1, 4);
		} else {
			++_nx;
			++_ny;
		}
	} else if (key == 'K') {
		if (isLowerCase) {
			k = max(k-1, 1);
		} else {
			++k;
		}
	}

	glutPostRedisplay();
}

//--------------- Initialize:  Parameter initialization ------------------
void initialize(void) {
	glClearColor(1.0, 1.0, 1.0, 0.0); 
	glPointSize(4.0); 
	glLineWidth(2.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIN_SIZE, 0, WIN_SIZE);
	k = 7;  // initial value of k
	cout << "k = " << k << "\n";
	_nx = 8;
	_ny = 8;
	_px = WIN_SIZE;
	_py = WIN_SIZE;
	_br = _bg = _bb = 1.0;
	glClearColor(_br, _bg, _bb, 1.0);
	glShadeModel(GL_SMOOTH);
}

//  ------- Main: Initialize glut window and register call backs ---------- 
int main(int argc, char **argv)  { 
	glutInit(&argc, argv);			
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  
	glutInitWindowSize(WIN_SIZE, WIN_SIZE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Cissoid");
	initialize();
	glutDisplayFunc(display); 
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyPressed); 
	glutMainLoop(); 
	return 0;
}