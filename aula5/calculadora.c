#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    int i;

    if(argc<2)
    {
        printf("ERROR: Argumentos insuficientes.\n");
        return EXIT_FAILURE;
    }

    double num1,num2,res;
    num1=atof(argv[1]);
    num2=atof(argv[3]);
    char op;
    op=argv[2][0];

        printf("Conta: %.2f %s %f.2.\n",num1,argv[2],num2);

        switch(op){
            case '+':       res=num1+num2;
                            break;
            
            case '-':       res=num1-num2;
                            break;
            
            case 'x':       res=num1*num2;
                            break;
            
            case '/':       res=num1/num2;
                            break;
            
            case 'p':       res=pow(num1,num2);
                            break;
            }

        printf("\n%.2f \n",res);

    return EXIT_SUCCESS;
}