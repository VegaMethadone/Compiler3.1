#ifndef ASM_GENERATION
#define ASM_GENERATION

#include "streamGraph.h"

typedef struct Place Place;
typedef struct OffsetArray OffsetArray;
typedef struct MarkArray MarkArray;
typedef struct Pair Pair;
typedef struct RegArray RegArray;

//  массив для хранения смещений
struct Place {
    char *identifier;
    int offset;  
};
struct OffsetArray {
    Place *data;
    int size;
    int cap;
    int currentOffset;
};

// массив для хранения меток
struct MarkArray {
    char **mark;
    int  size;
    int cap;
};


// массив для работы с регистрами
struct Pair {
    char *reg;
    bool used;
};
struct RegArray  {
    Pair **data;
    int size;
    int current;
};


void initFileOutput(FILE *file);


void parseControlFlowGraph(Block *block, int curNum, OffsetArray *array, MarkArray *markArray, bool ifelse);

//  Функции для генерации листинга asm
void ASM_FuncSignature(OperNode *node, OffsetArray *array);
void ASM_Var(OperNode *node, OffsetArray *array);
void ASM_Assignment(OperNode *node, OffsetArray *array);
void ASM_Condition(OperNode *node, OffsetArray *array, MarkArray *markArray, char *option);
void ASM_Call(OperNode *node, OffsetArray *array);
void ASM_Return(OperNode *node, OffsetArray *array);

// Хранения переменных со смещениями относительно sbp
OffsetArray *initOffsetArray(int capacity);
void addOffsetArray(OffsetArray *array, const char *identifier, int offset);
int containsOffsetArray(OffsetArray *array, const char *value);
void ASMfreeArray(OffsetArray *array);

// Массив для хранения меток
MarkArray *initMarkArray(int cap);
void pushMarkArray(MarkArray *array, char *value);
//void popMarkArray(MarkArray *array, char **place);
char *popMarkArray(MarkArray *array);
void freeMarkArray(MarkArray *array);

// Массив для хранения регистров
Pair *getNewPair(char *reg);
void initRegArray();
void cleanRegArray(RegArray *array);
void freeRegArray(RegArray *array);
void checkRegArray();


#endif