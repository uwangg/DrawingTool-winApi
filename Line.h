#include "Shape.h"
#pragma once
class Line : public Shape
{
public:
	Line();

	void SetPoint(POINT start, POINT end);

	// Draw
	void Print(HDC hdc);
	void IsNotSelectedPrint(HDC hdc);
	void IsSelectedPrint(HDC hdc);
	bool Select(POINT p);
	~Line();
};

