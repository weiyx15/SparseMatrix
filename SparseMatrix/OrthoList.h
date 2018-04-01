/*
* ϡ�������
* ����ʮ�������ʵ��
* �ó�Ա����ʵ��+, ����Ԫ��������*
* 20180401 first edition
*/
#ifndef _ORTHOLIST_H
#define _ORTHOLIST_H
#include<iostream>
#include<vector>
#include "matrix.h"
using namespace std;

template <class NodeValueType>		// NodeValueType: �ڵ�Ԫ�ص���������
class Node {						// �ڵ���ģ��
public:
	int row;						// �ڵ��к�
	int col;						// �ڵ��к�
	NodeValueType val;				// �ڵ���ֵ
	Node<NodeValueType>* right;		// ָ��ͬ���ҷ��ڵ��ָ��
	Node<NodeValueType>* down;		// ָ��ͬ���·��ڵ��ָ��
};

template <class ListValueType>		// ListValueType: �ڵ�Ԫ�ص���������
class OrthoList {					// ʮ��������
public:								
		OrthoList(vector< vector<ListValueType> > mat);// ���캯�������ڽӾ���mat����ʮ������
		OrthoList(int row, int col);// ���캯�����ӿ�����ͷָ�뿪ʼ����
		OrthoList(void);			// ���캯���������ʮ������
		OrthoList(const OrthoList<ListValueType> &b);	// �������캯�����ж�̬�ڴ����Ҫ���
		virtual ~OrthoList();				// ����������Ϊ���㶯̬�󶨵�Ҫ��,��Ϊ�麯����

protected:
	Node<ListValueType> **rHead, **cHead;		// ��ͷָ�����ͷָ��
	int nRow, nCol, nElement;					// ����������������Ԫ�ظ���

private:
	bool elementWiseAddition(Node<ListValueType> *aNode);// ���Ԫ�ؼӷ�, ��Addition����

public:
	Node<ListValueType> **getRHead() const;	// ������ͷָ��
	Node<ListValueType> **getCHead() const;	// ������ͷָ��
	int getRowNumber() const;				// ��������
	int getColumnNumber() const;			// ��������
	int getElementNumber() const;			// ���ط���Ԫ�ظ���
	void printFull() const;					// ���ڽӾ�����ʽ��ӡ
	void printSparse() const;				// ����Ԫ��(row, col, val)��ʽ��ӡ
	bool add(int r, int c, ListValueType v);			// ��ӽڵ�,false: (r,c)������Χ
	bool del(int r, int c);								// ɾ���ڵ�,true:�ɹ�ɾ��; false:û�ҵ�
	bool Addition(const OrthoList<ListValueType> &b);	// ����ӷ�,true:�ܼ�; false:����
	Matrix<ListValueType> toFull() const;				// ����Matrix����
	OrthoList<ListValueType>& operator= (const OrthoList<ListValueType> &b);
	// ����=�����
	friend OrthoList<ListValueType> operator*
	(const OrthoList<ListValueType> &a, const OrthoList<ListValueType> &b)
	// ��Ԫ������˫Ŀ�����*������˷���
	{
		int ra = a.getRowNumber();
		int ca = a.getColumnNumber();
		int rb = b.getRowNumber();
		int cb = b.getColumnNumber();
		if (ca != rb)									// �����Ͼ���˷��ĳߴ�Ҫ��
		{
			cout << "Error: matrices sizes do not match!" << endl;
			OrthoList<ListValueType> nullItem;			// ����void���캯�����Ƕ��������治��()
			return nullItem;							// ���ؿն���						
		}
		OrthoList<ListValueType> ans(ra, cb);			// ���������
		Node<ListValueType> ** bRHead = b.getRHead();
		Node<ListValueType> ** bCHead = b.getCHead();
		Node<ListValueType> ** aRHead = a.getRHead();
		Node<ListValueType> ** aCHead = a.getCHead();
		Node<ListValueType> ** cAns = ans.getCHead();
		Node<ListValueType> ** rAns = ans.getRHead();
		Node<ListValueType> *p, *q;						// �������ָ��
		ListValueType compute = 0;						// ans[i,j]�ļ�����
		bool has_value = false;							// �����Ƿ���ֵ
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

template <class ListValueType>
OrthoList <ListValueType>::OrthoList(vector< vector<ListValueType> > mat)
	:nRow(mat.size()), nCol(mat.at(0).size()), nElement(0),
	rHead(NULL), cHead(NULL)
{
	rHead = new Node<ListValueType>*[nRow];
	cHead = new Node<ListValueType>*[nCol]; 
	int i = 0, j = 0;
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}

	Node<ListValueType> *p, *q;						// �������ָ��
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			if (mat.at(i).at(j) != 0)				// ����Ǿ������Ԫ
			{
				nElement++;
				Node<ListValueType>* aNode = new Node<ListValueType>();
				aNode->row = i;						// �����½ڵ�
				aNode->col = j;
				aNode->val = mat.at(i).at(j);
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
					rHead[i] = new Node<ListValueType>();
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
					cHead[j] = new Node<ListValueType>();
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
					rHead[i] = new Node<ListValueType>();
					rHead[i]->right = aNode;
					cHead[j] = new Node<ListValueType>();
					cHead[j]->down = aNode;
				}
			}
		}
	}
}

