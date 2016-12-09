#include "Line.h"

Line::Line()
{
}

void Line::IsNotSelectedPrint(HDC hdc)
{
	MoveToEx(hdc, this->GetP1().x , this->GetP1().y, NULL);
	LineTo(hdc, this->GetP2().x, this->GetP2().y);
}

void Line::IsSelectedPrint(HDC hdc)
{
	IsNotSelectedPrint(hdc);
	Rectangle(hdc, GetP1().x - 3, GetP1().y - 3,
		GetP1().x + 3, GetP1().y + 3);
	Rectangle(hdc, GetP2().x - 3, GetP2().y - 3,
		GetP2().x + 3, GetP2().y + 3);
}

bool Line::Select(POINT p)
{
	double result;
	POINT lowY, highY;
	POINT lowX, highX;

	// y좌표에 따른 정렬
	if (GetP1().y > GetP2().y) {
		lowY = GetP2();
		highY = GetP1();
	} else {
		lowY = GetP1();
		highY = GetP2();
	}

	// x좌표에 따른 정렬
	if (GetP1().x > GetP2().x) {
		lowX = GetP2();
		highX = GetP1();
	} else {
		lowX = GetP1();
		highX = GetP2();
	}

	if (GetP1().x == GetP2().x) {
		// x1과 x2의 값이 같을 경우 x=a
		if (lowY.y <= p.y && highY.y >= p.y
			&& (lowY.x - 1) <= p.x && (lowY.x + 1) >= p.x)
			return TRUE;
		return FALSE;
	}
	else if (GetP1().y == GetP2().y) {
		// y1과 y2의 값이 같을 경우 y=a
		if (lowX.x <= p.x && highX.x >= p.x
			&& (lowX.y - 1) <= p.y && (lowX.y + 1) >= p.y)
			return TRUE;
		return FALSE;
	}
	else {
		// y절편 값 계산
		//result = p.y - (((GetP2().y - GetP1().y) / (GetP2().x - GetP1().x)) * (p.x - GetP1().x));
		//result = (GetP2().x - GetP1().x) * p.y - ((GetP2().y - GetP1().y) * (p.x - GetP1().x));
		result = (highX.x - lowX.x) * p.y - (highX.y - lowX.y) * (p.x - lowX.x);
		//std::cout << "result = "<< result << std::endl;
		//std::cout << "p: x = " << p.x << ", y = " << p.y << std::endl;
		//std::cout << "p1: x = " << GetP1().x << ", y = " << GetP1().y << std::endl;
		//std::cout << "p2: x = " << GetP2().x << ", y = " << GetP2().y << std::endl;
		//std::cout << "line = " << (highX.x - lowX.x) * lowX.y << std::endl;
		if (p.x >= lowX.x && p.x <= highX.x
			&& p.y >= (lowY.y - 1) && p.y <= (highY.y + 1)
			&& result >= ((highX.x - lowX.x) * lowX.y - 1500) && result <= ((highX.x - lowX.x) * lowX.y + 1500)) {
			std::cout << "line select!" << std::endl;
			return TRUE;
		}
		//if (p.x >= lowX.x && p.x <= highX.x
		//	&& p.y >= (lowY.y - 1) && p.y <= (highY.y + 1)
		//	&& result >= ((GetP2().x - GetP1().x) * (GetP1().y - 10)) && result <= ((GetP2().x - GetP1().x) * (GetP1().y + 10))) {
		//	std::cout << "line select!" << std::endl;
		//	return TRUE;
		//}
		return FALSE;
	}
	return FALSE;
}

//
void Line::SetPoint(POINT start, POINT end)
{
	p1 = start;
	p2 = end;
}
Line::~Line()
{
}
