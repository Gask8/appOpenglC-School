#include <iostream>
#include <GL/glut.h>

#include <math.h>
#define size 50
#define mayaX 20
#define mayaY 20
#define GRAVEDAD -9.8
#define MASA 2000
int sizeN = size*-1;
using namespace std;

void ajusta(int, int);
void teclado(unsigned char, int, int);
void circulo(float, float, int);
void inicializaP(struct point *p, int, int);
void dibujaPuntos();
void dibujaMaya();

struct point{
	double pos[2];
	double vel[2];
	double last[2]; //Used in Inertia
	double fX;
};

double h= 0.025; // h incrementos de tiempo
GLboolean bx=GL_FALSE;
GLboolean by=GL_FALSE;

struct point points[mayaX][mayaY];
GLubyte paleta[7][3]={{0,0,255},{0,255,255},{255,0,255},{0,255,0},{255,0,0},{50,40,80},{0,100,100}};

//Fuerzas==========================================
void integraVerlet(struct point *p){
	//Inertia
	double accX =0,accY=0;
	p->vel[0] = p->pos[0] - p->last[0];
	p->vel[1] = p->pos[1] - p->last[1];
	
	if (bx) accX=p->fX;
	if (by) accY=GRAVEDAD/MASA;
	
	double nextX = p->pos[0] + p->vel[0] + accX * h;
	double nextY = p->pos[1] + p->vel[1] + accY * h;
	
	p->last[0] = p->pos[0];
	p->last[1] = p->pos[1];
	 
	p->pos[0] = nextX;
	p->pos[1] = nextY;
}

void integraEuler(struct point *p){
	// v(t+h)=v(t)+F(t)/m*h
	if (bx) p->vel[0] += p->fX*h;
	if (by) p->vel[1] += GRAVEDAD/MASA*h;
	// x(t+h)=x(t) + v(t+h)*h
	p->pos[0] += p->vel[0]*h;
	p->pos[1] += p->vel[1]*h;
}
void constraints(struct point *p){
	//colisiones
	if(p->pos[0]>=size || p->pos[0]<=sizeN){ //X
		p->fX=-1*p->fX;
//		point.velocidad[0]=0;
		if(p->vel[0]<0 || p->vel[0]>0){
			p->vel[0]*=-1;
		}
		p->pos[0]=(p->pos[0]>=size)?size:sizeN;
	}
	
	if(p->pos[1]>=size){ //Y Arriba
		p->vel[1]= 0;
		p->pos[1]=size;
	}
	if(p->pos[1]<=sizeN){ //Y Abajo
		p->vel[1]= 1;
		p->pos[1]= sizeN;
	}
}
//DIBUJA y ANIMA=========================================
void dibuja(void){
	glClear(GL_COLOR_BUFFER_BIT);
	
    glPointSize(2);
    glColor3ubv(paleta[0]);
    dibujaMaya();
    
    glutSwapBuffers();
}
void anima(void){
//	integraEuler();
	int i,j;
	for(i=0;i<mayaX;i++){
		for(j=0;j<mayaY;j++){
			integraVerlet(&points[i][j]);
			constraints(&points[i][j]);	
		}
	}

	glutPostRedisplay();
}

//Main ========================================<<
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Maya");
	glutDisplayFunc(dibuja);
	glutReshapeFunc(ajusta);
	glutKeyboardFunc(teclado);
	glutIdleFunc(anima);
	
	int i,j,ix,jx;
	for(i=0,ix=mayaX;i<mayaX;i++,ix-=2){
		for(j=0,jx=mayaY;j<mayaY;j++,jx-=2){
			inicializaP(&points[i][j], ix, jx);
		}
	}
	
	glutMainLoop();
	return 0;
}

//Funciones de Glut ===================================
void ajusta(int alto, int ancho){
	glClearColor(255,255,255,0);
	glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
   	gluOrtho2D(sizeN, size, sizeN, size);
}
void teclado(unsigned char key, int x, int y) {
	switch (key) {
	case 27: exit(0);
	case 'x': bx = !bx ;break;
	case 'y': by = !by; break;
	}
}
//Funciones para dibuja ===============================
void circulo(float x, float y, int seg){
   int i;
   int radio=1;
   glBegin(GL_POLYGON);
     for(i=0; i<=360; i+=360/seg)
         glVertex2f(x+radio*cos(i*M_PI/180),y+radio*sin(i*M_PI/180));
   glEnd();
}
void dibujaPuntos(){
    glBegin(GL_POINTS);
    	int i,j;
		for(i=0;i<mayaX;i++){
			for(j=0;j<mayaY;j++){
				glVertex2f(points[i][j].pos[0],points[i][j].pos[1]);
			}
		}
    glEnd();
}
void dibujaMaya(){    
    int i,j;
	for(i=0;i<mayaX;i++){
		glBegin(GL_LINE_STRIP);
		for(j=0;j<mayaY;j++){
			glVertex2f(points[i][j].pos[0],points[i][j].pos[1]);
		}
		glEnd();
	}
	
	for(i=0;i<mayaX;i++){
		glBegin(GL_LINE_STRIP);
		for(j=0;j<mayaY;j++){
			glVertex2f(points[j][i].pos[0],points[j][i].pos[1]);
		}
		glEnd();
	}
    
}
//Funciones Otras ====================================
void inicializaP(struct point *p, int x, int y){
	p->pos[0]=x;
	p->pos[1]=y;
	p->last[0]=p->pos[0];
	p->last[1]=p->pos[1];
	p->vel[0]=0;
	p->vel[1]=0;
	p->fX = 0.001;
//	cout<<"x:"<<x<<endl;
//	cout<<"y:"<<y<<endl;
}

