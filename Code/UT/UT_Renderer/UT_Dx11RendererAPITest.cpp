#pragma once
#include <catch.hpp>
#include <cstdlib>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Utils/TestApp.h"

#include <Pigeon/Renderer/OrthographicCamera.h>

#include <Platform/DirectX11/Dx11Buffer.h>
#include <Platform/DirectX11/Dx11Context.h>
#include <Platform/DirectX11/Dx11RendererAPI.h>
#include <Platform/DirectX11/Dx11Shader.h>

namespace
{
	// Simple vertex shader
	char* s_VsCode =
		"struct VS_INPUT\n"
		"{\n"
		"	float4 Pos : POSITION;\n"
		"	float4 Col : COLOR;\n"
		"};\n"
		"struct PS_INPUT\n"
		"{\n"
		"	float4 Pos : SV_POSITION; \n"
		"	float4 Col : COLOR; \n"
		"};\n"
		"PS_INPUT main(VS_INPUT input)\n"
		"{\n"
		"	PS_INPUT output;\n"
		"	output.Pos = input.Pos; // Pass position to rasterizer\n"
		"	output.Col = input.Col; // Pass color to pixel shader\n"
		"	return output;\n"
		"};";

	// Simple pixel shader
	char* s_PsCode =
		"struct PS_INPUT\n"
		"{\n"
		"	float4 Pos : SV_POSITION;\n"
		"	float4 Col : COLOR;\n"
		"};\n"
		"float4 main(PS_INPUT input) : SV_TARGET\n"
		"{\n"
		"    return input.Col; // Output the interpolated color\n"
		"};";


	float s_OurVertices[3 * 7] = {
		 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 -0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	uint32_t s_Indices[3] = { 0, 1, 2 };

	glm::mat4 s_TestMat({
		1,  2,  3,  4,
		5,  6,  7,  8,
		9,  10, 11, 12,
		13, 14, 15, 16
	});

	glm::mat4 s_TestMat3({
		1,  2,  3,  
		4,  5,  6,  
		7,  8,  9
	});

	glm::mat4 s_TestMatTrnas({
		1,  5,  9,  13,
		2,  6,  10,  8,
		3,  7,  11, 15,
		4,  14, 12, 16
	});
	glm::mat4 s_TestMatIdent({
		1,  0,  0,  0,
		0,  1,  0,  0,
		0,  0,  1,  0,
		0,  0,  0,  1
	});

	glm::vec2 s_TestVec2(1.f);
	glm::vec3 s_TestVec3(1.f);
	glm::vec4 s_TestVec4(1.f);
	float s_TestFloat = 1.f;
	int s_TestInt = 1;

	struct CBData0
	{
		float value1;              // 4 bytes
		float value2;              // 4 bytes
		float value3;              // 4 bytes
		float value4;              // 4 bytes
	};

	struct CBData1
	{
		DirectX::XMMATRIX matrix; // 16-byte aligned
	};
	struct CBData2
	{
		DirectX::XMMATRIX matrix; // 16-byte aligned
		DirectX::XMFLOAT4 vector;  // 16-byte aligned (padded vec3)
		float value1;              // 4 bytes
		float value2;              // 4 bytes
		float padding[2];          // Padding for alignment - 8 bytes
	};

	struct CBDataInvalid
	{
		float value1;              // 4 bytes
		float value2;              // 4 bytes
	};
}

namespace CatchTestsetFail
{
	TEST_CASE("app.Renderer::Dx11RendererAPITest")
	{
		TestApp* app = static_cast<TestApp*>(pigeon::CreateApplication());

		const pigeon::Dx11RendererAPI* rendererAPI = static_cast<const pigeon::Dx11RendererAPI*>(pigeon::RenderCommand::GetRenderAPI());
		CHECK(rendererAPI->GetAPI() == pigeon::RendererAPI::API::DirectX11);
		CHECK(!rendererAPI->GetData().m_Initialized); //Not initialized until first frame;
		CHECK(rendererAPI->GetData().m_MainRenderTargetView == nullptr);
		CHECK(rendererAPI->GetData().m_ClearColor[0] == 1.f);
		CHECK(rendererAPI->GetData().m_ClearColor[1] == 1.f);
		CHECK(rendererAPI->GetData().m_ClearColor[2] == 1.f);
		CHECK(rendererAPI->GetData().m_ClearColor[3] == 1.f);

		pigeon::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 0.4f });