template <class ListValueType>
OrthoList <ListValueType>::OrthoList(int row, int col): nRow(row), nCol(col), nElement(0)
{
	rHead = new Node<ListValueType>*[nRow];
	cHead = new Node<ListValueType>*[nCol]; 
	int i = 0;
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
}

template <class ListValueType>
OrthoList <ListValueType>::OrthoList(void): nRow(0), nCol(0), nElement(0),
	rHead(NULL), cHead(NULL){}

template <class ListValueType>
OrthoList<ListValueType>::OrthoList(const OrthoList<ListValueType> &b): 
	nRow(b.getRowNumber()), nCol(b.getColumnNumber()), nElement(b.getElementNumber())
{
	int i = 0, j = 0;
	rHead = new Node<ListValueType>*[nRow];
	cHead = new Node<ListValueType>*[nCol]; 
	Node<ListValueType> **brHead = b.getRHead();
	Node<ListValueType> **bcHead = b.getCHead();
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
	Node<ListValueType> *bp, *p, *q;
	for (i=0; i<nRow; i++)
	{
		bp = brHead[i];
		if (bp)
		{
			bp = bp->right;
			while (bp)
			{
				Node<ListValueType>* aNode = new Node<ListValueType>();	// �����½ڵ�
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
				else if (rHead[bp->row] == NULL && cHead[bp->col] != NULL)
				{
					rHead[bp->row] = new Node<ListValueType>();
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
					cHead[bp->col] = new Node<ListValueType>();
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
					rHead[bp->row] = new Node<ListValueType>();
					rHead[bp->row]->right = aNode;
					cHead[bp->col] = new Node<ListValueType>();
					cHead[bp->col]->down = aNode;
				}
				bp = bp->right;
			}
		}
	}
}

template <class ListValueType>
OrthoList <ListValueType>::~OrthoList()
{
	int i = 0;
	Node<ListValueType> *p, *q;
	for (i=0; i<nRow; i++)
	{
		p = rHead[i];
		while(p != NULL)
		{
			q = p;
			p = p->right;
			delete q;
			q = NULL;
		}
	}
	delete rHead;
	delete cHead;
	rHead = NULL;
	cHead = NULL;
}

