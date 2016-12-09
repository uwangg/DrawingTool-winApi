#include "Shape.h"



Shape::Shape()
{
	//strlt.push_back(L"");
}


Shape::~Shape()
{
}


//void Shape::SelectedPrint(HDC hdc)
//{
//	Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
//}
int Shape::DrawTextCenter(HDC dc, LPCWSTR text, int cchText, LPCRECT lprc, UINT format)
{
	if (dc == NULL || lprc == NULL) return 0;

	RECT rect = *lprc;
	RECT rectResult = *lprc;

	// 글자가 차지하는 사각형 계산
	DrawText(dc, text, cchText, &rect, format | DT_CALCRECT);

	// 가로, 세로로 센터
	int marginH = max(0, ((lprc->bottom - lprc->top) - (rect.bottom - rect.top)) / 2);
	int marginW = max(0, ((lprc->right - lprc->left) - (rect.right - rect.left)) / 2);
	rectResult.top += marginH;
	rectResult.bottom -= marginH;
	rectResult.left += marginW;
	rectResult.right -= marginW;

	// 글자 그리기
	DrawText(dc, text, cchText, &rectResult, format);
	return rect.bottom;
}

void Shape::IsNotSelectedPrint(HDC hdc)
{
}

void Shape::IsSelectedPrint(HDC hdc)
{
}

bool Shape::Select(POINT p)
{
	return false;
}

// 꼭지점들중 선택된 점 반환
int Shape::SelectPoint(POINT p)
{
	//cout << "1. " << abs(p.x - p1.x) << ", " << abs(p.y - p1.y) << endl;
	//cout << "2. " << abs(p.x - p2.x) << ", " << abs(p.y - p2.y) << endl;
	if (abs(p.x - p1.x) <= 20 && abs(p.y - p1.y) <= 20)
	{
		return 1;
	}
	else if (abs(p.x - p2.x) <= 20 && abs(p.y - p2.y) <= 20)
	{
		return 2;
	}
	return 0;
}

// 도형 사이즈 변경
void Shape::Resize(POINT start, POINT end)
{
	if (abs(start.x - p1.x) <= 20 && abs(start.y - p1.y) <= 20)
	{  
		p1.x = end.x;
		p1.y = end.y;
		//cout << "걸림" << endl;
	}
	else if (abs(start.x - p2.x) <= 20 && abs(start.y - p2.y) <= 20) {
		p2.x = end.x;
		p2.y = end.y;
		//cout << "걸림" << endl;
	}
}

// 이동하기
void Shape::Move(POINT start, POINT end)
{
	int distanceX = end.x - start.x;	// x축 이동거리
	int distanceY = end.y - start.y;	// y축 이동거리
	p1.x += distanceX;
	p1.y += distanceY;
	p2.x += distanceX;
	p2.y += distanceY;
}

// 시작점, 끝점 정렬
void Shape::SetPoint(POINT start, POINT end)
{
	// 왼쪽 상단 -> 오른쪽 하단
	if (start.x <= end.x && start.y <= end.y) {
		p1 = start;
		p2 = end;
	}
	// 오른쪽 하단 -> 왼쪽 상단
	else if (start.x >= end.x && start.y >= end.y) {
		p1 = end;
		p2 = start;
	}
	// 오른쪽 상단 -> 왼쪽 하단
	else if (start.x >= end.x && start.y <= end.y) {
		p1.x = end.x;
		p1.y = start.y;
		p2.x = start.x;
		p2.y = end.y;
	}
	// 왼쪽 하단 -> 오른쪽 상단
	else {
		p1.x = start.x;
		p1.y = end.y;
		p2.x = end.x;
		p2.y = start.y;
	}
}