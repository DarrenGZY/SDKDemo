// WinInitialization.cpp : 定义应用程序的入口点。
//
//#include "Types.h"
#include "Scene/Scene.h"


unsigned char	MoveForward = 0,
MoveBack = 0,
MoveLeft = 0,
MoveRight = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wPraram, LPARAM lParam);
void OnKey(unsigned vk, bool down);
void UpdateCameraPosition(XMVECTOR & camPos);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Simple Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Learn to Program Windows",
		WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	SceneManager mgr;
	mgr.Init(hwnd);
	// 	TestRTV tester;
	// 	tester.Init(hwnd);

	MSG msg = {};
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		mgr.Render(hwnd);
		/*tester.Render(hwnd);*/
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYUP:
		OnKey((unsigned)wParam, false);
		break;
	case WM_KEYDOWN:
		OnKey((unsigned)wParam, true);
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void OnKey(unsigned vk, bool down)
{
	switch (vk)
	{
	case VK_UP:
		MoveForward = down ? (MoveForward | 1) : (MoveForward & ~1);
		break;
	case VK_DOWN:
		MoveBack = down ? (MoveBack | 1) : (MoveBack & ~1);
		break;
	}
}

void UpdateCameraPosition(XMVECTOR & camPos)
{
	XMVECTOR updateVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	if (MoveForward) updateVector += XMVectorSet(0.0f, 0.0f, 0.2f, 0.0f);
	if (MoveBack) updateVector += XMVectorSet(0.0f, 0.0f, -0.2f, 0.0f);

	camPos += updateVector;
}