template <class ListValueType>
OrthoList<ListValueType>& OrthoList<ListValueType>::operator= (const OrthoList<ListValueType> &b)
{
	nRow = b.getRowNumber();
	nCol = b.getColumnNumber();
	nElement = b.getElementNumber();
	rHead = new Node<ListValueType>*[nRow];
	cHead = new Node<ListValueType>*[nCol]; 
	Node<ListValueType> **brHead = b.getRHead();
	Node<ListValueType> **bcHead = b.getCHead();
	int i = 0, j = 0;
	for (i=0; i<nRow; i++)
	{
		rHead[i] = NULL;
	}
	for (i=0; i<nCol; i++)
	{
		cHead[i] = NULL;
	}
	Node<ListValueType> *bp, *p, *q;
	for (i=0; i<nRow; i++)
	{
		bp = brHead[i];
		if (bp)
		{
			bp = bp->right;
			while (bp)
			{
				Node<ListValueType>* aNode = new Node<ListValueType>();	// �����½ڵ�
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
					rHead[bp->row] = new Node<ListValueType>();
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
					cHead[bp->col] = new Node<ListValueType>();
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
					rHead[bp->row] = new Node<ListValueType>();
					rHead[bp->row]->right = aNode;
					cHead[bp->col] = new Node<ListValueType>();
					cHead[bp->col]->down = aNode;
				}
				bp = bp->right;
			}
		}
	}
	return *this;
}

template <class ListValueType>
Node<ListValueType> **OrthoList <ListValueType>::getRHead() const
{
	return rHead;
}

template <class ListValueType>
Node<ListValueType> **OrthoList <ListValueType>::getCHead() const
{
	return cHead;
}

template <class ListValueType>
int OrthoList <ListValueType>::getRowNumber() const
{
	return nRow;
}

template <class ListValueType>
int OrthoList <ListValueType>::getColumnNumber() const
{
	return nCol;
}

template <class ListValueType>
int OrthoList <ListValueType>::getElementNumber() const
{
	return nElement;
}

template <class ListValueType>
void OrthoList <ListValueType>::printFull() const
{
	int i = 0, j = 0;
	Node<ListValueType> *p;
	for (i=0; i<nRow; i++)
	{
		vector<int> line(nCol,0);						// �洢�����һ��, ��ʼ��Ϊȫ0
		if (rHead[i]!=NULL)
		{
			p = rHead[i]->right;
			while (p!=NULL)
			{
				line.at(p->col) = p->val;
				p = p->right;
			}
		}
		for (j=0; j<nCol; j++)
		{
			cout << line.at(j) << " ";
		}
		cout << endl;
	}
}

template <class ListValueType>
void OrthoList <ListValueType>::printSparse() const
{
	int i = 0;
	Node<ListValueType> *p;
	for (i=0; i<nRow; i++)
	{
		if (rHead[i]!=NULL)
		{
			p = rHead[i]->right;
			while (p!=NULL)
			{
				cout << "( " << p->row << ", " << p->col << ", " << p->val << " )" << endl;
				// ���(row, col, val)��Ԫ��
				p = p->right;
			}
		}
	}
}

