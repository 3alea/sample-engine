/// ---------------------------------------------------------------------------
/// \file			AEXGfxSystem.h
/// \author			Thomas Komair
/// \brief			Declare the interface for the AEX graphics engine
/// ---------------------------------------------------------------------------
#ifndef AEX_GRAPHICS_SYSTEM_H_
#define AEX_GRAPHICS_SYSTEM_H_

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include "..\Core\AEXCore.h"
#include <aexmath\AEXMathDefs.h>	// TWO_PI
#include <aexmath\AEXVec2.h>	// TWO_PI
#include <aexmath\AEXVec3.h>	// TWO_PI
#include "AEXColor.h"		// Color

#pragma warning (disable:4251) // dll and STL
namespace AEX
{
	// Forward Declarations of Graphics related classes
	class Renderable;
	class LineRenderer;
	class Camera;
	class Texture;
	class Shader;
	class ShaderProgram;
	class Model;

	// ---------------------------------------------------------------------------
	// CLASS: Graphics: Main system for the graphics engine
	class  Graphics : public ISystem
	{
		AEX_RTTI_DECL(Graphics, ISystem);
		AEX_SINGLETON(Graphics);
	public:

		// ISystem Inherited
		virtual bool Initialize();
		virtual ~Graphics();

		void SetViewport(s32 left, s32 bottom, s32 right, s32 top);
		void SetClearColor(const Color & col);
		void ClearFrameBuffer();
		void Present();

		// Rendererer porpoerties
		bool GetDepthTestEnabled();
		void SetDepthTestEnabled(bool enabled);
		bool GetVSyncEnabled();
		void SetVSyncEnabled(bool enabled);

		// Debug Draw
		void DrawLine(AEVec2 p0, AEVec2 p1, Color col = AEX::Color());
		void DrawLine(float x0, float y0, float x1, float y1, AEX::Color col = AEX::Color());
		void DrawRect(float x, float y, float w, float h, Color col = AEX::Color());
		void DrawOrientedRect(float x, float y, float w, float h, float angle, Color col = AEX::Color());
		void DrawCircle(f32 cX, f32 cY, f32 radius, Color col = AEX::Color(), f32 angle_start = 0, f32 angle_end = TWO_PI);


		#pragma region// Basic Resource Management
		//
		// TODO(Thomas): Move this to resource management system.
		//
		Model *			LoadModel(const char * modelFile);
		Texture *		LoadTexture(const char * textureFile);
		Shader *		LoadShader(const char * shaderFile);
		ShaderProgram * LoadShaderProgram(const char * shaderFile, Shader* pix, Shader * vert);

		Model *			GetModel(const char * modelName);
		Texture *		GetTexture(const char * textureName);
		Shader*			GetShader(const char * shaderName);
		ShaderProgram * GetShaderProgram(const char * shaderName);

		void			DeleteModel(Model * model);
		void			DeleteTexture(Texture * tex);
		void			DeleteShader(Shader * shader);
		void			DeleteShaderProgram(ShaderProgram * prog);

		void			DeleteModel(const char * modelName);
		void			DeleteTexture(const char * texName);
		void			DeleteShader(const char * shaderName);
		void			DeleteShaderProgram(const char * progName);
		#pragma endregion

	private:

		bool InitializeOpenGL();
		void TerminateOpenGL();

		bool						mbVSyncEnabled = true;
	};
}
#pragma warning (default:4251) // dll and STL

// Easy access to singleton
#define aexGraphics (Graphics::Instance())

// ---------------------------------------------------------------------------
#endif