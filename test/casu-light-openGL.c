/*
 ============================================================================
 Name        : CASUS' lights
 Author      : apc
 Version     : v1
 Description : C, Ansi-style
 ============================================================================
 */
#include <GL/glut.h>

#include <stdio.h>
#include <math.h>

// this program uses the OpnGL standard user window, rectangular, with opposite corners (-1,-1) and (1,1)

void display( void )
{

	// number of points in the background
	float delta_pt = 0.005; //spacing between points. Are equal in the x-axis and in the y-axis
	int dim_x = 402;  // = interval dimension/delta_pt +2;
	int dim_y = 402;

	// colors of the circles around the light
	float dark_color = 0.05;
	float light_color = 0.8;
	float intermediate_color_1 = 0.75;
	float intermediate_color_2 = 0.65;
	float intermediate_color_3 = 0.15;

	// distances around the light
	float d1 = 0.2; //near
	float d2 = 0.3;
	float d3 = 0.5;
	float d4 = 0.8;

	//casus
	int n_casu =2;
	float casu[n_casu][2];
	// centers
	casu[0][0] = -0.5 ; casu[0][1] = 0.5;
	casu[1][0] = 0.5 ; casu[1][1] = -0.5;

	// points in the background
	float x_init = -1.0;
	float points[dim_x][dim_y][2];  //points coordinates
	float color[dim_x][dim_y];      // points colors
	float value_y;
	int i,j,k;
	float distance_x, distance_y, distance;

	for(i = 0; i < dim_y; i++) {
		// calculating the coordinates of the first point in the line
		value_y = i*delta_pt;
		points [i][0][0] = x_init;
		points[i][0][1] = -1+value_y;
		// calculating its color, according to its distance to the CASUs' lights
		color[i][0] = dark_color;
		for (k = 1; k< n_casu; k++)  {
			distance_x = points[i][0][0]- casu[k][0];
			distance_y =  points[i][0][1]- casu[k][1];
			distance = sqrt( (distance_x * distance_x) + (distance_y * distance_y));
			if (distance <= d1 ) color[i][0]= light_color ;
			else if (distance <=d2) color[i][0] = color[i][0] + intermediate_color_1;
			else if (distance <=d3) color[i][0]= color[i][0] + intermediate_color_2;
			else if (distance <=d4) color[i][0]= color[i][0] +  intermediate_color_3;
		} // casu k cycle

		// calculating the coordinates of the remaining points in the line
		for (j = 1; j< dim_x; j++) {
			points [i][j][0] = x_init + j*delta_pt;
			points[i][j][1] = -1+value_y;
			// calculating their color, according to their distances to the CASUs' lights
			color[i][j] = dark_color;
			for (k = 0; k<n_casu; k++)  {
				distance_x = points[i][j][0]- casu[k][0];
				distance_y =  points[i][j][1]- casu[k][1];
				distance = sqrt( (distance_x * distance_x) + (distance_y * distance_y));
				if (distance <= d1 ) color[i][j]= light_color ;
				else if (distance <=d2) color[i][j]+= intermediate_color_1;
				else if (distance <=d3) color[i][j]+= intermediate_color_2;
				else if (distance <= d4) color[i][j]+= intermediate_color_3;
			} // casu k cycle
		} //j cycle
	}// i cycle


	// Drawing the background of the arena (using the standard interpolation of OpenGL
	glClearColor(1,1,0,0.0);
	glClear( GL_COLOR_BUFFER_BIT );

	for(i = 0; i < dim_y-1; i++) {
		for (j = 0; j< dim_x-1; j++) {
			glBegin( GL_POLYGON );

			glColor3f(color[i][j], color[i][j],color[i][j]);
			glVertex2f( points[i][j][0], points[i][j][1]);

			glColor3f(color[i][j+1], color[i][j+1],color[i][j+1]);
			glVertex2f( points[i][j+1][0], points[i][j+1][1]);

			glColor3f(color[i+1][j+1], color[i+1][j+1],color[i+1][j+1]);
			glVertex2f( points[i+1][j+1][0], points[i+1][j+1][1]);

			glColor3f(color[i+1][j], color[i+1][j],color[i+1][j]);
			glVertex2f( points[i+1][j][0], points[i+1][j][1]);

			glEnd();
		}
	}

	// Drawing CASUS' lights as simple rectangles around their center
	float delta = 0.05;  // =(dimension of the rectangle)/2
	glColor3f(0.5f,0.0f,0.0f);
	for (k = 0; k<n_casu; k++)  {
		glBegin( GL_POLYGON );
		glVertex2f( casu[k][0]-delta, casu[k][1]-delta);
		glVertex2f( casu[k][0]+delta, casu[k][1]-delta);
		glVertex2f( casu[k][0]+delta, casu[k][1]+delta);
		glVertex2f( casu[k][0]-delta, casu[k][1]+delta);
		glEnd();
	}
	glFlush();

} // display

int main( int argc, char** argv )
{
   /* Initialize GLUT */

   glutInit( &argc, argv );

   /* window in the screen */

   glutInitWindowSize( 800, 800 );
   glutInitWindowPosition( 100, 100 );

   glutCreateWindow( "casu_light" );

   /*  "callback function" */
   glutDisplayFunc( display );

   /* waiting events */
   glutMainLoop();

   return 0;   /* ANSI C */
}
