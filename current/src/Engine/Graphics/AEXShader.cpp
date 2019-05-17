/// ---------------------------------------------------------------------------
/// \file			AEXTexture.cpp
/// \author			Thomas Komair
/// \brief			Implementation of the texture interface
/// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "AEXGfxSystem.h"
#include "AEXShader.h"
#include <aexmath\AEXMath.h>
#include "..\Debug\MyDebug.h"
#include "..\Debug\CheckGLError.h"

#include "Extern\GL\glew.h"
#include "Extern\GL\wglew.h"
#include "Extern\GL\GL.h"

#include <fstream>
#include <sstream>

namespace AEX
{
	Shader::Shader()
		: mShaderType(eTypeCount)
		, mGLHandle(0)
	{}
	Shader::Shader(const char * filename, EType shaderType)
		: mShaderType(shaderType)
	{
		mFilename = filename;

		// Load
		Load();
	}
	void Shader::LoadSource(const char * filename)
	{
		if (filename)
			mFilename = filename;
		// open the shader file & load its content
		std::fstream shaderFile(mFilename.c_str(), std::ios::in);
		std::stringstream shaderSource;
		shaderSource << shaderFile.rdbuf();

		// save
		mSource = shaderSource.str();
	}
	void Shader::Compile()
	{
		if (mSource.empty())
			return;

		// create shader (delete if necessary)
		// NOTE(Thomas):this is here because the shader might not have been 
		// created yet (when calling default constructor for eample)
		bool created = mGLHandle != 0;
		SetShaderType(mShaderType, !created);

		// pass the code to OpenGl
		const char * shaderSourceStr = mSource.c_str();
		glShaderSource(mGLHandle, 1, &shaderSourceStr, NULL);

		// compile
		glCompileShader(mGLHandle);

		// sanity check
		GLint result;
		glGetShaderiv(mGLHandle, GL_COMPILE_STATUS, &result);
		if (GL_FALSE == result)
		{
			// error log
			GLint logLen;
			glGetShaderiv(mGLHandle, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0)
			{
				char * log = (char *)malloc(logLen);
				GLsizei written;
				glGetShaderInfoLog(mGLHandle, logLen, &written, log);
				char str[100];
				sprintf_s(str, "Shader Compilation Error: %s", mFilename.c_str());
				MessageBoxA(NULL, log, str, MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR);
				free(log);
			}
		}

	}
	void Shader::Load(const char * filename)
	{
		LoadSource(filename);
		Compile();
	}
	void Shader::CreateDeviceShader()
	{
		if (mGLHandle)
			glDeleteShader(mGLHandle);
		// create a new shader
		GLenum type = mShaderType == eVertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
		mGLHandle = glCreateShader(type);
	}
	void Shader::SetShaderType(Shader::EType shaderType, bool createDeviceShader)
	{
		// store type
		mShaderType = shaderType;

		// new shader otherwise - note that if the shader is attached
		// to a shader program, this program might stop working properly.
		// because the shader is no longer valid. 
		if (createDeviceShader)
			CreateDeviceShader();
	}
	ShaderProgram::ShaderProgram()
		: mGLHandle(0)
	{
		// create the shader program
		mGLHandle = glCreateProgram();
		std::memset(mShaders, NULL, sizeof(Shader*)*Shader::eTypeCount);
	}
	ShaderProgram::ShaderProgram(Shader * vertexShader, Shader * pixelShader)
		: ShaderProgram()
	{
		// detach shaders first
		for (u32 i = 0; i < Shader::eTypeCount; ++i){
			if (mShaders[i]){
				glDetachShader(mGLHandle, mShaders[i]->mGLHandle);
			}
		}
		// set shader poitners
		if (vertexShader)
			mShaders[vertexShader->mShaderType] = vertexShader;
		if (pixelShader)
			mShaders[pixelShader->mShaderType] = pixelShader;

		// Link with shaders right away
		Link();
	}
	void ShaderProgram::Link()
	{
		for (u32 i = 0; i < Shader::eTypeCount; ++i){
			if (mShaders[i]){
				glAttachShader(mGLHandle, mShaders[i]->mGLHandle);
			}
		}

		// link the shader
		glLinkProgram(mGLHandle);

		// error check
		GLint status;
		glGetProgramiv(mGLHandle, GL_LINK_STATUS, &status);
		if (GL_FALSE == status)
		{
			GLint logLen;
			glGetProgramiv(mGLHandle, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0)
			{
				char * log = (char *)malloc(logLen);
				GLsizei written;
				glGetProgramInfoLog(mGLHandle, logLen, &written, log);
				MessageBoxA(NULL, log, "Shader Linking Error", MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR);
				free(log);
			}
		}

		// detach shaders
		DetachShader();
	}
	void ShaderProgram::Bind()
	{
		glUseProgram(mGLHandle);
	}
	void ShaderProgram::Unbind()
	{
		glUseProgram(NULL);
	}
	void ShaderProgram::Load()
	{
		for (u32 i = 0; i < Shader::eTypeCount; ++i){
			if (mShaders[i]){
				mShaders[i]->Load();
			}
		}

		Link();
	}

