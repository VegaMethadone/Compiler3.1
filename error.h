#ifndef ERROR_DEFINITION
#define ERROR_DEFINITION

void yyerror(const char* s);
int yylex (void);

#endif

/*
    myProject
        |
        | - error
        |     | - error.c
        |     | - error.h
        |
        |
        | - node
        |     | - node.c
        |     | - node.h
        |
        |
        | - parser
        |     | - parser.l
        |     | - parser.y
        |
        |
        | - main.c
        | - Makefile 
*/      