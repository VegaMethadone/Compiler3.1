#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"


Node *rootTree = NULL;
int idCounter = 1;

Node *createNode(char  *type, Node  *left, Node *right, char *value) {
    Node *newNode = malloc(sizeof(Node));

    newNode->type = type;
    newNode->left = left;
    newNode->right = right;
    newNode->id = idCounter;

    idCounter++;

    char  *buffer =  malloc(1024 * sizeof(char));
    strcpy(buffer, value);
    newNode->value = buffer;

    if (strcmp(value, "r") == 0) {
        rootTree = newNode;
    }

    return  newNode; 
}

/*
void generateDot(Node *node, FILE *file) {
    if (node == NULL)
        return;

    if (node->left) {
        fprintf(file, "    \"%s\\n%s\" -> \"%s\\n%s\";\n",
                node->type, node->value,
                node->left->type, node->left->value);
        generateDot(node->left, file);
    }

    if (node->right) {
        fprintf(file, "    \"%s\\n%s\" -> \"%s\\n%s\";\n",
                node->type, node->value,
                node->right->type, node->right->value);
        generateDot(node->right, file);
    }
}
*/

void generateDot(Node *node, FILE *file) {
    if (node == NULL)
        return;

    if (node->left) {
        fprintf(file, "    \"%s\\n%s\\nID: %d\" -> \"%s\\n%s\\nID: %d\";\n",
                node->type, node->value, node->id,
                node->left->type, node->left->value, node->left->id);
        generateDot(node->left, file);
    }

    if (node->right) {
        fprintf(file, "    \"%s\\n%s\\nID: %d\" -> \"%s\\n%s\\nID: %d\";\n",
                node->type, node->value, node->id,
                node->right->type, node->right->value, node->right->id);
        generateDot(node->right, file);
    }
}

void createDotFile(Node *root, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "digraph G {\n");
    fprintf(file, "    node [shape=box];\n");

    generateDot(root, file);

    fprintf(file, "}\n");

    fclose(file);
}


void cleanTree(Node *node) {
    if (node == NULL)
        return;
    cleanTree(node->left);
    cleanTree(node->right);

    free(node->type);
    free(node->value);
    free(node);
}
