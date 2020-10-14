/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnakano <dnakano@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/13 15:48:02 by dnakano           #+#    #+#             */
/*   Updated: 2020/10/14 17:04:52 by dnakano          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <math.h>
#include <stdio.h>

typedef struct	s_float
{
	u_int8_t	sign;
	int8_t		exp;
	u_int32_t	num_int;
	u_int32_t	num_mts;
}				t_float;

t_float	store_ifloat(float num)
{
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
	return (ifloat);
}

// static void print_ifloat_dec(t_float ifloat, int digit)
// {
// 	int		i;
// 	int		d_i;
// 	int		res;
// 	int		a[4];

// 	a[0] = 5000;
// 	a[1] = a[0] / 2;
// 	a[2] = a[1] / 2;
// 	res = a[0] * ((ifloat.num_d | (1 << (31 - i))) ? 1 : 0);
// 	res += a[1] * ((ifloat.num_d | (1 << (31 - i - 1))) ? 1 : 0);
// 	res += a[2] * ((ifloat.num_d | (1 << (31 - i - 2))) ? 1 : 0);
// 	i = 0;
// 	while (i < digit && i < 23)
// 	{
// 		a[(i + 3) % 4] = a[(i + 2) % 4] / 2; 
// 		res += a[(i + 3) % 4] * ((ifloat.num_d | (1 << (31 - i - 3))) ? 1 : 0);
// 		write(1, "0" + res / 1000, 1);
// 		1
// 		a[(i + 1) % 4] *= 10;
// 		a[(i + 2) % 4] *= 10;
// 		a[(i + 3) % 4] *= 10;
// 	}
// }

static void print_ifloat_dec(t_float ifloat)
{
	int			i;
	u_int64_t	mst_dec[2];
	u_int64_t	tmp;

	mst_dec[0] = 0; //小数点以下 1~19ケタ
	mst_dec[1] = 0;	//小数点以下 20~24ケタ
	tmp = (u_int64_t)1e19 >> (-ifloat.exp - 1);
	i = 0;
	while (i < 19)
	{
		tmp /= 2;
		mst_dec[0] += (ifloat.num_mts & 1 << (31 - i)) ? tmp : 0; 
		printf("i = %2d: tmp = %019llu, mst_dec[0] = %019llu\n", i, tmp, mst_dec[0]);
		i++;
	}
	tmp *= (u_int64_t)1e5;
	while (i < 24)
	{
		tmp /= 2;
		printf("i = %2d: tmp = %019llu, mst_dec[1] = %05llu\n", i, tmp, mst_dec[1]);
		mst_dec[1] += (ifloat.num_mts & 1 << (31 - i)) ? tmp : 0; 
		i++;
	}
	// printf("%llu\n%llu\n", mst_dec[0], mst_dec[1]);
	mst_dec[0] += mst_dec[1] / (u_int64_t)1e5;
	tmp = mst_dec[1] / (u_int64_t)1e5;
	mst_dec[1] -= tmp * (u_int64_t)1e5;
	printf("mts_dec = %019llu%05llu\n", mst_dec[0], mst_dec[1]);
}

static void print_ifloat(float num, t_float ifloat)
{
	printf("\nnum = %.24f\n", num);
	printf("sign = %hhu\n", ifloat.sign);
	printf("exp = %hhd\n", ifloat.exp);
	printf("num_int = %u\n", ifloat.num_int);
	printf("num_mts = %u\n", ifloat.num_mts);
}

int		main(void)
{
	float		num;
	t_float		ifloat;

	num = 0.9;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);
	print_ifloat_dec(ifloat);

	num = 0.5;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);
	print_ifloat_dec(ifloat);

	num = 0.4;
	ifloat = store_ifloat(num);
	print_ifloat(num, ifloat);
	print_ifloat_dec(ifloat);

	return (0);
}


	// printf("num = %.24f\n", num);
	// printf("num = %.23f\n", num);
	// printf("num = %.22f\n", num);
	// printf("num = %.21f\n", num);
	// printf("num = %.20f\n", num);
	// printf("num = %.19f\n", num);
	// printf("num = %.18f\n", num);
	// printf("num = %.17f\n", num);
	// printf("num = %.16f\n", num);
	// printf("num = %.15f\n", num);
	// printf("num = %.14f\n", num);
	// printf("num = %.13f\n", num);
	// printf("num = %.12f\n", num);
	// printf("num = %.11f\n", num);
	// printf("num = %.10f\n", num);
	// printf("num = %.9f\n", num);
	// printf("num = %.8f\n", num);
	// printf("num = %.7f\n", num);
	// printf("num = %.6f\n", num);
	// printf("num = %.5f\n", num);
	// printf("num = %.4f\n", num);
	// printf("num = %.3f\n", num);
	// printf("num = %.2f\n", num);
	// printf("num = %.1f\n", num);
	// printf("num = %.0f\n", num);
