#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

typedef double complex cplx;
double PI;
double *buf;
cplx *res;
int N,P;

void* threadFunction(void *var)
{
	int thread_id = *(int*)var;

	int start = thread_id * N / P;
	int end = (thread_id + 1) * N / P;

	int k,i;

	for(k = start; k < end; k ++){
		cplx sum = 0;
		for(i = 0 ; i < N; i++){
			sum += buf[i] * cexp((-2 * PI * I * k * i) / N);
		}
		res[k] = sum;
	}
	return NULL;
}

int main(int argc, char **argv)
{
	P = atoi(argv[3]);
	int i;

	PI = atan2(1,1) * 4;


	FILE *in = fopen(argv[1], "rt");
  	FILE *out = fopen(argv[2], "wt");

  	if (in == NULL || out == NULL) {
    	fprintf(stdout, "Failed to open at least one file.\n");
    	exit(1);
  	}

  	int ret;
  	
  	ret = fscanf(in,"%d", &N);
  	if (ret != 1) {
    	fprintf(stdout, "Failed to read N from file.\n");
    	exit(1);
  	}

  	buf = malloc(sizeof(double) * N);
  	res = malloc(sizeof(cplx) * N);

  	for(i = 0; i < N; i ++){
  		ret = fscanf(in,"%lf",&buf[i]);
  		if (ret != 1) {
    		fprintf(stdout, "Failed to read N values.\n");
    		exit(1);
  		}
  	}

  	

	pthread_t tid[P];
	int thread_id[P];
	for(i = 0;i < P; i++)
		thread_id[i] = i;

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	fprintf(out, "%d\n", N);


	for(i = 0 ; i < N ; i++){
		fprintf(out, "%lf %lf\n",creal(res[i]),cimag(res[i]));
	}



	fclose(in);
	fclose(out);
	free(buf);
	free(out);
	return 0;
}