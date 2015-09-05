/*
 *
 *   Example program demonstrates 1024 bit Diffie-Hellman, El Gamal and RSA
 *   and 168 bit Elliptic Curve Diffie-Hellman 
 *
 */

#include <stdio.h>
#include "miracl.h"
#include <time.h>

/* large 1024 bit prime p for which (p-1)/2 is also prime */
char *primetext=
"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE649286651ECE65381FFFFFFFFFFFFFFFF";


char *text="MIRACL - Best multi-precision library in the World!\n";

int main()
{
    time_t seed;
    big a,b,c,d,e,f;
    big primes[2],pm[2];
    big_chinese ch;
    miracl *mip;
#ifndef MR_NOFULLWIDTH   
    mip=mirsys(1200,0);
#else
    mip=mirsys(1200,MAXBASE);
#endif
    a=mirvar(0);
    b=mirvar(0);
    c=mirvar(0);
    d=mirvar(0);
    e=mirvar(0);
    f=mirvar(0);

    time(&seed);
    irand((unsigned long)seed);   /* change parameter for different values */

    printf("First Diffie-Hellman Key exchange .... \n");



    mip->IOBASE=16;
    cinstr(a,primetext);
    
    mip->IOBASE=10;
    printf("Big Prime in decimal #1 =\n");
    cotnum(a,stdout);
    
    
    

    
    

}

