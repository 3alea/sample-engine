/// -------------------------------------------------------------------
/// \file			AEXGfxSystem.cpp
/// \author			Thomas Komair
/// \brief			Implementation of the AEX graphics engine
/// -------------------------------------------------------------------

// ---------------------------------------------------------------------------
// INCLUDES & DEPENDENCIES
#include <aexmath\AEXMath.h>			// Maths
#include "..\Platform\AEXWindow.h"		// Window
#include "..\Debug\MyDebug.h"			// Debug
#include "..\Debug\CheckGLError.h"		// Debug OpenGL
#include "..\Platform\AEXFilePath.h"		// File path
#include "AEXGraphics.h"				// ALL GRAPHICS

// OpenGL libraries
#include "Extern\GL\glew.h"
#include "Extern\GL\wglew.h"
#include "Extern\GL\GL.h"

// ---------------------------------------------------------------------------

namespace AEX
{
	// ------------------------------------------------------------------------
	// OpenGL variables - move this to classes

	// GDI Device context and GL Render context
	namespace internal
	{
		static HDC		ghDeviceContext = 0;
		static HGLRC	ghRenderContext = 0;
		static GLint	gMajorVersion = 3;
		static GLint	gMinorVersion = 3;

		// Helper
		void LoadDefaultResources();

		// Line Renderer
		Model*			gLineMesh = NULL;
		ShaderProgram*	gLineShader = NULL;
		u32				gLineVtxCount = 0;
		const u32		MAX_LINES = 2048;
		void InitLineRenderer()
		{
			if (!gLineMesh) {
				gLineMesh = new Model(Model::eLineList);
				for (u32 i = 0; i < MAX_LINES * 2; ++i)
					gLineMesh->AddVertex(Vertex());
				gLineMesh->UploadToGPU();
			}

			if (!gLineShader) {
				gLineShader = aexGraphics->GetShaderProgram("Line.shader");
			}
		}
		void FreeLineRenderer()
		{
			if (gLineMesh)
			{
				delete gLineMesh;
				gLineMesh = NULL;
			}
			gLineShader = NULL;
		}
		void DrawLineData()
		{
			if (!gLineMesh || !gLineShader || !gLineVtxCount)
				return;

			// set shader model matrix to identity
			if (gLineShader)
			{
				auto w = aexWindowMgr->GetMainWindow()->GetWidth();
				auto h = aexWindowMgr->GetMainWindow()->GetHeight();
				auto proj = AEMtx44::OrthoProjGL(f32(w), f32(h), 0.01f, 1000.f);
				auto view = AEMtx44::Translate(0, 0, -20);
				auto view_proj = proj * view;

				gLineShader->Bind();
				gLineShader->SetShaderUniform("mtxViewProj", &view_proj);
				gLineShader->SetShaderUniform("mtxModel", &AEMtx44::Identity());
			}

			// Upload the data
			gLineMesh->ReloadToGPU(0, gLineVtxCount);

			// Draw
			gLineMesh->Draw(0, gLineVtxCount);

			// reset the number of vertices
			gLineVtxCount = 0;
		}

	} using namespace internal;


	// ---------------------------------------------------------------------------
	Graphics::Graphics()
	{}

	// ---------------------------------------------------------------------------
	bool Graphics::Initialize()
	{
		// Initialize OpenGL
		DebugErrorIf(InitializeOpenGL() == false, "OpenGL Failed to initialize");

		// Load default resources
		LoadDefaultResources();

		// Create line renderer
		InitLineRenderer();

		// No errors
		return true;
	}

	// Initialize
	bool Graphics::InitializeOpenGL()
	{
		// Check that main window exists
		Window* pWin = WindowManager::Instance()->GetMainWindow();
		if (!pWin)
			return false;

		// Get main window Win32 handle
		HWND hWin = pWin->GetHandle();

		// create render context
		ghDeviceContext = 0;
		ghRenderContext = 0;

		// get the device context
		if ((ghDeviceContext = GetDC(hWin)) == 0)
			return false;

		PIXELFORMATDESCRIPTOR pfd;

		memset(&pfd, 0, sizeof(pfd));

		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;
		int iFormat = ChoosePixelFormat(ghDeviceContext, &pfd);

		SetPixelFormat(ghDeviceContext, iFormat, &pfd);

		// Create fake context
		if ((ghRenderContext = wglCreateContext(ghDeviceContext)) == 0)
		{
			ReleaseDC(hWin, ghDeviceContext);
			ghDeviceContext = 0;
			return false;
		}

		// set the current openGL render context
		wglMakeCurrent(ghDeviceContext, ghRenderContext);

		//// initialize glew
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			//Problem: glewInit failed, something is seriously wrong.
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			return false;
		}

