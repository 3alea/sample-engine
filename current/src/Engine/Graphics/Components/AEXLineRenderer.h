#pragma once
#ifndef AEX_LINE_RENDERER_H_
#define AEX_LINE_RENDERER_H_

#include "AEXRenderable.h"
#include <aexmath\AEXMath.h>
#include "AEXColor.h"
namespace AEX
{
	class  LineRenderer : public Renderable
	{
		AEX_RTTI_DECL(LineRenderer, Renderable);

	public:
		LineRenderer(u32 maxLines = 2048);
		virtual ~LineRenderer();
		virtual void Initialize();
		virtual void Shutdown();
		virtual void Render();

		// Set max lines: this will initiate data transfer. it's sslow
		void SetMaxLines(u32 maxLines);

		// Add line
		void DrawLine(AEVec2 start, AEVec2 end, Color col = Color());
		void DrawCircle(AEVec2 center, f32 radius, f32 angle_start = 0, f32 angle_end = TWO_PI, Color c = Color());
		void DrawRect(AEVec2 p, AEVec2 size, Color c = Color());
		
		void DrawLine(float x0, float y0, float x1, float y1, Color col = Color());
		void DrawRect(float x, float y, float w, float h, Color col = Color());
		void DrawOrientedRect(float x, float y, float w, float h, float angle, Color col = Color());
		void DrawCircle(f32 cX, f32 cY, f32 radius, f32 angle_start = 0, f32 angle_end = TWO_PI, Color col = Color());

			void Flush();
	private:
		Model * mpLineBuffer;
		u32	mVtxCount;
		u32 mMaxLines;
	};
}

#endif