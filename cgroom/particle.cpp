//#include "particle.h"
//void initPar(){
//	for (loop = 0; loop < MAX_PARTICLES; loop++)				// Initials All The Textures
//	{
//		particle[loop].active = true;								// Make All The Particles Active
//		particle[loop].life = 1.0f;								// Give All The Particles Full Life
//		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;	// Random Fade Speed
//		particle[loop].r = colors[loop*(12 / MAX_PARTICLES)][0];	// Select Red Rainbow Color
//		particle[loop].g = colors[loop*(12 / MAX_PARTICLES)][1];	// Select Red Rainbow Color
//		particle[loop].b = colors[loop*(12 / MAX_PARTICLES)][2];	// Select Red Rainbow Color
//		particle[loop].xi = float((rand() % 50) - 26.0f)*10.0f;		// Random Speed On X Axis
//		particle[loop].yi = float((rand() % 50) - 25.0f)*10.0f;		// Random Speed On Y Axis
//		particle[loop].zi = float((rand() % 50) - 25.0f)*10.0f;		// Random Speed On Z Axis
//		particle[loop].xg = 0.0f;									// Set Horizontal Pull To Zero
//		particle[loop].yg = -0.8f;								// Set Vertical Pull Downward
//		particle[loop].zg = 0.0f;									// Set Pull On Z Axis To Zero
//	}
//}