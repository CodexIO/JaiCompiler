#load "basic.jai";

/*  This
    is
    a
    multiline
    comment
*/

// This is a Comment

// TODO Support This
// alpha : string : "";


beta :: "b";
another :: "c";



a := 3;

b : string = "Hello Sailor!";

c :: 3.0;

d: bool = true;

e: bool = false;

p: *int;

// TODO MAKE THIS WORK
// p = *a;

a := square(c);
b := square(c) + 1;

a1 := 14;
a2 : float = a1 + 1;

square :: (x: float) -> float {
    return x * x;
}

multiply :: (x: float, y: float) -> float {
    return x * y;
}

Vector2 :: struct {
    x: float = 0;
    y: float = 0;
}

Color :: enum {
    BLACK, WHITE, ORANGE, BLUE
}

enumExample :: () {
    // TODO: MAKE THIS WORK!
    //e: Color;
    //e = Color.BLACK;
}

main :: () {
    printf("Lets go!");

    //enumExample();
    
    vectorExample();

    testproc();
    
    printf("GREAT SUCCESS");
}

vectorExample :: () {
    printVec :: (abc : Vector2) {
        printf("X: " + abc.x + " Y: " + abc.y);
    }

    v : Vector2;
    v.x = 5;
    v.y = 4;

    printVec(v);

    z: Vector2;
    z.y = 5;
    printVec(z);

    k: Vector2;
    k = addVec(v, z);

    printVec(k);

    printVec(addVec(k, k));

    addVec :: (a: Vector2, b: Vector2) -> Vector2 {
        result: Vector2;
        result.x = a.x + b.x;
        result.y = a.y + b.y;
        return result;
    }
}

testproc :: () {
    defer printf("Print this at the end");

    count :: 5;

    intermediates : int[count];

    for n: 0..count {
        squareInt :: (x: int) -> int { return x * x; }     

        intermediates[n] = squareInt(n);
    }

    x := #char "a";
    y := #char "b";

    if x < y then printf("" + x + " < " + y);

    for 1..10 {
        if it == 5 then break;
        if it == 3 then continue;

        printf("Indermediates[" + it + "] = " +  intermediates[it]);
    }

    i := 1;
    while i < 20 {
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

/*
    Multi-
    /* Line 
    Block- are not yet supported */
//    Comment TODO SUPPORT THIS
// */

testProc2 :: () {
    x := #char "a";
    y := #char "b";

    /*for it: intermediates {
    printf("Test2: " + x + "\n");
    }*/
    count :: 5;

    // Static array of ints:
    //intermediates : int[count];

    // Dynamic array of ints:
    // results : int []; // TODO We don't do that here

    for 0..count {
        //intermediates[n] = squareInt(n);
    }

    localFunction :: (x: int) { printf("Test:" + x + "\n"); }
}