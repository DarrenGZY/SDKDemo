#include "Scene.h"

void UpdateCameraPosition(XMVECTOR & camPos);

SceneManager::SceneManager()
{
	g_device = nullptr;
	g_devcontext = nullptr;
	g_swapchain = nullptr;
	g_factory = nullptr;
	g_RTV = nullptr;
	g_zbuffer = nullptr;
	g_ibuffer = nullptr;
	g_vbuffer = nullptr;
	g_cbuffer = nullptr;
	g_vertexShader = nullptr;
	g_pixelShader = nullptr;
	g_inputLayout = nullptr;
	g_Sampler = nullptr;
}


SceneManager::~SceneManager()
{
	SafeRelease(g_device);
	SafeRelease(g_devcontext);
	SafeRelease(g_swapchain);
	SafeRelease(g_factory);
	SafeRelease(g_RTV);
	SafeRelease(g_zbuffer);
	SafeRelease(g_ibuffer);
	SafeRelease(g_vbuffer);
	SafeRelease(g_cbuffer);
	SafeRelease(g_vertexShader);
	SafeRelease(g_pixelShader);
	SafeRelease(g_inputLayout);
	SafeRelease(g_Sampler);
}

void SceneManager::Init(HWND hwnd)
{
	HRESULT hr;

	//////////////////////////////////////
	// Create Device and Device Context
	//////////////////////////////////////
	hr = D3D11CreateDevice(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&g_device,
		nullptr,
		&g_devcontext);

	if (FAILED(hr))
	{
		// handle error
		return;
	}

	// Get DXGI factory
	IDXGIDevice* DxgiDevice = nullptr;
	hr = g_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr))
	{
		return;
	}

	IDXGIAdapter* DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	DxgiDevice->Release();
	DxgiDevice = nullptr;
	if (FAILED(hr))
	{
		return;
	}

	hr = DxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&g_factory));
	DxgiAdapter->Release();
	DxgiAdapter = nullptr;
	if (FAILED(hr))
	{
		return;
	}

	// Get Window Size
	RECT WindowRect;
	GetClientRect(hwnd, &WindowRect);
	UINT Width = WindowRect.right - WindowRect.left;
	UINT Height = WindowRect.bottom - WindowRect.top;

	//////////////////////////////////////
	// Create SwapChain
	//////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	RtlZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.Width = Width;
	SwapChainDesc.Height = Height;
	SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	hr = g_factory->CreateSwapChainForHwnd(g_device, hwnd, &SwapChainDesc, nullptr, nullptr, &g_swapchain);
	if (FAILED(hr))
	{
		return;
	}

	///////////////////////////////////////////////////////
	//Create Render Target View
	///////////////////////////////////////////////////////

	// Get backbuffer
	ID3D11Texture2D* backbuffer = nullptr;
	hr = g_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreateRenderTargetView(backbuffer, nullptr, &g_RTV);

	SafeRelease(backbuffer);

	if (FAILED(hr))
	{
		return;
	}

	///////////////////////////////////////////////////////
	//Create Depth Stencil View
	///////////////////////////////////////////////////////

	D3D11_TEXTURE2D_DESC texd;
	ZeroMemory(&texd, sizeof(texd));

	texd.Width = Width;
	texd.Height = Height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 1;
	texd.Format = DXGI_FORMAT_D32_FLOAT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D *pDepthBuffer = nullptr;
	hr = g_device->CreateTexture2D(&texd, NULL, &pDepthBuffer);
	if (FAILED(hr))
	{
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = g_device->CreateDepthStencilView(pDepthBuffer, &dsvd, &g_zbuffer);
	if (FAILED(hr))
	{
		return;
	}

	SafeRelease(pDepthBuffer);

	///////////////////////////////////////////////////////
	//Create Constant Buffer
	///////////////////////////////////////////////////////
	D3D11_BUFFER_DESC cbufferdesc;
	ZeroMemory(&cbufferdesc, sizeof(cbufferdesc));

	cbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	cbufferdesc.ByteWidth = sizeof(CBUFFER);
	cbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = g_device->CreateBuffer(&cbufferdesc, NULL, &g_cbuffer);
	if (FAILED(hr))
	{
		return;
	}

	///////////////////////////////////////////////////////
	//Create Sampler
	///////////////////////////////////////////////////////
	D3D11_SAMPLER_DESC SampDesc;
	ZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.MinLOD = 0;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_device->CreateSamplerState(&SampDesc, &g_Sampler);
	if (FAILED(hr))
	{
		return;
	}

	InitShaders();
	InitGraphics();
	
}


