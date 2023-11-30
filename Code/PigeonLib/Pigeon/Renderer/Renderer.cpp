#include "pch.h"
#include "Renderer.h"

namespace pigeon 
{
	void Renderer::BeginScene()
	{
		RenderCommand::Begin();
	}

	void Renderer::EndScene()
	{
		RenderCommand::End();
	}

	void Renderer::Submit()
	{
		RenderCommand::DrawIndexed();
	}
}