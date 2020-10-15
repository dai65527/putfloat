/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnakano <dnakano@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/13 15:48:02 by dnakano           #+#    #+#             */
/*   Updated: 2020/10/15 11:43:49 by dnakano          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>

#define FLT_MTSSIZE 151	// 最小指数126 + 仮数部24bit(ケチ含) + 計算用余裕1

typedef unsigned char 	t_uchar;

typedef struct	s_float
{
	u_int8_t	sign;
	int8_t		exp;
	u_int32_t	num_int;
	u_int32_t	num_mts;
	int8_t		mts_dec[FLT_MTSSIZE];
}				t_float;

void			mts_divbytwo(int8_t *mts, int size)
{
	int		i;

	i = 0;
	while (i < size - 1)
	{
		mts[i + 1] += (mts[i] % 2) * 10;
		mts[i] /= 2;
		i++;
	}
	i = 0;
	while (i < FLT_MTSSIZE)
		printf("%d", mts[i++]);
	printf("\n");
}

void			mts_add(int8_t *a, int8_t *b, int size)
{
	int		i;

	i = 0;
	// printf("a = ");
	// while (i < 50)
	// 	printf("%d", a[i++]);
	// printf("\n");
	i = 0;
	// printf("b = ");
	// while (i < FLT_MTSSIZE)
	// 	printf("%d", b[i++]);
	// printf("\n");
	i = size - 1;
	while (i >= 0)
	{
		a[i] += b[i];
		if (a[i] >= 10 && i != 0)
		{
			a[i] -= 10;
			a[i - 1] += 1;
		}
		i--;
	}
	// i = 0;
	// printf("a = ");
	// while (i < 50)
	// 	printf("%d", a[i++]);
	// printf("\n");
}

void			mts_leftshift(int8_t *mts, int size)
{
	int		i;

	i = 0;
	while (i < size - 1)
	{
		mts[i] = mts[i + 1];
		i++;
	}
	mts[size - 1] = 0;
}

void			store_ifloat_mts(t_float *ifloat)
{
	int			i;
	int8_t		mts[FLT_MTSSIZE];

	bzero(mts, sizeof(mts));
	bzero(ifloat->mts_dec, sizeof(ifloat->mts_dec));
	mts[0] = 5;
	i = 0;
	while (i < -ifloat->exp - 1)
	{
		mts_divbytwo(mts, FLT_MTSSIZE);
		i++;
	}
	i = 0;
	while (i < 24)
	{
		if (ifloat->num_mts & (1 << (31 - i)))
			mts_add(ifloat->mts_dec, mts, FLT_MTSSIZE);
		mts_divbytwo(mts, FLT_MTSSIZE);
		i++;
	}
}

t_float	store_ifloat(float num)
{
	int					i;
	int			offset;
	u_int32_t	mem;
	u_int32_t	frac;
	t_float		ifloat;

	memcpy(&mem, &num, 4);
	ifloat.sign = mem >> 31;
	ifloat.exp = (mem >> 23) - 127;
	frac = mem << 9;
	if (ifloat.exp > 23)
		offset = 23;
	else if (ifloat.exp < 0)
		offset = 0;
	offset = ifloat.exp;
	if (offset >= 0)
	{
		ifloat.num_int = (frac >> (32 - offset)) | (1 << offset);
		ifloat.num_mts = frac << offset;
	}
	else
	{
		ifloat.num_int = 0;
		ifloat.num_mts = (frac >> 1) | (1 << 31);
	}
	store_ifloat_mts(&ifloat);
	i = 0;
	return (ifloat);
}

static void print_ifloat(float num, t_float ifloat)
{
	int		i;

	printf("num = %.150f\n", num);
	printf("sign = %hhu\n", ifloat.sign);
	printf("exp = %hhd\n", ifloat.exp);
	printf("num_int = %u\n", ifloat.num_int);
	printf("num_mts = %u\n", ifloat.num_mts);
	printf("mts_dec = ");
	i = 0;
	while (i < FLT_MTSSIZE - 1)
		printf("%d", ifloat.mts_dec[i++]);
	printf("\n\n");
}

int		main(void)
{
	float		num;
	t_float		ifloat;

	// num = 110.625;
	// ifloat = store_ifloat(num);
	// print_ifloat(num, ifloat);

	// num = 3.1415926535;
	// ifloat = store_ifloat(num);
	// print_ifloat(num, ifloat);

	// num = 0.00031415926535;
	// ifloat = store_ifloat(num);
	// print_ifloat(num, ifloat);

	num = __FLT_MIN__;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num =2.3509885615147286e-38;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	// num = __FLT_MAX__;
	// ifloat = store_ifloat(num);
	// print_ifloat(num, ifloat);

	return (0);
}
