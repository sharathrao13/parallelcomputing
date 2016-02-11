#include <acml.h> //assumes AMD platform

const char* dgemm_desc = "BLAS dgemm.";
void square_dgemm( int n, double *A, double *B, double *C )
{
    dgemm( 'N','N', n,n,n, 1, A,n, B,n, 1, C,n );
}
