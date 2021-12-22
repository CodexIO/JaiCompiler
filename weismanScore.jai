//TODO:    THIS PROGRAM CURRENTLY ALLOCATES 30000 Anys... 
//CLEANUP: I hope we can cut this number down drastically...

main :: () {
    printf("Lets go!");
    testproc();
    printf("GREAT SUCCESS");
}

testproc :: () {
    defer printf("Print this at the end");

    count :: 5;

    for n: 0..count {
        squareInt :: (x: int) -> int { return x * x; }     
        printf("Hello " + n);
    }

    x := #char "a";
    y := #char "b";

    if x < y then printf("" + x + "is less than" + y);

    for 1..10 {
        if it == 3 then continue;
        if it == 5 then break;
        printf("It = " + it);
    }

    i := 1;
    while i < 2000 {
        printf("I: " + i);
        i = i + 1;
    }

    if !false printf("!false == true");

    factorial :: (x: int) -> int {
        if x <= 1 then return 1;
        return x * factorial(x - 1);
    }

    printf("Factorial of " + count + " = " + factorial(count));
}