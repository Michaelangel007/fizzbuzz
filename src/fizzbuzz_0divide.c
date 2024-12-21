#include <stdio.h>

int main()
{
    const char *MOD15[15] =
    {                  //  %3  %5  %15
          "FizzBuzz\n" //   0   0    0
        , "%d\n"       //            1
        , "%d\n"       //            2
        , "Fizz\n"     //   0        3
        , "%d\n"       //            4
        , "Buzz\n"     //       0    5
        , "Fizz\n"     //   0        6
        , "%d\n"       //            7
        , "%d\n"       //            8
        , "Fizz\n"     //   0        9
        , "Buzz\n"     //       0   10
        , "%d\n"       //           11
        , "Fizz\n"     //   0       12
        , "%d\n"       //           13
        , "%d\n"       //           14
    };

    int mod15 = 1;
    for( int n = 1; n <= 100; ++n )
    {
        printf( MOD15[ mod15 ], n );
        mod15++;
        if (mod15 == 0xF) mod15++; // mod 15 -> mod 16 == 0
        mod15 &= 0xF;              // mod 16
    }

    return 0;
}