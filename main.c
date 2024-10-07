#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "streamGraph.h"
#include "parser.tab.h"

extern int yyparse();
extern  FILE *yyin;
extern Node *rootTree;

void createDotFileName(const char *inputFile, char *newFileName, size_t newFileNameSize);

int main(int argc, char *argv[]) {

    if (argc > 1)  {
        FILE *inputFile = fopen(argv[1], "r");

        if (inputFile)  {
            yyin = inputFile;
            yyparse();
            fclose(inputFile);

            char dotFileName[256];
            createDotFileName(argv[1], dotFileName, sizeof(dotFileName));

           // bool addedEnd = false;
           // addEndNode(rootTree, &addedEnd);
            createDotStreamFile(rootTree, dotFileName);
           // createDotFile(rootTree, dotFileName);
        }else {
            fprintf(stderr, "The file couldn't be opened: %s\n", argv[1]);
        }
    }else  {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    }

    cleanTree(rootTree);
    return 0;
}


void createDotFileName(const char *inputFile, char *newFileName, size_t newFileNameSize) {
    const char *dot = strrchr(inputFile, '.');

    if (dot == NULL) {
        snprintf(newFileName, newFileNameSize, "%s.dot", inputFile);

    } else {
        size_t baseNameLen = dot - inputFile;

        if (baseNameLen + 4 < newFileNameSize) {
            snprintf(newFileName, newFileNameSize, "%.*s.dot", (int)baseNameLen, inputFile);

        } else {
            fprintf(stderr, "Filename buffer too small\n");
            exit(EXIT_FAILURE);
        }
    }
}


