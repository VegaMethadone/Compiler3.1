#ifndef NODE_DEFINITION
#define NODE_DEFINITION

#include <stdio.h>  

typedef struct Node Node;
extern Node *rootTree;

struct Node
{
    char *type;
    Node *left;
    Node *right;
    char *value;
    int id;
};

Node *createNode(char  *type, Node  *left, Node *right, char *value);
void generateDot(Node *node, FILE *file);
void createDotFile(Node *root, char *filename);
void cleanTree(Node *node);

#endif