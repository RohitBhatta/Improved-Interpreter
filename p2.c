#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <strings.h>

#define MISSING() do { \
	printf("Missing code at %s:%d\n",__FILE__,__LINE__); \
	exit(0); \
} while(0)

/* What is this? */
static jmp_buf escape;

//Global variables
char *token;
int idSize = 0;
int intSize = 0;
int first = 0;
int setCheck = 0;
int blockCheck = 0;
int elseCheck = 0;
int whileSetCheck = 0;
int whileBlock = 0;

struct Entry {
    struct Entry *next;
    char *name;
    int val;
};

struct Entry *table;

int get(char *id) {
    struct Entry *head = table;
    while (head != NULL) {
	if (strcmp(head -> name, id) == 0) {
	    return head -> val;
	}
	head = head -> next;
    }
    return 0;
}

void set(char *id, int value) {
    struct Entry *current = malloc(sizeof(struct Entry));
    if (first == 0) {
	table -> name = id;
	table -> val = value;
	table -> next = NULL;
	first++;
    }
    else {
	current = table;
	while (current -> next != NULL) {
	    current = current -> next;
	}
    }
    current -> next = malloc(sizeof(struct Entry));
    if (current -> next == NULL) {
	printf("Out of memory\n");
    }
    else {
	current = current -> next;
	current -> name = id;
	current -> val = value;
	current -> next = NULL;
	printf("%s", id);
	printf("%c", ':');
	printf("%d\n", value);
    }
}

static char *remaining() {
    return token;
}

static void error() {
    printf("error at '%s'\n", remaining());
    longjmp(escape, 1);
}

void consume(int length) {
    token += length;
    while (token[0] == ' ') {
	token++;
    }
}

int isWhile() {
    if (strncmp(token, "while", 5) == 0) {
	if (token[5] != 0 && ((token[5] >= 'a' && token[5] <= 'z') || 
	    (token[5] >= '0' && token[5] <= '9'))) {
		return 0;
	}
	else {
	    return 1;
	}
    }
    return 0;
}

int isIf() {
    if (strncmp(token, "if", 2) == 0) {
	if (token[2] != 0 && ((token[2] >= 'a' && token[2] <= 'z') || 
	    (token[2] >= '0' && token[2] <= '9'))) {
		return 0;
	}
	else {
	    return 1;
	}
    }
    return 0;
}

int isElse() {
    if (strncmp(token, "else", 4) == 0) {
	if (token[4] != 0 && ((token[4] >= 'a' && token[4] <= 'z') || 
	    (token[4] >= '0' && token[4] <= '9'))) {
		return 0;
	}
	else {
	    return 1;
	}
    }
    return 0;
}

int isSemi() {
    if (strncmp(token, ";", 1) == 0) {
	return 1;
    }
    return 0;
}

int isLeftBlock() {
    if (strncmp(token, "{", 1) == 0) {
	return 1;
    }
    return 0;
}

int isRightBlock() {
    if (strncmp(token, "}", 1) == 0) {
	return 1;
    }
    return 0;
}

int isEq() {
    if (strncmp(token, "=", 1) == 0) {
	return 1;
    }
    return 0;
}

int isEqEq() {
    if (strncmp(token, "==", 2) == 0) {
	return 1;
    }
    return 0;
}

int isLeft() {
    if (strncmp(token, "(", 1) == 0) {
	return 1;
    }
    return 0;
}

int isRight() {
    if (strncmp(token, ")", 1) == 0) {
	return 1;
    }
    return 0;
}

int isEnd() {
    if (token[0] == 0) {
	return 1;
    }
    return 0;
}

int isId() {
    idSize = 0;
    if (token[idSize] >= 'a' && token[idSize] <= 'z') {
	idSize++;
    }
    else {
	return 0;
    }
    while ((token[idSize] >= 'a' && token[idSize] <= 'z') || 
	(token[idSize] >= '0' && token[idSize] <= '9')) {
	idSize++;
    }
    if (idSize == 2) {
	if (token[0] == 'i' && token[1] == 'f') {
	    idSize = 0;
	    return 0;
	}
    }
    if (idSize == 4) {
	if (token[0] == 'e' && token[1] == 'l' && 
	    token[2] == 's' && token[3] == '4') {
	    idSize = 0;
	    return 0;
	}
    }
    if (idSize == 5) {
	if (token[0] == 'w' && token[1] == 'h' && token[2] == 'i' && 
	    token[3] == 'l' && token[4] == 'e') {
	    idSize = 0;
	    return 0;
	}
    }
    return 1;
}