void SceneManager::InitGraphics()
{
	D3DCONTEXT context = GetContext();

	Model* plane = new Plane(context);
	plane->Create(-10.0f, -1.0f, 10.0f, 10.0f, -1.1f, -10.0f, L"floor.jpg");
	plane->SetShaders(g_TexVShader, g_TexPShader);
	models.push_back(plane);

	Model* wall_left = new Plane(context);
	wall_left->Create(-10.1f, 10.0f, 10.0f, -10.0f, -1.1f, -10.0f, L"wall1.jpg");
	wall_left->SetShaders(g_TexVShader, g_TexPShader);
	models.push_back(wall_left);

	Model* wall_right = new Plane(context);
	wall_right->Create(10.0f, 10.0f, 10.0f, 10.1f, -1.1f, -10.0f, L"wall1.jpg");
	wall_right->SetShaders(g_TexVShader, g_TexPShader);
	models.push_back(wall_right);

	Model* cube = new Cube(context);
	cube->Create(-1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, XMFLOAT4(1.0f, 0.2f, 0.4f, 1.0f));
	cube->SetShaders(g_vertexShader, g_pixelShader); 
	models.push_back(cube);

}

void SceneManager::InitShaders()
{
	HRESULT hr;

	hr = g_device->CreateVertexShader(g_VS, ARRAYSIZE(g_VS), nullptr, &g_vertexShader);
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreatePixelShader(g_PS, ARRAYSIZE(g_PS), nullptr, &g_pixelShader);
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreateVertexShader(g_TexVS, ARRAYSIZE(g_TexVS), nullptr, &g_TexVShader);
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreatePixelShader(g_TexPS, ARRAYSIZE(g_TexPS), nullptr, &g_TexPShader);
	if (FAILED(hr))
	{
		return;
	}
}


void SceneManager::Render(HWND hwnd)
{
	RECT WindowRect;
	GetClientRect(hwnd, &WindowRect);
	UINT Width = WindowRect.right - WindowRect.left;
	UINT Height = WindowRect.bottom - WindowRect.top;

	CBUFFER cbuffer;

	XMMATRIX matView, matProj;

	static XMVECTOR camPos = XMVectorSet(0.0f, 3.0f, -5.0f, 0.0f);
	UpdateCameraPosition(camPos);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	matView = XMMatrixLookAtLH(camPos, lookAt, up);
	matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(110), (FLOAT)Width / (FLOAT)Height, 0.5f, 100.0f);

	cbuffer.final = matView*matProj;

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)Width;
	vp.Height = (FLOAT)Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	g_devcontext->RSSetViewports(1, &vp);

	g_devcontext->VSSetConstantBuffers(0, 1, &g_cbuffer);
	g_devcontext->UpdateSubresource(g_cbuffer, 0, 0, &cbuffer, 0, 0);

	g_devcontext->PSSetSamplers(0, 1, &g_Sampler);

	g_devcontext->OMSetRenderTargets(1, &g_RTV, nullptr);


	for (auto it = models.begin(); it != models.end(); ++it)
	{
		(*it)->Render();
	}

	}

void SceneManager::Present()
{
	g_swapchain->Present(1, 0);
}

D3DCONTEXT SceneManager::GetContext()
{
	return{ g_device, g_devcontext};
}

void SceneManager::SafeRelease(IUnknown *object)
{
	if (object)
	{
		object->Release();
		object = nullptr;
	}
}