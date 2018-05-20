#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "obj.h"

using namespace std;

GLuint loadOBJ(char * filename,GLuint sel) {
	GLMmodel *glm_model;
	GLuint list;
	glm_model = glmReadOBJ(filename);
	glmUnitize(glm_model);
	glmScale(glm_model, 5);
	glmFacetNormals(glm_model);
	glmVertexNormals(glm_model, 90);
	list = glmList(glm_model, sel|GLM_TEXTURE|GLM_MATERIAL);
	glmDelete(glm_model);
	return list;
}
