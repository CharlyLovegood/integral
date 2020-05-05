#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float integral = 0, sigma = 0.5, g = 0.5;
float END = 1, START =  0.001;
float ACCURACY = 0;
int ThNumber = 4;

struct args {
    float start;
    float end;
};

float f(float x) {
    return cos(1/x);
};

float f_second_derivative(float x) {
    return -sin(1/x)*2/pow(x, 3) - cos(1/x)/pow(x, 4);
};

float countError(float x_start, float x_end) {
    float counter_step = 0.00001, max = 0;
    for (float x=x_start; x<x_end; x+=counter_step) { 
        float f_SD = fabs(f_second_derivative(x));
        if (f_SD > max) {
            max = f_SD;
        }
    }
    float error = fabs((pow(x_start - x_end, 3)*max)/12);
    return error;
};


void *mythread(void *args) {
    float res = 0, step = 0.00001, error = 0;
    float x = ((struct args*)args)->start;

    while (x+step <= ((struct args*)args)->end) {
        error = countError(x, x+step);
        if (error < ACCURACY*g) {
            step = step / sigma;
        }
        else if (error < ACCURACY) {
            step = step;
        }
        else {
            step = step * sigma;
        }

        if (x+step > ((struct args*)args)->end) {
            break;
        }

        res += ((f(x)+f(x+step))*step)/2;
        x += step;
    };

    if(x < ((struct args*)args)->end) {
        step = ((struct args*)args)->end - x;
        res += ((f(x)+f(x+step))*step)/2;
        x += step;
    };

    integral += res;
    return NULL;
};


int main() {
    int i = 0;

    printf("ACCURACY = ");
    scanf("%f", &ACCURACY);

    pthread_t *thid = malloc(ThNumber*sizeof(pthread_t));
    pthread_t *mythid = malloc(ThNumber*sizeof(pthread_t));
    struct args *arguments = malloc(ThNumber*sizeof(struct args));

    float zone = END / ThNumber;

    for (i = 0; i < ThNumber; i++) {
        if (i==0) arguments[i].start = START;
        else arguments[i].start = i*zone;
        arguments[i].end = (i+1)*zone;

        if(pthread_create(&thid[i], (pthread_attr_t *)NULL, mythread, (void *)&arguments[i]) != 0) {
            printf ("Error on thread create\n");
            exit(-1);
        }   
    }

    for (i = 0; i < ThNumber; i++) {
        pthread_join(thid[i], (void **)NULL);
    }

    printf ("-----------------------\n");
    printf ("integral = %f\n", integral);

    return 0;
}
