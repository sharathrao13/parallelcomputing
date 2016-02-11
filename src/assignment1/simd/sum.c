#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif


#define CLOCK_RATE_GHZ 2.26e9

static __inline__ uint64_t RDTSC()
{
    uint32_t hi, lo;
    __asm__ volatile
	(
		"rdtsc"
		: "=a"(lo), "=d"(hi)
	);
	return (((uint64_t)hi) << 32) | ((uint64_t)lo);
}

static int sum_naive(int n, int *a)
{
    int sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += a[i];
	}
	return sum;
}

static int sum_unrolled(int n, int *a)
{
    int sum = 0;

    // unrolled loop
	for (int i = 0; i < n / 4 * 4; i += 4)
    {
        sum += a[i+0];
        sum += a[i+1];
        sum += a[i+2];
        sum += a[i+3];
    }

    // tail case
	for (int i = n / 4 * 4; i < n; i++)
	{
		sum += a[i];
	}

    return sum;
}

static int sum_vectorized(int n, int *a)
{
    int sum[4] __attribute__((aligned(32))) ={0};

    __m128i vsum = _mm_setzero_si128();

        

    for (int i = 0; i < n/4*4; i += 4)

    {

        __m128i v = _mm_loadu_si128((__m128i const *)&a[i]);

        vsum = _mm_add_epi32(vsum, v);

    }

    

    _mm_storeu_si128((__m128i *)sum,vsum);



    int finalsum=0;

    finalsum = sum[0]+sum[1]+sum[2]+sum[3];



    // tail case
    
    for (int i = n / 4 * 4; i < n;  i++)
    {
    
      	finalsum = finalsum + a[i];

    }
    
    return finalsum;


}

static int sum_vectorized_unrolled(int n, int *a)
{

    int sum1[4] __attribute__((aligned(32))) ={0};

    int sum2[4] __attribute__((aligned(32))) ={0};

    int sum3[4] __attribute__((aligned(32))) ={0};

    int sum4[4] __attribute__((aligned(32))) ={0};



    __m128i vsum1 = _mm_setzero_si128();

    __m128i vsum2 = _mm_setzero_si128();

    __m128i vsum3 = _mm_setzero_si128();

    __m128i vsum4 = _mm_setzero_si128();

        

    for (int i = 0; i < n/16*16; i += 16)

    {

        __m128i v1 = _mm_loadu_si128((__m128i const *)&a[i]);

        vsum1 = _mm_add_epi32(vsum1, v1);

        __m128i v2 = _mm_loadu_si128((__m128i const *)&a[i+4]);

        vsum2 = _mm_add_epi32(vsum2, v2);

        __m128i v3 = _mm_loadu_si128((__m128i const *)&a[i+8]);

        vsum3 = _mm_add_epi32(vsum3, v3);

        __m128i v4 = _mm_loadu_si128((__m128i const *)&a[i+12]);

        vsum4 = _mm_add_epi32(vsum4, v4);

    }

    

    _mm_storeu_si128((__m128i *)sum1,vsum1);

    _mm_storeu_si128((__m128i *)sum2,vsum2);

    _mm_storeu_si128((__m128i *)sum3,vsum3);

    _mm_storeu_si128((__m128i *)sum4,vsum4);



    int finalsum=0;

    finalsum = sum1[0]+sum1[1]+sum1[2]+sum1[3]+sum2[0]+sum2[1]+sum2[2]+sum2[3]+sum3[0]+sum3[1]+sum3[2]+sum3[3]+sum4[0]  	+sum4[1]+sum4[2]+sum4[3];



    // tail case
    
         for (int i = n / 16 * 16; i < n;  i++)
             {
    
            	finalsum = finalsum + a[i];
    
           	    }
    
      	        return finalsum;


}



void benchmark(int n, int *a, int(*computeSum)(int, int*), char *name)
{
    // warm up cache
    int sum = computeSum(n, a);

    // measure
    uint64_t beginCycle = RDTSC();
    sum += computeSum(n, a);
	uint64_t cycles = RDTSC() - beginCycle;
    
    double microseconds = cycles/CLOCK_RATE_GHZ*1e6;
    
    // print results
	printf("%20s: ", name);
	if (sum == 2 * sum_naive(n, a))
	{
		printf("%.2f microseconds\n", microseconds);
	}
	else
	{
		printf("ERROR!\n");
	}
}

int main(int argc, char **argv)
{
    const int n = 7777;
    
    // initialize the array with random values
	srand48(time(NULL));
	int a[n] __attribute__((aligned(16)));
	for (int i = 0; i < n; i++)
	{
		a[i] = lrand48();
	}
    
    // benchmark series of codes
	benchmark(n, a, sum_naive, "naive");
	benchmark(n, a, sum_unrolled, "unrolled");
	benchmark(n, a, sum_vectorized, "vectorized");
	benchmark(n, a, sum_vectorized_unrolled, "vectorized unrolled");

    return 0;
}

