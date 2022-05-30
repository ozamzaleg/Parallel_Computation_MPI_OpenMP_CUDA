#include "myProto.h"
#include <stdio.h>
#include <math.h>

int test(int *data, int n)
{
	int i;
	int counter = 0;
	for (i = 0; i < n; i++)
	{
		if (f(data[i]) > 0)
		{
			{
				counter++;
			}
		}
	}
	return counter;
}
double f(int i)
{
	int j;
	double value;
	double result = 0;

	for (j = 1; j < HEAVY; j++)
	{
		value = (i + 1) * (j % 10);
		result += cos(value);
	}
	return cos(result);
}
