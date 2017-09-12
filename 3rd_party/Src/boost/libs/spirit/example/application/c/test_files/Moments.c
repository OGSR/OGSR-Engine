/* -*- mode: c -*-
 * $Id: Moments.c,v 1.2 2003/01/31 18:13:15 martin_wille Exp $
 * http://www.bagley.org/~doug/shootout/
 * from Waldek Hebisch
 */

/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXLINELEN 128
*/

/* kmedian permutes elements of a to get
   a[i]<=a[k] for i<k
   a[i]>=a[k] for i>k
    See. N. Wirth, Algorithms+data structures = Programs
*/

void kmedian(double *a, int n, int k) 
{
    while (1){
        int j=random()%n; 
        double b = a[j];
        int i=0;
            j = n-1;
            while(1) {
                while( a[i]<b ) i++;
                while( a[j]>b ) j--;
                if(i<j) {
                    double tmp=a[i];
                    a[i]=a[j];
                    a[j]=tmp;
                    i++;
                    j--;
                } else {
                    if(a[j]<b) j++;
                    if(a[i]>b) i--;
            break;
                }
            }
        if(i<k) {
            k-=i+1;
            n-=i+1;
            a+=i+1;
        } else if (j>k) { 
            n=j;
        } else return;
    }
}

double max(double *a, int n)
{
    int j;
    double temp=a[0];
    for(j=1;j<n;j++) { 
        if(a[j]>temp) {
            temp=a[j];
        }
    }
    return temp;
}


int
main() {
    char line[MAXLINELEN];
    int i, n = 0, mid = 0;
    double sum = 0.0;
    double mean = 0.0;
    double average_deviation = 0.0;
    double standard_deviation = 0.0;
    double variance = 0.0;
    double skew = 0.0;
    double kurtosis = 0.0;
    double median = 0.0;
    double deviation = 0.0;
    int array_size = 4096;

    double *nums = (double *)malloc(array_size * sizeof(double));

    while (fgets(line, MAXLINELEN, stdin)) {
    sum += (nums[n++] = atof(line));
    if (n == array_size) {
        array_size *= 2;
        nums = (double *)realloc(nums, array_size * sizeof(double));
    }
    }
    mean = sum/n;
    for (i=0; i<n; i++) {
    double dev = nums[i] - mean;
    double dev2=dev*dev;
    double dev3=dev2*dev;
    double dev4=dev3*dev;
    average_deviation += fabs(dev);
    variance += dev2 /*pow(deviation,2)*/;
    skew += dev3 /* pow(deviation,3) */;
    kurtosis += dev4 /* pow(deviation,4) */;
    }
    average_deviation /= n;
    variance /= (n - 1);
    standard_deviation = sqrt(variance);
    if (variance) {
    skew /= (n * variance * standard_deviation);
    kurtosis = (kurtosis/(n * variance * variance)) - 3.0;
    }
    mid = (n/2);
    kmedian(nums, n, mid); 
    median = n % 2 ? nums[mid] : (nums[mid] + max(nums,mid))/2;
    free(nums);
    printf("n:                  %d\n", n);
    printf("median:             %f\n", median);
    printf("mean:               %f\n", mean);
    printf("average_deviation:  %f\n", average_deviation);
    printf("standard_deviation: %f\n", standard_deviation);
    printf("variance:           %f\n", variance);
    printf("skew:               %f\n", skew);
    printf("kurtosis:           %f\n", kurtosis);

    return(0);
}
