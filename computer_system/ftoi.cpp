

#include<cstdio>

union type
{
	int i;
	float f;
};
float itof(int x)
{
	unsigned int sign = 0;
	if (x < 0)
	{
		sign = 1;
		x = -x;
	}
	int count = 0;
	int tmp = x;
	const int max1 = (1 << 31);
	while ((max1&tmp) == 0 && count != 32)
	{
		tmp <<= 1;
		count++;
	}
	count = 31 - count;

	unsigned int e = count + 127;
	if (count == -1)
		e = 0;
	type f;
	if (count > 23)
		f.i = ((sign << 31) | (e << 23) | ((x >> (count - 23))&((1 << 23) - 1)));
	else
	{

		f.i = ((sign << 31) | (e << 23) | ((x << (23 - count)))&((1 << 23) - 1));
	}

	return f.f;
}

int ftoi(float x)
{
	int sign = 0;
	if (x < 0)
	{
		sign = 1;
		x = -x;
	}

	type res;
	type tmp;
	tmp.f = x;
	int e = ((tmp.i >> 23)&((1 << 8) - 1));
	if (e == 0)
		return 0;
	e -= 127;
	if (e < 0)
	{
		return 0;
	}
	res.i = (((tmp.i >> (23 - e))&((1 << e) - 1)) | (1 << e));
	if (sign == 1)
		res.i = -res.i;
	return res.i;

}

int main()
{
	//int k;
	//while (scanf("%d", &k) == 1)
	//{
	//	printf("%lf\n", itof(k));
	//}
	float f;
	while (scanf("%f", &f) == 1)
	{
		printf("%d\n", ftoi(f));
	}
}