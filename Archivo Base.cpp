#include <GL/glut.h>
#include <stdio.h>
#define GRAVEDAD -9.8
#define MASA 200 // masa de la pelota

struct pelota{
	double posicion[3];
	double velocidad[3];
};

double h= 0.025; // h incrementos de tiempo
double posicion[]= {0,0,0};
double velocidad[]= {0,0,0};
pelota pelotas[7];
GLubyte paleta[7][3]={{0,0,255},{0,255,255},{255,0,255},{0,255,0},{255,0,0},{50,40,80},{0,100,100},};

double fzaX[7]={.02,.02,.02,.02,.02,.02,.02,};
double fzaZ[7]={-.05,-.05,-.05,-.05,-.05,-.05,-.05,};

GLboolean bx=GL_FALSE;
GLboolean by=GL_FALSE;
GLboolean bz=GL_FALSE;

void integraEuler(){
	int i;
	for(i=0;i<7;i++){
		// v(t+h)=v(t)+F(t)/m*h
		if (bx) pelotas[i].velocidad[0] += fzaX[i]*h;
		if (by) pelotas[i].velocidad[1] += GRAVEDAD/MASA*h;
		if (bz) pelotas[i].velocidad[2] += fzaZ[i]*h;
		// x(t+h)=x(t) + v(t+h)*h
		pelotas[i].posicion[0] += pelotas[i].velocidad[0]*h;
		pelotas[i].posicion[1] += pelotas[i].velocidad[1]*h;
		pelotas[i].posicion[2] += pelotas[i].velocidad[2]*h;
	}
}
void ajusta(int alto, int ancho){
	glClearColor(0.5,0.5,0.5,0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(float)alto/ancho, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void dibuja(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(0,0,0);
	glLineWidth(2);
	
	glPushMatrix();
		glTranslatef(0,0,-16);
		glutWireCube(11);
		int i;
		for(i=0;i<7;i++){
		glColor3ubv(paleta[i]);
		glPushMatrix();
			glTranslated(pelotas[i].posicion[0],pelotas[i].posicion[1],pelotas[i].posicion[2]);
			glutSolidSphere(.4,20,20);
		glPopMatrix();
		}
	glPopMatrix();
	
	glutSwapBuffers();
}
void mueve(void){
	integraEuler();
	//colisiones
	int i,j;
	for(i=0;i<7;i++){
		if(pelotas[i].posicion[0]>=5 || pelotas[i].posicion[0]<=-5){ //X
		fzaX[i]=-1*fzaX[i];
		pelotas[i].velocidad[0]=0;
		pelotas[i].posicion[0]=(pelotas[i].posicion[0]>=5)?4.8:-4.8;
		}
		if(pelotas[i].posicion[1]>=5){ //Y Arriba
		pelotas[i].velocidad[1]= 0;
		pelotas[i].posicion[1]= 4.2;
		}
		if(pelotas[i].posicion[1]<=-5){ //Y Abajo
		pelotas[i].velocidad[1]= 0.9;
		pelotas[i].posicion[1]= -4.8;
		}
		if(pelotas[i].posicion[2]>=5 || pelotas[i].posicion[2]<=-5){ //Z
		fzaZ[i]=-1*fzaZ[i];
		pelotas[i].velocidad[2]=0;
		pelotas[i].posicion[2]=(pelotas[i].posicion[2]>=5)?4.8:-4.8;
		}
	}
	glutPostRedisplay();
}
void teclado(unsigned char key, int x, int y) {
	switch (key) {
	case 27: exit(0);
	case 'x': bx = !bx ;break;
	case 'y': by = !by; break;
	case 'z': bz = !bz ;
	}
}
int main(int argc, char** argv){
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Bolitas");
	glutDisplayFunc(dibuja);
	glutReshapeFunc(ajusta);
	glutKeyboardFunc(teclado);
	glutIdleFunc(mueve);
	int i;
	for(i=0;i<7;i++){
		pelotas[i].posicion[0]=i-3;
		pelotas[i].posicion[1]=0;
		pelotas[i].posicion[2]=0;
		pelotas[i].velocidad[0]=0;
		pelotas[i].velocidad[1]=0;
		pelotas[i].velocidad[2]=0;
	}
	glutMainLoop();
	return 0;
}
