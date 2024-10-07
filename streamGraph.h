#ifndef STREAM_DEFINITION
#define STREAM_DEFINITION

#include <stdbool.h>
#include "node.h"
#include "slice.h"



typedef struct OperNode OperNode;
typedef struct Block Block;
typedef struct StreamGraph StreamGraph;
typedef struct DynamicArray DynamicArray;
typedef struct GraphArray GraphArray;


struct DynamicArray {
    int *data;        
    size_t size;      
    size_t capacity;
};

struct OperNode{
    char *op;
    char *type;
    char *value;
    OperNode *left;
    OperNode *right;
    int id;
};
 
struct  Block{
    char *type;
    Slice *value;
    Block *left;
    Block *right;
    OperNode *operations;
    int id;
};

struct GraphArray
{
    StreamGraph **data;
    int size;
};



struct StreamGraph{
    Block *start;
};

void initArray(DynamicArray *array, size_t initialCapacity);
void addElement(DynamicArray *array, int element);
bool contains(DynamicArray *array, int element);
void freeArray(DynamicArray *array);

OperNode  *newOperNode();

void addEndNode(Node *root, bool *placed);
Block *createNewBlock();
StreamGraph  *initStreamGraph();

void freeBlock(Block *block);
void freeStreamGraph(StreamGraph *graph);



bool findElseStatement(Node *node);
//Block  *parseTree(Node *node);
GraphArray *initGraphArray(int size);
void parseTree(Node *node, Block *block, Block *backdoor);

void printOperations(OperNode *node, FILE *file);
void generateDotStream(Block *block, FILE *file, DynamicArray *array);
void createDotStreamFile(Node *root, char *filename);


// Добавления дерево операций
void parseVarWithoutCall(Node *node, OperNode *parent, bool isLeft);
void parseIfStatement(Node *node, OperNode *parent, bool isLeft);
void parseFuncSignature(Node *node, OperNode *parent, bool isLeft);
void parseCallFunc(Node *node, OperNode *parent, bool isLeft);

// генерация asm file
char* generateASMFileName(char *filename);


#endif