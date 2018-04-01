/*
* 复数稀疏矩阵类
* OrthoList的派生类
* 增加了从文件读入的构造函数
*/
#ifndef _COMPLEXORTHOLIST_H
#define _COMPLEXORTHOLIST_H
#include "OrthoList.h"
#include "matrix.h"
#include<fstream>
#include "D:\用户目录\Documents\Visual Studio 2012\Projects\Complex\Complex\complex.h"

class ComplexOrthoList : public OrthoList<Complex>
{
public:
	ComplexOrthoList(ifstream &fin);				// 构造函数：从文件读入
	ComplexOrthoList(void);							// 默认构造函数
	ComplexOrthoList(const ComplexOrthoList &b);	// 拷贝构造函数
	ComplexOrthoList & operator =(const ComplexOrthoList &b);	
	// 重载=运算符, 继承类不能继承基类的=运算符
	friend ComplexOrthoList operator *(const ComplexOrthoList &a, const ComplexOrthoList &b)
	// 重载*运算符：基类的友元函数不是继承类的友元函数
	{
		int ra = a.getRowNumber();
		int ca = a.getColumnNumber();
		int rb = b.getRowNumber();
		int cb = b.getColumnNumber();
		if (ca != rb)									// 不符合矩阵乘法的尺寸要求
		{
			cout << "Error: matrices sizes do not match!" << endl;
			ComplexOrthoList nullItem;					// 调用void构造函数就是对象名后面不加()
			return nullItem;							// 返回空对象						
		}
		ComplexOrthoList ans;							// 构造计算结果
		Node<Complex> ** bRHead = b.getRHead();
		Node<Complex> ** bCHead = b.getCHead();
		Node<Complex> ** aRHead = a.getRHead();
		Node<Complex> ** aCHead = a.getCHead();
		Node<Complex> ** cAns = ans.getCHead();
		Node<Complex> ** rAns = ans.getRHead();
		Node<Complex> *p, *q;						// 链表遍历指针
		Complex compute = 0;						// ans[i,j]的计算结果
		bool has_value = false;						// 该项是否有值
		int i = 0, j = 0;
		for (i=0; i<ra; i++)
		{
			for (j=0; j<cb; j++)
			{
				has_value = false;						// 有值flag重置为false
				p = aRHead[i];
				q = bCHead[j];
				compute = 0;							// 乘法计算结果清零
				if (p && q)
				{
					p = p->right;
					q = q->down;
					while (p && q)
					{
						if (p->col < q->row)			// q在p后面
						{
							p = p->right;				// p往后赶
						}
						else if (p->col > q->row)		// p在q后面
						{
							q = q->down;				// q往后赶
						}
						else							// p->col == q->row
						{
							has_value = true;
							compute += p->val * q->val;	// ans[i,j] += A[i,p] * B[p,j];
							p = p->right;				// p,q一起往后赶
							q = q->down;				// p,q一起往后赶
						}
					}
				}
				if (has_value)							// 如果有非零值
				{
					ans.add(i, j, compute);
				}
			}
		}
		return ans;
	}
};

ComplexOrthoList::ComplexOrthoList(ifstream &fin)
{
	fin >> nRow >> nCol;				// 首行2个整数是矩阵尺寸
	nElement = 0;						// 初始化nElement
	rHead = new Node<Complex>*[nRow];	// 初始化行头指针
	cHead = new Node<Complex>*[nCol];	// 初始化列头指针
	int i = 0, j = 0;					// 接下来每行有4个数,头两个是坐标,注意从1开始不是从0开始
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
	double Real = 0, Imag = 0;			// 后2个是实部/虚部
	Node<Complex> *p, *q;				// 链表遍历指针
	while (fin >> i >> j >> Real >> Imag)
	{
		i--; j--;						// 下标化为从0开始的形式
		Complex com(Real,Imag);
		Node<Complex>* aNode = new Node<Complex>();
		aNode->row = i;					// 创建新节点
		aNode->col = j;
		aNode->val = com;
		aNode->right = NULL;
		aNode->down = NULL;
		if (rHead[i] && cHead[j])
		{
			p = rHead[i];
			q = cHead[j];
			while (p->right != NULL)
			{
				p = p->right;
			}
			p->right = aNode;
			while (q->down != NULL)
			{
				q = q->down;
			}
			q->down = aNode;
		}
		else if (rHead[i] == NULL && cHead[j] != NULL)
		{
			rHead[i] = new Node<Complex>();
			rHead[i]->right = aNode;
			q = cHead[j];
			while (q->down != NULL)
			{
				q = q->down;
			}
			q->down = aNode;
		}
		else if (cHead[j] == NULL && rHead[i] != NULL)
		{
			cHead[j] = new Node<Complex>();
			cHead[j]->down = aNode;
			p = rHead[i];
			while (p->right != NULL)
			{
				p = p->right;
			}
			p->right = aNode;
		}
		else
		{
			rHead[i] = new Node<Complex>();
			rHead[i]->right = aNode;
			cHead[j] = new Node<Complex>();
			cHead[j]->down = aNode;
		}
	}
}

ComplexOrthoList::ComplexOrthoList(void):OrthoList<Complex>(){}

ComplexOrthoList::ComplexOrthoList(const ComplexOrthoList &b):OrthoList<Complex>(b){}

ComplexOrthoList &ComplexOrthoList::operator =(const ComplexOrthoList &b)
{
	nRow = b.getRowNumber();
	nCol = b.getColumnNumber();
	nElement = b.getElementNumber();
	rHead = new Node<Complex>*[nRow];
	cHead = new Node<Complex>*[nCol]; 
	Node<Complex> **brHead = b.getRHead();
	Node<Complex> **bcHead = b.getCHead();
	int i = 0, j = 0;
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
	Node<Complex> *bp, *p, *q;
	for (i=0; i<nRow; i++)
	{
		bp = brHead[i];
		if (bp)
		{
			bp = bp->right;
			while (bp)
			{
				Node<Complex>* aNode = new Node<Complex>();	// 创建新节点
				aNode->row = bp->row;						
				aNode->col = bp->col;
				aNode->val = bp->val;
				aNode->right = NULL;
				aNode->down = NULL;
				if (rHead[bp->row] && cHead[bp->col])
				{
					p = rHead[bp->row];
					q = cHead[bp->col];
					while (p->right != NULL)
					{
						p = p->right;
					}
					p->right = aNode;
					while (q->down != NULL)
					{
						q = q->down;
					}
					q->down = aNode;
				}
				else if (rHead[i] == NULL && cHead[j] != NULL)
				{
					rHead[bp->row] = new Node<Complex>();
					rHead[bp->row]->right = aNode;
					q = cHead[bp->col];
					while (q->down != NULL)
					{
						q = q->down;
					}
					q->down = aNode;
				}
				else if (cHead[bp->col] == NULL && rHead[bp->row] != NULL)
				{
					cHead[bp->col] = new Node<Complex>();
					cHead[bp->col]->down = aNode;
					p = rHead[bp->row];
					while (p->right != NULL)
					{
						p = p->right;
					}
					p->right = aNode;
				}
				else
				{
					rHead[bp->row] = new Node<Complex>();
					rHead[bp->row]->right = aNode;
					cHead[bp->col] = new Node<Complex>();
					cHead[bp->col]->down = aNode;
				}
				bp = bp->right;
			}
		}
	}
	return *this;
}

#endif