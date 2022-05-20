#include <iostream>
#include <GL/glut.h>

#include <math.h>
#define GRAVEDAD -9.8
#define MASA 200 // masa de la pelota
using namespace std;

struct pelota{
	double posicion[2];
	double velocidad[2];
};

float radio=5;
double h= 0.025; // h incrementos de tiempo
double fzaX=.09;
double posicion[]= {0,0,0};
double velocidad[]= {0,0,0};
pelota pelotas;
GLubyte paleta[7][3]={{0,0,255},{0,255,255},{255,0,255},{0,255,0},{255,0,0},{50,40,80},{0,100,100},};


GLboolean bx=GL_FALSE;
GLboolean by=GL_FALSE;

void integraEuler(){
	// v(t+h)=v(t)+F(t)/m*h
	if (bx) pelotas.velocidad[0] += fzaX*h;
	if (by) pelotas.velocidad[1] += GRAVEDAD/MASA*h;
	// x(t+h)=x(t) + v(t+h)*h
	pelotas.posicion[0] += pelotas.velocidad[0]*h;
	pelotas.posicion[1] += pelotas.velocidad[1]*h;
}
void ajusta(int alto, int ancho){
	glClearColor(255,255,255,0);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
   	gluOrtho2D(-50, 50, -50, 50);
}
void circulo(float x, float y, int seg){
   int i;
   glBegin(GL_POLYGON);
     for(i=0; i<=360; i+=360/seg)
         glVertex2f(x+radio*cos(i*M_PI/180),y+radio*sin(i*M_PI/180));
   glEnd();
}
void dibuja(void){
	glClear(GL_COLOR_BUFFER_BIT);
	
//	glColor3ub(0,0,0);
//	glLineWidth(2);
//	glPushMatrix();
//		glTranslatef(0,0,-16);
//		glutWireCube(11);
//		glColor3ubv(paleta[0]);
//		glPushMatrix();
//			glTranslated(pelotas.posicion[0],pelotas.posicion[1],pelotas.posicion[2]);
//			glutSolidSphere(.4,20,20);
//		glPopMatrix();
//	glPopMatrix();
//	glutSwapBuffers();
	
   glColor3ubv(paleta[0]);
   circulo(pelotas.posicion[0],pelotas.posicion[1],150);
   glutSwapBuffers();
}
void anima(void){
	integraEuler();
	//colisiones
	if(pelotas.posicion[0]>=50 || pelotas.posicion[0]<=-50){ //X
	fzaX=-1*fzaX;
	pelotas.velocidad[0]=0;
	pelotas.posicion[0]=(pelotas.posicion[0]>=50)?45:-45;
	}
	if(pelotas.posicion[1]>=50){ //Y Arriba
	pelotas.velocidad[1]= 0;
	pelotas.posicion[1]= 42;
	}
	if(pelotas.posicion[1]<=-50){ //Y Abajo
	pelotas.velocidad[1]= 2;
	pelotas.posicion[1]= -48;
	}
	glutPostRedisplay();
}
void teclado(unsigned char key, int x, int y) {
	switch (key) {
	case 27: exit(0);
	case 'x': bx = !bx ;break;
	case 'y': by = !by; break;
	}
}
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Maya");
	glutDisplayFunc(dibuja);
	glutReshapeFunc(ajusta);
	glutKeyboardFunc(teclado);
	glutIdleFunc(anima);
	
	pelotas.posicion[0]=0;
	pelotas.posicion[1]=0;
	pelotas.velocidad[0]=0;
	pelotas.velocidad[1]=0;
	
	glutMainLoop();
	return 0;
}