// ��(r,c)λ�ü����½ڵ㣬��(r,c)λ�����нڵ㣬���滻Ϊ��ֵ
template <class ListValueType>
bool OrthoList <ListValueType>::add(int r, int c, ListValueType v)
{
	if (r>=nRow || c>=nCol || r<0 || c<0)
	{
		return false;
	}
	Node<ListValueType> *aNode = new Node<ListValueType>();
	aNode -> row = r;
	aNode -> col = c;
	aNode -> val = v;
	aNode -> right = NULL;
	aNode -> down = NULL;
	bool r_flag = false;								// �Ƿ��������Ѿ���ӹ�aNode�ı�־
	bool c_flag = false;								// �Ƿ��������Ѿ��޸����ӹ�ϵ
	Node<ListValueType> *p, *q, *tmpNode;				// ��������ָ��
	// �����в�����Ԫ�ػ��޸ľ�Ԫ��
	if (rHead[r] && cHead[c])
	{
		p = rHead[r]->right;
		q = cHead[c]->down;
		if (p->col > c)									// c����ֵС��rHead[r]���׵���ֵ
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			nElement++;
			r_flag = true;
			// �޸��е����ӹ�ϵ
			if (q->row > r)
			{
				cHead[c]->down = aNode;
				aNode->down = q;
				c_flag = true;
			}
			else
			{
				while (q->down != NULL)
				{
					if (q->row < r && q->down->row > r)
					{
						tmpNode = q->down;
						q->down = aNode;
						aNode->down = tmpNode;
						c_flag = true;
						break;
					}
					else
					{
						q = q->down;
					}
				}
				if (c_flag == false)
				{
					q->down = aNode;
				}
			}
		}
		else if (p->col == c)					// ������滻
		// ��Ϊwhileѭ������: p->right != NULL, ����ÿ�е�0���ڵ���Ҫ��������
		{
			p->val = v;
			delete aNode;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->right->col == c)				// ������滻
				// ������һ���ڵ�, ����д��Ϊ�˼���whileѭ������: p->right != NULL
				{
					p -> right -> val = v;
					delete aNode;							// ��ʱ��������½ڵ�
					r_flag = true;
					break;
				}
				else if (p->col < c && p->right->col >c)	// ���������
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					nElement++;
					r_flag = true;
					// �޸��е����ӹ�ϵ
					if (q->row > r)
					{
						cHead[c]->down = aNode;
						aNode->down = q;
						c_flag = true;
					}
					else
					{
						while (q->down != NULL)
						{
							if (q->row < r && q->down->row >r)
							{
								tmpNode = q->down;
								q->down = aNode;
								aNode->down = tmpNode;
								c_flag = true;
								break;
							}
							else
							{
								q = q->down;
							}
						}
						if (c_flag == false)
						{
							q->down = aNode;
						}
					}
					break;
				}
				else
				{
					p = p->right;
				}
			}
			if (r_flag == false)
			{
				p->right = aNode;	// ����whileѭ����β˵��aNode->c��������p->c
				nElement++;
				// �޸��е����ӹ�ϵ
				if (q->row > r)
				{
					cHead[c]->down = aNode;
					aNode->down = q;
					c_flag = true;
				}
				else
				{
					while (q->down != NULL)
					{
						if (q->row < r && q->down->row >r)
						{
							tmpNode = q->down;
							q->down = aNode;
							aNode->down = tmpNode;
							c_flag = true;
							break;
						}
						else
						{
							q = q->down;
						}
					}
					if (c_flag == false)
					{
						q->down = aNode;
					}
				}
			}
		}
	}
	else if (rHead[r] == NULL && cHead[c] != NULL)
	{
		rHead[r] = new Node<ListValueType>();
		rHead[r]->right = aNode;
		q = cHead[c];
		int c_flag = false;								// �Ƿ��Ѿ��������޸����ӹ�ϵ
		// �޸��е����ӹ�ϵ
		if (q->row > r)
		{
			cHead[c]->down = aNode;
			aNode->down = q;
			c_flag = true;
		}
		else
		{
			while (q->down != NULL)
			{
				if (q->row < r && q->down->row >r)
				{
					tmpNode = q->down;
					q->down = aNode;
					aNode->down = tmpNode;
					c_flag = true;
					break;
				}
				else
				{
					q = q->down;
				}
			}
			if (c_flag == false)
			{
				q->down = aNode;
			}
		}
	}
	else if (cHead[c] == NULL && rHead[r] != NULL)
	{
		cHead[c] = new Node<ListValueType>();
		cHead[c]->down = aNode;
		p = rHead[r];
		int r_flag = false;								// �Ƿ��Ѿ��������޸����ӹ�ϵ
		// �޸��е����ӹ�ϵ
		if (p->col > c)
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->col < c && p->right->col >c)
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					r_flag = true;
					break;
				}
				else
				{
					p = p->right;
				}
			}
			if (r_flag == false)
			{
				p->right = aNode;
			}
		}
	}
	else
	{
		rHead[r] = new Node<ListValueType>();
		rHead[r]->right = aNode;
		cHead[c] = new Node<ListValueType>();
		cHead[c]->down = aNode;
	}
	return true;
}

