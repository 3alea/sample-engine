/// ---------------------------------------------------------------------------
/// \file			AEXShader.h
/// \author			Thomas Komair
/// \brief			Declares the interface for OpenGL shaders and programs.
/// ---------------------------------------------------------------------------
#ifndef AEX_SHADER_H_
#define AEX_SHADER_H_
// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "..\Core\AEXCore.h"

#pragma warning (disable:4251) // dll and STL
namespace AEX
{
	struct IEvent;
	struct AEMtx44;
	struct AEMtx33;
	struct AEVec2;
	struct AEVec3;
	struct Color;

	class  Shader : public IBase
	{
		AEX_RTTI_DECL(Shader, IBase);

	public: enum EType{ eVertex = 0, ePixel, eTypeCount };
	public:
		Shader();
		Shader(const char * filename, EType shaderType);
		void Load(const char * filename = NULL);
		void LoadSource(const char *filename = NULL);
		void Compile();

		EType GetShaderType(){ return mShaderType; }
		// this will recreate the opengl shader if necessary
		void SetShaderType(EType st, bool createDeviceShader = false);
		void CreateDeviceShader();

	private:
		friend class Graphics;
		friend class ShaderProgram;
		u32 mGLHandle;
		std::string	mSource;
		std::string mFilename;
		EType mShaderType;
	};

	class  ShaderProgram : public IBase
	{
		AEX_RTTI_DECL(ShaderProgram, IBase);

	public:
		ShaderProgram();
		ShaderProgram(Shader * vertexShader, Shader * pixelShader);

		void Bind();
		void Unbind();
		void Load();

		Shader* GetShader(Shader::EType shaderType);
		void SetShader(Shader * pShader, bool link = false);

		void SetShaderUniform(const char * name, const s32 *value, int count = 1);
		void SetShaderUniform(const char * name, const u32 *value, int count = 1);
		void SetShaderUniform(const char * name, const f32 *value, int count = 1);
		void SetShaderUniform(const char * name, const AEMtx44* value, int count = 1);
		void SetShaderUniform(const char * name, const AEMtx33 *value, int count = 1);
		void SetShaderUniform(const char * name, const AEVec2 *value, int count = 1);
		void SetShaderUniform(const char * name, const AEVec3 *value, int count = 1);
		void SetShaderUniform(const char * name, const Color *value, int count = 1);


		u32 GetOpenGLHandle();

	private:
		friend class Graphics;
		void CreateOpenGLObject();
		void CompileShadersAndLink();
		void Link();
		void CompileShaders();
		void AttachShader(Shader::EType which = Shader::eTypeCount);
		void DetachShader(Shader::EType which = Shader::eTypeCount);

		u32 mGLHandle;
		Shader * mShaders[Shader::eTypeCount];
	};
}
#pragma warning (default:4251) // dll and STL
//-----------------------------------------------------------------------------
#endif