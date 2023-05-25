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
	FILE* original_file = fopen(path, "r+"); //读取指定路径的数据集
	struct Animals* original_data = NULL;
	original_data = csv_parser(original_file);
	int sum = 0, i = 0;
	double x = 0;
	while (original_data[i].name)  //循环为原始数据集内各条数据去除标识（动物名）、生成拉普拉斯噪音并加噪
	{
		if (original_data[i].carrots + x >= 55)
		{
			sum++;
		}
		i++;
	}
	// printf("Animals which carrots cost > 55: %d\n",sum); //输出加噪后的数据集中，每日食用胡萝卜大于55的动物个数
	x = laplace_data(beta, &seed); //产生拉普拉斯随机数
	// printf("Added noise:%f\t%s \t%f\n",x,"Query Sum (Under DP):", sum+x); 
	return sum + x;
}


int main()
{
	long int seed;
	int sen = 1;  //对于一个单属性的数据集，其敏感度为1
	double beta;
	srand((unsigned)time(NULL)); //生成基于时间的随机种子（srand方法）
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

	if (beta <= 0 || !beta)//当输入的beta值无效时，默认设定beta值为1
	{
		beta = 1.0;
	}
	printf("Under privacy budget %f, sanitized query result with laplace noise:\n", beta);
	// each query laplace epsilon
	beta = beta / query_frequency;
	printf("Each query privacy budget %f\n", beta);
	beta = sen / beta; //拉普拉斯机制下，实际公式的算子beta为敏感度/预算

	double* org_data_result;
	org_data_result = (double*)malloc(query_times * sizeof(double));
	printf("Animals which carrots cost > 55: %d\n", 90); // print org result
	for (int i = 0; i < query_times; i++) {
		seed = rand() % 10000 + 10000; //随机种子产生
		double temp = csv_analysis("./zoo.csv", beta, seed); //先调用原始数据集
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
		seed = rand() % 10000 + 10000; //随机种子更新
		double temp = csv_analysis("./zoo_nb.csv", beta, seed); //再调用相邻数据集
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