// ϡ�����this�뵥��Ԫ��aNode(r,c,v)���, this(r,c)����������, ���������
// ����bool OrthoList <ListValueType>::add(int r, int c, ListValueType v)�Ĵ�������޸�
template <class ListValueType>
bool OrthoList <ListValueType>::elementWiseAddition(Node<ListValueType> *aNode)
{
	int r = aNode->row;
	int c = aNode->col;
	ListValueType v = aNode->val;
	if (r>=nRow || c>=nCol || r<0 || c<0)
	{
		return false;									// �ߴ����������false
	}
	bool r_flag = false;								// �Ƿ��������Ѿ���ӹ�aNode�ı�־
	bool c_flag = false;								// �Ƿ��������Ѿ��޸����ӹ�ϵ
	Node<ListValueType> *p, *q, *tmpNode;				// ��������ָ��
	// �����в�����Ԫ�ػ��޸ľ�Ԫ��
	if (rHead[r] && cHead[c])
	{
		p = rHead[r]->right;
		q = cHead[c]->down;
		if (p->col > c)									// c����ֵС��rHead[r]���׵���ֵ
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			r_flag = true;
			// �޸��е����ӹ�ϵ
			if (q->row > r)
			{
				cHead[c]->down = aNode;
				aNode->down = q;
				c_flag = true;
			}
			else
			{
				while (q->down != NULL)
				{
					if (q->row < r && q->down->row > r)
					{
						tmpNode = q->down;
						q->down = aNode;
						aNode->down = tmpNode;
						c_flag = true;
						break;
					}
					else
					{
						q = q->down;
					}
				}
				if (c_flag == false)
				{
					q->down = aNode;
				}
			}
		}
		else if (p->col == c)					// ������滻
		// ��Ϊwhileѭ������: p->right != NULL, ����ÿ�е�0���ڵ���Ҫ��������
		{
			p->val += v;						// ���滻��Ϊ���
			delete aNode;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->right->col == c)				// ������滻
				// ������һ���ڵ�, ����д��Ϊ�˼���whileѭ������: p->right != NULL
				{
					p -> right -> val += v;
					delete aNode;							// ��ʱ��������½ڵ�
					aNode = NULL;
					r_flag = true;
					break;
				}
				else if (p->col < c && p->right->col >c)	// ���������
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					r_flag = true;
					// �޸��е����ӹ�ϵ
					if (q->row > r)
					{
						cHead[c]->down = aNode;
						aNode->down = q;
						c_flag = true;
					}
					else
					{
						while (q->down != NULL)
						{
							if (q->row < r && q->down->row >r)
							{
								tmpNode = q->down;
								q->down = aNode;
								aNode->down = tmpNode;
								c_flag = true;
								break;
							}
							else
							{
								q = q->down;
							}
						}
						if (c_flag == false)
						{
							q->down = aNode;
						}
					}
					break;
				}
				else
				{
					p = p->right;
				}
			}
			if (r_flag == false)
			{
				p->right = aNode;	// ����whileѭ����β˵��aNode->c��������p->c
				// �޸��е����ӹ�ϵ
				if (q->row > r)
				{
					cHead[c]->down = aNode;
					aNode->down = q;
					c_flag = true;
				}
				else
				{
					while (q->down != NULL)
					{
						if (q->row < r && q->down->row >r)
						{
							tmpNode = q->down;
							q->down = aNode;
							aNode->down = tmpNode;
							c_flag = true;
							break;
						}
						else
						{
							q = q->down;
						}
					}
					if (c_flag == false)
					{
						q->down = aNode;
					}
				}
			}
		}
	}
	else if (rHead[r] == NULL && cHead[c] != NULL)
	{
		rHead[r] = new Node<ListValueType>();
		rHead[r]->right = aNode;
		q = cHead[c];
		int c_flag = false;								// �Ƿ��Ѿ��������޸����ӹ�ϵ
		// �޸��е����ӹ�ϵ
		if (q->row > r)
		{
			cHead[c]->down = aNode;
			aNode->down = q;
			c_flag = true;
		}
		else
		{
			while (q->down != NULL)
			{
				if (q->row < r && q->down->row >r)
				{
					tmpNode = q->down;
					q->down = aNode;
					aNode->down = tmpNode;
					c_flag = true;
					break;
				}
				else
				{
					q = q->down;
				}
			}
			if (c_flag == false)
			{
				q->down = aNode;
			}
		}
	}
	else if (cHead[c] == NULL && rHead[r] != NULL)
	{
		cHead[c] = new Node<ListValueType>();
		cHead[c]->down = aNode;
		p = rHead[r];
		int r_flag = false;								// �Ƿ��Ѿ��������޸����ӹ�ϵ
		// �޸��е����ӹ�ϵ
		if (p->col > c)
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->col < c && p->right->col >c)
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					r_flag = true;
					break;
				}
				else
				{
					p = p->right;
				}
			}
			if (r_flag == false)
			{
				p->right = aNode;
			}
		}
	}
	else
	{
		rHead[r] = new Node<ListValueType>();
		rHead[r]->right = aNode;
		cHead[c] = new Node<ListValueType>();
		cHead[c]->down = aNode;
	}
	return true;
}

