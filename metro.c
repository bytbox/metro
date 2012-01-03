#define GL_GLEXT_PROTOTYPES

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdio.h>
#include <math.h>

#ifdef DEBUG

#include <stdarg.h>

void debugf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

#else

void debugf(const char *fmt, ...) {

}

#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32
#define MAX_FPS 40

#define POINT_SIZE 2.5
#define LINE_WIDTH 3.0

#define VSIZE (sizeof(float) * 4)

#define SCREEN_CAPTION "Metro Racer"

#ifndef BOOL
#define BOOL char
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 0xff
#endif

BOOL quit = FALSE;

void init_gl();
void display_gl();
void update_board();
void main_loop();

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL);
	init_gl();
	SDL_WM_SetCaption(SCREEN_CAPTION, NULL);

	while (!quit) {
		main_loop();
	}

	SDL_Quit();
	return 0;
}

float *pipeX;
float *pipeY;
float *pipeZ;

struct vertex {
	float x, y, z, a;
};

float *pipeVertexData;
float *pipeColorData;
int vdSize = 80;
int used = 0;
int pipelen;

void init_pipe() {
	float *pipeA;
	float *pipeB;

	FILE *f = fopen("level1.map", "r");
	fscanf(f, "%d\n", &pipelen);

	pipeA = (float *)malloc(sizeof(float) * pipelen);
	pipeB = (float *)malloc(sizeof(float) * pipelen);
	pipeX = (float *)malloc(sizeof(float) * pipelen);
	pipeY = (float *)malloc(sizeof(float) * pipelen);
	pipeZ = (float *)malloc(sizeof(float) * pipelen);

	int lpos = 0, i;
	pipeA[0] = pipeB[0] = 0;
	while (!feof(f)) {
		int pos;
		float a, b;
		fscanf(f, "%d %f %f\n", &pos, &a, &b);
		for (i = lpos; i < pos; i++) {
			pipeA[i] = pipeA[lpos];
			pipeB[i] = pipeB[lpos];
		}
		pipeA[pos] = a;
		pipeB[pos] = b;
	}
	for (i = lpos; i < pipelen; i++) {
		pipeA[i] = pipeA[lpos];
		pipeB[i] = pipeB[lpos];
	}
	fclose(f);

	for (i = 0; i < pipelen; i++) {
		double p = i/50.0;
		pipeX[i] = 0;
		pipeY[i] = cos(p/5);
		pipeZ[i] = p;
	}

	free(pipeA);
	free(pipeB);

	pipeVertexData = calloc(VSIZE, pipelen * vdSize);
	pipeColorData = calloc(VSIZE, pipelen * vdSize);

	int a = 0;
	int b = 0;
	int x = 0;
	for (i = 0; i < pipelen-1; i+=1) {
		int i2 = i + 1;
		int ti;
		double t;
		double dt = 2*M_PI/20;
		for (ti = 0; ti < 20; ti++) {
			t = dt * ti;
			double t2 = t + dt;
			pipeVertexData[a++] = pipeX[i] + sin(t);
			pipeVertexData[a++] = pipeY[i] + cos(t);
			pipeVertexData[a++] = pipeZ[i];
			pipeVertexData[a++] = 1.0;

			pipeVertexData[a++] = pipeX[i] + sin(t2);
			pipeVertexData[a++] = pipeY[i] + cos(t2);
			pipeVertexData[a++] = pipeZ[i];
			pipeVertexData[a++] = 1.0;

			pipeVertexData[a++] = pipeX[i2] + sin(t2);
			pipeVertexData[a++] = pipeY[i2] + cos(t2);
			pipeVertexData[a++] = pipeZ[i2];
			pipeVertexData[a++] = 1.0;

			pipeVertexData[a++] = pipeX[i2] + sin(t);
			pipeVertexData[a++] = pipeY[i2] + cos(t);
			pipeVertexData[a++] = pipeZ[i2];
			pipeVertexData[a++] = 1.0;
			used += 4;
			int __;
			for (__ = 0; __ < 4; __++) {
				pipeColorData[b++] = 1;
				pipeColorData[b++] = x%2;
				pipeColorData[b++] = x%3;
				pipeColorData[b++] = 1;
			}
			x++;
		}
		x=0;
	}
}

void init_gl() {
	// set up the viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set up the perspective
	double aspect = ((double)SCREEN_WIDTH)/((double)SCREEN_HEIGHT);
	gluPerspective(60, aspect, 0.1, 300.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);

	float ambient[] = {0.3, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// enable antialiasing
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(LINE_WIDTH);
	glPointSize(POINT_SIZE);

	init_pipe();
}

void display_gl() {
	static int pos=0;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		pipeX[pos], pipeY[pos], pipeZ[pos], // eye
		pipeX[pos+1], pipeY[pos+1], pipeZ[pos+1], // center
		1+pipeX[pos], pipeY[pos], pipeZ[pos] // up
	);

	glLoadIdentity();
	// set camera
	gluLookAt(
		pipeX[pos], pipeY[pos], pipeZ[pos],
		pipeX[pos+1], pipeY[pos+1], pipeZ[pos+1],
		1, 0, 0
	);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();


	//glVertexPointer(4, GL_FLOAT, 0, pipeData);
	//glDrawArrays(GL_LINE_STRIP, 0, pipelen);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(4, GL_FLOAT, 0, pipeVertexData);
	glColorPointer(4, GL_FLOAT, 0, pipeColorData);
	glDrawArrays(GL_QUADS, 0, used);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glPopMatrix();

	pos++;

	if (pos >= pipelen-10) pos=0;
}

void main_loop() {
	static int j = 0;
	int startj;
	if (j == 0) startj = SDL_GetTicks();

	int start = SDL_GetTicks();

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			quit = TRUE;
			break;
		}
	}

	display_gl();

	SDL_GL_SwapBuffers();

	// Enforce framerate cap
	static int i = 0;
	i++;
	if (SDL_GetTicks() - start < 1000 / MAX_FPS) {
		SDL_Delay(1000 / MAX_FPS - SDL_GetTicks() + start);
	}

	++j;
	if (j == MAX_FPS) {
		debugf("%.1f fps\n", (MAX_FPS*1000.0)/((double)(SDL_GetTicks()-startj)));
		j = 0;
	}
}
