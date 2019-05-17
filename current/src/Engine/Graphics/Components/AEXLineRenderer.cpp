#include "AEXLineRenderer.h"
#include "AEXModel.h"
#include "AEXShader.h"

namespace AEX
{
	LineRenderer::LineRenderer(u32 maxLines)
		: Renderable()
		, mMaxLines(maxLines)
		, mVtxCount(0)
		, mpLineBuffer(0)
	{
		// allocate new model
		mpLineBuffer = new Model(Model::eLineList);
		for (u32 i = 0; i < mMaxLines*2; ++i)
			mpLineBuffer->AddVertex(Vertex());
		mpLineBuffer->UploadToGPU();
	}

	LineRenderer::~LineRenderer()
	{
		if (mpLineBuffer)
			delete mpLineBuffer;
	}

	void LineRenderer::Initialize()
	{
		Renderable::Initialize();
	}
	void LineRenderer::Shutdown()
	{
		Renderable::Shutdown();
	}
	void LineRenderer::Render()
	{
		if (!mpLineBuffer)
			return;

		// set shader model matrix to identity
		if (pShaderRes)
		{
			pShaderRes->Bind();
			pShaderRes->SetShaderUniform("mtxModel", &AEMtx44::Identity());
		}

		// Upload the data
		mpLineBuffer->ReloadToGPU(0, mVtxCount);

		// Draw
		mpLineBuffer->Draw(0, mVtxCount);


		// Flush
		Flush();
	}
	void LineRenderer::DrawLine(AEVec2 start, AEVec2 end, Color col)
	{
		if (mVtxCount >= (mMaxLines * 2))
			return;

		mpLineBuffer->SetVertexPos(mVtxCount, start);
		mpLineBuffer->SetVertexColor(mVtxCount++, col);
		mpLineBuffer->SetVertexPos(mVtxCount, end);
		mpLineBuffer->SetVertexColor(mVtxCount++, col);
	}
	void LineRenderer::DrawCircle(AEVec2 center, f32 radius, f32 angle_start, f32 angle_end, Color col)
	{
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
	void LineRenderer::DrawRect(AEVec2 p, AEVec2 size, Color c)
	{
		AEX::AEVec2 top_left = p + AEX::AEVec2(-size.x / 2.0f, size.y / 2.0f);
		AEX::AEVec2 top_right = p + AEX::AEVec2(size.x / 2.0f, size.y / 2.0f);
		AEX::AEVec2 bot_left = p + AEX::AEVec2(-size.x / 2.0f, -size.y / 2.0f);
		AEX::AEVec2 bot_right = p + AEX::AEVec2(size.x / 2.0f, -size.y / 2.0f);

		DrawLine(top_left, top_right, c);
		DrawLine(top_right, bot_right, c);
		DrawLine(bot_left, bot_right, c);
		DrawLine(bot_left, top_left, c);
	}

	void LineRenderer::DrawLine(float x0, float y0, float x1, float y1, Color col)
	{
		DrawLine(AEVec2(x0, y0), AEVec2(x1, y1), col);
	}
	void LineRenderer::DrawRect(float x, float y, float w, float h, Color col)
	{
		DrawRect(AEVec2(x, y), AEVec2(w, h), col);
	}
	void LineRenderer::DrawOrientedRect(float x, float y, float w, float h, float angle, Color col)
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
	void LineRenderer::DrawCircle(f32 cX, f32 cY, f32 radius, f32 angle_start, f32 angle_end, Color col)
	{
		DrawCircle(AEVec2(cX, cY), radius, angle_start, angle_end, col);
	}
	void LineRenderer::Flush()
	{
		mVtxCount = 0;
	}

	void LineRenderer::SetMaxLines(u32 maxLines)
	{
		mMaxLines = maxLines;
		if (mpLineBuffer)
			delete mpLineBuffer;
			mpLineBuffer = new Model(Model::eLineList);
		for (u32 i = 0; i < mMaxLines * 2; ++i)
			mpLineBuffer->AddVertex(Vertex());
		mpLineBuffer->UploadToGPU();
	}
}	