template <class ListValueType>
bool OrthoList <ListValueType>::del(int r, int c)
{
	if (r>=nRow || c>= nCol || r<0 || c<0)						// �±곬��������Χ
	{
		return false;
	}
	Node<ListValueType> *p, *q;									// �������ָ��
	Node<ListValueType> *tmpNode;								// ����ɾ���ڵ���м����
	p = rHead[r];
	q = cHead[c];
	if (p == NULL)												// ����Ϊ��
	{
		return false;
	}
	else
	{
		while (p->right != NULL)
		{
			if (p->right->col == c)								// �ҵ���ɾ��Ԫ��
			{
				// �����е����ӹ�ϵ
				while (q->down->row != r)						// ����cHead[c]��row==r				
				{
					q = q->down;
				}
				if (q == cHead[c] && q->down->down == NULL)		// ɾ�����Ǹ���Ψһ��Ԫ��
				{
					delete cHead[c];
					cHead[c] = NULL;
				}
				else
				{
					q->down = q->down->down;					// �޸��е����ӹ�ϵ
				}
				if (p == rHead[r] && p->right->right == NULL)	// ɾ�����Ǹ��е�Ψһ��Ԫ��
				{
					delete rHead[r]->right;
					rHead[r]->right = NULL;
					delete rHead[r];
					rHead[r] = NULL;
				}
				else
				{
					tmpNode = p->right;
					p->right = p->right->right;					// �޸��е����ӹ�ϵ
					delete tmpNode;
					tmpNode = NULL;
				}
				nElement--;
				return true;
			}
			p  = p->right;
		}
		return false;											// �Ҳ�����ɾ����Ԫ��
	}
}

template <class ListValueType>
bool OrthoList<ListValueType>::Addition(const OrthoList<ListValueType> &b)
{
	int rb = b.getRowNumber();
	int cb = b.getColumnNumber();
	Node<ListValueType> ** bRHead = b.getRHead();
	Node<ListValueType> ** bCHead = b.getCHead();
	if (rb != nRow || cb != nCol)
	{
		return false;								// ��С��ƥ��: ����false
	}
	Node<ListValueType> *pb;						// ����b��ָ��
	int i = 0;
	for (i=0; i<nRow; i++)
	{
		pb = bRHead[i];
		if (pb != NULL)
		{
			pb = pb->right;
			while (pb != NULL)
			{
				Node<ListValueType> *aNode = new Node<ListValueType>();
				aNode -> row = pb -> row;
				aNode -> col = pb -> col;
				aNode -> val = pb -> val;
				if (!elementWiseAddition(aNode))	// ��������ӷ�˽�з���
				{
					return false;
				}
				pb = pb->right;
			}
		}
	}
	return true;
}

template <class ListValueType>
Matrix<ListValueType> OrthoList <ListValueType>::toFull() const
{
	Matrix<ListValueType> mat(nRow, nCol);
	int i = 0;
	Node<ListValueType> *p;
	for (i=0; i<nRow; i++)
	{
		if (rHead[i]!=NULL)
		{
			p = rHead[i]->right;
			while (p!=NULL)
			{
				mat(p->row, p->col) = p->val;
				p = p->right;
			}
		}
	}
	return mat;
}


#endif