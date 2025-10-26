#include <stdio.h>

// now proceed with the recurisivy ¿? 


unsigned Long int suma (unsigned Long int i)
{
    if (i==1) return 1;
    else return (i + suma(i-1))
}

int main ( 
    {
            unsigned long int tope = 1000;
            unsigned long int result  = suma(tope-1); 
           // for (unsigned lon int i=0; i<tope; i++)
        //       suma+= i; 
            print ("suma: %u\n\r",result );
            return 0; 
    }
)