#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "laplace.h"
#include "csvpackage.h"
#include <time.h>
extern int rand();
extern void srand(unsigned);


double csv_analysis(char* path, double beta, long int seed)
{
	FILE* original_file = fopen(path, "r+"); //��ȡָ��·�������ݼ�
	struct Animals* original_data = NULL;
	original_data = csv_parser(original_file);
	int sum = 0, i = 0;
	double x = 0;
	while (original_data[i].name)  //ѭ��Ϊԭʼ���ݼ��ڸ�������ȥ����ʶ����������������������˹����������
	{
		if (original_data[i].carrots + x >= 55)
		{
			sum++;
		}
		i++;
	}
	// printf("Animals which carrots cost > 55: %d\n",sum); //������������ݼ��У�ÿ��ʳ�ú��ܲ�����55�Ķ������
	x = laplace_data(beta, &seed); //����������˹�����
	// printf("Added noise:%f\t%s \t%f\n",x,"Query Sum (Under DP):", sum+x); 
	return sum + x;
}


int main()
{
	long int seed;
	int sen = 1;  //����һ�������Ե����ݼ��������ж�Ϊ1
	double beta;
	srand((unsigned)time(NULL)); //���ɻ���ʱ���������ӣ�srand������
	beta = 0;
	printf("Please input laplace epsilon:");
	scanf("%lf", &beta);

	// input the support times
	int query_frequency = 20;
	// query times: the times we do the query
	int query_times;
	printf("Please input query times:");
	query_times = 1;
	scanf("%d", &query_times);

	if (beta <= 0 || !beta)//�������betaֵ��Чʱ��Ĭ���趨betaֵΪ1
	{
		beta = 1.0;
	}
	printf("Under privacy budget %f, sanitized query result with laplace noise:\n", beta);
	// each query laplace epsilon
	beta = beta / query_frequency;
	printf("Each query privacy budget %f\n", beta);
	beta = sen / beta; //������˹�����£�ʵ�ʹ�ʽ������betaΪ���ж�/Ԥ��

	double* org_data_result;
	org_data_result = (double*)malloc(query_times * sizeof(double));
	printf("Animals which carrots cost > 55: %d\n", 90); // print org result
	for (int i = 0; i < query_times; i++) {
		seed = rand() % 10000 + 10000; //������Ӳ���
		double temp = csv_analysis("./zoo.csv", beta, seed); //�ȵ���ԭʼ���ݼ�
		org_data_result[i] = temp;
		// printf("Query%d result: \t%f\n", i+1, temp);
	}

	// compute the average
	double org_average = 0;
	for (int i = 0; i < query_times; i++) {
		org_average += org_data_result[i];
	}
	printf("Org Sum Query result: \t%f\n", org_average);
	org_average = org_average / query_times;
	printf("Org Average Query result: \t%f\n", org_average);

	printf("==================Using neighbour dataset==================\n");
	double* new_data_result;
	new_data_result = (double*)malloc(query_times * sizeof(double));
	printf("Animals which carrots cost > 55: %d\n", 89); // print org result
	for (int i = 0; i < query_times; i++) {
		seed = rand() % 10000 + 10000; //������Ӹ���
		double temp = csv_analysis("./zoo_nb.csv", beta, seed); //�ٵ����������ݼ�
		new_data_result[i] = temp;
		// printf("Query%d result: \t%f\n", i+1, temp);
	}

	// compute the average
	double new_average = 0;
	for (int i = 0; i < query_times; i++) {
		new_average += new_data_result[i];
	}
	printf("New Sum Query result: \t%f\n", new_average);
	new_average = new_average / query_times;
	printf("New Average Query result: \t%f\n", new_average);

	return 0;
}
