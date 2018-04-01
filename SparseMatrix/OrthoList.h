/*
* 稀疏矩阵类
* 基于十字链表的实现
* 用成员函数实现+, 用友元函数重载*
* 20180401 first edition
*/
#ifndef _ORTHOLIST_H
#define _ORTHOLIST_H
#include<iostream>
#include<vector>
#include "matrix.h"
using namespace std;

template <class NodeValueType>		// NodeValueType: 节点元素的数据类型
class Node {						// 节点类模板
public:
	int row;						// 节点行号
	int col;						// 节点列号
	NodeValueType val;				// 节点数值
	Node<NodeValueType>* right;		// 指向同行右方节点的指针
	Node<NodeValueType>* down;		// 指向同列下方节点的指针
};

template <class ListValueType>		// ListValueType: 节点元素的数据类型
class OrthoList {					// 十字链表类
public:								
		OrthoList(vector< vector<ListValueType> > mat);// 构造函数：从邻接矩阵mat计算十字链表
		OrthoList(int row, int col);// 构造函数：从空行列头指针开始构建
		OrthoList(void);			// 构造函数：构造空十字链表
		OrthoList(const OrthoList<ListValueType> &b);	// 拷贝构造函数：有动态内存分配要深拷贝
		virtual ~OrthoList();				// 析构函数（为满足动态绑定的要求,设为虚函数）

protected:
	Node<ListValueType> **rHead, **cHead;		// 行头指针和列头指针
	int nRow, nCol, nElement;					// 行数，列数，非零元素个数

private:
	bool elementWiseAddition(Node<ListValueType> *aNode);// 逐个元素加法, 被Addition调用

public:
	Node<ListValueType> **getRHead() const;	// 返回行头指针
	Node<ListValueType> **getCHead() const;	// 返回列头指针
	int getRowNumber() const;				// 返回行数
	int getColumnNumber() const;			// 返回列数
	int getElementNumber() const;			// 返回非零元素个数
	void printFull() const;					// 按邻接矩阵形式打印
	void printSparse() const;				// 按三元组(row, col, val)形式打印
	bool add(int r, int c, ListValueType v);			// 添加节点,false: (r,c)超过范围
	bool del(int r, int c);								// 删除节点,true:成功删除; false:没找到
	bool Addition(const OrthoList<ListValueType> &b);	// 矩阵加法,true:能加; false:不能
	Matrix<ListValueType> toFull() const;				// 返回Matrix对象
	OrthoList<ListValueType>& operator= (const OrthoList<ListValueType> &b);
	// 重载=运算符
	friend OrthoList<ListValueType> operator*
	(const OrthoList<ListValueType> &a, const OrthoList<ListValueType> &b)
	// 友元：重载双目运算符*（矩阵乘法）
	{
		int ra = a.getRowNumber();
		int ca = a.getColumnNumber();
		int rb = b.getRowNumber();
		int cb = b.getColumnNumber();
		if (ca != rb)									// 不符合矩阵乘法的尺寸要求
		{
			cout << "Error: matrices sizes do not match!" << endl;
			OrthoList<ListValueType> nullItem;			// 调用void构造函数就是对象名后面不加()
			return nullItem;							// 返回空对象						
		}
		OrthoList<ListValueType> ans(ra, cb);			// 构造计算结果
		Node<ListValueType> ** bRHead = b.getRHead();
		Node<ListValueType> ** bCHead = b.getCHead();
		Node<ListValueType> ** aRHead = a.getRHead();
		Node<ListValueType> ** aCHead = a.getCHead();
		Node<ListValueType> ** cAns = ans.getCHead();
		Node<ListValueType> ** rAns = ans.getRHead();
		Node<ListValueType> *p, *q;						// 链表遍历指针
		ListValueType compute = 0;						// ans[i,j]的计算结果
		bool has_value = false;							// 该项是否有值
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

	Node<ListValueType> *p, *q;						// 链表遍历指针
	for (i=0; i<nRow; i++)
	{
		for (j=0; j<nCol; j++)
		{
			if (mat.at(i).at(j) != 0)				// 如果是矩阵非零元
			{
				nElement++;
				Node<ListValueType>* aNode = new Node<ListValueType>();
				aNode->row = i;						// 创建新节点
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
				Node<ListValueType>* aNode = new Node<ListValueType>();	// 创建新节点
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
				Node<ListValueType>* aNode = new Node<ListValueType>();	// 创建新节点
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
		vector<int> line(nCol,0);						// 存储矩阵的一行, 初始化为全0
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
				// 输出(row, col, val)三元组
				p = p->right;
			}
		}
	}
}

// 在(r,c)位置加入新节点，若(r,c)位置已有节点，则替换为新值
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
	bool r_flag = false;								// 是否在行中已经添加过aNode的标志
	bool c_flag = false;								// 是否在列中已经修改连接关系
	Node<ListValueType> *p, *q, *tmpNode;				// 遍历链表指针
	// 向行中插入新元素或修改旧元素
	if (rHead[r] && cHead[c])
	{
		p = rHead[r]->right;
		q = cHead[c]->down;
		if (p->col > c)									// c的列值小于rHead[r]行首的列值
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			nElement++;
			r_flag = true;
			// 修改列的连接关系
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
		else if (p->col == c)					// 相等则替换
		// 因为while循环条件: p->right != NULL, 所以每行第0个节点需要单独讨论
		{
			p->val = v;
			delete aNode;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->right->col == c)				// 相等则替换
				// 考虑下一个节点, 这样写是为了兼容while循环条件: p->right != NULL
				{
					p -> right -> val = v;
					delete aNode;							// 此时无须添加新节点
					r_flag = true;
					break;
				}
				else if (p->col < c && p->right->col >c)	// 不等则插入
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					nElement++;
					r_flag = true;
					// 修改列的连接关系
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
				p->right = aNode;	// 到达while循环结尾说明aNode->c大于所有p->c
				nElement++;
				// 修改列的连接关系
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
		int c_flag = false;								// 是否已经在列中修改连接关系
		// 修改列的连接关系
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
		int r_flag = false;								// 是否已经在行中修改连接关系
		// 修改行的连接关系
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

// 稀疏矩阵this与单个元素aNode(r,c,v)相加, this(r,c)不存在则建新, 存在则相加
// 复用bool OrthoList <ListValueType>::add(int r, int c, ListValueType v)的代码进行修改
template <class ListValueType>
bool OrthoList <ListValueType>::elementWiseAddition(Node<ListValueType> *aNode)
{
	int r = aNode->row;
	int c = aNode->col;
	ListValueType v = aNode->val;
	if (r>=nRow || c>=nCol || r<0 || c<0)
	{
		return false;									// 尺寸溢出，返回false
	}
	bool r_flag = false;								// 是否在行中已经添加过aNode的标志
	bool c_flag = false;								// 是否在列中已经修改连接关系
	Node<ListValueType> *p, *q, *tmpNode;				// 遍历链表指针
	// 向行中插入新元素或修改旧元素
	if (rHead[r] && cHead[c])
	{
		p = rHead[r]->right;
		q = cHead[c]->down;
		if (p->col > c)									// c的列值小于rHead[r]行首的列值
		{
			rHead[r]->right = aNode;
			aNode->right = p;
			r_flag = true;
			// 修改列的连接关系
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
		else if (p->col == c)					// 相等则替换
		// 因为while循环条件: p->right != NULL, 所以每行第0个节点需要单独讨论
		{
			p->val += v;						// 将替换改为相加
			delete aNode;
			r_flag = true;
		}
		else
		{
			while (p->right != NULL)
			{
				if (p->right->col == c)				// 相等则替换
				// 考虑下一个节点, 这样写是为了兼容while循环条件: p->right != NULL
				{
					p -> right -> val += v;
					delete aNode;							// 此时无须添加新节点
					aNode = NULL;
					r_flag = true;
					break;
				}
				else if (p->col < c && p->right->col >c)	// 不等则插入
				{
					tmpNode = p->right;
					p->right = aNode;
					aNode->right = tmpNode;
					r_flag = true;
					// 修改列的连接关系
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
				p->right = aNode;	// 到达while循环结尾说明aNode->c大于所有p->c
				// 修改列的连接关系
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
		int c_flag = false;								// 是否已经在列中修改连接关系
		// 修改列的连接关系
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
		int r_flag = false;								// 是否已经在行中修改连接关系
		// 修改行的连接关系
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
	if (r>=nRow || c>= nCol || r<0 || c<0)						// 下标超出索引范围
	{
		return false;
	}
	Node<ListValueType> *p, *q;									// 链表遍历指针
	Node<ListValueType> *tmpNode;								// 用于删除节点的中间变量
	p = rHead[r];
	q = cHead[c];
	if (p == NULL)												// 该行为空
	{
		return false;
	}
	else
	{
		while (p->right != NULL)
		{
			if (p->right->col == c)								// 找到欲删除元素
			{
				// 更改列的连接关系
				while (q->down->row != r)						// 遍历cHead[c]找row==r				
				{
					q = q->down;
				}
				if (q == cHead[c] && q->down->down == NULL)		// 删掉的是该列唯一的元素
				{
					delete cHead[c];
					cHead[c] = NULL;
				}
				else
				{
					q->down = q->down->down;					// 修改列的连接关系
				}
				if (p == rHead[r] && p->right->right == NULL)	// 删掉的是该行的唯一的元素
				{
					delete rHead[r]->right;
					rHead[r]->right = NULL;
					delete rHead[r];
					rHead[r] = NULL;
				}
				else
				{
					tmpNode = p->right;
					p->right = p->right->right;					// 修改行的连接关系
					delete tmpNode;
					tmpNode = NULL;
				}
				nElement--;
				return true;
			}
			p  = p->right;
		}
		return false;											// 找不到欲删除的元素
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
		return false;								// 大小不匹配: 返回false
	}
	Node<ListValueType> *pb;						// 遍历b的指针
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
				if (!elementWiseAddition(aNode))	// 调用逐个加法私有方法
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