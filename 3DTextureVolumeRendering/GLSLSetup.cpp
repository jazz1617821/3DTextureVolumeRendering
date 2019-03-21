
#include "GLSLSetup.h"

#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

///*
#define printOpenGLError() printOglError(__FILE__, __LINE__)

GLenum Program[5] = {0};
int ProgramAmount = 0;

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}
//*/
char* TextRead(char* filename)
{
	FILE* fptr = fopen(filename,"r");
	char* buffer = 0;
	unsigned int size;
	if(fptr == NULL){
	}else{
		fseek(fptr,0,SEEK_END);
		size = ftell(fptr);
		buffer = new char[size+1];
		//rewind to load file
		rewind(fptr);
		size = fread(buffer,sizeof(char),size,fptr);
		buffer[size] = '\0';
		fclose(fptr);
	}
	return buffer;
}

void printInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,&infologLength);
	printf("*************** output ***************\n");
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
	printf("**************************************\n");
}

void SetGLShaderLanguage(char* VertexShaderFileName ,char* FragmentShaderFileName)
{
	GLenum VertexShader = 0,FragmentShader = 0,GeometryShader = 0;
	char* VSContext;
	char* FSContext;
	//create shader program
	Program[ProgramAmount] = glCreateProgramObjectARB();
	//create shader handers
	VertexShader   = glCreateShaderObjectARB(GL_VERTEX_SHADER);
	FragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
	//GeometryShader = glCreateShaderObjectARB(GL_GEOMETRY_SHADER);
	//read progran context to file buffer
	VSContext = TextRead(VertexShaderFileName);
	FSContext = TextRead(FragmentShaderFileName);
	//GSContext = TextRead(GeometryShaderFileName);
	if(VSContext == 0 || FSContext == 0){
		printf("No File . Load Shader failure. \n");
		exit(0);
	}
	printf("Vertex   Shader Context : [\n%s\n]\n",VSContext);
	printf("Fragment Shader Context : [\n%s\n]\n",FSContext);
	//printf("Geometry Shader Context : [\n%s\n]\n",GSContext);
	//give source to shader
	glShaderSourceARB(VertexShader,1,(const GLchar**)&VSContext,NULL);
	glShaderSourceARB(FragmentShader,1,(const GLchar**)&FSContext,NULL);
	//Complier all shader
	glCompileShaderARB(VertexShader);
	glCompileShaderARB(FragmentShader);
	printInfoLog(VertexShader);
	printInfoLog(FragmentShader);
	//attach shaders to program
	glAttachObjectARB(Program[ProgramAmount],VertexShader);
	glAttachObjectARB(Program[ProgramAmount],FragmentShader);
	//Line and set shader program
	glLinkProgramARB(Program[ProgramAmount]);
	glUseProgramObjectARB(Program[ProgramAmount]);
	ProgramAmount++;
}

int ReturnProgramID(int ProgramIndex)
{
	return (int)Program[ProgramIndex];
}