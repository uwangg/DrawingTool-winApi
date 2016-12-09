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

	// ���ڰ� �����ϴ� �簢�� ���
	DrawText(dc, text, cchText, &rect, format | DT_CALCRECT);

	// ����, ���η� ����
	int marginH = max(0, ((lprc->bottom - lprc->top) - (rect.bottom - rect.top)) / 2);
	int marginW = max(0, ((lprc->right - lprc->left) - (rect.right - rect.left)) / 2);
	rectResult.top += marginH;
	rectResult.bottom -= marginH;
	rectResult.left += marginW;
	rectResult.right -= marginW;

	// ���� �׸���
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

// ���������� ���õ� �� ��ȯ
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

// ���� ������ ����
void Shape::Resize(POINT start, POINT end)
{
	if (abs(start.x - p1.x) <= 20 && abs(start.y - p1.y) <= 20)
	{  
		p1.x = end.x;
		p1.y = end.y;
		//cout << "�ɸ�" << endl;
	}
	else if (abs(start.x - p2.x) <= 20 && abs(start.y - p2.y) <= 20) {
		p2.x = end.x;
		p2.y = end.y;
		//cout << "�ɸ�" << endl;
	}
}

// �̵��ϱ�
void Shape::Move(POINT start, POINT end)
{
	int distanceX = end.x - start.x;	// x�� �̵��Ÿ�
	int distanceY = end.y - start.y;	// y�� �̵��Ÿ�
	p1.x += distanceX;
	p1.y += distanceY;
	p2.x += distanceX;
	p2.y += distanceY;
}

// ������, ���� ����
void Shape::SetPoint(POINT start, POINT end)
{
	// ���� ��� -> ������ �ϴ�
	if (start.x <= end.x && start.y <= end.y) {
		p1 = start;
		p2 = end;
	}
	// ������ �ϴ� -> ���� ���
	else if (start.x >= end.x && start.y >= end.y) {
		p1 = end;
		p2 = start;
	}
	// ������ ��� -> ���� �ϴ�
	else if (start.x >= end.x && start.y <= end.y) {
		p1.x = end.x;
		p1.y = start.y;
		p2.x = start.x;
		p2.y = end.y;
	}
	// ���� �ϴ� -> ������ ���
	else {
		p1.x = start.x;
		p1.y = end.y;
		p2.x = end.x;
		p2.y = start.y;
	}
}