		// Define context attributes
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, gMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, gMinorVersion,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		// Check that GLEW is supported
		if (wglewIsSupported("WGL_ARB_create_context") == 1)
		{
			// Create a new context and delete the temporary one
			HGLRC temp = ghRenderContext;
			ghRenderContext = wglCreateContextAttribsARB(ghDeviceContext, 0, attribs);
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(temp);
			wglMakeCurrent(ghDeviceContext, ghRenderContext);
		}

		// Check for valid OpenGL version
		char strError[20];
		sprintf_s(strError, "GL_VERSION_%d_%d", gMajorVersion, gMinorVersion);
		if (!glewIsSupported(strError))
		{
			char sErrorMessage[255], sErrorTitle[255];
			sprintf_s(sErrorMessage, "OpenGL %d.%d is not supported! Please download latest GPU drivers!", gMajorVersion, gMinorVersion);
			sprintf_s(sErrorTitle, "OpenGL %d.%d Not Supported", gMajorVersion, gMinorVersion);
			MessageBox(hWin, sErrorMessage, sErrorTitle, MB_ICONINFORMATION);
			return false;
		}

		// disable backface removal
		glDisable(GL_CULL_FACE);

		// Enable blending
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Select which texture unit subsequent texture state calls will affect. 
		// by default, all textures will be applied on texture unit 0
		glActiveTexture(GL_TEXTURE0);

		// enable depth testing
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		check_gl_error();
		