		CHECK(rendererAPI->GetData().m_ClearColor[0] == 0.1f);
		CHECK(rendererAPI->GetData().m_ClearColor[1] == 0.2f);
		CHECK(rendererAPI->GetData().m_ClearColor[2] == 0.3f);
		CHECK(rendererAPI->GetData().m_ClearColor[3] == 0.4f);

		pigeon::Renderer::BeginScene();

		CHECK(rendererAPI->GetData().m_Initialized);
		CHECK(rendererAPI->GetData().m_MainRenderTargetView != nullptr);
		pigeon::Renderer::Submit(0);
		pigeon::Renderer::EndScene();

		delete app;
	}

	TEST_CASE("app.Renderer::Dx11ContextTest")
	{
		TestApp* app = static_cast<TestApp*>(pigeon::CreateApplication());

		pigeon::Dx11Context* dx11Context = static_cast<pigeon::Dx11Context*>(app->GetWindow().GetGraphicsContext());
		CHECK(dx11Context->GetData().m_HWnd != nullptr);
		CHECK(dx11Context->GetHeight() == 720);
		CHECK(dx11Context->GetWidth() == 1280);
		CHECK(dx11Context->GetPd3dDevice() != nullptr);
		CHECK(dx11Context->GetPd3dDeviceContext() != nullptr);
		CHECK(dx11Context->GetSwapChain() != nullptr);
		CHECK(!dx11Context->NeedsResize());

		dx11Context->SetSize(123, 456);
		CHECK(dx11Context->NeedsResize());
		CHECK(dx11Context->GetHeight() == 720);
		CHECK(dx11Context->GetWidth() == 1280);
		pigeon::Renderer::BeginScene();
		CHECK(!dx11Context->NeedsResize());
		CHECK(dx11Context->GetHeight() == 456);
		CHECK(dx11Context->GetWidth() == 123);

		dx11Context->Shutdown();
		CHECK(dx11Context->GetData().m_HWnd == nullptr);
		CHECK(dx11Context->GetPd3dDevice() == nullptr);
		CHECK(dx11Context->GetPd3dDeviceContext() == nullptr);
		CHECK(dx11Context->GetSwapChain() == nullptr);
		delete app;
	}

	TEST_CASE("app.Renderer::Dx11Buffers")
	{
		TestApp* app = static_cast<TestApp*>(pigeon::CreateApplication());

		pigeon::Dx11Context* dx11Context = static_cast<pigeon::Dx11Context*>(app->GetWindow().GetGraphicsContext());


		SECTION("Vertex buffer")
		{
			pigeon::VertexBuffer* vertexBuffer = pigeon::VertexBuffer::Create(s_OurVertices, sizeof(s_OurVertices));
			pigeon::Dx11VertexBuffer* dxVB = static_cast<pigeon::Dx11VertexBuffer*>(vertexBuffer);
			CHECK(dxVB->GetData().m_Buffer != nullptr);
			dxVB->Bind();
			dxVB->Unbind();
			delete vertexBuffer;
		}
		SECTION("Index buffer")
		{
			pigeon::IndexBuffer* indexBuffer = pigeon::IndexBuffer::Create(s_Indices, sizeof(s_Indices) / sizeof(uint32_t));
			pigeon::Dx11IndexBuffer* dxIB = static_cast<pigeon::Dx11IndexBuffer*>(indexBuffer);
			CHECK(dxIB->GetData().m_Buffer != nullptr);
			CHECK(dxIB->GetCount() == 3);
			dxIB->Bind();
			dxIB->Unbind();
			delete indexBuffer;
		}
		SECTION("Constant buffer")
		{
			CBDataInvalid dataInvalid;
			pigeon::Dx11ConstantBuffer<CBDataInvalid>* constantBufferInvalid = new pigeon::Dx11ConstantBuffer<CBDataInvalid>(&dataInvalid);
			CHECK(constantBufferInvalid->GetData().m_Buffer == nullptr); //Error creating, invalid alignment

			CBData0 data;
			pigeon::Dx11ConstantBuffer<CBData0>* constantBuffer = new pigeon::Dx11ConstantBuffer<CBData0>(&data);
			CHECK(constantBuffer->GetData().m_Buffer != nullptr);
			constantBuffer->Bind(0);

			CBData1 data1;
			pigeon::Dx11ConstantBuffer<CBData1>* constantBuffer1 = new pigeon::Dx11ConstantBuffer<CBData1>(&data1);
			CHECK(constantBuffer1->GetData().m_Buffer != nullptr);
			constantBuffer1->Bind(0);

			CBData2 data2;
			pigeon::Dx11ConstantBuffer<CBData2>* constantBuffer2 = new pigeon::Dx11ConstantBuffer<CBData2>(&data2);
			CHECK(constantBuffer2->GetData().m_Buffer != nullptr);
			constantBuffer2->Bind(0);

			delete constantBufferInvalid;
			delete constantBuffer;
			delete constantBuffer1;
			delete constantBuffer2;
		}

		delete app;
	}

	TEST_CASE("app.Renderer::Dx11Shader")
	{
		TestApp* app = static_cast<TestApp*>(pigeon::CreateApplication());

		pigeon::Dx11Context* dx11Context = static_cast<pigeon::Dx11Context*>(app->GetWindow().GetGraphicsContext());

		pigeon::Shader* shader;

		pigeon::BufferLayout buffLayout = {
			{ pigeon::ShaderDataType::Float3, "POSITION" },
			{ pigeon::ShaderDataType::Float4, "COLOR" }
		};

		shader = pigeon::Shader::Create(s_VsCode, s_PsCode, buffLayout);
		pigeon::Dx11Shader* dxShader = static_cast<pigeon::Dx11Shader*>(shader);

		CHECK(dxShader->GetData().m_InputLayout != nullptr);
		CHECK(dxShader->GetData().m_PixelShader != nullptr);
		CHECK(dxShader->GetData().m_VertexShader != nullptr);
		dxShader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", s_TestMat);
		shader->UploadUniformMat4("u_Unknown", s_TestMat);
		shader->UploadUniformMat3("u_Unknown", s_TestMat3);
		shader->UploadUniformInt("u_Unknown", s_TestInt);
		shader->UploadUniformFloat("u_Unknown", s_TestFloat);
		shader->UploadUniformFloat2("u_Unknown", s_TestVec2);
		shader->UploadUniformFloat3("u_Unknown", s_TestVec3);
		shader->UploadUniformFloat4("u_Unknown", s_TestVec4);

		dxShader->Unbind();

		delete app;
	}

	TEST_CASE("Renderer::OrthographicCamera")
	{
		const glm::vec4 ortoValues(-0.5f, 0.5f, -0.5f, 0.5f);
		const glm::vec3 pos0(0.f, 0.f, 0.f);
		const glm::vec3 pos1(-1.f, 2.f, 3.f);
		const glm::vec3 pos2(245.f, 672.6f, 12.341f);
		const float rot0 = 0.f;
		const float rot1 = 1.5f;
		const float rot2 = -4.5f;

		pigeon::OrthographicCamera camera(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w);
		CHECK(camera.GetPosition() == pos0);
		CHECK(camera.GetRotation() == rot0);
		glm::mat4 projMat = camera.GetProjectionMatrix();
		glm::mat4 viewMat = camera.GetViewMatrix();
		glm::mat4 projViewMat = camera.GetViewProjectionMatrix();

		CHECK(projMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f));
		CHECK(viewMat == glm::mat4(1.f));
		CHECK(projViewMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f));

		camera.SetPosition(pos1);
		CHECK(camera.GetPosition() == pos1);
		CHECK(camera.GetRotation() == rot0);
		camera.SetRotation(rot1);
		CHECK(camera.GetPosition() == pos1);
		CHECK(camera.GetRotation() == rot1);

		projMat = camera.GetProjectionMatrix();
		viewMat = camera.GetViewMatrix();
		projViewMat = camera.GetViewProjectionMatrix();

		glm::mat4 inverseTransform = glm::inverse(glm::translate(glm::mat4(1.0f), pos1) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rot1), glm::vec3(0, 0, 1)));

		CHECK(projMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f));
		CHECK(viewMat == inverseTransform);
		CHECK(projViewMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f) * inverseTransform);

		camera.SetRotation(rot2);
		CHECK(camera.GetPosition() == pos1);
		CHECK(camera.GetRotation() == rot2);
		camera.SetPosition(pos2);
		CHECK(camera.GetPosition() == pos2);
		CHECK(camera.GetRotation() == rot2);

		projMat = camera.GetProjectionMatrix();
		viewMat = camera.GetViewMatrix();
		projViewMat = camera.GetViewProjectionMatrix();

		inverseTransform = glm::inverse(glm::translate(glm::mat4(1.0f), pos2) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rot2), glm::vec3(0, 0, 1)));

		CHECK(projMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f));
		CHECK(viewMat == inverseTransform);
		CHECK(projViewMat == glm::ortho(ortoValues.x, ortoValues.y, ortoValues.z, ortoValues.w, -1.0f, 1.0f) * inverseTransform);

	}
} // End namespace: CatchTestsetFail
