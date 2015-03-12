#ifndef UTILITY_H_
#define UTILITY_H_

#include "gl_core_4_4.h"

bool LoadShader(char* vertex_filename, 
				char* geometry_filename, 
				char* fragment_filename, 
				GLuint* result);

bool LoadShaderType(char* filename,	
						GLenum shader_type,
						unsigned int* output);

#endif //UTILITY_H_