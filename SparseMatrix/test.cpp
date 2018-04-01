#include "OrthoList.h"
#include "matrix.h"
#include "D:\用户目录\Documents\Visual Studio 2012\Projects\Complex\Complex\complex.h"
#include "ComplexOrthoList.h"
#include<ctime>

const int TEST_TIME = 1;							// 重复测试取平均的次数

int main()
{
	vector< vector<int> > mat, mat1;
	vector<int> aLine;
	// 第一行：0,1,0
	aLine.push_back(0);
	aLine.push_back(1);
	aLine.push_back(0);
	mat.push_back(aLine);
	aLine.clear();
	// 第二行：2,0,0
	aLine.push_back(2);
	aLine.push_back(0);
	aLine.push_back(0);
	mat.push_back(aLine);
	aLine.clear();
	// 第三行：0,0,3
	aLine.push_back(0);
	aLine.push_back(0);
	aLine.push_back(3);
	mat.push_back(aLine);
	
	cout << "**********************" << endl;
	cout << "****OrthoList test****" << endl;
	cout << "**********************" << endl;
	OrthoList<int> testOList(mat);
	testOList.printSparse();
	testOList.printFull();
	cout << "Add elements" << endl;
	testOList.add(0,0,-1);
	testOList.add(0,1,-2);
	testOList.add(0,2,-3);
	testOList.add(1,0,-4);
	testOList.add(1,1,-5);
	testOList.add(1,2,-6);
	testOList.add(2,0,-7);
	testOList.add(2,1,-8);
	testOList.add(2,2,-9);
	testOList.printSparse();
	testOList.printFull();
	cout << "Delete elements" << endl;
	testOList.del(0,0);
	testOList.del(1,1);
	testOList.del(1,0);
	testOList.printSparse();
	testOList.printFull();
	OrthoList<int> oList1(testOList);
	cout << " New sparse matrix: " << endl;
	oList1.printFull();
	cout << " matrix1 + matrix2: " << endl;
	testOList.Addition(oList1);
	testOList.printSparse();
	testOList.printFull();
	cout << "matrix1 * martrix2: " << endl;
	OrthoList<int> oList2(oList1);
	oList2 = testOList * oList1;
	oList2.printFull();

	cout << "**********************" << endl;
	cout << "*OrthoList test done**" << endl;
	cout << "**********************" << endl;
	cout << endl;
	cout << "----------------------" << endl;
	cout << "-----matrix test------" << endl;
	cout << "----------------------" << endl;
	int m = 2, n = 2;
    Matrix<int> a = oList1.toFull();
    Matrix<int> b = testOList.toFull();
    cout<<"Matrix a"; a.display(); cout<<endl;
    cout<<"Matrix b"; b.display(); cout<<endl;
    Matrix<int> c;
	c = (a+b);
    cout<<"a+b: "; c.display(); cout<<endl;
    Matrix<int> d;
	d = (a*b);
    cout<<"a*b:"; d.display(); 
	cout << "----------------------" << endl;
	cout << "---matrix test done---" << endl;
	cout << "----------------------" << endl << endl;

	cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << "@@@ run time test @@@@" << endl;
	cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
	ifstream fin("Real.txt");					// 数据文件
	assert(fin);								// 文件是否正常打开
	ComplexOrthoList sY(fin);					// sY: sparse Y matrix
	fin.close();
	fin.open("Real_2.txt");
	assert(fin);
	ComplexOrthoList sY1(fin);
	fin.close();
	ComplexOrthoList sY2(sY);
	Matrix<Complex> dY = sY.toFull();			// dY: dense Y matrix
	Matrix<Complex> dY1 = sY1.toFull();
	Matrix<Complex> dY2(dY);
	int i = 0;
	clock_t t0;
	// 测试矩阵加法
	t0 = clock();
	for (i=0; i<TEST_TIME; i++)
	{
		sY.Addition(sY1);
	}
	clock_t run_sparse_add = clock()-t0;
	run_sparse_add /= TEST_TIME;
	cout << TEST_TIME << " times SPARSE MATRIX addition: " 
		<< run_sparse_add << " ms" << endl;
	t0 = clock();
	for (i=0; i<TEST_TIME; i++)
	{
		dY2 = dY + dY1;
	}
	clock_t run_dense_add = clock()-t0;
	run_dense_add /= TEST_TIME;
	cout << TEST_TIME << " times DENSE MATRIX addition: " 
		<< run_dense_add << " ms" << endl;
	// 测试矩阵乘法
	t0 = clock();
	for (i=0; i<TEST_TIME; i++)
	{
		sY2 = sY * sY1;
	}
	clock_t run_sparse_mul = clock()-t0;
	run_sparse_mul /= TEST_TIME;
	cout << TEST_TIME << " times SPARSE MATRIX multiplication: " 
		<< run_sparse_mul << " ms" << endl;
	t0 = clock();
	for (i=0; i<TEST_TIME; i++)
	{
		dY2 = dY * dY1;
	}
	clock_t run_dense_mul = clock()-t0;
	run_dense_mul /= TEST_TIME;
	cout << TEST_TIME << " times DENSE MATRIX multipication: " 
		<< run_dense_mul << " ms" << endl;
	return 0;
}