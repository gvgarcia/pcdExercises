/* File:    omp_trap1.c
 * Purpose: Estimate definite integral (or area under curve) using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -fopenmp -o omp_trap1 omp_trap1.c
 * Usage:   ./omp_trap1 <number of threads>
 *
 * Notes:   
 *   1.  The function f(x) is hardwired.
 *   2.  In this version, each thread explicitly computes the integral
 *       over its assigned subinterval, a critical directive is used
 *       for the global sum.
 *   3.  This version assumes that n is evenly divisible by the 
 *       number of threads
 *
 * IPP:  Section 5.2.1 (pp. 216 and ff.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);
double f(double x);    /* Function we're integrating */
//void Trap(double a, double b, int n, double* global_result_p);
double LocalTrap(double a, double b, int n);

int main(int argc, char* argv[]) {
   double  global_result = 0.0;  /* Store result in global_result */
   double  a, b;                 /* Left and right endpoints      */
   int     n;                    /* Total number of trapezoids    */
   int     thread_count;
   double initTime = 0.0;
   double endTime = 0.0;


   if (argc != 5) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   a = (double)strtol(argv[2], NULL, 10);
   b = (double)strtol(argv[3], NULL, 10);
   n = strtol(argv[4], NULL, 10);

   if (n % thread_count != 0) Usage(argv[0]);

//#  pragma omp parallel num_threads(thread_count) 
//   Trap(a, b, n, &global_result);

   initTime = omp_get_wtime();
   global_result = 0.0;
   # pragma omp parallel num_threads(thread_count)
   {
      # pragma omp critical
      global_result += LocalTrap(a, b, n);
   }
   endTime = omp_get_wtime();

   printf("With n = %d trapezoids, our estimate\n", n);
   printf("of the integral from %f to %f = %.14e\n",
      a, b, global_result);
   printf("Total time: %f\n", (endTime - initTime));
   return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads> <a> <b> <n>\n", prog_name);
   fprintf(stderr, "   number of trapezoids must be evenly divisible by\n");
   fprintf(stderr, "   number of threads\n");
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input arg:   x
 * Return val:  f(x)
 */
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  /* f */

/*------------------------------------------------------------------
 * Function:    LocalTrap
 * Purpose:     Use trapezoidal rule to estimate definite integral
 * Input args:  
 *    a: left endpoint
 *    b: right endpoint
 *    n: number of trapezoids
 * Output arg:
 *    integral:  estimate of integral from a to b of f(x)
 */
double LocalTrap(double a, double b, int n){//, double* global_result_p) {
   double  h, x, my_result;
   double  local_a, local_b;
   int  i, local_n;
   int my_rank = omp_get_thread_num();
   int thread_count = omp_get_num_threads();

   h = (b-a)/n; 
   local_n = n/thread_count;  
   local_a = a + my_rank*local_n*h; 
   local_b = local_a + local_n*h; 
   my_result = (f(local_a) + f(local_b))/2.0; 
   for (i = 1; i <= local_n-1; i++) {
     x = local_a + i*h;
     my_result += f(x);
   }
   my_result = my_result*h; 
   return my_result;
//#  pragma omp critical 
//   *global_result_p += my_result; 
   
}  /* LocalTrap */
