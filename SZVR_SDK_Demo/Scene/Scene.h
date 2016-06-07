#pragma once

//#include "../Types.h"
#include "Cube.h"
#include <vector>

class SceneManager
{
public:
	SceneManager();
	void Init(HWND hwnd);
	void Render(HWND hwnd);
	D3DCONTEXT GetContext();
	~SceneManager();

private:
	void InitGraphics();
	void InitShaders();
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
	ID3D11InputLayout *g_inputLayout;			// Input Layout
	ID3D11SamplerState *g_Sampler;				// Sampler for Pixel Shader

	std::vector<Cube*> cubes;
};

