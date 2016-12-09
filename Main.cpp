//  도형 그리기까지 완성
// 도형 선택 및 이동 완성
// 함수로 정리 완료
// 도형 선택, 이동, 사이즈변경 완료

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

int fontHeight = 0;	// 문자열 높이

					//// 캐럿이 있는 위치
					//int linePos = 0;	// 라인에 있는 번째수
					//int line = 0;	// 몇번째 라인에 있는지
					//int pos = 0;	// 전체에서 몇번째에 있는지


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;	// 클래스 여분 바이트 수
	WndClass.cbWndExtra = 0;	// 윈도우 여분 바이트 수
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// 윈도우의 배경색
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);	// 프로그램에서 사용할 커서
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// 프로그램에서 사용할 아이콘
	WndClass.hInstance = hInstance;	// 프로그램의 인스턴스 핸들
	WndClass.lpfnWndProc = (WNDPROC)WndProc;	// 프로시저의 함수명
	WndClass.lpszClassName = lpszClass;	// 구조체로 만들어질 클래스명
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);	// 프로그램에서 사용할 메뉴
	WndClass.style = CS_HREDRAW | CS_VREDRAW;	// 윈도우 스타일

												// 윈도우 클래스 생성
	RegisterClass(&WndClass);

	// 윈도우창 만들기 (윈도우 객체 생성)
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, (HMENU)NULL, hInstance, NULL);

	// 만든 윈도우창 띄우기
	ShowWindow(hWnd, nCmdShow);

	// 메세지 루프 돌리기
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);	// 키보드 입력 이벤트 중 문자 입력을 처리하는 함수
									// 문자 키가 눌렸는지를 검사, 눌렸다면 WM_CHAR 메세지를 발생시키는 기능

		DispatchMessage(&Message);
	}

	return Message.wParam;
	return 0;
}

void init();

void WmCharStr(HWND hWnd, WPARAM wParam, Shape * s);
void WmImeStr(HWND hWnd, LPARAM lParam, Shape * s);
void transformCoordinte(POINT & p, double zoomRange, POINT startPoint);
// 파일입출력
BOOL GetOpenFilePath(HWND hWnd);	// 불러올 파일 경로 얻기
BOOL GetSaveFilePath(HWND hWnd);	// 저장할 파일 경로
void OpenFile(wchar_t filename[], HWND hWnd);	// 파일 불러오기
void SaveFile(wchar_t filename[], HWND hWnd);	// 파일 저장하기

// xml 파일 입출력
void OpenXmlFile(wchar_t filename[], HWND hWnd);
void SaveXmlFile(wchar_t filename[]);

bool isCreating = FALSE;	// 도형이 만들어지는중인지 아닌지
bool isSelected = FALSE;	// 선택된게 존재하는지 아닌지
bool isResizing = FALSE;
bool isMoving = FALSE;
bool bCapture;
int zoom;
XFORM XForm = { 1, 0, 0, 1, 0, 0 };
// 줌의 범위
double zoomRange[] = { 0.25, 0.5 , 1, 1.25, 1.5, 2 ,3 };

POINT startPos;	// 원점의 위치
list<Shape *> lt;
Shape * shape;

