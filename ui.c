#include <Windows.h>
#include "bullet_fight.h"
HINSTANCE hinst;
HINSTANCE hinstance;
HWND hwndFighter,hwnd;
HBITMAP hbmpSky;
LPBULLET lpbullet = NULL;
DWORD dwTimerElapse = 1000;
HWND hwnd; //
UINT msg; // ��Ϣ
WPARAM wParam; // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
LPARAM lParam;

#define TIMER_ID 12340
#define TRANS_BK_COLOR RGB(255,255,255)

#define FIGHTER_HEIGHT	50
#define FIGHTER_WIDTH	50

// �ɻ��ڷ�����������£�ÿ���ƶ������أ�
#define FIGHTER_MOVE_STEP 5

LONG OnCreate()
{
	BITMAP bmp;
	hbmpSky = LoadImage(NULL, "sky.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hbmpSky == NULL)
	{
		MessageBox(hwnd, "bmp file not find", "ERROR!",
			MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	if (RegisterFighterClass())
	{
		CreateFighterWindow();
	}
	InitBulletAndFights();
	SetTimer(hwnd, TIMER_ID, dwTimerElapse, NULL);
	return 0;
}
LONG OnResizeAndMove(HWND hwnd, LPRECT lprect)
{
	MoveWindow(hwndFighter,
		(lprect->left + lprect->right - FIGHTER_WIDTH) / 2,
		lprect->bottom - FIGHTER_HEIGHT,
		FIGHTER_WIDTH,
		FIGHTER_HEIGHT,
		TRUE);
	return 0;
}
LONG OnPaint(HWND hwnd)
{
    HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	unsigned int num = GetBulletsNum();
	unsigned int i;
	hdc = BeginPaint(hwnd, &ps);
	// Create a memory device compatible with the above DC variable
	hdcMem = CreateCompatibleDC(hdc);
	// Select the new bitmap
	GetClientRect(hwnd, &rect);
	//// Copy the bits from the memory DC into the current dc
	StretchBlt(hdc,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		hdcMem,
		0, 0, 1400, 1050,
		SRCCOPY);
	SelectObject(hdcMem, GetStockObject(WHITE_BRUSH));
	num = GetBulletsNum();
   //GetClientRect(hwnd, &rect);
	//StretchBlt(hdc, 0, 0, rect.right, rect.bottom,
	//	hdcMem, 0, 0, 1024, 1024, SRCCOPY);
	// Restore the old bitmap
	for (i = 0; i < num; i++) {
		LPBULLET lpBullet = GetBulletAt(i);
		lpBullet = GetBulletAt(i);
		Ellipse(hdcMem,
			GetBulletX(lpbullet),
			GetBulletY(lpbullet),
			GetBulletX(lpbullet) + 20,
			GetBulletY(lpbullet) + 20);
	}
	DeleteDC(hdcMem);
	EndPaint(hwnd, &ps);
	return 0;
}
LONG OnKeyDown(HWND hwnd, UINT vk)
{
	RECT rectFighter;
	switch (vk)
	{
	case VK_LEFT:
		GetWindowRect(hwndFighter, &rectFighter);
		MoveWindow(hwndFighter,
			rectFighter.left - FIGHTER_MOVE_STEP,
			rectFighter.top,
			FIGHTER_WIDTH,
			FIGHTER_HEIGHT,
			TRUE);
		break;
	case VK_RIGHT:
		GetWindowRect(hwndFighter, &rectFighter);
		MoveWindow(hwndFighter,
			rectFighter.left + FIGHTER_MOVE_STEP,
			rectFighter.top,
			FIGHTER_WIDTH,
			FIGHTER_HEIGHT,
			TRUE);
		break;
	case VK_UP:
		GetWindowRect(hwndFighter, &rectFighter);
		MoveWindow(hwndFighter,
			rectFighter.left,
			rectFighter.top - FIGHTER_MOVE_STEP,
			FIGHTER_WIDTH,
			FIGHTER_HEIGHT,
			TRUE);
		break;
	case VK_DOWN:
		GetWindowRect(hwndFighter, &rectFighter);
		MoveWindow(hwndFighter,
			rectFighter.left,
			rectFighter.top + FIGHTER_MOVE_STEP,
			FIGHTER_WIDTH,
			FIGHTER_HEIGHT,
			TRUE);
		break;
	case 'S':
	{
		POINT pt;
		GetWindowRect(hwndFighter, &rectFighter);
		pt.x =( rectFighter.left + rectFighter.right) / 2;
		pt.y = rectFighter.top;
		ScreenToClient(hwnd, &pt);
		NewBullet(pt.x, pt.y);
	}
	break;
	default:
		break;
		return 0;
	}
}
LONG OnTimer(HWND hwnd)
{
	unsigned int num = GetBulletsNum();
	unsigned int i;
	LPBULLET lpBullet;
	for (i = 0; i < num; i++) {
		lpBullet = GetBulletAt(i);
		MoveBullet(lpbullet);
	}
	return 0;
}
LONG CALLBACK MainWndProc(
	HWND hwnd, //
	UINT msg, // ��Ϣ
	WPARAM wParam, // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
	LPARAM lParam); // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�

LONG CALLBACK FighterWndProc(
	HWND hwnd, //
	UINT msg, // ��Ϣ
	WPARAM wParam, // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
	LPARAM lParam); // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�

HBITMAP hbmpFighter;

BOOL RegisterFighterClass()
{
	WNDCLASS wc;
	// ���ھ����hwnd�����������ڵľ�������������ֻ�õ���һ�����ڡ�
	HWND hwnd;

	MSG msg;
	int fGotMessage;

	// Fill in the window class structure with parameters 
	// that describe the main window. 
	// ���������ʽ���������õ���ʽ��ʾ�����ڴ�С�仯����Ҫ�ػ�
	wc.style = CS_HREDRAW | CS_VREDRAW;
	// һ������ָ�룬�������������������Ϣ�� ��� MainWndProc������ע�͡�
	wc.lpfnWndProc = FighterWndProc;
	// no extra class memory 
	wc.cbClsExtra = 0;
	// no extra window memory
	wc.cbWndExtra = 0;
	// handle to instance 
	wc.hInstance = hinst;
	// hIcon��Ա����ָ�����ڵ�ͼ��
	// ����ֱ��ʹ��LoadIcon����������һ��ϵͳԤ�����ͼ�꣬������Ա��Ҳ�����Լ�����ͼ�ꡣ
	wc.hIcon = NULL;
	// Cursor������꣬�������趨����������ʽ��
	// ֱ��ʹ��LoadCursor API����������һ��ϵͳԤ����Ĺ����ʽ������IDC_CROSS,IDC_HAND����ʽ 
	wc.hCursor = LoadCursor(NULL,
		IDC_CROSS);
	// GetStockObject�Ĺ����Ǽ���һ��ϵͳԤ���壨��ջ�У���GDI����
	// ������ص���һ����ɫ�Ļ�ˢ���йػ�ˢ��GDI�������GDI˵����
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(TRANS_BK_COLOR);
	// ���ڵĲ˵�����Դ����
	wc.lpszMenuName = NULL;
	// ����������һ�����֣��ڴ�������ʱ��Ҫ������֡�
	wc.lpszClassName = "fighter_class";
	// Register the window class. 

	if (!RegisterClass(&wc))
	{
		// ����ע��ʧ�ܣ���Ϣ����ʾ�����˳���
		MessageBox(NULL, "��������classʧ��", "����", MB_ICONERROR | MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL CreateFighterWindow()
{

	hwndFighter = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST|WS_EX_NOACTIVATE,
		"fighter_class",			// �����������������Ѿ�ע���˵Ĵ�����
		"",			// title-bar string 
		WS_POPUP | WS_VISIBLE,	// ���ڵ�style�������ʾΪtop-level window 
		CW_USEDEFAULT,			// ����ˮƽλ��default horizontal POINT 
		CW_USEDEFAULT,			// ���ڴ�ֱλ��default vertical POINT 
		CW_USEDEFAULT,			// ���ڿ�� default width 
		CW_USEDEFAULT,			// ���ڸ߶� default height 
		(HWND)NULL,				// �����ھ�� no owner window 
		(HMENU)NULL,			// ���ڲ˵��ľ�� use class menu 
		hinst,				// Ӧ�ó���ʵ����� handle to application instance 
		(LPVOID)NULL);			// ָ�򸽼����ݵ�ָ�� no window-creation data 
	if (!hwndFighter)
	{
		// ���ڴ���ʧ�ܣ���Ϣ����ʾ�����˳���
		MessageBox(NULL, "��������ʧ��", "����", MB_ICONERROR | MB_OK);
		return FALSE;
	}


	if (!SetLayeredWindowAttributes(
		hwndFighter, TRANS_BK_COLOR,
		255, LWA_COLORKEY | LWA_ALPHA))
	{
		DWORD dwError = GetLastError();
	}

	// ���ڴ����ɹ����������С�
	// ��ʾ���ڣ�WinMain������nCmdShow���������﷢�����ã�һ�㶼����ΪSW_SHOW
	ShowWindow(hwndFighter, SW_SHOW);
	// ˢ�´��ڣ��򴰿ڷ���һ��WM_PAINT��Ϣ��ʹ�ô��ڽ����ػ档
	UpdateWindow(hwndFighter);
	return TRUE;
}

LONG CALLBACK FighterWndProc(
	HWND hwnd, //
	UINT msg, // ��Ϣ
	WPARAM wParam, // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
	LPARAM lParam) // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	// ע�⣬��switch-case, ÿ��������������ã�ֻ�����뵽һ��case�С�
	switch (msg)
	{
		// �����ڱ�����ʱ���յ��ĵ�һ����Ϣ����WM_CREATE��
		// һ���յ������Ϣ��������У�������������һЩ��ʼ���Ĺ���
	case WM_CREATE:

		MoveWindow(
			hwnd, 100, 100,
			FIGHTER_WIDTH, FIGHTER_HEIGHT,
			TRUE);
		hbmpFighter = LoadImage(NULL, "fight.bmp",
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hbmpFighter == NULL)
		{
			MessageBox(hwnd, "bmp file not find", "ERROR!",
				MB_OK | MB_ICONERROR);
			ExitProcess(0);
		}
		break;

		// ��ϵͳ��Ϊ�����ϵ�GDI����Ӧ�ñ��ػ�ʱ�����򴰿ڷ���һ��WM_PAINT��Ϣ��
		// ��ȻӦ�ó���Ҳ����ͨ������ UpateWindow�������򴰿ڷ���һ��WM_PAINT��Ϣ��
		// ����ʹ��GDI�ڴ����ϻ���ͼ�εĳ��� �����롱 д�����
		// ���������WM_PAINT��Ϣ�Ĵ�������л���GDIͼ�Σ���ô�ڴ���ˢ��ʱ�ͻᱻ�±�Ĩ���͸���
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// Create a memory device compatible with the above DC variable
		hdcMem = CreateCompatibleDC(hdc);
		// Select the new bitmap
		SelectObject(hdcMem, hbmpFighter);
		GetClientRect(hwnd, &rect);
		//// Copy the bits from the memory DC into the current dc
		StretchBlt(hdc,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			hdcMem,
			0, 0, 240, 217,
			SRCCOPY);
		//GetClientRect(hwnd, &rect);
		//StretchBlt(hdc, 0, 0, rect.right, rect.bottom,
		//	hdcMem, 0, 0, 1024, 1024, SRCCOPY);
		// Restore the old bitmap
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		break;

	case WM_KEYDOWN:
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_TIMER:
		break;

	case WM_DESTROY:
		ExitProcess(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

/*******************************************************************************
* ##########   ��ں���WinMain    ##########
*
* WindowsӦ�ó������ں�����WinMain���������λ���������Ʊ�׼C���Ե�main����
*
*******************************************************************************/

int WINAPI WinMain(
	 // ����ʵ��������ڳ������У����̴������ɲ���ϵͳ��Ӧ�ó�����
	HINSTANCE hPrevInstance, // �����̵ĳ���ʵ�����
	LPSTR lpCmdLine,  // �����в�������λ����������C����main��������argc��argv,����û�а��ո�����з�
	int nCmdShow)   // ����ָ�������Ƿ���Ҫ��ʾ�Ĳ�����
{

	/*******************************************************************************
	* ##########   ���� API ����   ##########
	*
	* API�� Application Programming Interface����д������Ӧ�ó����̽ӿڡ�
	* API��Windows����ϵͳ�ṩ��Ӧ�ó�����õ�һϵ�й��ܵĽӿں���������Ӧ�ó�����ò���ϵͳ�ĵײ㹦��
	*
	* API������ʹ�÷�������C���ԵĿ⺯������printf��fopen��fread�ȡ�
	* ��VS������ʹ��C����C++���б���ǣ�ֻ��#include <windows.h>���ɵ��ô󲿷�Windows API������
	*
	* ������һ�˴�����ʹ�õ��� LoadIcon LoadCursor GetStockObject RegisterClass CreateWindow�Ⱥ�������API
	*
	* WindowsӦ�ó��򿪷������е���Ҫ��������ʹ��API��
	*
	*******************************************************************************/

	/*******************************************************************************
	* ##########   ������   ##########
	*
	* �ڴ�������֮ǰ��������Ҫע��һ�������࣬������������Ϊͬһ����Ĵ��ڡ�
	* һ����������Դ���������ڣ�����ͬһ������Ĵ��ھ�����ͬ����Ϣ�����������Ƶ���ʽ��
	*
	* �������C++�е���Ӣ�Ķ���class���������ǲ���ͬһ���
	* C++��ʹ��class�ؼ��ֶ��壬����������ʹ��RegisterClass API������Windowsϵͳע�ᡣ
	*
	* ����� WNDCLASS wc ��ע�ᴰ����� API����RegisterClass�Ĳ�������һ���ṹ�����ͣ�
	* �����洢һ��������ĸ�������
	*
	*******************************************************************************/
	WNDCLASS wc;
	// ���ھ����hwnd�����������ڵľ�������������ֻ�õ���һ�����ڡ�
	HWND hwnd;

	MSG msg;
	int fGotMessage;

	hinst = hinstance;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	// ���������ʽ���������õ���ʽ��ʾ�����ڴ�С�仯����Ҫ�ػ�
	wc.style = CS_HREDRAW | CS_VREDRAW;
	// һ������ָ�룬�������������������Ϣ�� ��� MainWndProc������ע�͡�
	wc.lpfnWndProc = MainWndProc;
	// no extra class memory 
	wc.cbClsExtra = 0;
	// no extra window memory
	wc.cbWndExtra = 0;
	// handle to instance 
	wc.hInstance = hinstance;
	// hIcon��Ա����ָ�����ڵ�ͼ��
	// ����ֱ��ʹ��LoadIcon����������һ��ϵͳԤ�����ͼ�꣬������Ա��Ҳ�����Լ�����ͼ�ꡣ
	wc.hIcon = LoadIcon(NULL,
		IDI_ERROR);
	// Cursor������꣬�������趨����������ʽ��
	// ֱ��ʹ��LoadCursor API����������һ��ϵͳԤ����Ĺ����ʽ������IDC_CROSS,IDC_HAND����ʽ 
	wc.hCursor = LoadCursor(NULL,
		IDC_CROSS);
	// GetStockObject�Ĺ����Ǽ���һ��ϵͳԤ���壨��ջ�У���GDI����
	// ������ص���һ����ɫ�Ļ�ˢ���йػ�ˢ��GDI�������GDI˵����
	wc.hbrBackground = (HBRUSH)GetStockObject(
		WHITE_BRUSH);
	// ���ڵĲ˵�����Դ����
	wc.lpszMenuName = "MainMenu";
	// ����������һ�����֣��ڴ�������ʱ��Ҫ������֡�
	wc.lpszClassName = "MainWClass";

	// Register the window class. 

	if (!RegisterClass(&wc))
	{
		// ����ע��ʧ�ܣ���Ϣ����ʾ�����˳���
		MessageBox(NULL, "��������classʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}
	// ����ע��ɹ����������С�


	// Create the main window. 

	hwnd = CreateWindow(
		"MainWClass",			// �����������������Ѿ�ע���˵Ĵ�����
		"",		// title-bar string 
		WS_OVERLAPPEDWINDOW,	// ���ڵ�style�������ʾΪtop-level window 
		CW_USEDEFAULT,			// ����ˮƽλ��default horizontal POINT 
		CW_USEDEFAULT,			// ���ڴ�ֱλ��default vertical POINT 
		CW_USEDEFAULT,			// ���ڿ�� default width 
		CW_USEDEFAULT,			// ���ڸ߶� default height 
		(HWND)NULL,				// �����ھ�� no owner window 
		(HMENU)NULL,			// ���ڲ˵��ľ�� use class menu 
		hinstance,				// Ӧ�ó���ʵ����� handle to application instance 
		(LPVOID)NULL);			// ָ�򸽼����ݵ�ָ�� no window-creation data 

	if (!hwnd)
	{
		// ���ڴ���ʧ�ܣ���Ϣ����ʾ�����˳���
		MessageBox(NULL, "��������ʧ��", "����", MB_ICONERROR | MB_OK);
		return -1;
	}

	// ���ڴ����ɹ����������С�

	// ��ʾ���ڣ�WinMain������nCmdShow���������﷢�����ã�һ�㶼����ΪSW_SHOW
	ShowWindow(hwnd, nCmdShow);

	// ˢ�´��ڣ��򴰿ڷ���һ��WM_PAINT��Ϣ��ʹ�ô��ڽ����ػ档
	UpdateWindow(hwnd);


	/*******************************************************************************
	* ##########   MSG���ͺ���Ϣ    ##########
	*
	* Windows����Ӧ�ó�����һ����Ϣ�����ĳ����ܡ�
	* Windows����Ӧ�ó��������Ժ������߳̾�һֱ����
	* "�ȴ���Ϣ--��ȡ��Ϣ--ЧӦ��Ϣ" ��ѭ��֮�У�ֱ�������˳���
	*
	* ��ν��Ϣ��Windowsϵͳ���е�һ��ϵͳ����ͻ��ƣ�
	* �ǲ���ϵͳ��ĳЩϵͳ״̬����ʱ�ӣ����û����������������������ϵͳ�жϽ��з�װ�Ժ�
	* ���Է��͵�Ӧ�ó��򣬲���Ӧ�ó�����д�����û�̬����
	*
	* "MSG"����������ʾ"��Ϣ"���������͡� ���ﶨ����һ��MSG���͵ı�����������Ϊmsg
	*
	*******************************************************************************/


	// ���½�����Ϣѭ������ȡ��Ϣ--������Ϣ--������Ϣ���ɴ��ڵ���Ϣ��������������Ϣ��
	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0
		&& fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

HBITMAP hbmpSky;

LONG CALLBACK MainWndProc(
	HWND hwnd, //
	UINT msg, // ��Ϣ
	WPARAM wParam, // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
	LPARAM lParam) // ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
{
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rectNew;
	LPRECT lprect;
	// ע�⣬��switch-case, ÿ��������������ã�ֻ�����뵽һ��case�С�
	switch (msg)
	{
		// �����ڱ�����ʱ���յ��ĵ�һ����Ϣ����WM_CREATE��
		// һ���յ������Ϣ��������У�������������һЩ��ʼ���Ĺ���
	case WM_CREATE:
		OnCreate(hwnd);
		break;

		// ��ϵͳ��Ϊ�����ϵ�GDI����Ӧ�ñ��ػ�ʱ�����򴰿ڷ���һ��WM_PAINT��Ϣ��
		// ��ȻӦ�ó���Ҳ����ͨ������ UpateWindow�������򴰿ڷ���һ��WM_PAINT��Ϣ��
		// ����ʹ��GDI�ڴ����ϻ���ͼ�εĳ��� �����롱 д�����
		// ���������WM_PAINT��Ϣ�Ĵ�������л���GDIͼ�Σ���ô�ڴ���ˢ��ʱ�ͻᱻ�±�Ĩ���͸���
	case WM_PAINT:
		OnPaint(hwnd);
		break;

	case WM_MOVING:
		lprect = (LPRECT)lParam;
	    break;
	case WM_SIZE:
	{
		RECT rectNew;
		GetWindowRect(hwnd, &rectNew);
		OnResizeAndMove(hwnd, &rectNew);
		break;
	}
	case WM_KEYDOWN:
	{
		OnKeyDown(hwnd, (UINT)wParam);
		break;
	}
	case WM_LBUTTONDOWN:
		break;

	case WM_TIMER:
		OnTimer(hwnd);
		SendMessage(hwnd, WM_PAINT, 0, 0);
		break;

	case WM_DESTROY:
		ExitProcess(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}