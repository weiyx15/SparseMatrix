/*
* 稠密矩阵类
* 基于一维数组的实现
* 用友元函数重载+/-/*
* 20180401 first edition
*/
#ifndef _MATRIX_H
#define _MATRIX_H
#include <iostream>
#include <cassert>
using namespace std;
template <class T>
class Matrix {
    public:
        int m, n;
    private:
        T* data;
    public:
        //constructor
        Matrix(int r=0, int c=0): m(r), n(c) {
            if (r>0 && c>0) {
                data = new T[m*n];
                if (data==NULL) {
                    cerr<<"Allocate memory failed!"<<endl;
                    exit(1);
                }
                memset(data, 0, m*n*sizeof(T));
            } else
                data = NULL;
        }
		// copy constructor
		Matrix(const Matrix<T> &a): m(a.m), n(a.n)	// 深拷贝
		{
			int i = 0, j = 0;
			if (m>0 && n>0) {
                data = new T[m*n];
                if (data==NULL) {
                    cerr<<"Allocate memory failed!"<<endl;
                    exit(1);
                }
				else
				{
					T* aData = a.getData();
					for (i=0; i<m; i++)
					{
						for (j=0; j<n; j++)
						{
							data[i+j*m] = aData[i+j*m];
						}
					}
				}
            } else
                data = NULL;
        }
        //deconstructor
        ~Matrix() {
            m = n = 0;
            delete[] data;
            data = NULL;
        }
		// get pointer: data
		T* getData() const
		{
			return data;
		}
        //entry access
        T& operator() (int i, int j) const{
            assert(i>=0 && i<m && j>=0 && j<n);
            return data[i+j*m];
        }
        //assign
        Matrix<T>& operator= (const Matrix<T> &a) {
            m = a.m;    n = a.n;
			if (data != NULL)
			{
				delete[] data;
				data = NULL;
			}
            data = new T[m*n*sizeof(T)];
            if (data==NULL) {
                cerr<<"Allocate memory failed!"<<endl;
                exit(1);
            }
            for (int i=0; i<m; i++)
                for (int j=0; j<n; j++)
                    data[i+j*m] = a(i, j);
            return *this;
        }
        //display
        void display() {
            cout<<"("<<m<<", "<<n<<"):"<<endl;
            for (int i=0; i<m; i++) {
                for (int j=0; j<n; j++)
                    cout<<data[i+j*m]<<"\t";
                cout<<endl;
            }
        }
		// 友元：双目操作符+，非成员函数
		friend Matrix<T> operator+ (const Matrix<T>& a, const Matrix<T>& b)
		{
			assert(a.m==b.m && a.n==b.n);
			Matrix<T> r(a.m, a.n);
			for (int i=0; i<a.m; i++)
				for (int j=0; j<a.n; j++)
					r(i, j) = a(i, j) + b(i, j);
			return r;
		}
		// 友元：双目操作符-，非成员函数
		friend Matrix<T> operator- (const Matrix<T>& a, const Matrix<T>& b)
		{
			assert(a.m==b.m && a.n==b.n);
			Matrix<T> r(a.m, a.n);
			for (int i=0; i<a.m; i++)
				for (int j=0; j<a.n; j++)
					r(i, j) = a(i, j) - b(i, j);
			return r;
		}
		// 友元：双目操作符*，非成员函数
		friend Matrix<T> operator* (const Matrix<T>& a, const Matrix<T>& b)
		{
			assert(a.n==b.m);
			Matrix<T> r(a.m, b.n);
			for (int i=0; i<a.m; i++)
				for (int j=0; j<b.n; j++)
					for (int k=0; k<a.n; k++)
						r(i, j) += a(i, k) * b(k, j);
			return r;
		}

};

#endif