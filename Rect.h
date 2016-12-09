#include "Shape.h"
#include <string>
#pragma once
class Rect : public Shape
{
private:
	POINT right = {0,};	// ���� ��� ��ǥ
	POINT left = {0,};	// ���� �ϴ� ��ǥ
	//wstring str = L"";	// �ȿ� �� ���ڿ�
public:
	Rect();

	// GET & SET
	POINT GetRight() { return this->right; }
	void SetRight(POINT p) { this->right = p; }
	POINT GetLeft() { return this->left; }
	void SetLeft(POINT p) { this->left = p; }
	//wstring GetStr() { return this->str; }
	//void SetStr(wstring str) { this->str = str; }

	// Draw
	void IsNotSelectedPrint(HDC hdc);
	void IsSelectedPrint(HDC hdc);
	bool Select(POINT p);
 	~Rect();
};

