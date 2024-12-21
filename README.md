# Introduction

Apparently [FizzBuzz](https://imranontech.com/2007/01/24/using-fizzbuzz-to-find-developers-who-grok-coding/) is a good ["weedout" programming question](https://blog.codinghorror.com/why-cant-programmers-program/) which was inspired from [Making Wrong Code Look Wrong](https://www.joelonsoftware.com/2005/05/11/making-wrong-code-look-wrong/)

> Write a program that prints the numbers from 1 to 100. But for multiples of three print “Fizz” instead of the number and for the multiples of five print “Buzz”. For numbers which are multiples of both three and five print “FizzBuzz”.

There is also a FizzBuzzBazz variation where we also have multiples of 7 print "Bazz".

You can find solutions for various [languages](http://wiki.c2.com/?FizzBuzzTest)

My solutions for FizzBuzz and FizzBuzzBazz follow:

# FizzBuzz

## FizzBuzz Naive -- Three Divides

The naive solution uses 3 divides.

```c
#include <stdio.h>

int main()
{
    for( int n = 1; n <= 100; ++n )
    {
             if ((n % 15) == 0) printf( "FizzBuzz\n" );
        else if ((n %  3) == 0) printf( "Fizz\n"     );
        else if ((n %  5) == 0) printf(     "Buzz\n" );
        else                    printf( "%d\n", n    );
    }

    return 0;
}

```

## FizzBuzz Array -- One Divide

We can get rid of all branches if we can use a single division (via Least Common Multiple) instead of three divisions.

```c
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

    for( int n = 1; n <= 100; ++n )
        printf( MOD15[ n % 15 ], n );

    return 0;
}

```

How/Why does this work?

In C the [Calling Convention](https://en.wikipedia.org/wiki/Calling_convention) is that the caller is responsible for stack cleanup, [cdecl](cdecl), so we can push a dummy integer on the stack _even if it isn't used._  See [x86 Calling Conventions](https://en.wikipedia.org/wiki/X86_calling_conventions) if you want the details.

```
        printf( "Fizz", 123 );
```

## FizzBuzz Linearization

If we are trying to minimize space we can linearize the nested if-else conditions into a linear array. We convert the if-else-thens into an binary number and thus we end up with an array of 4 elements.

```
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

```

## FizzBuzz Counter -- Zero Divides

We can optimize the division completely out:

```
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
```

## FizzBuzz 6502 Assembly

```assembly
; --- Consts ---        ; NOTE: Keep in sync T_* and aText
TYPE_FIZZ  EQU 0        ; offset 0
TYPE_FZBZ  EQU 5        ; offset 5
TYPE_BUZZ  EQU 9        ; offset 9
TYPE_NUM   EQU 128      ; Code uses BMI

; --- Zero Page Variables ---
zMod15  EQU $FD
zNum    EQU $FE

; --- ROM ---
PRBYTE  EQU $FDDA       ; Print A reg in HEX
COUT    EQU $FDED       ; Print character in A

; --- CODE ---
        ORG $300
        
        LDY #1
        STY zNum
Next    STY zMod15    

        LDY zMod15
        LDX aType,Y     ; zNum MOD 15 result type
        BMI PrintNum    ; TYPE_NUM is MINUS
        CPX #TYPE_FZBZ
        BNE PrintStr
        INC zMod15      ; Skip 16; code below clamps to zero, then +1
PrintStr
        LDA aText,X
        BEQ NextNumber
        JSR COUT
        INX
        BNE PrintStr    ; Always
PrintNum
        LDA zNum
        JSR PRBYTE
NextNumber
        LDA #" "        ; SPC high-bit
        JSR COUT

        INC zNum
        LDA zNum
        CMP #101
        BEQ Done

        LDA zMod15      ; mod 16
        AND #$0F        ; code above incr 15->16
        TAY             ; one byte shorter then CLC, ADC #$01
        INY
        BPL Next        ; Always, clamped to [00,0F] via AND above

; --- DATA ---        
Done
aType
        RTS
;       DB TYPE_FZBZ       ; [0] FIZZ BUZZ -- this is never referenced so we can overlay code + data
        DB TYPE_NUM        ; [1] *
        DB TYPE_NUM        ; [2] *
        DB TYPE_FIZZ       ; [3] FIZZ
        DB TYPE_NUM        ; [4] *
        DB TYPE_BUZZ       ; [5]     BUZZ
        DB TYPE_FIZZ       ; [6] FIZZ
        DB TYPE_NUM        ; [7] *
        DB TYPE_NUM        ; [8] *
        DB TYPE_FIZZ       ; [9] FIZZ
        DB TYPE_BUZZ       ;[10]     BUZZ
        DB TYPE_NUM        ;[11] *
        DB TYPE_FIZZ       ;[12] FIZZ
        DB TYPE_NUM        ;[13] *
        DB TYPE_NUM        ;[14] *
        DB TYPE_FZBZ       ;[15] FIZZBUZZ

aText                   ; Offset
        ASC "FIZZ",00   ; 0 TYPE_FIZZ
        ASC "FIZZ"      ; 5 TYPE_FZBZ, **intentional** fall into BUZZ
        ASC "BUZZ",00   ; 9 TYPE_BUZZ
```

## Expected FizzBuzz Output

```
1
2
Fizz
4
Buzz
Fizz
7
8
Fizz
Buzz
11
Fizz
13
14
FizzBuzz
16
17
Fizz
19
Buzz
Fizz
22
23
Fizz
Buzz
26
Fizz
28
29
FizzBuzz
31
32
Fizz
34
Buzz
Fizz
37
38
Fizz
Buzz
41
Fizz
43
44
FizzBuzz
46
47
Fizz
49
Buzz
Fizz
52
53
Fizz
Buzz
56
Fizz
58
59
FizzBuzz
61
62
Fizz
64
Buzz
Fizz
67
68
Fizz
Buzz
71
Fizz
73
74
FizzBuzz
76
77
Fizz
79
Buzz
Fizz
82
83
Fizz
Buzz
86
Fizz
88
89
FizzBuzz
91
92
Fizz
94
Buzz
Fizz
97
98
Fizz
Buzz
```


# FizzBuzzBazz

A varition of FizzBuzz is called FizzBuzzBazz which adds a _Bazz_ for every number divisible by 7.

## FizzBuzzBazz Linearization

It is trivial to extend the linearized version.  The array size is the next power of two, in this case we have three binary states:

* is mod 3
* is mod 5
* is mod 7

```c
#include <stdio.h>

int main()
{
    const char *MOD357[8] =
    {                      // 7 5 3
          "%d\n"           // 0 0 0
        , "Fizz\n"         // 0 0 1
        , "Buzz\n"         // 0 1 0
        , "FizzBuzz\n"     // 0 1 1
        , "Bazz\n"         // 1 0 0
        , "FizzBazz\n"     // 1 0 1
        , "BuzzBazz\n"     // 1 1 0
        , "FizzBuzzBazz\n" // 1 1 1

    };

    for( int n = 1; n <= 100; ++n )
    {
        int isMod3 = ((n % 3) == 0);
        int isMod5 = ((n % 5) == 0);
        int isMod7 = ((n % 7) == 0);
        int offset = isMod7*4 + isMod5*2 + isMod3; // linearize

        printf( MOD357[ offset ], n );
    }

    return 0;
}

```

## Expected FizzBuzzBazz Output

```
1
2
Fizz
4
Buzz
Fizz
Bazz
8
Fizz
Buzz
11
Fizz
13
Bazz
FizzBuzz
16
17
Fizz
19
Buzz
FizzBazz
22
23
Fizz
Buzz
26
Fizz
Bazz
29
FizzBuzz
31
32
Fizz
34
BuzzBazz
Fizz
37
38
Fizz
Buzz
41
FizzBazz
43
44
FizzBuzz
46
47
Fizz
Bazz
Buzz
Fizz
52
53
Fizz
Buzz
Bazz
Fizz
58
59
FizzBuzz
61
62
FizzBazz
64
Buzz
Fizz
67
68
Fizz
BuzzBazz
71
Fizz
73
74
FizzBuzz
76
Bazz
Fizz
79
Buzz
Fizz
82
83
FizzBazz
Buzz
86
Fizz
88
89
FizzBuzz
Bazz
92
Fizz
94
Buzz
Fizz
97
Bazz
Fizz
```

# Optimization

Write-up in-progress...

