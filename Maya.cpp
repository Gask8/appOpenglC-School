#include <iostream>
#include <GL/glut.h>
#include <math.h>
//variables del programa general
#define size 50
#define mayaX 25
#define mayaY 25
//variables de particulas
#define GRAVEDAD -0.00001
#define MASA 0.01
//Variables de links
#define RESTINGD 2
#define STIFFNESS 0.08
#define CURTAINSEN 10
const int sizeN = size*-1;
const int numLin = mayaX*(mayaX-1)+mayaY*(mayaY-1);
using namespace std;

void ajusta(int, int);
void teclado(unsigned char, int, int);
void inicializaP(struct point *p, int, int, double);
void inicializaL(struct link *l, struct point *p1, struct point *p2);
void inicializador();
void dibujaPuntos();
void dibujaMaya();

//STRUCTS
struct point{
	double pos[2];
	double vel[2];
	double last[2];
	double accX, accY;
	double fX;
	GLboolean pinned;
  	float pinX, pinY;
	//Lista de links
	struct link *lin[4];
	GLboolean blink[4];
	int cont;
};
struct link{
	int id;
	struct point *p1;
	struct point *p2;
	float restingDistance;
  	float stiffness;
  	float tearSensitivity;
  	GLboolean roto;
};

double h= 0.025; // h incrementos de tiempo
GLboolean bx=GL_FALSE;
GLboolean by=GL_TRUE;

struct point points[mayaX][mayaY];
struct link links[numLin];

//Fuerzas==========================================
void fuerzaCuerda(struct link *l){
	double diffX = l->p1->pos[0] - l->p2->pos[0];
    double diffY = l->p1->pos[1] - l->p2->pos[1];
    double d = sqrt(diffX * diffX + diffY * diffY);
    double difference = (l->restingDistance - d) / d;
    
	//Quitar linea de punto a travez de su arreglo
    if (d > l->tearSensitivity){
    	int i;
    	for (i=0; i < l->p1->cont; i++) {
			if(l->p1->lin[i]->id==l->id){
				l->p1->blink[i]=GL_FALSE;
			}
		}
		for (i=0; i < l->p2->cont; i++) {
			if(l->p2->lin[i]->id==l->id){
				l->p2->blink[i]=GL_FALSE;
			}
		}
		l->roto=GL_FALSE;
	}

    double im1 = 1 / MASA;
    double im2 = 1 / MASA;
    double scalarP1 = (im1 / (im1 + im2)) * STIFFNESS;
    double scalarP2 = STIFFNESS - scalarP1;
    double translateX = diffX * scalarP1 * difference;
	double translateY = diffY * scalarP2 * difference;
	l->p1->pos[0] += translateX;
    l->p1->pos[1] += translateY;
    l->p2->pos[0] -= translateX;
    l->p2->pos[1] -= translateY;
}
void integraVerlet(struct point *p){
	//Inertia
	
	if (bx) p->accX=p->fX/MASA;
	if (by) p->accY=GRAVEDAD/MASA;
	p->vel[0] = p->pos[0] - p->last[0];
	p->vel[1] = p->pos[1] - p->last[1];
	//dampen velocity
	p->vel[0] *= 0.999;
  	p->vel[1] *= 0.999;
	double nextX = p->pos[0] + p->vel[0] + p->accX * h;
	double nextY = p->pos[1] + p->vel[1] + p->accY * h;
	p->last[0] = p->pos[0];
	p->last[1] = p->pos[1];
	p->pos[0] = nextX;
	p->pos[1] = nextY;
	
	p->accX = 0;
	p->accY = 0;
}
void constraints(struct point *p){
	//colisiones
	//Revisar condiciones de cuerda
	int i;
	for (i=0; i < p->cont; i++) {
		if(p->blink[i]){
			fuerzaCuerda(p->lin[i]);
		}
	}
	//Hacer que nada se salga del margen
	if(p->pos[0]>=size || p->pos[0]<=sizeN){ //X
		p->fX=-1*p->fX;
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
	
	//pinned no se mueven
	if (p->pinned) {
      p->pos[0] = p->pinX;
      p->pos[1] = p->pinY; 
    }
}
//DIBUJA y ANIMA=========================================
void dibuja(void){
	glClear(GL_COLOR_BUFFER_BIT);
	
    glPointSize(2);
    glColor3ub(0,0,0);
    dibujaPuntos();
    dibujaMaya();
    
    glutSwapBuffers();
}
void anima(void){
	int i,j;
	
	for(i=0;i<mayaX;i++){
		for(j=0;j<mayaY;j++){
			integraVerlet(&points[i][j]);
			constraints(&points[i][j]);	
		}
	}

	glutPostRedisplay();
}

//Main ========================================<<<<<<<<<<<<<<<<<<<<<<<
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Maya");
	glutDisplayFunc(dibuja);
	glutReshapeFunc(ajusta);
	glutKeyboardFunc(teclado);
	glutIdleFunc(anima);
	//Iniciar todas las variables
	inicializador();
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
	int c =0;
	glBegin(GL_LINES);
	for(c=0;c<numLin;c++){
		if(links[c].roto){
			glVertex2f(links[c].p1->pos[0],links[c].p1->pos[1]);
			glVertex2f(links[c].p2->pos[0],links[c].p2->pos[1]);	
		}
	}
	glEnd();
}
//Funciones Inicializadoras ====================================
void inicializaP(struct point *p, int x, int y,double f){
	p->pos[0]=x;
	p->pos[1]=y;
	p->last[0]=p->pos[0];
	p->last[1]=p->pos[1];
	p->vel[0]=0;
	p->vel[1]=0;
	p->fX =f;
	p->accX=0;
	p->accY=0;
	p->cont=0;
	p->pinned = GL_FALSE;
	p->blink[0]=GL_FALSE;
	p->blink[1]=GL_FALSE;
	p->blink[2]=GL_FALSE;
	p->blink[3]=GL_FALSE;
}
void inicializaL(struct link *l, struct point *p1a, struct point *p2a,int uid){
	l->id=uid;
	l->p1=p1a;
	l->p1->lin[l->p1->cont]=l;
	l->p1->blink[l->p1->cont]=GL_TRUE;
	l->p1->cont++;
	l->p2=p2a;
	l->p2->lin[l->p2->cont]=l;
	l->p2->blink[l->p2->cont]=GL_TRUE;
	l->p2->cont++;
	l->restingDistance=RESTINGD;
	l->stiffness=STIFFNESS;
	l->tearSensitivity=CURTAINSEN;
	l->roto=GL_TRUE;
}
void inicializador(){
	int i,j,ix,jx;
	for(i=0,ix=mayaX;i<mayaX;i++,ix-=2){
		for(j=0,jx=mayaY;j<mayaY;j++,jx-=2){
			if(j==mayaX-1){
				inicializaP(&points[i][j], ix, jx,0.0001);	
			} else {
				inicializaP(&points[i][j], ix, jx,0);
			}
			if(j==0){
				points[i][j].pinned=GL_TRUE;
				points[i][j].pinX=ix;
				points[i][j].pinY=jx;
			}
		}
	}
	
	int c=0;
	for(i=0;i<mayaX;i++){
		for(j=0;j<mayaY-1;j++){
			inicializaL(&links[c], &points[i][j], &points[i][j+1],c);
			c++;
		}
	}
	for(i=0;i<mayaX-1;i++){
		for(j=0;j<mayaY;j++){
			inicializaL(&links[c], &points[i][j], &points[i+1][j],c);
			c++;
		}
	}
}