	Shader* ShaderProgram::GetShader(Shader::EType shaderType)
	{
		if (shaderType < Shader::eTypeCount)
			return mShaders[shaderType];
		return NULL;
	}
	void ShaderProgram::SetShader(Shader * pShader, bool link)
	{
		if (!pShader)
			return;

		Shader::EType & shaderType = pShader->mShaderType;

		// set shader
		DetachShader(shaderType);
		mShaders[shaderType] = pShader;

		// link if necessary
		if (link)
			Link();
	}
	void ShaderProgram::AttachShader(Shader::EType shaderType)//deflt = all
	{
		// specific shadertype ->set if necessary
		if (shaderType != Shader::eTypeCount){
			if (mShaders[shaderType]){
				glAttachShader(mGLHandle, mShaders[shaderType]->mGLHandle);
			}
		}
		else
		{
			for (int i = 0; i < Shader::eTypeCount; ++i){
				if (mShaders[i]){
					glAttachShader(mGLHandle, mShaders[i]->mGLHandle);
				}
			}
		}
	}
	void ShaderProgram::DetachShader(Shader::EType shaderType)//deflt = all
	{
		// specific shadertype ->set if necessary
		if (shaderType != Shader::eTypeCount){
			if (mShaders[shaderType]){
				glDetachShader(mGLHandle, mShaders[shaderType]->mGLHandle);
			}
		}
		else
		{
			for (int i = 0; i < Shader::eTypeCount; ++i){
				if (mShaders[i]){
					glDetachShader(mGLHandle, mShaders[i]->mGLHandle);
				}
			}
		}
	}

	u32 ShaderProgram::GetOpenGLHandle()
	{
		return mGLHandle;
	}

	void ShaderProgram::SetShaderUniform(const char * name, const s32 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		glUniform1iv(loc, count, value);
	}
	void ShaderProgram::SetShaderUniform(const char * name, const u32 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		glUniform1uiv(loc, count, value);
	}
	void ShaderProgram::SetShaderUniform(const char * name, const f32 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniform1fv(loc, count, value);
		check_gl_error();
	}
	void ShaderProgram::SetShaderUniform(const char * name, const AEMtx44* value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniformMatrix4fv(loc, count, GL_TRUE, value->v);
		check_gl_error();
	}
	void ShaderProgram::SetShaderUniform(const char * name, const AEMtx33 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniformMatrix3fv(loc, count, GL_TRUE, value->v);
		check_gl_error();
	}
	void ShaderProgram::SetShaderUniform(const char * name, const AEVec2 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniform2fv(loc, count, value->v);
		check_gl_error();
	}
	void ShaderProgram::SetShaderUniform(const char * name, const AEVec3 *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniform3fv(loc, count, value->v);
		check_gl_error();
	}
	void ShaderProgram::SetShaderUniform(const char * name, const Color *value, int count)
	{
		GLint loc = glGetUniformLocation(mGLHandle, name);
		check_gl_error();
		glUniform4fv(loc, count, value->v);
		check_gl_error();
	}

}