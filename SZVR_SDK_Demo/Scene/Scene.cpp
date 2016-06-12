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
	g_TexRTV = nullptr;
	g_TexPShader = nullptr;
	g_TexVShader = nullptr;
	g_distortVBuffer = nullptr;
	g_distortPShader = nullptr;
	g_distortVShader = nullptr;
	g_distortIBuffer = nullptr;
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
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = Width;
	desc.Height = Height;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;

	hr = g_device->CreateTexture2D(&desc, nullptr, &g_renderTarget);

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = desc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	hr = g_device->CreateRenderTargetView(g_renderTarget, nullptr, &g_TexRTV);

	if (FAILED(hr))
	{
		return;
	}

	ID3D11Texture2D* backbuffer = nullptr;
	hr = g_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreateRenderTargetView(backbuffer, nullptr, &g_RTV);
	if (FAILED(hr))
	{
		return;
	}

	SafeRelease(backbuffer);

//	D3D11_TEXTURE2D_DESC desc;
//	g_renderTarget->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
	shaderDesc.Format = desc.Format;
	shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;
	shaderDesc.Texture2D.MipLevels = desc.MipLevels;

	hr = g_device->CreateShaderResourceView(g_renderTarget, &shaderDesc, &g_SRV);
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
	DistortionInit();
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
	cube->Create(-1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, XMFLOAT4(0.0f, 0.2f, 0.2f, 1.0f));
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

	hr = g_device->CreateVertexShader(g_BVS, ARRAYSIZE(g_BVS), nullptr, &g_distortVShader);
	if (FAILED(hr))
	{
		return;
	}

	hr = g_device->CreatePixelShader(g_BPS, ARRAYSIZE(g_BPS), nullptr, &g_distortPShader);
	if (FAILED(hr))
	{
		return;
	}
}

void SceneManager::DistortionInit()
{
	HRESULT hr;

	///////////////////////////////////////////////////////
	//Create Input Layout
	///////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT NumElements = ARRAYSIZE(Layout);
	hr = g_device->CreateInputLayout(Layout, NumElements, g_BVS, ARRAYSIZE(g_BVS), &g_distortInputLayout);


	///////////////////////////////////////////////////////
	//Create Vertex Buffer
	///////////////////////////////////////////////////////
	VERTEX Vertices[] =
	{
		{ XMFLOAT3(-2.0f, -2.0f, 0), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-2.0f, 2.0f, 0), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(2.0f, -2.0f, 0), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(2.0f, 2.0f, 0), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(VERTEX)*ARRAYSIZE(Vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;

	hr = g_device->CreateBuffer(&bufferDesc, &InitData, &g_distortVBuffer);
	if (FAILED(hr))
	{
		return;
	}

	///////////////////////////////////////////////////////
	//Create Index Buffer
	///////////////////////////////////////////////////////
	DWORD Indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};

	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD)*ARRAYSIZE(Indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Indices;

	hr = g_device->CreateBuffer(&bufferDesc, &InitData, &g_distortIBuffer);
	if (FAILED(hr))
	{
		return;
	}

	//hr = CreateWICTextureFromFile(g_device, g_devcontext, L"floor.jpg", nullptr, &g_SRV, NULL);  //Debug only
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
	cbuffer.LightVector = XMFLOAT4(-0.3f, 0.3f, 0.0f, 0.0f);
	cbuffer.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbuffer.AmbientColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)Width;
	vp.Height = (FLOAT)Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_devcontext->ClearDepthStencilView(g_zbuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	g_devcontext->RSSetViewports(1, &vp);

	g_devcontext->VSSetConstantBuffers(0, 1, &g_cbuffer);
	g_devcontext->UpdateSubresource(g_cbuffer, 0, 0, &cbuffer, 0, 0);

	g_devcontext->PSSetSamplers(0, 1, &g_Sampler);

	g_devcontext->OMSetRenderTargets(1, &g_TexRTV, g_zbuffer);

	for (auto it = models.begin(); it != models.end(); ++it)
	{
		(*it)->Render();
	}
}

void SceneManager::DistortionRender()
{
	FLOAT color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	g_devcontext->ClearRenderTargetView(g_TexRTV, color);
	g_devcontext->ClearRenderTargetView(g_RTV, color);
	g_devcontext->ClearDepthStencilView(g_zbuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	g_devcontext->IASetInputLayout(g_distortInputLayout);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	g_devcontext->IASetVertexBuffers(0, 1, &g_distortVBuffer, &stride, &offset);
	g_devcontext->IASetIndexBuffer(g_distortIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_devcontext->VSSetShader(g_distortVShader, nullptr, NULL);
	g_devcontext->PSSetShader(g_distortPShader, nullptr, NULL);
	g_devcontext->PSSetShaderResources(0, 1, &g_SRV);

	g_devcontext->OMSetRenderTargets(1, &g_RTV, g_zbuffer);

	g_devcontext->DrawIndexed(6, 0, 0);
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