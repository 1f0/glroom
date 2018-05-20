#include "dspMap.h"
int Height(unsigned char*pHeightMap, int X, int Y){
	int x = X % MAP_SIZE;								// Error Check Our x Value
	int y = Y % MAP_SIZE;								// Error Check Our y Value

	if (!pHeightMap) return 0;							// Make Sure Our Data Is Valid
	//0.257*R' + 0.504*G' + 0.098*B' + 16
	return (int)(0.257*pHeightMap[(x + y * MAP_SIZE) * 3] + 0.504*pHeightMap[(x + y * MAP_SIZE) * 3 + 1] + 0.098*pHeightMap[(x + y * MAP_SIZE) * 3+2] + 2);			// RGB to Scale
}

void SetVertexColor(BYTE *pHeightMap, int x, int y)		// Sets The Color Value For A Particular Index, Depending On The Height Index
{
	if (!pHeightMap) return;							// Make Sure Our Height Data Is Valid

	//float fColor = (Height(pHeightMap, x, y) / 256.0f);
	float cR = pHeightMap[(x + y * MAP_SIZE) * 3] / 256.0f;
	float cG = pHeightMap[(x + y * MAP_SIZE) * 3 + 1] / 256.0f;
	float cB = pHeightMap[(x + y * MAP_SIZE) * 3 + 2] / 256.0f;

	// Assign This Blue Shade To The Current Vertex
	glColor3f(cR, cG, cB);
}

void RenderHeightMap(unsigned char pHeightMap[]){
	int X = 0, Y = 0;
	int x, y, z;
	if (!pHeightMap) return;

	glDisable(GL_LIGHTING);
	if (bRender)									// What We Want To Render
		glBegin(GL_QUADS);							// Render Polygons
	else
		glBegin(GL_LINES);							// Render Lines Instead

	for (X = 0; X < (MAP_SIZE - STEP_SIZE); X += STEP_SIZE)
		for (Y = 0; Y < (MAP_SIZE - STEP_SIZE); Y += STEP_SIZE){
			// Get The (X, Y, Z) Value For The Bottom Left Vertex
			x = X;
			y = Height(pHeightMap, X, Y);
			z = Y;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered (Integer Points Are Faster)

			// Get The (X, Y, Z) Value For The Top Left Vertex
			x = X;
			y = Height(pHeightMap, X, Y + STEP_SIZE);
			z = Y + STEP_SIZE;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered

			// Get The (X, Y, Z) Value For The Top Right Vertex
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y + STEP_SIZE);
			z = Y + STEP_SIZE;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered

			// Get The (X, Y, Z) Value For The Bottom Right Vertex
			x = X + STEP_SIZE;
			y = Height(pHeightMap, X + STEP_SIZE, Y);
			z = Y;

			// Set The Color Value Of The Current Vertex
			SetVertexColor(pHeightMap, x, z);

			glVertex3i(x, y, z);						// Send This Vertex To OpenGL To Be Rendered
		}
	glEnd();
	glEnable(GL_LIGHTING);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);					// Reset The Color
}