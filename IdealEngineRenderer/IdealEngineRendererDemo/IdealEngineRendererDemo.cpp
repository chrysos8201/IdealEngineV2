// IdealEngineRendererDemo.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "IdealEngineRendererDemo.h"
#define MAX_LOADSTRING 100

#include "../IdealEngineRenderer/public/IdealRendererV2Factory.h"
#include "../IdealEngineRenderer/public/IdealRendererV2.h"
#include <memory>

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


DWORD g_FrameCount = 0;
ULONGLONG g_PrvFrameCheckTick = 0;
ULONGLONG g_PrvUpdateTick = 0;
HWND g_hWnd;
const int WIDTH = 800;
const int HEIGHT = 600;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
  
    // DLL Load
	HMODULE hDll = LoadLibraryA("..//x64//Debug//IdealEngineRenderer.dll");
    typedef void (*CreateRendererFactory)(std::shared_ptr<Ideal::IdealRendererV2>&, const IdealRendererV2Desc&);
	CreateRendererFactory CreateRenderer = (CreateRendererFactory)GetProcAddress(hDll, "CreateRenderer2");

	typedef void (*DestroyRendererFactory)(std::shared_ptr<Ideal::IdealRendererV2>&);
	DestroyRendererFactory DestroyRenderer = (DestroyRendererFactory)GetProcAddress(hDll, "DestroyRenderer");

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IDEALENGINERENDERERDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IDEALENGINERENDERERDEMO));

    MSG msg = {};

    //////////////////////// Init

	IdealRendererV2Desc rendererDesc;
	rendererDesc.Hwnd = g_hWnd;
	rendererDesc.Width = WIDTH;
	rendererDesc.Height = HEIGHT;
	rendererDesc.bDebugMode = true;
	//rendererDesc.bDebugMode = false;
	rendererDesc.bTearingSupport = true;

	std::shared_ptr<Ideal::IdealRendererV2> IdealRenderer;
	CreateRenderer(IdealRenderer, rendererDesc);
	//IdealRenderer->Init();

    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			g_FrameCount++;
			ULONGLONG curTick = GetTickCount64();
			if (curTick - g_PrvUpdateTick > 16)
			{
				g_PrvUpdateTick = curTick;
			}
			if (curTick - g_PrvFrameCheckTick > 1000)
			{
				g_PrvFrameCheckTick = curTick;
				WCHAR wchTxt[64];
				swprintf_s(wchTxt, L"FPS:%u", g_FrameCount);
				SetWindowText(g_hWnd, wchTxt);
				g_FrameCount = 0;
			}
            			// 게임 루프
			// 게임 루프는 여기에 작성
			IdealRenderer->Render();
        }
    }
    IdealRenderer->Destroy();
	DestroyRenderer(IdealRenderer);
    FreeLibrary(hDll);
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IDEALENGINERENDERERDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT windowRect = { 0,0, WIDTH, HEIGHT };
   ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);
   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
			{
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}