int isMul() {
    if (strncmp(token, "*", 1) == 0) {
	return 1;
    }
    return 0;
}

int isPlus() {
    if (strncmp(token, "+", 1) == 0) {
	return 1;
    }
    return 0;
}

char *getId() {
    char *myId;
    myId = malloc(sizeof(char) * idSize);
    for (int i = 0; i < idSize; i++) {
	myId[i] = token[i];
    }
    return myId;
}

int isInt() {
    intSize = 0;
    if (token[intSize] >= '0' && token[intSize] <= '9') {
	intSize++;
    }
    else {
	return 0;
    }
    while ((token[intSize] >= '0' && token[intSize] <= '9') || 
	(token[intSize] == '_')) {
	intSize++;
    }
    return 1;
}

int getInt() {
    int myInt = token[0] - '0';
    for (int i = 1; i < intSize; i++) {
	if (token[i] >= '0' && token[i] <= '9') {
	    int temp = token[i] - '0';
	    myInt = myInt * 10 + temp;
	}
    }
    return myInt;
}

/* Forward declarations */
int expression();
void seq();

/* handle id, literals, and (...) */
int e1() {
    if (isLeft()) {
        consume(1);
        int v = expression();
        if (!isRight()) {
            error();
        }
        consume(1);
        return v;
    } else if (isInt()) {
        int v = getInt();
        consume(intSize);
        return v;
    } else if (isId()) {
        char *id = getId();
        consume(idSize);
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
        consume(1);
        value = value * e1();
    }
    return value;
}

/* handle '+' */
int e3() {
    int value = e2();
    while (isPlus()) {
        consume(1);
        value = value + e2();
    }
    return value;
}

/* handle '==' */
int e4() {
    int value = e3();
    while (isEqEq()) {
        consume(2);
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
	//printf("%c\n", 'i');
        consume(idSize);
        if (!isEq())
            error();
        consume(1);
        int v = expression();
	if (setCheck == 1 || elseCheck == 1) {

	}
	else {
	    set(id, v);
	}

        if (isSemi()) {
            consume(1);
        }

        return 1;
    } else if (isLeftBlock()) {
        consume(1);
	if (setCheck == 1) {
	    blockCheck = 1;
	}
        seq();
        if (!isRightBlock())
            error();
        consume(1);
	setCheck = 0;
	blockCheck = 0;
        return 1;
    } else if (isIf()) {
        consume(2);
        int ifCheck = expression();
	if (ifCheck == 1 && setCheck == 0) {
	    statement();
            if (isElse()) {
                consume(4);
            }
	    setCheck = 1;
	    statement();
	    setCheck = 0;
	}
	else if (setCheck == 1) {
	    statement();
	    if (isElse()) {
		consume(4);
		elseCheck = 1;
		statement();
		elseCheck = 0;
	    }
	}
	else {
	    setCheck = 1;
	    statement();
	    setCheck = 0;
	    if (isElse()) {
		consume(4);
		statement();
	    }
	}
        return 1;
    } else if (isWhile()) {
        /* Implement while */
	consume(5);
	int whileCheck = expression();
	//printf("%d\n", get("go"));
	//printf("%d\n", get("n"));
	//printf("%d\n", whileCheck);
	//printf("%c\n", 'a');
	if (setCheck == 0) {
	    while(whileCheck) {
		printf("%c\n", 'b');
		statement();
		whileCheck = expression();
	    }
	}
	else {
	    statement();
	}
	setCheck = 0;
        return 1;
    } else if (isSemi()) {
        consume(1);
	setCheck = 0;
	blockCheck = 0;
	elseCheck = 0;
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
    table = malloc(sizeof(struct Entry));
    int index = 0;
    while (prog[index] != 0) {
	index++;
    }
    int size = index;
    index = 0;
    token = malloc(sizeof(char) * size);
    token = prog;
    int x = setjmp(escape);
    if (x == 0) {
        program();
    }
}

int main(int argc, char *argv[]) {
    interpret(argv[1]);
    return 0;
}
