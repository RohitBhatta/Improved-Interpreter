#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#define MISSING() do { \
    printf("Missing code at %s:%d\n",__FILE__,__LINE__); \
    exit(0); \
} while(0)

/* What is this? */
static jmp_buf escape;

int get(char *id) {
    MISSING();
    return 0;
}

void set(char *id, int value) {
    MISSING();
}

static char *remaining() {
    MISSING();
    return 0;
}

static void error() {
    printf("error at '%s'\n", remaining());
    longjmp(escape, 1);
}

void consume() {
    MISSING();
}

int isWhile() {
    MISSING();
    return 0;
}

int isIf() {
    MISSING();
    return 0;
}

int isElse() {
    MISSING();
    return 0;
}

int isSemi() {
    MISSING();
    return 0;
}

int isLeftBlock() {
    MISSING();
    return 0;
}

int isRightBlock() {
    MISSING();
    return 0;
}

int isEq() {
    MISSING();
    return 0;
}

int isEqEq() {
    MISSING();
    return 0;
}

int isLeft() {
    MISSING();
    return 0;
}

int isRight() {
    MISSING();
    return 0;
}

int isEnd() {
    MISSING();
    return 0;
}

int isId() {
    MISSING();
    return 0;
}

int isMul() {
    MISSING();
    return 0;
}

int isPlus() {
    MISSING();
    return 0;
}

char *getId() {
    MISSING();
    return 0;
}

int isInt() {
    MISSING();
    return 0;
}

int getInt() {
    MISSING();
    return 0;
}

/* Forward declarations */
int expression();
void seq();

/* handle id, literals, and (...) */
int e1() {
    if (isLeft()) {
        consume();
        int v = expression();
        if (!isRight()) {
            error();
        }
        consume();
        return v;
    } else if (isInt()) {
        int v = getInt();
        consume();
        return v;
    } else if (isId()) {
        char *id = getId();
        consume();
        return get(id);
    } else {
        error();
        return 0;
    }
}

/* handle '*' */
int e2() {
    int value = e1();
    while (isMul()) {
        consume();
        value = value * e1();
    }
    return value;
}

/* handle '+' */
int e3() {
    int value = e2();
    while (isPlus()) {
        consume();
        value = value + e2();
    }
    return value;
}

/* handle '==' */
int e4() {
    int value = e3();
    while (isEqEq()) {
        consume();
        value = value == e3();
    }
    return value;
}

int expression() {
    return e4();
}

int statement() {
    if (isId()) {
        char *id = getId();
        consume();
        if (!isEq())
            error();
        consume();
        int v = expression();
        set(id, v);

        if (isSemi()) {
            consume();
        }

        return 1;
    } else if (isLeftBlock()) {
        consume();
        seq();
        if (!isRightBlock())
            error();
        consume();
        return 1;
    } else if (isIf()) {
        consume();
        /*int c =*/ expression();
        statement();
        if (isElse()) {
            consume();
            statement();
        }
        return 1;
    } else if (isWhile()) {
        /* Implement while */
        MISSING();
        return 1;
    } else if (isSemi()) {
        consume();
        return 1;
    } else {
        return 0;
    }
}

void seq() {
    while (statement()) ;
}

void program() {
    seq(1);
    if (!isEnd())
        error();
}

void interpret(char *prog) {
    /* initialize global variables */
    MISSING();
    int x = setjmp(escape);
    if (x == 0) {
        program();
    }
}

int main(int argc, char *argv[]) {
    interpret(argv[1]);
    return 0;
}
