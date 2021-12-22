//#load "basic.jai";

a := 3 + 5 * 4;

e : bool = 3 <= 5 || true;

f : double = 5 / (2 + 3);

b : string;

c : float = square("hi");

d :: true;

alpha :: "a";
beta : string = "b";

Vector2 :: struct {
    x: float = 0;
    y: float = 0;
}

Color :: enum {
    BLACK, WHITE, ORANGE, BLUE
}

square :: (x: float) -> float {
    return x * x;
}

main :: () {
    foo :: 42;
    if true then printf("True");
    else printf("False");
}

/*
expression     → logicOr ;
logicOr        → logicAnd ( || logicAnd)* ;
logicAnd       → equality ( && equality)* ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary | call ;
call           → primary ( "(" arguments? ")" )* ;
primary        → NUMBER | STRING | "true" | "false" | "null" | "(" expression ")" ;

arguments      → expression ( "," expression )* ;

*/