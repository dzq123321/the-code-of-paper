#if 1
#include <iostream>
#include <ctime>
#include<time.h>
#define dimension 500
#define MR_PAIRING_SSP
#define AES_SECURITY 128
#include "pairing_1.h"
#define PERSON 6		 // sample recipient
int* RandVecADD(int*a, int n);
void Print(int*a, int n);

int main()
{
	cout << "our inner " << dimension << endl;
	 clock_t startTime, endTime;
	 PFC pfc(AES_SECURITY);     // initialise pairing-friendly curve
	G1 g1;
	G1 g2;
	G1 h;
	time_t seed;
	time(&seed);
	irand((long)seed);
	pfc.random(g1);
	pfc.random(g2);
	pfc.random(h);
	int T = 42;
	G1 GEK,IEK;
	//计算评估密钥
	GEK = pfc.mult(g1, T);
	IEK = pfc.mult(h, T);
	cout << "pparam time" << endl;
	int x1[dimension];
	RandVecADD(x1, dimension);
	int x2[dimension];
	RandVecADD(x2, dimension);
	int L[dimension];
	RandVecADD(L, dimension);
	int H11[dimension];
	RandVecADD(H11, dimension);
	int H21[dimension];
	RandVecADD(H21, dimension);
	int H12[dimension];
	RandVecADD(H12, dimension);
	int H22[dimension];
	RandVecADD(H22, dimension);
	startTime = clock();
	int E = 0;
	for (int i = 0; i < dimension; i++)
	{
		E = E + L[i];
	}
	int Lmd = 0;
	for (int i = 0; i < dimension; i++)
	{
		Lmd = Lmd + L[i] * H21[i];
	}
	endTime = clock();
	cout << "pparamtime=" << endTime - startTime << endl;
	int b1 = 13;
	int dt = 39;
	G1 ht= pfc.mult(h, dt);
	G1 PK1;
	PK1 = pfc.mult(h, b1);
	G1 VK1[dimension];
	G1 tmp4;
	tmp4 = pfc.mult(g1, b1);
	for (int i = 0; i < dimension; i++)
	{
		VK1[i] = pfc.mult(tmp4, L[i]+H11[i]);
	}
	G1 tmp3;
	G1 VK2[dimension];
	tmp3 = pfc.mult(g1, b1*dt);
	for (int i = 0; i < dimension; i++)
	{
		VK2[i] = pfc.mult(tmp3, L[i] + H11[i]);
	}
	G1 MU1[dimension];
	G1 tmp1, tmp2;
	startTime = clock();
	tmp1 = pfc.mult(g1, b1*T);
	tmp2 = pfc.mult(g2, b1);
	for (int i = 0; i < dimension; i++)
	{
		MU1[i] = pfc.mult(tmp1, L[i]+H11[i]) + pfc.mult(tmp2, H21[i] + x1[i]);
	}
	endTime = clock();
	cout << "1标签生成=" << endTime - startTime << endl;
	//////////////////////////////M2
	int b2 = 21;
	G1 PK2;
	PK2 = pfc.mult(h, b2);
	G1 MU2[dimension];
	tmp1 = pfc.mult(g1, b2*T);
	tmp2 = pfc.mult(g2, b2);
	for (int i = 0; i < dimension; i++)
	{
		MU2[i] = pfc.mult(tmp1, L[i]+H12[i]) + pfc.mult(tmp2, H22[i] + x2[i]);
	}
	///////////////////////////////////////////云
	startTime = clock();
	int y1 = 0;
	for (int i = 0; i < dimension; i++)
	{
		y1 = y1 + x1[i] * x2[i];
	}
	int y2 = 0;
	for (int i = 0; i < dimension; i++)
	{
		y2 = y2 + H21[i] * x2[i];
	}
	G1 poof1;
	for (int i = 0; i < dimension; i++)
	{
		tmp1 = pfc.mult(MU2[i], H21[i]);
		poof1 = poof1 + tmp1;
	}
	G1 poof2;
	for (int i = 0; i < dimension; i++)
	{
		tmp1 = pfc.mult(VK1[i], x2[i]);
		poof2 = poof2 + tmp1;
	}
	G1 poof3;
	for (int i = 0; i < dimension; i++)
	{
		tmp1 = pfc.mult(VK2[i], x2[i]);
		poof3 = poof3 + tmp1;
	}
	G1 poof4;
	for (int i = 0; i < dimension; i++)
	{
		tmp1 = pfc.mult(MU1[i], x2[i]);
		poof4 = poof4 + tmp1;
	}
	endTime = clock();
	cout << "云计算时间=" << endTime - startTime << endl;
	GT  N, V;
	int S12 = 0;
	for (int i = 0; i < dimension; i++)
	{
		S12 = S12 + H12[i]* H21[i];  //   g^ d
	}
	int S22 = 0;
	for (int i = 0; i < dimension; i++)
	{
		S22 = S22 + H22[i] * H21[i];  //   g^ d
	}
	startTime = clock();
	N = pfc.pairing(h, poof1);
	V = pfc.pairing(PK2, pfc.mult(GEK, (Lmd+S12)) + pfc.mult(g2, S22+y2));
	if (N == V)
		cout << "Signature verifies" << endl;
	else
		cout << "Signature is bad" << endl;
	N = pfc.pairing(h, poof3);
	V = pfc.pairing(ht, poof2);
	if (N == V)
		cout << "Signature verifies" << endl;
	else
		cout << "Signature is bad" << endl;
	N = pfc.pairing(h, poof4);
	/*V = pfc.pairing(poof2, IEK)* pfc.pairing(PK1, pfc.mult(g2, y1+y2));*/
	V = pfc.pairing(pfc.mult(poof2,T)+ pfc.mult(g2, b1*y1 + b1 * y2),h);
	if (N == V)
		cout << "Signature verifies" << endl;
	else
		cout << "Signature is bad" << endl;
	endTime = clock();
	cout << "验证时间=" << endTime - startTime << endl;

	cout << "--------------------" << endl;
	startTime = clock();
	N = pfc.pairing(h, poof1);
	endTime = clock();
	cout << "验证时间=" << endTime - startTime << endl;
	return 0;
}

int* RandVecADD(int*a, int n)
{
	int t = 0;
	for (int i = 0; i < n; ++i)
	{
		a[i] = i + 1;
		if (a[i] > 9)
			a[i] = a[i] % 10;
		if (a[i] == 0);
		a[i] = a[i] + 1;
	}

	for (int i = n - 1; i > 0; --i) {//从最后面开始遍历，因为我们能控制产生的随机数范围

		int mark = rand() % i;//获得下标
		t = a[mark];
		a[mark] = a[i];
		a[i] = t;

	}
	return a;
}
void Print(int*ar, int n)
{
	for (int i = 0; i < dimension; i++)
		printf("%d ", ar[i]);
	printf("\n");
}
#endif