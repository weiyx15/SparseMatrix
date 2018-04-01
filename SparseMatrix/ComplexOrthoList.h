/*
* ����ϡ�������
* OrthoList��������
* �����˴��ļ�����Ĺ��캯��
*/
#ifndef _COMPLEXORTHOLIST_H
#define _COMPLEXORTHOLIST_H
#include "OrthoList.h"
#include "matrix.h"
#include<fstream>
#include "D:\�û�Ŀ¼\Documents\Visual Studio 2012\Projects\Complex\Complex\complex.h"

class ComplexOrthoList : public OrthoList<Complex>
{
public:
	ComplexOrthoList(ifstream &fin);				// ���캯�������ļ�����
	ComplexOrthoList(void);							// Ĭ�Ϲ��캯��
	ComplexOrthoList(const ComplexOrthoList &b);	// �������캯��
	ComplexOrthoList & operator =(const ComplexOrthoList &b);	
	// ����=�����, �̳��಻�ܼ̳л����=�����
	friend ComplexOrthoList operator *(const ComplexOrthoList &a, const ComplexOrthoList &b)
	// ����*��������������Ԫ�������Ǽ̳������Ԫ����
	{
		int ra = a.getRowNumber();
		int ca = a.getColumnNumber();
		int rb = b.getRowNumber();
		int cb = b.getColumnNumber();
		if (ca != rb)									// �����Ͼ���˷��ĳߴ�Ҫ��
		{
			cout << "Error: matrices sizes do not match!" << endl;
			ComplexOrthoList nullItem;					// ����void���캯�����Ƕ��������治��()
			return nullItem;							// ���ؿն���						
		}
		ComplexOrthoList ans;							// ���������
		Node<Complex> ** bRHead = b.getRHead();
		Node<Complex> ** bCHead = b.getCHead();
		Node<Complex> ** aRHead = a.getRHead();
		Node<Complex> ** aCHead = a.getCHead();
		Node<Complex> ** cAns = ans.getCHead();
		Node<Complex> ** rAns = ans.getRHead();
		Node<Complex> *p, *q;						// �������ָ��
		Complex compute = 0;						// ans[i,j]�ļ�����
		bool has_value = false;						// �����Ƿ���ֵ
		int i = 0, j = 0;
		for (i=0; i<ra; i++)
		{
			for (j=0; j<cb; j++)
			{
				has_value = false;						// ��ֵflag����Ϊfalse
				p = aRHead[i];
				q = bCHead[j];
				compute = 0;							// �˷�����������
				if (p && q)
				{
					p = p->right;
					q = q->down;
					while (p && q)
					{
						if (p->col < q->row)			// q��p����
						{
							p = p->right;				// p�����
						}
						else if (p->col > q->row)		// p��q����
						{
							q = q->down;				// q�����
						}
						else							// p->col == q->row
						{
							has_value = true;
							compute += p->val * q->val;	// ans[i,j] += A[i,p] * B[p,j];
							p = p->right;				// p,qһ�������
							q = q->down;				// p,qһ�������
						}
					}
				}
				if (has_value)							// ����з���ֵ
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
	fin >> nRow >> nCol;				// ����2�������Ǿ���ߴ�
	nElement = 0;						// ��ʼ��nElement
	rHead = new Node<Complex>*[nRow];	// ��ʼ����ͷָ��
	cHead = new Node<Complex>*[nCol];	// ��ʼ����ͷָ��
	int i = 0, j = 0;					// ������ÿ����4����,ͷ����������,ע���1��ʼ���Ǵ�0��ʼ
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
	double Real = 0, Imag = 0;			// ��2����ʵ��/�鲿
	Node<Complex> *p, *q;				// �������ָ��
	while (fin >> i >> j >> Real >> Imag)
	{
		i--; j--;						// �±껯Ϊ��0��ʼ����ʽ
		Complex com(Real,Imag);
		Node<Complex>* aNode = new Node<Complex>();
		aNode->row = i;					// �����½ڵ�
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
				Node<Complex>* aNode = new Node<Complex>();	// �����½ڵ�
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