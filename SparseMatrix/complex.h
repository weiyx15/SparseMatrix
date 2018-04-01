/*
* 复数类
* 用友元函数重载复数的+/-/*运算
* 20180401 first edition
*/
#ifndef _COMPLEX_H
#define _COMPLEX_H
#include <iostream>
using namespace std;
class Complex {
public:
	Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) { }
	friend Complex operator+(const Complex &c1, const Complex &c2);
	friend Complex operator-(const Complex &c1, const Complex &c2);
	friend Complex operator*(const Complex &c1, const Complex &c2);
	Complex & operator+=(const Complex &c);
	friend ostream & operator<<(ostream &out, const Complex &c);
private:
	double real; //复数实部
	double imag; //复数虚部
};

Complex operator+(const Complex &c1, const Complex &c2){
	return Complex(c1.real+c2.real, c1.imag+c2.imag);
}
Complex operator-(const Complex &c1, const Complex &c2){
	return Complex(c1.real-c2.real, c1.imag-c2.imag);
}
Complex operator*(const Complex &c1, const Complex &c2){
	return Complex(c1.real*c2.real-c1.imag*c2.imag, c1.imag*c2.real+c1.real*c2.imag);
}
Complex & Complex::operator+=(const Complex &c)
{
	*this = *this + c;
	return *this;
}
ostream & operator<<(ostream &out, const Complex &c){
	out << "(" << c.real << ", " << c.imag << ")";
return out;
}

#endif