#if 0
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
	cout << "our group by " << dimension << endl;
	clock_t startTime, endTime; 
	PFC pfc(AES_SECURITY);    // initialise pairing-friendly curve
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
	G1 GEK;
	//计算评估密钥
	GEK = pfc.mult(g1, T);
	cout << "pparam time" << endl;
	int x[dimension];
	RandVecADD(x, dimension);
	int L[dimension];
	RandVecADD(L, dimension);
	int H11[dimension];
	RandVecADD(H11, dimension);
	int H21[dimension];
	RandVecADD(H21, dimension);
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
	//cout << "pparamtime=" << endTime - startTime << endl;
	int b = 13;
	G1 PK;
	PK = pfc.mult(h,b);
	G1 MU[dimension];
	G1 tmp1, tmp2;
	startTime = clock();
	tmp1 = pfc.mult(g1, b*T);
	tmp2 = pfc.mult(g2, b);
	for (int i = 0; i < dimension; i++)
	{
		MU[i] = pfc.mult(tmp1, L[i]+H11[i]) + pfc.mult(tmp2, H21[i] + x[i]);
	}
	endTime = clock();
	cout << "标签生成="  << endTime - startTime << endl;
	//--------------------云计算--------
	startTime = clock();
	int res=0;
	for (int i = 0; i < dimension; ++i)
	{
		res = res + x[i];
	}
	G1 Poof;
	for (int i = 0; i < dimension; i++)
	{
		Poof = Poof + MU[i];  //   g^ d
	}
	endTime = clock();
	cout << "云计算时间=" << endTime - startTime << endl;
	int S1 = 0;
	for (int i = 0; i < dimension; i++)
	{
		S1 = S1 + H11[i];  //   g^ d
	}
	int S2 = 0;
	for (int i = 0; i < dimension; i++)
	{
		S2 = S2+ H21[i];  //   g^ d
	}
	startTime = clock();
	GT  N, V;
	 N = pfc.pairing(h, Poof);
	 V = pfc.pairing(PK,pfc.mult (GEK,(E+S1))+pfc.mult(g2, S2+res));
	 if (N == V)
	 	cout << "Signature verifies" << endl;
	 else
	 	cout << "Signature is bad" << endl;
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