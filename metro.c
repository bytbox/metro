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

float cubeData[] = {
	 0.5,  0.5,  0.5, 1.0,
	 0.5,  0.5, -0.5, 1.0,
	-0.5,  0.5,  0.5, 1.0,
	-0.5,  0.5, -0.5, 1.0,
	 0.5, -0.5,  0.5, 1.0,
	 0.5, -0.5, -0.5, 1.0,
	-0.5, -0.5,  0.5, 1.0,
	-0.5, -0.5, -0.5, 1.0,

	 0.5,  0.5,  0.5, 1.0,
	-0.5,  0.5,  0.5, 1.0,
	 0.5, -0.5,  0.5, 1.0,
	-0.5, -0.5,  0.5, 1.0,
	 0.5,  0.5, -0.5, 1.0,
	-0.5,  0.5, -0.5, 1.0,
	 0.5, -0.5, -0.5, 1.0,
	-0.5, -0.5, -0.5, 1.0,

	 0.5,  0.5,  0.5, 1.0,
	 0.5, -0.5,  0.5, 1.0,
	-0.5,  0.5,  0.5, 1.0,
	-0.5, -0.5,  0.5, 1.0,
	 0.5,  0.5, -0.5, 1.0,
	 0.5, -0.5, -0.5, 1.0,
	-0.5,  0.5, -0.5, 1.0,
	-0.5, -0.5, -0.5, 1.0,
};
float *pipeData;
float *pipeVertexData;
int pipelen;

void init_pipe() {
	FILE *f = fopen("level1.map", "r");
	fscanf(f, "%d\n", &pipelen);
	while (!feof(f)) {
		int pos;
		float a, b;
		fscanf(f, "%d %f %f\n", &pos, &a, &b);
	}
	fclose(f);

	pipeData = malloc(VSIZE * pipelen);

	int i;
	for (i = 0; i < pipelen; i++) {
		pipeData[i * 4 + 0] = cos(((float)i)/15.0);
		pipeData[i * 4 + 1] = sin(((float)i)/15.0);
		pipeData[i * 4 + 2] = i;
		pipeData[i * 4 + 3] = 1.0f;
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

	// enable antialiasing
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(LINE_WIDTH);
	glPointSize(POINT_SIZE);

	init_pipe();
}

void display_gl() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	gluLookAt(
		0, 0, -2, // eye
		0, 0, 0, // center
		1, 0, 0 // up
	);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(4, GL_FLOAT, 0, cubeData);
	glDrawArrays(GL_LINES, 0, sizeof(cubeData)/VSIZE);
	
	glVertexPointer(4, GL_FLOAT, 0, pipeData);
	glDrawArrays(GL_LINES, 0, pipelen);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
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
