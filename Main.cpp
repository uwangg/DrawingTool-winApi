//  ���� �׸������ �ϼ�
// ���� ���� �� �̵� �ϼ�
// �Լ��� ���� �Ϸ�
// ���� ����, �̵�, ������� �Ϸ�

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <list>
#include "resource.h"
#include "Circle.h"
#include "Line.h"
#include "Rect.h"

#import <msxml4.dll>
#include <locale.h>

using namespace std;

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPWSTR lpszClass = L"uwangg's Drawing Tool";

int fontHeight = 0;	// ���ڿ� ����

					//// ĳ���� �ִ� ��ġ
					//int linePos = 0;	// ���ο� �ִ� ��°��
					//int line = 0;	// ���° ���ο� �ִ���
					//int pos = 0;	// ��ü���� ���°�� �ִ���


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;	// Ŭ���� ���� ����Ʈ ��
	WndClass.cbWndExtra = 0;	// ������ ���� ����Ʈ ��
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// �������� ����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// ���α׷����� ����� Ŀ��
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// ���α׷����� ����� ������
	WndClass.hInstance = hInstance;	// ���α׷��� �ν��Ͻ� �ڵ�
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	// ���ν����� �Լ���
	WndClass.lpszClassName = lpszClass;	// ����ü�� ������� Ŭ������
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);	// ���α׷����� ����� �޴�
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	// ������ ��Ÿ��

												// ������ Ŭ���� ����
	RegisterClass(&WndClass);

	// ������â ����� (������ ��ü ����)
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, (HMENU)NULL, hInstance, NULL);

	// ���� ������â ����
	ShowWindow(hWnd, nCmdShow);

	// �޼��� ���� ������
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);	// Ű���� �Է� �̺�Ʈ �� ���� �Է��� ó���ϴ� �Լ�
									// ���� Ű�� ���ȴ����� �˻�, ���ȴٸ� WM_CHAR �޼����� �߻���Ű�� ���

		DispatchMessage(&Message);
	}

	return Message.wParam;
	return 0;
}

void init();

void WmCharStr(HWND hWnd, WPARAM wParam, Shape * s);
void WmImeStr(HWND hWnd, LPARAM lParam, Shape * s);
void transformCoordinte(POINT & p, double zoomRange, POINT startPoint);
// ���������
BOOL GetOpenFilePath(HWND hWnd);	// �ҷ��� ���� ��� ���
BOOL GetSaveFilePath(HWND hWnd);	// ������ ���� ���
void OpenFile(wchar_t filename[], HWND hWnd);	// ���� �ҷ�����
void SaveFile(wchar_t filename[], HWND hWnd);	// ���� �����ϱ�

// xml ���� �����
void OpenXmlFile(wchar_t filename[], HWND hWnd);
void SaveXmlFile(wchar_t filename[]);

bool isCreating = FALSE;	// ������ ��������������� �ƴ���
bool isSelected = FALSE;	// ���õȰ� �����ϴ��� �ƴ���
bool isResizing = FALSE;
bool isMoving = FALSE;
bool bCapture;
int zoom;
XFORM XForm = { 1, 0, 0, 1, 0, 0 };
// ���� ����
double zoomRange[] = { 0.25, 0.5 , 1, 1.25, 1.5, 2 ,3 };

POINT startPos;	// ������ ��ġ
list<Shape *> lt;
Shape * shape;

