#include <cstdio>
#include "gl_core_4_4.h"

bool LoadShader(char* vertex_filename, char* fragment_filename, GLuint* result)
{
	bool succeeded = true;

	FILE* vertex_file = fopen( vertex_filename, "r" );
	FILE* fragment_file = fopen( fragment_filename, "r" );

	if ( vertex_file == 0 || fragment_file == 0 )
	{
		succeeded = false;
	}
	else
	{
		fseek(vertex_file, 0, SEEK_END);
		int vertex_file_length = ftell(vertex_file);
		fseek(vertex_file, 0, SEEK_SET);

		fseek(fragment_file, 0, SEEK_END);
		int fragment_file_length = ftell(fragment_file);
		fseek(fragment_file, 0, SEEK_SET);

		char *vs_source = new char[vertex_file_length];
		char *fs_source = new char[fragment_file_length];

		vertex_file_length = fread(vs_source, 1, vertex_file_length, vertex_file);
		fragment_file_length = fread(fs_source, 1, fragment_file_length, fragment_file);

		int success = GL_FALSE;
		int log_length = 0;

		unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex_shader, 1, &vs_source, &vertex_file_length);
		glCompileShader(vertex_shader);

		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
			printf("%s\n", log);
			delete[] log;
			succeeded = false;
		}

		glShaderSource(fragment_shader, 1, &fs_source, &fragment_file_length);
		glCompileShader(fragment_shader);

		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
			printf("%s\n", log);
			delete[] log;
			succeeded = false;
		}

		*result = glCreateProgram();
		glAttachShader(*result, vertex_shader);
		glAttachShader(*result, fragment_shader);
		glLinkProgram(*result);

		glGetProgramiv(*result, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);

			char* log = new char[log_length];
			glGetProgramInfoLog(*result, log_length, 0, log);

			printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n\n");
			printf("%s", log);

			delete[] log;
			succeeded = false;
		}

		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);

		delete[] vs_source;
		delete[] fs_source;
	}

	fclose(vertex_file);
	fclose(fragment_file);

	return succeeded;
}