		return true;
	}
	void Graphics::TerminateOpenGL()
	{
		// TODO delete the data


		// release the context
		wglMakeCurrent(NULL, NULL);
		if (ghRenderContext)
		{
			wglDeleteContext(ghRenderContext);
			ghRenderContext = NULL;
		}
	}

	// ---------------------------------------------------------------------------
	Graphics::~Graphics()
	{
		FreeLineRenderer();
		TerminateOpenGL();
	}

	// TODO(Thomas): Implement this for next lesson
	//void Graphics::Render(bool clear, bool present)
	//{
	//	// Disable face culling for backface triangles. 
	//	glDisable(GL_CULL_FACE);

	//	// Clear the whole frame buffer to black
	//	u32 winW = aexWindowMgr->GetMainWindow()->GetWidth();
	//	u32 winH = aexWindowMgr->GetMainWindow()->GetHeight(); 

	//	// clear if necessary
	//	if (clear){
	//		glViewport(0, 0, winW, winH); // set viewport (left, bot, right, top).
	//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // set clear color to black
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	}
	//	// enable scissor test (to define viewport clipping region). 
	//	glEnable(GL_SCISSOR_TEST);

	//	// start rendering of cameras
	//	FOR_EACH(camIt, mCameras)
	//	{
	//		// get the camrea
	//		Camera* pCam = (*camIt);
	//		if (pCam->IsEnabled() == false) // skip if not enabled.
	//			continue;

	//		// get viewport dimensions from current
	//		// camera
	//		Viewport vp = pCam->GetViewport();
	//		GLsizei w = (GLsizei)vp.GetRect().x;
	//		GLsizei h = (GLsizei)vp.GetRect().y;
	//		GLint lef = (GLint)vp.GetPosition().x;
	//		GLint bot = (GLint)vp.GetPosition().y;

	//		// set camera viewport in OpenGL
	//		glViewport(lef, bot, w, h);check_gl_error();
	//		glScissor(lef, bot, w, h);check_gl_error();

	//		// clear the viewport if necessary
	//		if (pCam->ClearBG()){
	//			Color c = pCam->GetViewport().GetColor();
	//			glClearColor(c.r, c.g, c.b, c.a);check_gl_error();
	//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);check_gl_error();
	//		}

	//		// set this cam as the current camera and update it
	//		mCurrentCamera = pCam;
	//		mCurrentCamera->Update();

	//		// Compute view matrix and adapt it to openGL
	//		AEMtx44 view = mCurrentCamera->ComputeViewMatrix();

	//		// Compute projection using helper function
	//		AEMtx44 proj = mCurrentCamera->ComputeProjectionMatrix();

	//		// Concatenate
	//		AEMtx44 MVP = proj * view;

	//		// Draw the renderables
	//		FOR_EACH(renderIt, mRenderables)
	//		{
	//			if ((*renderIt)->mIsVisible && (*renderIt)->IsEnabled())
	//			{
	//				if ((*renderIt)->pShaderRes)
	//				{
	//					(*renderIt)->pShaderRes->Bind();
	//					(*renderIt)->pShaderRes->SetShaderUniform("mtxViewProj", &MVP);
	//				}
	//				(*renderIt)->Render();
	//			}
	//		}

	//		// Draw the debug lines (once per camera)
	//		if (mLineRenderer->pShaderRes)
	//		{
	//			// set debug drawer shader data
	//			mLineRenderer->pShaderRes->Bind();
	//			mLineRenderer->pShaderRes->SetShaderUniform("mtxViewProj", &MVP);
	//			// render line renderer contents
	//			mLineRenderer->Render();
	//		}
	//	} // END LOOP: For each camera. 

	//	// Reset line renderer for next frame. 
	//	mLineRenderer->Flush();

	//	// Test
	//	glDisable(GL_SCISSOR_TEST);

	//	if (present)// swap the frame buffers
	//		Present();
	//}
	void Graphics::SetViewport(s32 left, s32 bottom, s32 right, s32 top)
	{
		glScissor(left, bottom, abs(right - left), abs(top - bottom));
		glViewport(left, bottom, abs(right-left), abs(top-bottom));
	}
	void Graphics::SetClearColor(const Color & col)
	{
		glClearColor(col.r, col.g, col.b, col.a); // set clear color to black
	}
	void Graphics::ClearFrameBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Graphics::Present()
	{
		// DrawLine data at the end
		DrawLineData();

		SwapBuffers(ghDeviceContext);
	}
	// ------------------------------------------------------------------------
	// DEBUG DRAWING METHODS

	// ------------------------------------------------------------------------
	void Graphics::DrawLine(AEVec2 p0, AEVec2 p1, Color col)
	{

		if (gLineVtxCount >= (MAX_LINES * 2))
			return;

		gLineMesh->SetVertexPos(gLineVtxCount, p0);
		gLineMesh->SetVertexColor(gLineVtxCount++, col);
		gLineMesh->SetVertexPos(gLineVtxCount, p1);
		gLineMesh->SetVertexColor(gLineVtxCount++, col);
	}
	void Graphics::DrawLine(float x0, float y0, float x1, float y1, Color col)
	{
		DrawLine(AEVec2(x0, y0), AEVec2(x1, y1), col);
	}
	void Graphics::DrawRect(float x, float y, float w, float h, Color c)
	{
		AEVec2 p = { x,y };
		AEVec2 s = { w,h };

		AEX::AEVec2 top_left = p + AEX::AEVec2(-s.x / 2.0f, s.y / 2.0f);
		AEX::AEVec2 top_right = p + AEX::AEVec2(s.x / 2.0f, s.y / 2.0f);
		AEX::AEVec2 bot_left = p + AEX::AEVec2(-s.x / 2.0f, -s.y / 2.0f);
		AEX::AEVec2 bot_right = p + AEX::AEVec2(s.x / 2.0f, -s.y / 2.0f);

		DrawLine(top_left, top_right, c);
		DrawLine(top_right, bot_right, c);
		DrawLine(bot_left, bot_right, c);
		DrawLine(bot_left, top_left, c);
	}
	void Graphics::DrawOrientedRect(float x, float y, float w, float h, float angle, Color col)
	{
		AEVec2 axis_x; axis_x.FromAngle(angle);
		AEVec2 axis_y = axis_x.Perp();

		axis_x *= w * 0.5f;
		axis_y *= h * 0.5f;

		// get corners
		AEVec2 pos = { x, y };
		AEVec2 tR = pos + axis_x + axis_y;
		AEVec2 tL = pos - axis_x + axis_y;
		AEVec2 bR = pos + axis_x - axis_y;
		AEVec2 bL = pos - axis_x - axis_y;

		// draw as obb
		DrawLine(tR, tL);
		DrawLine(tL, bL);
		DrawLine(bL, bR);
		DrawLine(bR, tR);
	}
	void Graphics::DrawCircle(f32 cX, f32 cY, f32 radius, Color col, f32 angle_start, f32 angle_end)
	{
		AEVec2 center = { cX, cY };
		f32 iterations = 24;
		if (angle_start > angle_end)
			angle_end += TWO_PI;

		f32 angle = angle_start;
		f32 angle_range = angle_end - angle_start;
		f32 angle_inc = angle_range / (f32)iterations;
		AEVec2 v1, v0;
		for (u32 i = 0; i < iterations; ++i, angle += angle_inc)
		{
			// TODO(Thomas): This is pretty slow... 
			v0 = center + AEVec2(cos(angle), sin(angle)) * radius;
			v1 = center + AEVec2(cos(angle + angle_inc), sin(angle + angle_inc)) * radius;
			DrawLine(v0, v1, col);
		}
	}


	// ------------------------------------------------------------------------
	// OPENGL METHODS
	
	// ------------------------------------------------------------------------

	bool Graphics::GetDepthTestEnabled()
	{
		GLboolean enabled;
		glGetBooleanv(GL_DEPTH_TEST, &enabled);
		return enabled != 0;
	}
	void Graphics::SetDepthTestEnabled(bool enabled)
	{
		bool currentEnabled = GetDepthTestEnabled();
		if (currentEnabled == enabled)
			return;
		if (enabled == true)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	bool Graphics::GetVSyncEnabled()
	{
		return mbVSyncEnabled;
	}
	void Graphics::SetVSyncEnabled(bool enabled)
	{
		if (enabled == mbVSyncEnabled)return;
		mbVSyncEnabled = enabled;
		wglSwapIntervalEXT(enabled ? 1 : 0);
	}
	#pragma region Resource Management 
	// ------------------------------------------------------------------------
	AEX_PTR_DICTIONARY(Shader) sShaders;
	AEX_PTR_DICTIONARY(ShaderProgram) sShaderProgs;
	AEX_PTR_DICTIONARY(Texture) sTextures;
	AEX_PTR_DICTIONARY(Model) sModels;
	AEX_DICTIONARY(void*) sResources;
	template <typename T>
	T* AddResource(T* res, const char * resName = NULL)
	{
		// FIRST CALL - THIS IS A HACK (LAZY INIT)
		// This can also be done pre-main.
		if (sResources.empty())
		{
			sResources[Shader::TYPE().GetName()] = (void*)&sShaders;
			sResources[ShaderProgram::TYPE().GetName()] = (void*)&sShaderProgs;
			sResources[Texture::TYPE().GetName()] = (void*)&sTextures;
			sResources[Model::TYPE().GetName()] = (void*)&sModels;
		}

		// cast to base! Note: C++ rtti will make sure that res is base. 
		IBase * res_base = dynamic_cast<IBase*>(res);

		// name
		if (NULL == resName)
			resName = res_base->GetName();
		
		// typedef for resource map type
		typedef AEX_PTR_DICTIONARY(T) RM_TYPE;

		// find resourceMap and add the resource
		AEX_DICTIONARY(void*)::iterator it = sResources.find(T::TYPE().GetName());
		if (it != sResources.end())
		{
			//  cast the resMap
			RM_TYPE* resMap = (RM_TYPE*)(it->second);

			//@sanity check
			DebugErrorIf(resMap == NULL, "ERROR");
			
			// add the resource
			if (resMap)
			{
				(*resMap)[resName] = res;
			}
		}
		return res;
	}

	template <typename T>
	T * GetResouce(const char * resName)
	{
		// typedef for resource map type
		typedef AEX_PTR_DICTIONARY(T) RM_TYPE;

		// find resourceMap and add the resource
		AEX_DICTIONARY(void*)::iterator it = sResources.find(T::TYPE().GetName());
		if (it != sResources.end())
		{
			//  cast the resMap
			RM_TYPE* resMap = (RM_TYPE*)(it->second);

			// return the resource it exist
			if (resMap && (*resMap).find(resName) != (*resMap).end()){
				return (*resMap)[resName];
			}
		}
		// return null otherwise
		return NULL;
	}

	template <typename T>
	void RemoveResource(T * res)
	{
		// Sanity check
		if (NULL == res)
			return;

		// typedef for resource map type
		typedef AEX_PTR_DICTIONARY(T) RM_TYPE;

		// find resourceMap and add the resource
		AEX_DICTIONARY(void*)::iterator it = sResources.find(T::TYPE().GetName());
		if (it != sResources.end())
		{
			//  cast the resMap
			RM_TYPE* resMap = (RM_TYPE*)(it->second);

			// return the resource it exist
			RM_TYPE::iterator resourceIt = (*resMap).find(res->GetName());
			if (resMap && resourceIt != (*resMap).end()){
				resMap->erase(resourceIt);
			}
		}
	}

	ShaderProgram * Graphics::LoadShaderProgram(const char * shaderFile, Shader* pix, Shader * vert)
	{
		FilePath fp(shaderFile);
		std::string resName = fp.mFilename + fp.mExtension;
		ShaderProgram * shaderProg = new ShaderProgram; shaderProg->SetName(resName.c_str());
		shaderProg->SetShader(pix);
		shaderProg->SetShader(vert, true);

		// to resrouces
		return AddResource<ShaderProgram>(shaderProg);
	}
	Texture *		Graphics::LoadTexture(const char * textureFile)
	{
		FilePath fp(textureFile);
		std::string resName = fp.mFilename + fp.mExtension;
		Texture * tex = new Texture(); tex->SetName(resName.c_str());
		tex->LoadFromFile(textureFile);

		// to resrouces
		return AddResource<Texture>(tex);
	}
	Model *			Graphics::LoadModel(const char * modelFile)
	{
		FilePath fp(modelFile);
		std::string modelName = fp.mFilename + fp.mExtension;
		Model * pModel = new Model(); pModel->SetName(modelName.c_str());

		// by default, we're going to make a quad
		// Create a quad by default
		f32 h = 0.5f;
		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, h), AEX::AEVec2(0, 1), AEX::Color(1, 0, 0)));
		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, -h), AEX::AEVec2(0, 0), AEX::Color(1, 1, 0)));
		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, -h), AEX::AEVec2(1, 0), AEX::Color(0, 1, 0)));

		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(-h, h), AEX::AEVec2(0, 1), AEX::Color(1, 0, 0)));
		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, -h), AEX::AEVec2(1, 0), AEX::Color(0, 1, 0)));
		pModel->AddVertex(AEX::Vertex(AEX::AEVec2(h, h), AEX::AEVec2(1, 1), AEX::Color(0, 0, 1)));
		pModel->UploadToGPU();

		// to resrouces
		return AddResource<Model>(pModel);
	}
	Shader *		Graphics::LoadShader(const char * shaderFile)
	{
		// sanity check
		if (!shaderFile)
			return NULL;

		// get path
		FilePath fp(shaderFile);

		// determing shader type (vertex or fragment).
		bool isFrag = fp.mExtension == ".frag";
		Shader::EType shader_type = isFrag ? Shader::ePixel : Shader::eVertex;

		// create shader and init
		std::string resName = fp.mFilename + fp.mExtension;
		Shader * pShader = new Shader; pShader->SetName(resName.c_str());
		if (pShader !=  NULL){
			pShader->SetShaderType(shader_type, true);	// set shader type
			pShader->Load(shaderFile);	// load
		}

		// to resrouces
		return AddResource<Shader>(pShader);
	}

	Shader*			Graphics::GetShader(const char * shaderName)
	{
		return GetResouce<Shader>(shaderName);
	}
	ShaderProgram * Graphics::GetShaderProgram(const char * shaderProgName)
	{
		return GetResouce<ShaderProgram>(shaderProgName);

	}
	Texture *		Graphics::GetTexture(const char * textureName)
	{
		return GetResouce<Texture>(textureName);

	}
	Model *			Graphics::GetModel(const char * modelName)
	{
		return GetResouce<Model>(modelName);

	}


	void			Graphics::DeleteModel(Model * model)
	{
		if (model)
		{
			// remove model from resolurces
			RemoveResource(model);
			delete model;
		}
	}
	void			Graphics::DeleteTexture(Texture * tex)
	{
		if (tex)
		{
			// remove model from resolurces
			RemoveResource(tex);
			delete tex;
		}
	}
	void			Graphics::DeleteShader(Shader * shader)
	{
		if (shader)
		{
			// remove model from resolurces
			RemoveResource(shader);
			delete shader;
		}
	}
	void			Graphics::DeleteShaderProgram(ShaderProgram * prog)
	{
		if (prog)
		{
			// remove model from resolurces
			RemoveResource(prog);
			delete prog;
		}
	}

	void			Graphics::DeleteModel(const char * modelName)
	{
		DeleteModel(GetModel(modelName));
	}
	void			Graphics::DeleteTexture(const char * texName)
	{
		DeleteModel(GetModel(texName));

	}
	void			Graphics::DeleteShader(const char * shaderName)
	{
		DeleteModel(GetModel(shaderName));

	}
	void			Graphics::DeleteShaderProgram(const char * progName)
	{
		DeleteModel(GetModel(progName));

	}
	#pragma endregion

	namespace internal
	{
		void LoadDefaultResources()
		{
			auto vert = aexGraphics->LoadShader(".\\data\\Shaders\\VertexTransform.vert");
			auto frag = aexGraphics->LoadShader(".\\data\\Shaders\\Line.frag");
			aexGraphics->LoadShaderProgram(".\\data\\Shaders\\Line.shader", vert, frag);

			frag = aexGraphics->LoadShader(".\\data\\Shaders\\TextureMap.frag");
			aexGraphics->LoadShaderProgram(".\\data\\Shaders\\TextureMap.shader", vert, frag);

			frag = aexGraphics->LoadShader(".\\data\\Shaders\\VertexColor.frag");
			aexGraphics->LoadShaderProgram(".\\data\\Shaders\\VertexColor.shader", vert, frag);
		}
	}
}// namespace AEX
