#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <list>
#include <string>
using namespace std;
#pragma once
class Shape
{
protected:
	// 상대 좌표
	POINT p1 = {0,};	// 좌측 상단 좌표
	POINT p2 = {0,};	// 우측 하단 좌표
	wstring str = L"";
	int pos = 0;	// 전체에서 몇번째에 있는지

	// 절대 좌표 (원래 좌표) zoom = 1일때
	POINT originP1 = { 0, };
	POINT originP2 = { 0, };

	POINT pan = { 0, };

public:
	Shape();

	//static POINT GetWindowP() { return windowP; }
	//static void SetWindowP(POINT p) { windowP = p; }
	// GET & SET
	POINT GetP1() { return this->p1; }
	void SetP1(POINT p) { this->p1 = p; }
	POINT GetP2() { return this->p2; }
	void SetP2(POINT p) { this->p2 = p; }
	POINT GetOriginP1() { return this->originP1; }
	void SetOriginP1(POINT p) { this->originP1 = p; }
	POINT GetOriginP2() { return this->originP2; }
	void SetOriginP2(POINT p) { this->originP2 = p; }

	wstring& GetStr() { return this->str; }
	void SetStr(wstring str) { this->str = str; }
	//void SetStrLt(list<wstring>& lt) { this->strlt = lt; }
	int getPos() { return this->pos; }
	void SetPos(int pos) { this->pos = pos; }

	// set & sort Point
	virtual void SetPoint(POINT start, POINT end);

	// Draw
	virtual void IsNotSelectedPrint(HDC hdc);
	virtual void IsSelectedPrint(HDC hdc);
	virtual bool Select(POINT p);	// 점 p가 도형 안에 들어있는지 아닌지
	int DrawTextCenter(HDC dc, LPCWSTR text, int cchText, LPCRECT lprc, UINT format);

	// changeSize
	int SelectPoint(POINT p);
	void Resize(POINT start, POINT end);

	// 이동하기
	void Move(POINT start, POINT end);

	// input string
	//void WmCharStr(HWND hWnd, WPARAM wParam, list<wstring>& lt);
	//void WmImeStr(HWND hWnd, LPARAM lParam, list<wstring>& lt);

	~Shape();
};
