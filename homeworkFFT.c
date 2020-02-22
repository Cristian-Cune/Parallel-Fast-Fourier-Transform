#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <complex.h>

typedef double complex cplx;
double PI;
cplx *buf;
cplx *res;
int N,P;

void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void* fft(void *var)//thread function
{
	int thread_id = *(int*)var;
	//normal fft
 	if( P == 1){
		_fft(buf, res, N, 1);

	} else if(P == 2){
		/*one thread gets the odd elements and the other, the even ones*/
		if(thread_id == 0){
			_fft(res, buf, N, 2);
		}else{
			_fft(res+1,buf+1,N,2);
		}

	} else if(P == 4){
		/*each thread gets N/4 elements*/
		if(thread_id == 0){
			_fft(res , buf, N, 4);

		}else if(thread_id == 1){
			_fft(res+1,buf+1,N,4);
		
		}else if(thread_id == 2){
			_fft(res+2,buf+2,N,4);

		}else if(thread_id == 3){
			_fft(res+3,buf+3,N,4);
		}
	}
	return NULL;
}
 
 
void show( cplx buf[], FILE *f) {
	fprintf(f,"%d\n", N);
	for (int i = 0; i < N; i++)
			fprintf(f,"%lf %lf\n", creal(buf[i]), cimag(buf[i]));
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

  	buf = malloc(sizeof(cplx) * N);
  	res = malloc(sizeof(cplx) * N);

  	for(i = 0; i < N; i ++){
  		double value;
  		ret = fscanf(in,"%lf",&value);
  		if (ret != 1) {
    		fprintf(stdout, "Failed to read N values.\n");
    		exit(1);
  		}
  		buf[i] = value + 0 * I;
  		res[i] = buf[i];
  	}

  	pthread_t tid[P];
	int thread_id[P];
	for(i = 0;i < P; i++)
		thread_id[i] = i;


  	if(P == 1){
  		fft(NULL);
  	
  	} else if(P == 2){
  		//crating 2 threads for the odd/even positions
  		for(i = 0; i < P; i++) {
			pthread_create(&(tid[i]), NULL, fft, &(thread_id[i]));
		}

		for(i = 0; i < P; i++) {
			pthread_join(tid[i], NULL);
		}

		//forming the final result from the odd and even parts
		for (int i = 0; i < N; i += 2) {
			cplx t = cexp(-I * PI * i / N) * res[i + 1];
			buf[i / 2]     = res[i] + t;
			buf[(i + N)/2] = res[i] - t;
		}
  	} else if(P == 4){
  		//creating 4 threads
  		for(i = 0; i < P; i++) {
			pthread_create(&(tid[i]), NULL, fft, &(thread_id[i]));
		}

		for(i = 0; i < P; i++) {
			pthread_join(tid[i], NULL);
		}

		//join 2 pieces 
		for (int i = 0; i < N; i += 2 * 2) {
			cplx t = cexp(-I * PI * i / N) * res[i + 2];
			buf[i / 2]     = res[i] + t;
			buf[(i + N)/2] = res[i] - t;
		}
			
		//join the other 2 pieces
		for (int i = 0; i < N; i += 2 * 2) {
			cplx t = cexp(-I * PI * i / N) * res[i + 3];
			buf[i / 2 + 1]     = res[i + 1] + t;
			buf[(i + N)/2 +1 ] = res[i + 1 ] - t;
		}
		
		for(i = 0;i < N; i++) res[i] = buf[i];

		//join the final 2 pieces obtainted to get the final result
		for (int i = 0; i < N; i += 2 * 1) {
			cplx t = cexp(-I * PI * i / N) * res[i + 1];
			buf[i / 2]     = res[i] + t;
			buf[(i + N)/2] = res[i] - t;
		}
		
  	}

	
 	show(buf,out);
 	fclose(in);
	fclose(out);
	free(buf);
	free(out);
	return 0;
}
 