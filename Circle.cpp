#include "Circle.h"
#include <math.h>


Circle::Circle()
{
}

void Circle::IsNotSelectedPrint(HDC hdc)
{
	HBRUSH hBrush, oldBrush;
	RECT rt;
	rt.left = p1.x;
	rt.top = p1.y;
	rt.right = p2.x;
 	rt.bottom = p2.y;

	hBrush = CreateSolidBrush(RGB(255, 170, 170));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Ellipse(hdc, p1.x, p1.y, p2.x, p2.y);
	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	//auto i_lt = GetStrLt().begin();
	/*cout << "len = " << (*i_lt).length() << endl;
	cout << "rt : l=" << rt.left << ", t=" << rt.top << ", r=" << rt.right << ", b=" << rt.bottom << endl;*/
	SetBkMode(hdc, TRANSPARENT);
	DrawTextCenter(hdc, str.c_str(), -1, &rt, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
	//DrawTextCenter(hdc, (*i_lt).c_str(), -1, &rt, DT_CENTER|DT_WORDBREAK|DT_EDITCONTROL);
	//DrawTextCenter(hdc, L"HELLOWORLD¾È³çÇÏ¼¼¿ë", -1, &rt, DT_LEFT|DT_WORDBREAK|DT_EDITCONTROL);
}

void Circle::IsSelectedPrint(HDC hdc)
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

bool Circle::Select(POINT p)
{
	//if ((100000 * pow(2 * p.x - (GetP1().x + GetP2().x), 2) / pow((GetP2().x - GetP1().x), 2)
	//	+ 100000 * pow((2 * p.y - (GetP1().y + GetP2().y)), 2) / pow((GetP2().x - GetP1().y), 2))
	//	<= 1) {
	//	std::cout << "circle true" << std::endl;
	//	return true;
	//}
	if (((10000 * pow(2 * p.x - GetP1().x - GetP2().x, 2)) / pow((GetP2().x - GetP1().x), 2)
		+ (10000 * pow((2 * p.y - GetP1().y - GetP2().y), 2)) / pow((GetP2().x - GetP1().y), 2))
		<= 10000) {
		//std::cout << "circle true" << std::endl;
		return true;
	}
	return false;
}

Circle::~Circle()
{
}
