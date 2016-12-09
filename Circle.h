#include <iostream>
#include <string>
#include <windows.h>
#include "Shape.h"
#pragma once
class Circle : public Shape
{
private:
	POINT top = {0,};	// »ó´Ü ÁÂÇ¥
	POINT right = {0,};	// ¿ìÃø ÁÂÇ¥
	POINT bottom = {0, };	// ÇÏ´Ü ÁÂÇ¥
	POINT left = {0, };	// ÁÂÃø ÁÂÇ¥
	//wstring str = L"";	// ¾È¿¡ µé¾î°¥ ¹®ÀÚ¿­

public:
	Circle();

	// GET & SET
	POINT GetTop() { return this->top; }
	void SetTOP(POINT p) { this->top = p; }
	POINT GetRight() { return this->right; }
	void SetRight(POINT p) { this->right = p; }
	POINT GetBottom() { return this->bottom; }
	void SetBottom(POINT p) { this->bottom = p; }
	POINT GetLeft() { return this->left; }
	void SetLeft(POINT p) { this->left = p; }

	//wstring GetStr() { return this->str; }
	//void SetStr(wstring str) { this->str = str; }

	// Draw
	void IsNotSelectedPrint(HDC hdc);
	void IsSelectedPrint(HDC hdc);
	bool Select(POINT p);
	~Circle();
};