// ���� �����
OPENFILENAME OFN, SFN;
wchar_t str[100], openFileName[100] = L"", saveFileName[100] = L"*.txt";
wchar_t filter[] = L"�ؽ�Ʈ����(.txt)\0*.txt;*.doc;*.xml\0�������(*.*) \0*.*\0";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rt;

	static POINT start, old, end;	// Ŭ�� �������� ����
	static bool clicked = FALSE;	// ���콺 ���ʹ�ư�� Ŭ������
	static int position = 0;	// ������ ��ġ

	//Shape * shape = NULL;
	//static list<Shape *> lt;
	static list<Shape *>::iterator selectLt;

	// �д�
	int xInc, yInc;

	// �� �Ӽ�
	HPEN hPen, oldPen;

	// ���� �Ӽ�
	HFONT hFont, OldFont;
	HFONT g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;

	switch (iMessage) {
	case WM_CREATE:
		startPos = { 0, };
		zoom = 2;	// 100% ����
		return 0;
	case WM_COMMAND:
		switch (wParam) {
		case 40001:	// ��
			shape = new Circle();
			isCreating = TRUE;	// ������ ������̴�!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40002:	// �簢��
			shape = new Rect();
			isCreating = TRUE;	// ������ ������̴�!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40003:	// ��
			shape = new Line();
			isCreating = TRUE;	// ������ ������̴�!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40004:	// ���� ����
			cout << "���� ���� ��ư" << endl;
			if (GetOpenFilePath(hWnd))
				OpenFile(openFileName, hWnd);
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		case 40005:	// ���� ����
			cout << "���� ���� ��ư" << endl;
			if (GetSaveFilePath(hWnd))
				SaveFile(saveFileName, hWnd);
			break;
		case 40006:	// xml ���� ����
			if (GetOpenFilePath(hWnd))
				OpenXmlFile(openFileName, hWnd);
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		case 40007:	// xml ���� ����
			if (GetSaveFilePath(hWnd))
				SaveXmlFile(saveFileName);
			break;
		case 40008:	// ���� �����
			init();
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		}
		return 0;
	case WM_LBUTTONDOWN:	// ���콺 ���� Ŭ��
							// ����̽� ��ǥ
		start.x = LOWORD(lParam);
		start.y = HIWORD(lParam);

		transformCoordinte(start, zoomRange[zoom], startPos);
		old.x = start.x;	// ���� ��ǥ�� ����ص� ����
		old.y = start.y;
		clicked = TRUE;	// ���콺 Ŭ�� ����

						// 1. ���� �׸���
		if (isCreating) {
			isSelected = TRUE;
			return 0;
		}

		if (isSelected) {	// ���õ� ������ ������ ��
							// ���õ� �������� �����ִ� ������ �������� ��
			auto i_lt = lt.begin();
			for (int i = 0; i < position; i++) {
				if ((*i_lt)->Select(start)) {
					position = i;
					isMoving = TRUE;	// ������ �̵���ų�� ����!
					isResizing = FALSE;	// ������ ������¡ ��ų�� ����!
					selectLt = i_lt;	// ���õ� ������ ����
					return 0;
				}
				i_lt++;
			}
			// 
			if ((*selectLt)->SelectPoint(start) >= 1
				&& (*selectLt)->SelectPoint(start) <= 2) {
				isResizing = TRUE;	// ������ ������¡ ��ų�� ����!
				isMoving = FALSE;	// ������ �̵���ų�� ����!
				return 0;
			}

			// �����ȿ� �ִ� ������ �ƴ���
			if ((*selectLt)->Select(start)) {
				isResizing = FALSE;	// ������ ������¡ ��ų�� ����!
				isMoving = TRUE;	// ������ �̵���ų�� ����!
			}
			else {
				for (int i = position; i < lt.size(); i++) {
					if ((*i_lt)->Select(start)) {
						//cout << "�ڿ��ִ� ���� ã��" << endl;
						position = i;
						isMoving = TRUE;	// ������ �̵���ų�� ����!
						isResizing = FALSE;	// ������ ������¡ ��ų�� ����!
						selectLt = i_lt;	// ���õ� ������ ����
						return 0;
					}
					i_lt++;
				}
				//cout << "�ڿ��ִ� ���� ��ã��" << endl;
				position = lt.size();
				isSelected = FALSE;
			}
		}
		else {	// ���õ� ������ �������� ���� ��
			position = 0;
			for (auto i_lt = lt.begin(); i_lt != lt.end(); i_lt++)
			{
				// ������ ã������
				if ((*i_lt)->Select(start)) {
					//cout << "ã�Ҵ�! = " << position << std::endl;
					isSelected = TRUE;	// ������ ���õǾ�������
					isMoving = TRUE;	// ������ �̵���ų�� ����!
					isResizing = FALSE;	// ������ ������¡ ��ų�� ����!
					selectLt = i_lt;	// ���õ� ������ ����
					break;
				}
				position++;
			}
			// ������ ��ã���� ��
			if (position == lt.size())
			{
				isMoving = FALSE;
				isResizing = FALSE;
				isSelected = FALSE;
			}
		}
		return 0;
	case WM_RBUTTONDOWN:	// �д�
		start.x = LOWORD(lParam);
		start.y = HIWORD(lParam);
		//transformCoordinte(start, zoomRange[zoom], startPos);

		old.x = start.x;	// ���� ��ǥ�� ����ص� ����
		old.y = start.y;
		bCapture = TRUE;
		//SetCapture(hWnd);
		return 0;
	case WM_MOUSEMOVE: {
		end.x = LOWORD(lParam);
		end.y = HIWORD(lParam);
		//transformCoordinte(end, zoomRange[zoom], startPos);
		hdc = GetDC(hWnd);

		if (bCapture) {	// ���콺 ������ ��ư Ŭ���϶�
						//GetClientRect(hWnd, &rt);
						// x ������ ���
			startPos.x += (end.x - old.x);
			// y ������ ���
			startPos.y += (end.y - old.y);
			//cout << "���� (" << startPos.x << ", " << startPos.y << ")" << endl;
			InvalidateRgn(hWnd, NULL, TRUE);
			old.x = end.x;
			old.y = end.y;
			//cout << "startpos : " << startPos.x << "," << startPos.y << endl;
			return 0;
		}

		if (clicked) {	// ���콺 ���� ��ư Ŭ���� ��
			transformCoordinte(end, zoomRange[zoom], startPos);
			// 1. ���� �׸���
			if (isCreating) {
				//shape->SetEnd(end);
				shape->SetPoint(start, end);
			}
			// 2. ���� �̵��ϱ�
			if (isMoving) {
				(*selectLt)->Move(old, end);
			}
			// 3. ���� ��������
			if (isResizing) {
				(*selectLt)->Resize(old, end);
			}
			old.x = end.x;
			old.y = end.y;
			InvalidateRgn(hWnd, NULL, TRUE);
			return 0;
			ReleaseDC(hWnd, hdc);
		}
	}
	case WM_RBUTTONUP:
		bCapture = FALSE;	// ���콺 ������ ��ư Ŭ�� ��
		//ReleaseCapture();
		return 0;
	case WM_LBUTTONUP:
		end.x = LOWORD(lParam);
		end.y = HIWORD(lParam);
		transformCoordinte(end, zoomRange[zoom], startPos);

		clicked = FALSE;	// ���콺 ���� ��ư Ŭ�� ��

							// 1. ���� �׸���
		if (isCreating) {
			shape->SetPoint(start, end);
			lt.push_front(shape);
			selectLt = lt.begin();
			position = 0;
			isCreating = FALSE;
		}
		// 2. ���� �̵��ϱ�
		if (isMoving) {
			(*selectLt)->Move(old, end);
		}
		// 3. ���� ��������
		if (isResizing) {
			(*selectLt)->Resize(old, end);
		}

		isMoving = FALSE;
		isResizing = FALSE;

		InvalidateRgn(hWnd, NULL, TRUE);
		return 0;

	case WM_MOUSEWHEEL: {
		// LOWORD(wParam) : Ű���� ����
		// HIWORD(wParam) : ���� �������Ÿ�
		// HIWORD(wParam)�� +120�̸� ���� ��, -120�̸� �Ʒ��� ��
		//cout << "�� �Ÿ� : " << (short)HIWORD(wParam) << endl;
		if ((short)HIWORD(wParam) == 120)
		{
			if (zoom >= (sizeof(zoomRange) / sizeof(double)) - 1)
				return 0;
			zoom++;
		}
		else {
			if (zoom == 0)
				return 0;
			zoom--;
		}
		//cout << "zoom : " << zoom << endl;
		InvalidateRgn(hWnd, NULL, TRUE);

		return 0;
	}
	case WM_PAINT: {
		wchar_t wstr[50];
		_itow_s(zoomRange[zoom] * 100, wstr, 10);
		wcscat_s(wstr, L"%");
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10, 10, wstr, wcslen(wstr));
		GetClientRect(hWnd, &rt);
		int prevGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
		XForm.eM11 = zoomRange[zoom];
		XForm.eM22 = zoomRange[zoom];
		XForm.eDx = startPos.x;
		XForm.eDy = startPos.y;
		SetWorldTransform(hdc, &XForm);

		GetObject(g_hfFont, sizeof(LOGFONT), &lf);
		hFont = CreateFontIndirect(&lf);
		OldFont = (HFONT)SelectObject(hdc, hFont);
		int cnt = 0;
		if (!lt.empty()) {
			// ��������� �����Ȱͺ��� �׷���
			for (auto i_lt = lt.rbegin(); i_lt != lt.rend(); i_lt++)
			{
				if (cnt == (lt.size() - position - 1))
				{
					(*i_lt)->IsSelectedPrint(hdc);
				}
				else {
					(*i_lt)->IsNotSelectedPrint(hdc);
				}
				cnt++;
			}
		}
		// �׷����� ���϶�
		if (isCreating)
			shape->IsSelectedPrint(hdc);

		SelectObject(hdc, OldFont);
		DeleteObject(hFont);
		SetGraphicsMode(hdc, prevGraphicsMode);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_CHAR: {
		auto i_lt = lt.begin();
		for (int i = 0; i < position; i++)
			i_lt++;
		if (typeid(*i_lt) == typeid(Line))
			return 0;
		if (lt.empty())
			return 0;
		WmCharStr(hWnd, wParam, (*i_lt));
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_IME_COMPOSITION: {
		auto i_lt = lt.begin();
		for (int i = 0; i < position; i++)
			i_lt++;
		if (typeid(*i_lt) == typeid(Line))
			return 0;
		if (lt.empty())
			return 0;
		WmImeStr(hWnd, lParam, (*i_lt));
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

// �ʱ�ȭ
void init()
{
	isCreating = FALSE;	// ������ ��������������� �ƴ���
	isSelected = FALSE;	// ���õȰ� �����ϴ��� �ƴ���
	isResizing = FALSE;
	isMoving = FALSE;

	startPos = { 0, };
	zoom = 2;	// 100% ����

	// list ����
	for (list<Shape *>::iterator it = lt.begin(); it != lt.end(); it++)
		delete (*it);
	lt.clear();
}

// WM_CHAR
void WmCharStr(HWND hWnd, WPARAM wParam, Shape * s)
{
	HDC hdc;
	wstring str;
	//POINT pt;
	SIZE sz;
	int p = s->getPos();
	//auto i_lt = (s->GetStrLt()).begin();

	hdc = GetDC(hWnd);
	//CreateCaret(hWnd, NULL, 2, 15);
	//SetCaretPos(s->GetP1().x, s->GetP1().y);
	//ShowCaret(hWnd);
	switch (wParam) {
	case VK_RETURN:	// ���͸� ��������
					//s->GetStrLt().push_back(L"");
					//(*i_lt).insert(p, L"\n");
		s->GetStr().insert(p, L"\n");
		p++;
		s->SetPos(p);
		//	lt.push_back(L"");
		//	linePos = 0;g
		//	line++;
		//	GetCaretPos(&pt);
		//	SetCaretPos(0, pt.y + fontHeight);
		break;
	default:
		str = wParam;
		if ((s->GetStr()).length() == 0) {
			s->SetStr(str);

			p++;
			s->SetPos(p);

			break;
		}
		s->GetStr().insert(p, str);
		p++;
		s->SetPos(p);
		break;
	}
	ReleaseDC(hWnd, hdc);
}
// IME
void WmImeStr(HWND hWnd, LPARAM lParam, Shape * s) {
	HDC hdc;
	HIMC hIMC;
	POINT pt;	// �޾ƿ� ĳ�� ��ġ
				//SIZE sz;
				// ù��° �Է¹������� �ƴ���
	static bool firstInput = TRUE;
	// ������ ���ڿ�
	static wstring oldStr;
	int p = s->getPos();

	hdc = GetDC(hWnd);
	hIMC = ImmGetContext(hWnd);	// IME �ڵ��� ����
	if (hIMC == NULL) {
		return;
	}

	int cLen = 0;
	wchar_t comp[4] = { 0, };
	//list<wstring>::iterator i_lt;

	// IME ���������� �Ϸ�
	if (lParam & GCS_RESULTSTR) {
		cLen = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
		if (cLen > 0) {
			ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, comp, cLen);
			comp[cLen] = 0;	// ���ڿ��� ������
							//(*i_lt).insert(linePos, comp);
			(s->GetStr()).erase(p, oldStr.length());
			(s->GetStr()).insert(p, comp);
			/*(*i_lt).erase(linePos, oldStr.length());
			(*i_lt).insert(linePos, comp);*/
			//GetTextExtentPoint(hdc, comp, wcslen(comp), &sz);
			//GetCaretPos(&pt);
			//SetCaretPos(sz.cx + pt.x, pt.y);
			firstInput = TRUE;
			p++;
			s->SetPos(p);
			//pos++;	// ĳ�� ��ĭ �ڷ�
			//linePos++;	// ���� ĳ����ġ�� ��ĭ �ڷ�
		}
	}
	else if (lParam & GCS_COMPSTR) {	// IME ���� ������
		cLen = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
		if (cLen > 0) {
			ImmGetCompositionStringW(hIMC, GCS_COMPSTR, comp, cLen);
			//GetTextExtentPoint(hdc, comp, wcslen(comp), &sz);
			//CreateCaret(hWnd, NULL, sz.cx, 15);
			//ShowCaret(hWnd);
			if (firstInput) {
				oldStr = comp;
				firstInput = FALSE;
			}
			else {
				//(*i_lt).erase(linePos, oldStr.length());
				(s->GetStr()).erase(p, oldStr.length());
			}
			//(*i_lt).insert(linePos, comp);
			(s->GetStr()).insert(p, comp);
		}
	}
	// IME �ڵ� ����
	ImmReleaseContext(hWnd, hIMC);
	ReleaseDC(hWnd, hdc);
}
// �� ��ǥ �̵�
void transformCoordinte(POINT & p, double zoomRange, POINT startPoint)
{
	p.x = (p.x - startPoint.x) / zoomRange;
	p.y = (p.y - startPoint.y) / zoomRange;
	//p.x = p.x / zoomRange - startPoint.x;
	//p.y = p.y / zoomRange - startPoint.y;
	//p.x = p.x - startPoint.x;
	//p.y = p.y - startPoint.y;
}

BOOL GetOpenFilePath(HWND hWnd)
{
	memset(&OFN, 0, sizeof(OPENFILENAME));

	OFN.lStructSize = sizeof(OPENFILENAME);	// OPENFILENAME ����ü�� ũ��
	OFN.hwndOwner = hWnd;	// ��ȭ������ ���� ������
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = openFileName;	// ������ ������ ���� ���
	OFN.nMaxFile = 100;	// ������ ���� �̸��� �ִ� ����
	OFN.lpstrInitialDir = L".";	// ��ȭ���ڿ� ���������� ����
	OFN.lpstrTitle = L"������ �����ϼ���";

	if (GetOpenFileName(&OFN) == FALSE)
	{
		DWORD dw = CommDlgExtendedError();
		return FALSE;
	}

	wsprintf(str, L"%s ������ ���ڽ��ϱ�?", OFN.lpstrFile);
	MessageBox(hWnd, str, L"���� ����", MB_OK);
	return TRUE;
}

void OpenFile(wchar_t filename[], HWND hWnd)
{
	//FILE *fPtr;
	HDC hdc;
	int line = 0;
	// ���� ���� �� �б�
	ifstream input(filename);
	char inputline[MAX_PATH];
	wchar_t tmpInputLine[MAX_PATH];

	// ���ڿ� �ڸ���
	wchar_t split[] = L" ";
	wchar_t *context = NULL;
	wchar_t *token;

	if (input.fail()) {
		MessageBox(NULL, NULL, NULL, NULL);
		return;
	}

	init();

	while (!input.eof()) {
		input.getline(inputline, sizeof(inputline));
		memset(tmpInputLine, 0, sizeof(tmpInputLine));	// ��Ƽ����Ʈ -> �����ڵ� ��ȯ
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, inputline, -1, tmpInputLine, strlen(inputline));
		if (line == 0)	// zoom �Է¹���
		{
			zoom = _wtoi(tmpInputLine);
			//cout << "zoom : " << zoom << endl;
		}
		else if (line == 1)	// ���� �Է¹���
		{
			wchar_t *ptr = wcstok_s(tmpInputLine, split, &context);      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
																		 //ptr = wcstok_s(ptr, split, &context);      // ���� ���ڿ��� �߶� �����͸� ��ȯ
			startPos.x = _wtoi(ptr);
			ptr = wcstok_s(NULL, split, &context);      // ���� ���ڿ��� �߶� �����͸� ��ȯ
			startPos.y = _wtoi(ptr);
			//cout << "start : " << startPos.x << ", " << startPos.y << endl;
		}
		else {	// ������ ������ ������
			//Shape * shape;
			POINT p1, p2;
			wchar_t *str;
			wchar_t *ptr = wcstok_s(tmpInputLine, split, &context);      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
																		 //ptr = wcstok_s(ptr, split, &context);      // ���� ���ڿ��� �߶� �����͸� ��ȯ
			p1.x = _wtoi(wcstok_s(NULL, split, &context));
			p1.y = _wtoi(wcstok_s(NULL, split, &context));
			//shape->SetP1(p1);      // ���� ���ڿ��� �߶� �����͸� ��ȯ);
			p2.x = _wtoi(wcstok_s(NULL, split, &context));
			p2.y = _wtoi(wcstok_s(NULL, split, &context));
			//shape->SetP2(p2);
			if (wcscmp(ptr, L"Circle") == 0) {	// shapeName == Circle
				shape = new Circle();
			}
			else if (wcscmp(ptr, L"Rect") == 0) {	// shapeName == Rect
				shape = new Rect();
			}
			else if (wcscmp(ptr, L"Line") == 0) {	// shapeName == Line
				shape = new Line();
			}
			shape->SetP1(p1);
			shape->SetP2(p2);
			//if(!str.empty())
			//cout << "context = " << context << endl;
			str = wcstok_s(NULL, split, &context);
			if (str != NULL)
				shape->SetStr(str);
			//shape->SetStr(wcstok_s(NULL, split, &context));
			//shape->SetStr(wcstok_s(NULL, split, &context));
			/*cout << shape->GetP1().x << ", " << shape->GetP1().y << " / "
			<< shape->GetP2().x << ", " << shape->GetP2().y << " / "
			<< shape->GetStr().c_str() << endl;*/
			lt.push_back(shape);
			//cout << "typeid = " << typeid(*shape).name() << endl;
		}
		cout << "line = " << line << endl;
		line++;
	}
	input.close();
}

BOOL GetSaveFilePath(HWND hWnd)
{
	memset(&OFN, 0, sizeof(OPENFILENAME));

	SFN.lStructSize = sizeof(OPENFILENAME);	// OPENFILENAME ����ü�� ũ��
	SFN.hwndOwner = hWnd;	// ��ȭ������ ���� ������
	SFN.lpstrFilter = filter;
	SFN.lpstrFile = saveFileName;	// ������ ������ ���� ���
	SFN.nMaxFile = 100;	// ������ ���� �̸��� �ִ� ����
	SFN.lpstrInitialDir = L".";	// ��ȭ���ڿ� ���������� ����
	SFN.lpstrTitle = L"�ٸ� �̸����� ����~";

	if (GetSaveFileName(&SFN) == FALSE)
	{
		DWORD dw = CommDlgExtendedError();
		return FALSE;
	}

	wsprintf(str, L"%s ���Ϸ� �����ϰڽ��ϱ�?", SFN.lpstrFile);
	MessageBox(hWnd, str, L"�����ϱ� ����", MB_OK);
	return TRUE;
}

void SaveFile(wchar_t filename[], HWND hWnd)
{
	locale::global(locale("Korean"));
	wofstream output(filename);
	int line = 0;

	output << zoom << endl;
	output << startPos.x << " " << startPos.y;

	for (list<Shape*>::iterator i_lt = lt.begin(); i_lt != lt.end(); i_lt++)
	{
		cout << "typename = " << typeid(*(*i_lt)).name()
			<< ", len = " << (*i_lt)->GetStr().length() << endl;
		if (typeid(*(*i_lt)) == typeid(Circle)) {
			output << endl << L"Circle" << " " << (*i_lt)->GetP1().x << " "
				<< (*i_lt)->GetP1().y << " " << (*i_lt)->GetP2().x << " "
				<< (*i_lt)->GetP2().y << " " << (*i_lt)->GetStr().c_str();
		}
		else if (typeid(*(*i_lt)) == typeid(Rect)) {
			output << endl << L"Rect" << " " << (*i_lt)->GetP1().x << " "
				<< (*i_lt)->GetP1().y << " " << (*i_lt)->GetP2().x << " "
				<< (*i_lt)->GetP2().y << " " << (*i_lt)->GetStr().c_str();
		}
		else if (typeid(*(*i_lt)) == typeid(Line)) {
			output << endl << L"Line" << " " << (*i_lt)->GetP1().x << " "
				<< (*i_lt)->GetP1().y << " " << (*i_lt)->GetP2().x << " "
				<< (*i_lt)->GetP2().y;
		}
	}
	cout << "lt.size = " << lt.size() << endl;
	output.close();
}

// xml ���� �����
void OpenXmlFile(wchar_t filename[], HWND hWnd)
{

	// ���ڿ� �ڸ���
	wchar_t split[] = L" ";
	wchar_t *context = NULL;
	wchar_t *ptr = NULL;      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
	//Shape * shape;

	if (SUCCEEDED(::CoInitialize(NULL)))	// ������
	{
		::setlocale(LC_ALL, "");
		{
			_bstr_t bstr1;
			IXMLDOMDocumentPtr xmlDocument;
			IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
			_variant_t variant;	// ��Ʈ����Ʈ�� ���� �����Ҷ��� VARIANT���� ���

			VARIANT_BOOL variantBool;

			IXMLDOMNodeListPtr xmlNodeList = NULL;
			IXMLDOMNodePtr xmlNode1 = NULL, xmlNode2 = NULL, xmlNode3 = NULL;

			if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument))))
			{
				::wprintf(L"xmlDocument�� �������� �ʾҽ��ϴ�.\n");
				::CoUninitialize();
				return;
			}

			init();

			// <?xml version="1.0"?> �κ��� ����� ���� �ҽ�
			// version �׸񿡴� 1.0 ���� �ο�
			variant = filename;
			xmlDocument->load(variant, &variantBool);
			if (!variantBool)
				return;

			BSTR bstr;
			wchar_t * tmpStr = NULL;

			// zoom ��������
			xmlDocument->selectSingleNode(L"/file/zoom", &xmlNode1);
			xmlNode1->get_text(&bstr);
			zoom = _wtoi((const wchar_t *)bstr);

			// startPoint ��������
			xmlDocument->selectSingleNode(L"/file/startPoint", &xmlNode2);
			xmlNode2->get_text(&bstr);

			tmpStr = (wchar_t *)bstr;
			startPos.x = _wtoi(wcstok_s(tmpStr, split, &context));
			startPos.y = _wtoi(wcstok_s(NULL, split, &context));

			cout << "zoom = " << zoom << ", start = " << startPos.x << ", " << startPos.y << endl;
			// shapeList ��������
			xmlDocument->selectSingleNode(L"/file/ShapeList", &xmlNode3);
			long nodeLen = 0;
			POINT p1, p2;
			xmlNode3->selectNodes(L"//shape", &xmlNodeList);
			xmlNodeList->get_length(&nodeLen);
			cout << "nodeLen = " << nodeLen << endl;
			for (int i = 0; i < nodeLen; i++)
			{
				IXMLDOMNodePtr child1 = NULL, child2 = NULL;
				xmlNodeList->get_item(i, &child1);
				cout << "i = " << i << endl;

				// Ŭ���� �̸�
				child1->selectSingleNode(L"name", &child2);
				child2->get_text(&bstr);
				if (wcscmp(bstr, L"Circle") == 0)
					shape = new Circle();
				else if (wcscmp(bstr, L"Rect") == 0)
					shape = new Rect();
				else if (wcscmp(bstr, L"Line") == 0)
					shape = new Line();
				::wprintf(L"name = %s\n", (const wchar_t *)bstr);

				// P1
				child1->selectSingleNode(L"p1", &child2);
				child2->get_text(&bstr);
				tmpStr = (wchar_t *)bstr;
				p1.x = _wtoi(wcstok_s(tmpStr, split, &context));
				p1.y = _wtoi(wcstok_s(NULL, split, &context));
				cout << "p1 = " << p1.x << ", " << p1.y << endl;
				shape->SetP1(p1);
				//::wprintf(L"p1 = %s\n", (const wchar_t *)bstr);

				// P2
				child1->selectSingleNode(L"p2", &child2);
				child2->get_text(&bstr);
				tmpStr = (wchar_t *)bstr;
				p2.x = _wtoi(wcstok_s(tmpStr, split, &context));
				p2.y = _wtoi(wcstok_s(NULL, split, &context));
				cout << "p2 = " << p2.x << ", " << p2.y << endl;
				shape->SetP2(p2);
				//::wprintf(L"p2 = %s\n", (const wchar_t *)bstr);

					// String
				child1->selectSingleNode(L"string", &child2);
				child2->get_text(&bstr);
				cout << "strlen = " << wcslen(bstr) << endl;
				if (bstr == NULL)
					bstr = L"";
				::wprintf(L"string = %s\n", (const wchar_t *)bstr);
				tmpStr = (wchar_t *)bstr;
				shape->SetStr(tmpStr);

				//::wprintf(L"string = %s\n", (const wchar_t *)bstr);
				lt.push_back(shape);
			}
			::SysFreeString(bstr);
		}
		::CoUninitialize();
	}
}
void SaveXmlFile(wchar_t filename[])
{
	if (SUCCEEDED(::CoInitialize(NULL)))	// ������
	{
		::setlocale(LC_ALL, "");
		{
			_bstr_t bstr1, bstr2, bstr3;
			// XML ������ �ٷ�� ���� XML DOM Documen �ν��Ͻ� ����
			// COM Ŭ������ �̸����� Ptr�̶� �ٴ� �ڷ����� ���� -> ����Ʈ ������
			// �ڽ��� ����� �������� ������ �޸𸮸� �ڵ����� �����ϰ� �������
			IXMLDOMDocumentPtr xmlDocument;
			IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
			IXMLDOMElementPtr xmlElement1, xmlElement2, xmlElement3, xmlElement4;
			IXMLDOMAttributePtr xmlAttribute;
			_variant_t variant;	// ��Ʈ����Ʈ�� ���� �����Ҷ��� VARIANT���� ���
			if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument))))
			{
				::wprintf(L"xmlDocument�� �������� �ʾҽ��ϴ�.\n");
				::CoUninitialize();
				return;
			}
			// <?xml version="1.0"?> �κ��� ����� ���� �ҽ�
			// version �׸񿡴� 1.0 ���� �ο�
			bstr1 = TEXT("xml");
			bstr2 = TEXT("version=\"1.0\" encoding=\"UNICODE\"");

			// ������ �׸�� ������ ProcessingInstruction�� ���� 
			// ������ �Ű������� ���� ������ �ν��Ͻ��� ��ȯ��
			xmlDocument->createProcessingInstruction(bstr1, bstr2, &xmlProcessingInstruction);

			// XMLDocument�� �߰�
			xmlDocument->appendChild(xmlProcessingInstruction, NULL);

			// <document> </document> �κ�  �߰�
			bstr1 = TEXT("file");
			xmlDocument->createElement(bstr1, &xmlElement1);
			xmlDocument->appendChild(xmlElement1, NULL);	// root element�� xmldocument�� �߰�
			{
				wchar_t buf[MAX_PATH];
				bstr1 = TEXT("zoom");
				wsprintf(buf, L"%d", zoom);
				bstr2 = buf;
				xmlDocument->createElement(bstr1, &xmlElement2);
				xmlElement2->put_text(bstr2);
				xmlElement1->appendChild(xmlElement2, NULL);

				bstr1 = TEXT("startPoint");
				wsprintf(buf, L"%d %d", startPos.x, startPos.y);
				bstr2 = buf;
				xmlDocument->createElement(bstr1, &xmlElement2);
				xmlElement2->put_text(bstr2);
				xmlElement1->appendChild(xmlElement2, NULL);

				bstr1 = TEXT("ShapeList");
				xmlDocument->createElement(bstr1, &xmlElement2);
				xmlElement1->appendChild(xmlElement2, NULL);
				{
					for (list<Shape*>::iterator i_lt = lt.begin(); i_lt != lt.end(); i_lt++) {
						// class��
						wchar_t * name = NULL;
						if (typeid(*(*i_lt)) == typeid(Circle))
							name = L"Circle";
						else if (typeid(*(*i_lt)) == typeid(Rect))
							name = L"Rect";
						else if (typeid(*(*i_lt)) == typeid(Line))
							name = L"Line";

						bstr1 = TEXT("shape");
						xmlDocument->createElement(bstr1, &xmlElement3);
						xmlElement2->appendChild(xmlElement3, NULL);
						{
							bstr1 = TEXT("name");
							wsprintf(buf, L"%s", name);
							bstr2 = buf;
							xmlDocument->createElement(bstr1, &xmlElement4);
							xmlElement4->put_text(bstr2);
							xmlElement3->appendChild(xmlElement4, NULL);

							bstr1 = TEXT("p1");
							wsprintf(buf, L"%d %d", (*i_lt)->GetP1().x, (*i_lt)->GetP1().y);
							bstr2 = buf;
							xmlDocument->createElement(bstr1, &xmlElement4);
							xmlElement4->put_text(bstr2);
							xmlElement3->appendChild(xmlElement4, NULL);

							bstr1 = TEXT("p2");
							wsprintf(buf, L"%d %d", (*i_lt)->GetP2().x, (*i_lt)->GetP2().y);
							bstr2 = buf;
							xmlDocument->createElement(bstr1, &xmlElement4);
							xmlElement4->put_text(bstr2);
							xmlElement3->appendChild(xmlElement4, NULL);

							bstr1 = TEXT("string");
							bstr2 = (*i_lt)->GetStr().c_str();
							xmlDocument->createElement(bstr1, &xmlElement4);
							xmlElement4->put_text(bstr2);
							xmlElement3->appendChild(xmlElement4, NULL);

						}
					}
				}
			}

			//wcscat_s(filename, sizeof(filename), ".xml");
			// �����ϱ�
			bstr1 = filename;
			variant = bstr1;
			xmlDocument->save(variant);
		}
		::CoUninitialize();
	}
}