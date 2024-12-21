#include <stdio.h>

int main()
{
    const char *MOD35[4] =
    {                  // 5 3
          "%d\n"       // 0 0
        , "Fizz\n"     // 0 1
        , "Buzz\n"     // 1 0
        , "FizzBuzz\n" // 1 1
    };

    for( int n = 1; n <= 100; ++n )
    {
        int isMod3 = ((n % 3) == 0);
        int isMod5 = ((n % 5) == 0);
        int offset = isMod5*2 + isMod3;

        printf( MOD35[ offset ], n );
    }

    return 0;
}
