#pragma once

//#include "../Types.h"
#include "Cube.h"
#include "Plane.h"
#include "Model.h"
#include <vector>

class SceneManager
{
public:
	SceneManager();
	void Init(HWND hwnd);
	void Render(HWND hwnd);
	void DistortionRender();
	void Present();
	D3DCONTEXT GetContext();
	~SceneManager();

private:
	void InitGraphics();
	void InitShaders();
	void DistortionInit();
	void SafeRelease(IUnknown *object);

private:
	ID3D11Device *g_device;						// Device
	ID3D11DeviceContext *g_devcontext;			// Device Context
	IDXGISwapChain1 *g_swapchain;				// SwapChain
	IDXGIFactory2 *g_factory;					// Factory
	ID3D11RenderTargetView* g_RTV;				// Render Target View
	ID3D11ShaderResourceView *g_SRV;			// Shader Resource View
	ID3D11DepthStencilView* g_zbuffer;			// Depth Buffer
	ID3D11Buffer *g_cbuffer;					// Constant Buffer
	ID3D11Buffer *g_ibuffer;					// Index Buffer
	ID3D11Buffer *g_vbuffer;					// Vertex Buffer
	ID3D11VertexShader *g_vertexShader;			// Vertex Shader
	ID3D11PixelShader *g_pixelShader;			// Pixel Shader

	ID3D11Texture2D *g_renderTarget;
	ID3D11RenderTargetView *g_TexRTV;

	ID3D11VertexShader *g_TexVShader;			// Vertex Shader for texture
	ID3D11PixelShader *g_TexPShader;			// Pixel Shader for texture

	ID3D11InputLayout *g_inputLayout;			// Input Layout
	ID3D11SamplerState *g_Sampler;				// Sampler for Pixel Shader

	// Distortion Render
	ID3D11VertexShader *g_distortVShader;
	ID3D11PixelShader *g_distortPShader;
	ID3D11InputLayout *g_distortInputLayout;
	ID3D11Buffer *g_distortIBuffer;
	ID3D11Buffer *g_distortVBuffer;

	std::vector<Model*> models;
};