// 파일 입출력
OPENFILENAME OFN, SFN;
wchar_t str[100], openFileName[100] = L"", saveFileName[100] = L"*.txt";
wchar_t filter[] = L"텍스트문서(.txt)\0*.txt;*.doc;*.xml\0모든파일(*.*) \0*.*\0";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rt;

	static POINT start, old, end;	// 클릭 시작점과 끝점
	static bool clicked = FALSE;	// 마우스 왼쪽버튼의 클릭상태
	static int position = 0;	// 도형의 위치

	//Shape * shape = NULL;
	//static list<Shape *> lt;
	static list<Shape *>::iterator selectLt;

	// 패닝
	int xInc, yInc;

	// 펜 속성
	HPEN hPen, oldPen;

	// 글자 속성
	HFONT hFont, OldFont;
	HFONT g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;

	switch (iMessage) {
	case WM_CREATE:
		startPos = { 0, };
		zoom = 2;	// 100% 상태
		return 0;
	case WM_COMMAND:
		switch (wParam) {
		case 40001:	// 원
			shape = new Circle();
			isCreating = TRUE;	// 도형을 만들것이다!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40002:	// 사각형
			shape = new Rect();
			isCreating = TRUE;	// 도형을 만들것이다!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40003:	// 선
			shape = new Line();
			isCreating = TRUE;	// 도형을 만들것이다!
			isMoving = FALSE;
			isResizing = FALSE;
			break;
		case 40004:	// 파일 열기
			cout << "파일 열기 버튼" << endl;
			if (GetOpenFilePath(hWnd))
				OpenFile(openFileName, hWnd);
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		case 40005:	// 파일 저장
			cout << "파일 저장 버튼" << endl;
			if (GetSaveFilePath(hWnd))
				SaveFile(saveFileName, hWnd);
			break;
		case 40006:	// xml 파일 열기
			if (GetOpenFilePath(hWnd))
				OpenXmlFile(openFileName, hWnd);
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		case 40007:	// xml 파일 저장
			if (GetSaveFilePath(hWnd))
				SaveXmlFile(saveFileName);
			break;
		case 40008:	// 새로 만들기
			init();
			InvalidateRgn(hWnd, NULL, TRUE);
			break;
		}
		return 0;
	case WM_LBUTTONDOWN:	// 마우스 왼쪽 클릭
							// 디바이스 좌표
		start.x = LOWORD(lParam);
		start.y = HIWORD(lParam);

		transformCoordinte(start, zoomRange[zoom], startPos);
		old.x = start.x;	// 이전 좌표를 기억해둘 변수
		old.y = start.y;
		clicked = TRUE;	// 마우스 클릭 시작

						// 1. 도형 그리기
		if (isCreating) {
			isSelected = TRUE;
			return 0;
		}

		if (isSelected) {	// 선택된 도형이 존재할 때
							// 선택된 도형보다 위에있는 도형을 선택했을 때
			auto i_lt = lt.begin();
			for (int i = 0; i < position; i++) {
				if ((*i_lt)->Select(start)) {
					position = i;
					isMoving = TRUE;	// 도형을 이동시킬수 있음!
					isResizing = FALSE;	// 도형을 리사이징 시킬수 있음!
					selectLt = i_lt;	// 선택된 도형의 정보
					return 0;
				}
				i_lt++;
			}
			// 
			if ((*selectLt)->SelectPoint(start) >= 1
				&& (*selectLt)->SelectPoint(start) <= 2) {
				isResizing = TRUE;	// 도형을 리사이징 시킬수 있음!
				isMoving = FALSE;	// 도형을 이동시킬수 없음!
				return 0;
			}

			// 도형안에 있는 점인지 아닌지
			if ((*selectLt)->Select(start)) {
				isResizing = FALSE;	// 도형을 리사이징 시킬수 없음!
				isMoving = TRUE;	// 도형을 이동시킬수 있음!
			}
			else {
				for (int i = position; i < lt.size(); i++) {
					if ((*i_lt)->Select(start)) {
						//cout << "뒤에있는 도형 찾음" << endl;
						position = i;
						isMoving = TRUE;	// 도형을 이동시킬수 있음!
						isResizing = FALSE;	// 도형을 리사이징 시킬수 없음!
						selectLt = i_lt;	// 선택된 도형의 정보
						return 0;
					}
					i_lt++;
				}
				//cout << "뒤에있는 도형 못찾음" << endl;
				position = lt.size();
				isSelected = FALSE;
			}
		}
		else {	// 선택된 도형이 존재하지 않을 때
			position = 0;
			for (auto i_lt = lt.begin(); i_lt != lt.end(); i_lt++)
			{
				// 도형을 찾았을때
				if ((*i_lt)->Select(start)) {
					//cout << "찾았다! = " << position << std::endl;
					isSelected = TRUE;	// 도형이 선택되어져있음
					isMoving = TRUE;	// 도형을 이동시킬수 있음!
					isResizing = FALSE;	// 도형을 리사이징 시킬수 없음!
					selectLt = i_lt;	// 선택된 도형의 정보
					break;
				}
				position++;
			}
			// 도형을 못찾았을 때
			if (position == lt.size())
			{
				isMoving = FALSE;
				isResizing = FALSE;
				isSelected = FALSE;
			}
		}
		return 0;
	case WM_RBUTTONDOWN:	// 패닝
		start.x = LOWORD(lParam);
		start.y = HIWORD(lParam);
		//transformCoordinte(start, zoomRange[zoom], startPos);

		old.x = start.x;	// 이전 좌표를 기억해둘 변수
		old.y = start.y;
		bCapture = TRUE;
		//SetCapture(hWnd);
		return 0;
	case WM_MOUSEMOVE: {
		end.x = LOWORD(lParam);
		end.y = HIWORD(lParam);
		//transformCoordinte(end, zoomRange[zoom], startPos);
		hdc = GetDC(hWnd);

		if (bCapture) {	// 마우스 오른쪽 버튼 클릭일때
						//GetClientRect(hWnd, &rt);
						// x 증가값 계산
			startPos.x += (end.x - old.x);
			// y 증가값 계산
			startPos.y += (end.y - old.y);
			//cout << "원점 (" << startPos.x << ", " << startPos.y << ")" << endl;
			InvalidateRgn(hWnd, NULL, TRUE);
			old.x = end.x;
			old.y = end.y;
			//cout << "startpos : " << startPos.x << "," << startPos.y << endl;
			return 0;
		}

		if (clicked) {	// 마우스 왼쪽 버튼 클릭일 때
			transformCoordinte(end, zoomRange[zoom], startPos);
			// 1. 도형 그리기
			if (isCreating) {
				//shape->SetEnd(end);
				shape->SetPoint(start, end);
			}
			// 2. 도형 이동하기
			if (isMoving) {
				(*selectLt)->Move(old, end);
			}
			// 3. 도형 리사이즈
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
		bCapture = FALSE;	// 마우스 오른쪽 버튼 클릭 끝
		//ReleaseCapture();
		return 0;
	case WM_LBUTTONUP:
		end.x = LOWORD(lParam);
		end.y = HIWORD(lParam);
		transformCoordinte(end, zoomRange[zoom], startPos);

		clicked = FALSE;	// 마우스 왼쪽 버튼 클릭 끝

							// 1. 도형 그리기
		if (isCreating) {
			shape->SetPoint(start, end);
			lt.push_front(shape);
			selectLt = lt.begin();
			position = 0;
			isCreating = FALSE;
		}
		// 2. 도형 이동하기
		if (isMoving) {
			(*selectLt)->Move(old, end);
		}
		// 3. 도형 리사이즈
		if (isResizing) {
			(*selectLt)->Resize(old, end);
		}

		isMoving = FALSE;
		isResizing = FALSE;

		InvalidateRgn(hWnd, NULL, TRUE);
		return 0;

	case WM_MOUSEWHEEL: {
		// LOWORD(wParam) : 키보드 상태
		// HIWORD(wParam) : 휠이 굴러간거리
		// HIWORD(wParam)이 +120이면 위로 휠, -120이면 아래로 휠
		//cout << "휠 거리 : " << (short)HIWORD(wParam) << endl;
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
			// 만들어진지 오래된것부터 그려줌
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
		// 그려지는 중일때
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

// 초기화
void init()
{
	isCreating = FALSE;	// 도형이 만들어지는중인지 아닌지
	isSelected = FALSE;	// 선택된게 존재하는지 아닌지
	isResizing = FALSE;
	isMoving = FALSE;

	startPos = { 0, };
	zoom = 2;	// 100% 상태

	// list 정리
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
	case VK_RETURN:	// 엔터를 눌렀을때
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
	POINT pt;	// 받아올 캐럿 위치
				//SIZE sz;
				// 첫번째 입력받은건지 아닌지
	static bool firstInput = TRUE;
	// 이전에 문자열
	static wstring oldStr;
	int p = s->getPos();

	hdc = GetDC(hWnd);
	hIMC = ImmGetContext(hWnd);	// IME 핸들을 얻음
	if (hIMC == NULL) {
		return;
	}

	int cLen = 0;
	wchar_t comp[4] = { 0, };
	//list<wstring>::iterator i_lt;

	// IME 문자조합이 완료
	if (lParam & GCS_RESULTSTR) {
		cLen = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
		if (cLen > 0) {
			ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, comp, cLen);
			comp[cLen] = 0;	// 문자열의 마지막
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
			//pos++;	// 캐럿 한칸 뒤로
			//linePos++;	// 줄의 캐럿위치도 한칸 뒤로
		}
	}
	else if (lParam & GCS_COMPSTR) {	// IME 문자 조합중
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
	// IME 핸들 해제
	ImmReleaseContext(hWnd, hIMC);
	ReleaseDC(hWnd, hdc);
}
// 논리 좌표 이동
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

	OFN.lStructSize = sizeof(OPENFILENAME);	// OPENFILENAME 구조체의 크기
	OFN.hwndOwner = hWnd;	// 대화상자의 주인 윈도우
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = openFileName;	// 선택한 파일의 절대 경로
	OFN.nMaxFile = 100;	// 선택한 파일 이름의 최대 길이
	OFN.lpstrInitialDir = L".";	// 대화상자에 먼저보여줄 폴더
	OFN.lpstrTitle = L"파일을 선택하세요";

	if (GetOpenFileName(&OFN) == FALSE)
	{
		DWORD dw = CommDlgExtendedError();
		return FALSE;
	}

	wsprintf(str, L"%s 파일을 열겠습니까?", OFN.lpstrFile);
	MessageBox(hWnd, str, L"열기 선택", MB_OK);
	return TRUE;
}

void OpenFile(wchar_t filename[], HWND hWnd)
{
	//FILE *fPtr;
	HDC hdc;
	int line = 0;
	// 파일 열기 및 읽기
	ifstream input(filename);
	char inputline[MAX_PATH];
	wchar_t tmpInputLine[MAX_PATH];

	// 문자열 자르기
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
		memset(tmpInputLine, 0, sizeof(tmpInputLine));	// 멀티바이트 -> 유니코드 변환
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, inputline, -1, tmpInputLine, strlen(inputline));
		if (line == 0)	// zoom 입력받음
		{
			zoom = _wtoi(tmpInputLine);
			//cout << "zoom : " << zoom << endl;
		}
		else if (line == 1)	// 원점 입력받음
		{
			wchar_t *ptr = wcstok_s(tmpInputLine, split, &context);      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
																		 //ptr = wcstok_s(ptr, split, &context);      // 다음 문자열을 잘라서 포인터를 반환
			startPos.x = _wtoi(ptr);
			ptr = wcstok_s(NULL, split, &context);      // 다음 문자열을 잘라서 포인터를 반환
			startPos.y = _wtoi(ptr);
			//cout << "start : " << startPos.x << ", " << startPos.y << endl;
		}
		else {	// 나머지 도형의 정보들
			//Shape * shape;
			POINT p1, p2;
			wchar_t *str;
			wchar_t *ptr = wcstok_s(tmpInputLine, split, &context);      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
																		 //ptr = wcstok_s(ptr, split, &context);      // 다음 문자열을 잘라서 포인터를 반환
			p1.x = _wtoi(wcstok_s(NULL, split, &context));
			p1.y = _wtoi(wcstok_s(NULL, split, &context));
			//shape->SetP1(p1);      // 다음 문자열을 잘라서 포인터를 반환);
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

	SFN.lStructSize = sizeof(OPENFILENAME);	// OPENFILENAME 구조체의 크기
	SFN.hwndOwner = hWnd;	// 대화상자의 주인 윈도우
	SFN.lpstrFilter = filter;
	SFN.lpstrFile = saveFileName;	// 선택한 파일의 절대 경로
	SFN.nMaxFile = 100;	// 선택한 파일 이름의 최대 길이
	SFN.lpstrInitialDir = L".";	// 대화상자에 먼저보여줄 폴더
	SFN.lpstrTitle = L"다른 이름으로 저장~";

	if (GetSaveFileName(&SFN) == FALSE)
	{
		DWORD dw = CommDlgExtendedError();
		return FALSE;
	}

	wsprintf(str, L"%s 파일로 저장하겠습니까?", SFN.lpstrFile);
	MessageBox(hWnd, str, L"저장하기 선택", MB_OK);
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

// xml 파일 입출력
void OpenXmlFile(wchar_t filename[], HWND hWnd)
{

	// 문자열 자르기
	wchar_t split[] = L" ";
	wchar_t *context = NULL;
	wchar_t *ptr = NULL;      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
	//Shape * shape;

	if (SUCCEEDED(::CoInitialize(NULL)))	// 성공시
	{
		::setlocale(LC_ALL, "");
		{
			_bstr_t bstr1;
			IXMLDOMDocumentPtr xmlDocument;
			IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
			_variant_t variant;	// 어트리뷰트의 값을 전달할때는 VARIANT형을 사용

			VARIANT_BOOL variantBool;

			IXMLDOMNodeListPtr xmlNodeList = NULL;
			IXMLDOMNodePtr xmlNode1 = NULL, xmlNode2 = NULL, xmlNode3 = NULL;

			if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument))))
			{
				::wprintf(L"xmlDocument가 생성되지 않았습니다.\n");
				::CoUninitialize();
				return;
			}

			init();

			// <?xml version="1.0"?> 부분을 만들기 위한 소스
			// version 항목에는 1.0 값을 부여
			variant = filename;
			xmlDocument->load(variant, &variantBool);
			if (!variantBool)
				return;

			BSTR bstr;
			wchar_t * tmpStr = NULL;

			// zoom 가져오기
			xmlDocument->selectSingleNode(L"/file/zoom", &xmlNode1);
			xmlNode1->get_text(&bstr);
			zoom = _wtoi((const wchar_t *)bstr);

			// startPoint 가져오기
			xmlDocument->selectSingleNode(L"/file/startPoint", &xmlNode2);
			xmlNode2->get_text(&bstr);

			tmpStr = (wchar_t *)bstr;
			startPos.x = _wtoi(wcstok_s(tmpStr, split, &context));
			startPos.y = _wtoi(wcstok_s(NULL, split, &context));

			cout << "zoom = " << zoom << ", start = " << startPos.x << ", " << startPos.y << endl;
			// shapeList 가져오기
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

				// 클래스 이름
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
	if (SUCCEEDED(::CoInitialize(NULL)))	// 성공시
	{
		::setlocale(LC_ALL, "");
		{
			_bstr_t bstr1, bstr2, bstr3;
			// XML 문서를 다루기 위한 XML DOM Documen 인스턴스 생성
			// COM 클래스는 이름끝에 Ptr이라 붙는 자료형을 제공 -> 스마트 포인터
			// 자신이 선언된 스코프가 끝날때 메모리를 자동으로 해제하게 만들어짐
			IXMLDOMDocumentPtr xmlDocument;
			IXMLDOMProcessingInstructionPtr xmlProcessingInstruction;
			IXMLDOMElementPtr xmlElement1, xmlElement2, xmlElement3, xmlElement4;
			IXMLDOMAttributePtr xmlAttribute;
			_variant_t variant;	// 어트리뷰트의 값을 전달할때는 VARIANT형을 사용
			if (!SUCCEEDED(xmlDocument.CreateInstance(__uuidof(DOMDocument))))
			{
				::wprintf(L"xmlDocument가 생성되지 않았습니다.\n");
				::CoUninitialize();
				return;
			}
			// <?xml version="1.0"?> 부분을 만들기 위한 소스
			// version 항목에는 1.0 값을 부여
			bstr1 = TEXT("xml");
			bstr2 = TEXT("version=\"1.0\" encoding=\"UNICODE\"");

			// 지정한 항목과 값으로 ProcessingInstruction을 생성 
			// 마지막 매개변수를 통해 생성된 인스턴스가 반환됨
			xmlDocument->createProcessingInstruction(bstr1, bstr2, &xmlProcessingInstruction);

			// XMLDocument에 추가
			xmlDocument->appendChild(xmlProcessingInstruction, NULL);

			// <document> </document> 부분  추가
			bstr1 = TEXT("file");
			xmlDocument->createElement(bstr1, &xmlElement1);
			xmlDocument->appendChild(xmlElement1, NULL);	// root element를 xmldocument에 추가
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
						// class명
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
			// 저장하기
			bstr1 = filename;
			variant = bstr1;
			xmlDocument->save(variant);
		}
		::CoUninitialize();
	}
}