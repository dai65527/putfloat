/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnakano <dnakano@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/13 15:48:02 by dnakano           #+#    #+#             */
/*   Updated: 2020/10/15 16:20:56 by dnakano          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>

#define FLT_MTSSIZE 151	// 最小指数126 + 仮数部24bit(ケチ含) + 計算用余裕1
#define FLT_INTSIZE 39	// 最大桁数

typedef unsigned char 	t_uchar;

typedef struct	s_float
{
	u_int8_t	sign;
	int8_t		exp;
	u_int32_t	frac;
	u_int32_t	int_bin;
	u_int32_t	mts_bin;
	int8_t		int_dec[FLT_INTSIZE];
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
}

void			arr_add(int8_t *a, int8_t *b, int size)
{
	int		i;

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

	bzero(ifloat->mts_dec, sizeof(ifloat->mts_dec));
	if (!ifloat->mts_bin)
		return ;
	bzero(mts, sizeof(mts));
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
		if (ifloat->mts_bin & (1 << (31 - i)))
			arr_add(ifloat->mts_dec, mts, FLT_MTSSIZE);
		mts_divbytwo(mts, FLT_MTSSIZE);
		i++;
	}
}

void			itg_dbl(int8_t *itg, int size)
{
	int		i;

	i = size - 1;
	while(i >= 0)
	{
		itg[i] *= 2;
		if (i < size - 1 && itg[i + 1] >= 10)
		{
			itg[i] += 1;
			itg[i + 1] -= 10;
		}
		i--;
	}
	i = 0;
	// while (i < size)
	// 	printf("%d", itg[i++]);
	// printf("\n");
}

void			store_ifloat_int(t_float *ifloat)
{
	int			i;
	int8_t		itg[FLT_INTSIZE];

	bzero(ifloat->int_dec, sizeof(ifloat->int_dec));
	if (!ifloat->int_bin)
		return ;
	bzero(itg, sizeof(itg));
	itg[FLT_INTSIZE - 1] = 1;
	i = 0;
	while (i < 24)
	{
		if (ifloat->int_bin & (1 << i))
			arr_add(ifloat->int_dec, itg, FLT_INTSIZE);
		itg_dbl(itg, FLT_INTSIZE);
		i++;
	}
	while (i < ifloat->exp + 1)
	{
		itg_dbl(ifloat->int_dec, FLT_INTSIZE);
		i++;
	}
}

t_float	store_ifloat(float num)
{
	int			offset;
	u_int32_t	mem;
	t_float		ifloat;

	memcpy(&mem, &num, 4);
	ifloat.sign = mem >> 31;
	ifloat.exp = (mem >> 23) - 127;
	ifloat.frac = mem << 9;
	offset = (ifloat.exp >= 23) ? 23 : ifloat.exp;
	if (offset >= 0)
	{
		ifloat.int_bin = (ifloat.frac >> (32 - offset)) | (1 << offset);
		ifloat.mts_bin = ifloat.frac << offset;
	}
	else
	{
		ifloat.int_bin = 0;
		ifloat.mts_bin = (ifloat.frac >> 1) | (1 << 31);
	}
	store_ifloat_int(&ifloat);
	store_ifloat_mts(&ifloat);
	return (ifloat);
}

static void print_ifloat(float num, t_float ifloat)
{
	int		i;

	printf("num = %.150f\n", num);
	printf("sign = %hhu\n", ifloat.sign);
	printf("exp = %hhd\n", ifloat.exp);
	printf("int_bin = %u\n", ifloat.int_bin);
	printf("mts_bin = %u\n", ifloat.mts_bin);
	printf("int_dec = ");
	i = 0;
	while (i < FLT_INTSIZE)
		printf("%d", ifloat.int_dec[i++]);
	printf("\nmts_dec = ");
	i = 0;
	while (i < FLT_MTSSIZE - 1)
		printf("%d", ifloat.mts_dec[i++]);
	printf("\n\n");
}

int		main(void)
{
	float		num;
	t_float		ifloat;

	num = 110.625;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 3.1415926535;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = __FLT_MIN__;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 1e4;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 1.11111e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 2.11111e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 4.11111e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 8.11111e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 3.1415926535e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 12.1415926535e7;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 3.1415926535e8;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 3.1415926535e9;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 6.1415926535e9;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 3.1415926535e10;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = __FLT_MAX__;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	num = 0.0F;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);

	return (0);
}
