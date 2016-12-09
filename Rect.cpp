#include "Rect.h"



Rect::Rect()
{
}

void Rect::IsNotSelectedPrint(HDC hdc)
{
	HBRUSH hBrush, oldBrush;
	RECT rt;
	rt.left = p1.x;
	rt.top = p1.y;
	rt.right = p2.x;
	rt.bottom = p2.y;

	hBrush = CreateSolidBrush(RGB(116, 223, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	//auto i_lt = GetStrLt().begin();
	SetBkMode(hdc, TRANSPARENT);
	//cout << "strlt.size = " << strlt.size() << endl;
	//DrawTextCenter(hdc, (*i_lt).c_str(), -1, &rt, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	//cout << "str.size = " << str.length() << endl;
	DrawTextCenter(hdc, str.c_str(), -1, &rt, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
}

void Rect::IsSelectedPrint(HDC hdc)
{
	IsNotSelectedPrint(hdc);
	Rectangle(hdc, p1.x - 3, p1.y - 3,
		p1.x + 3, p1.y + 3);
	Rectangle(hdc, p2.x - 3, p2.y - 3,
		p2.x + 3, p2.y + 3);
	Rectangle(hdc, p1.x - 3, p2.y - 3,
		p1.x + 3, p2.y + 3);
	Rectangle(hdc, p2.x - 3, p1.y - 3,
		p2.x + 3, p1.y + 3);
}

bool Rect::Select(POINT p)
{
	POINT tmp1, tmp2;
	tmp1 = GetP1();
	tmp2 = GetP2();
	if (tmp1.x > tmp2.x && tmp1.y > tmp2.y) {
		// 오른쪽 하단에서 왼쪽 상단으로 그렸을 때
		SetP1(tmp2);
		SetP2(tmp1);
	}
	else if (tmp1.x <= tmp2.x && tmp1.y > tmp2.y) {
		// 왼쪽 하단에서 오른쪽 상단으로 그렸을 때
		tmp1.y = GetP2().y;
		tmp2.y = GetP1().y;
		SetP1(tmp1);
		SetP2(tmp2);
	}
	else if (tmp1.x > tmp2.x && tmp1.y <= tmp2.y) {
		// 오른쪽 상단에서 왼쪽 하단으로 그렸을 때
		tmp1.x = GetP2().x;
		tmp2.x = GetP1().x;
		SetP1(tmp1);
		SetP2(tmp2);
	}

	// 범위 지정
	if ((p.x >= GetP1().x) && (p.x <= GetP2().x)
		&& (p.y >= GetP1().y) && (p.y <= GetP2().y)) {
		//std::cout << "rect true" << std::endl;
		return TRUE;
	}
	return FALSE;
}

Rect::~Rect()
{
}
