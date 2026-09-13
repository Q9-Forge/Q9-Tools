#include <stdio.h>

// ISO C style - modern function declarations
int add(int a, int b)
{
    return a + b;
}

int multiply(int x, int y)
{
    int result = x * y;
    return result;
}

void printHello(const char *name)
{
    printf("Hello, %s!\n", name);
}

int main(int argc, char *argv[])
{
    int sum = add(5, 3);
    int product = multiply(4, 7);

    printf("Sum: %d\n", sum);
    printf("Product: %d\n", product);
    printHello("World");

    return 0;
}
