#include "primes.h"

/* Sunarthsh gia ton pio kontino megalytero prime */
int high(int n)
{
int i,j,k;
 for(i=n;i<=i+1;i++)
 {
     k=0;
     for(j=1;j<=i;j++)
     {
         if(i%j==0)
         {
             k++;

         }
     }
     if(k==2)
     {
        // printf("%d",i);
         break;
     }

 }
  return i;
}


/* Sunarthsh gia ton pio kontino mikrotero prime */
int low(int n)
{
int i,j,k;
 for(i=n;i>0;i--)
 {
     k=0;
     for(j=1;j<=i;j++)
     {
         if(i%j==0)
         {
             k++;
         }
     }
     if(k==2)
     {
         //printf("%d",i);
         break;
     }

 }
  return i;
}

/* Sunarthsh gia thn epilogh toy kontinoterou prime ston ariumo n. */
int primes(int n)
{
    int l,h,x,y,p,q;
    l=low(n);
    h=high(n);
    q=l-h;
    x=n-l;
    y=h-n;
    p=x-y;

    if(q==0)
        return n;
    else if(p==0)
        return h;
    else if(x<y)
        return l;
    else
        return h;
}
