#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "asmGeneration.h"

int streamIdCounter = 1;
int operationIdCounter = 1;


void initArray(DynamicArray *array, size_t initialCapacity) {
    array->data = malloc(initialCapacity * sizeof(int)); 
    array->size = 0;         
    array->capacity = initialCapacity; 
}


void addElement(DynamicArray *array, int element) {
    if (array->size == array->capacity) {
        array->capacity *= 2;
        array->data = realloc(array->data, array->capacity * sizeof(int));
    }
    array->data[array->size++] = element;
}


bool contains(DynamicArray *array, int element) {
    for (size_t i = 0; i < array->size; i++) {
        if (array->data[i] == element) {
            return true; 
        }
    }
    return false; 
}

void freeArray(DynamicArray *array) {
    free(array->data); 
    array->data = NULL; 
    array->size = 0; 
    array->capacity = 0;
}

























OperNode  *newOperNode() {
    OperNode  *newNode = malloc(sizeof(OperNode));

    newNode->left = NULL;
    newNode->right = NULL;

    newNode->type = "";
    newNode->value = "";
    newNode->op =  "";

    newNode->id = operationIdCounter;
    operationIdCounter++;

    return  newNode;
}




void addEndNode(Node *root, bool *placed)  {
    if (root == NULL || *placed) {
       // printf("return\n");
        return; 
    }
   // printf("current node id: %d type: %s\n", root->id, root->type);
    if (root->right == NULL && ((strcmp(root->type, "listStatement") == 0) || (strcmp(root->type, "sourceItem") == 0) || ((strcmp(root->type, "block") == 0) && root->left == NULL))) {
        Node *newNode = malloc(sizeof(Node));

        //newNode->id = -1;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->type = "end";
        newNode->value = "end";

        root->right = newNode;
        *placed = true;
       // printf("Found place, father node id: %d\n", root->id);
        return;
    } 
    

   
    //printf("go to  the right\n");
    addEndNode(root->right, placed);
    //printf("go to  the left\n");
    addEndNode(root->left, placed);
} 


Block *createNewBlock() {
    Block *newBlock = malloc(sizeof(Block));

    char *typeBuffer = malloc(1024 *sizeof(char));

    newBlock->type = typeBuffer;
    newBlock->value = initSlice(0);
    
    newBlock->left = NULL;
    newBlock->right =  NULL;

    newBlock->operations = newOperNode();

    newBlock->id = streamIdCounter;
    streamIdCounter++;

    return newBlock;
}

StreamGraph  *initStreamGraph() {
    StreamGraph *graph = malloc(sizeof(StreamGraph));
    Block *start = createNewBlock();
    
    start->type = "start";
    start->value = appendSlice(start->value, "start");

    graph->start = start;

    return graph;
}

void freeBlock(Block *block) {
    if (block == NULL) {
        return;
    }

    if (block->type != NULL) {
        free(block->type);
    }
    if (block->value != NULL) {
        freeSlice(block->value);
    }

    freeBlock(block->left);
    freeBlock(block->right);

    free(block);
}

void freeStreamGraph(StreamGraph *graph) {
    if (graph == NULL) {
        return;
    }
    freeBlock(graph->start);

    free(graph);
}






bool findElseStatement(Node *node) {
    if ((node == NULL) || (strcmp(node->type, "if") == 0)){
        return  false;
    }
    if (strcmp(node->type, "else")  == 0) {
        return true;
    }
    
    bool left  = findElseStatement(node->left);
    bool right = findElseStatement(node->right);

    return left || right;
}



//////////////////////////////////////////





void parseVarWithoutCall(Node *node, OperNode *parent, bool isLeft) {
    if (node == NULL)
        return;

    OperNode *newOper = NULL;
    if (strcmp(node->type, "var") == 0){
        newOper = newOperNode();


        if (strcmp(node->left->value, "return") == 0)  {
            parent->op = "ASM_Return";
            newOper->op = "ASM_Type_Return";
        }else {
            newOper->op = "ASM_Type";
            parent->op = "ASM_INIT";
        }

        newOper->value = node->left->value;
        parent->left = newOper;


        parseVarWithoutCall(node->right, parent, false);

    }else if (strcmp(node->type, "assignment") == 0) {
        newOper = newOperNode();
        newOper->op = "ASM_Write";
        
        parent->right = newOper;

        OperNode *identNode = newOperNode();
        identNode->op = "ASM_Ident";
        identNode->value = node->left->value;

        newOper->left = identNode;

        parseVarWithoutCall(node->right, newOper, false);        
    }else if (
        (strcmp(node->type, "TIMES") == 0)  ||
        (strcmp(node->type, "MINUS") == 0)  ||
        (strcmp(node->type, "PLUS") == 0)  ||
        (strcmp(node->type, "DIVIDE") == 0)
    )  {
        newOper = newOperNode();
        newOper->op = node->type;

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;
        
        parseVarWithoutCall(node->left, newOper, true);
        parseVarWithoutCall(node->right, newOper, false);
    }else if  ( (strcmp(node->type, "INT") == 0) || (strcmp(node->type, "STR") == 0) || (strcmp(node->type, "IDENTIFIER") == 0)) {
        newOper = newOperNode();
        newOper->op = "ASM_Value";
        newOper->type = node->type;
        newOper->value = node->value;

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;
        
        return;   
    }
    // test
    else if (strcmp(node->type, "CALL") == 0)  {
        newOper = newOperNode();
        newOper->op = "ASM_PopStack";

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;
        
        return;
    }
    //
    else {
        parseVarWithoutCall(node->left, parent, isLeft);
        parseVarWithoutCall(node->right, parent, isLeft);
    }

}


void parseIfStatement(Node *node, OperNode *parent, bool isLeft) {
    if (node ==  NULL)
        return;
    
    OperNode *newOper = NULL;
    if ((strcmp(node->type, "if") == 0) || (strcmp(node->type, "while") == 0)) {
        node = node->left;
        parent->op = node->type;

        parseIfStatement(node->left,  parent, true);
        parseIfStatement(node->right, parent, false);

    }else if  ( (strcmp(node->type, "INT") == 0) || (strcmp(node->type, "STR") == 0) || (strcmp(node->type, "IDENTIFIER") == 0)) {
        newOper = newOperNode();
        newOper->op = "ASM_Value";
        newOper->type = node->type;
        newOper->value = node->value;

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;
        
        return;
    }else if (strcmp(node->type, "dowhile") == 0) {
        node = node->right;
        parent->op = node->type;

        parseIfStatement(node->left,  parent, true);
        parseIfStatement(node->right, parent, false);
    }
}


void parseFuncSignature(Node *node, OperNode *parent, bool isLeft) {
    if (node == NULL)
        return;
    
    OperNode *newOper = NULL;
    if (strcmp(node->type, "funcSignature") == 0) {
        parent->op = "ASM_Func";
        newOper =  newOperNode();
        newOper->op = "ASM_FuncDef";
        parent->left = newOper;

        OperNode *returnType = newOperNode();
        OperNode *funcName = newOperNode();

        newOper->left = returnType;
        newOper->right = funcName;


        returnType->value = node->left->left->value;
        returnType->op = "ASM_Value";

        funcName->value = node->left->right->value;
        funcName->op = "ASM_Ident";

        parseFuncSignature(node->right, parent, false);
    }else if (strcmp(node->type, "listArgDef") == 0) {
        newOper = newOperNode();
        newOper->op = "ASM_FuncArg";

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;

        parseFuncSignature(node->left, newOper, true);
        parseFuncSignature(node->right, newOper, false);
    }else if (strcmp(node->type, "argDef") == 0) {
        newOper = newOperNode();
        newOper->op = "ASM_ArgDef";

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;

        parseFuncSignature(node->left, newOper, true);
        parseFuncSignature(node->right, newOper, false); 
    }else if  ( (strcmp(node->type, "TYPEDEF") == 0) || (strcmp(node->type, "IDENTIFIER") == 0)) {
        newOper = newOperNode();
        newOper->op = "ASM_Value";
        newOper->type = node->type;
        newOper->value = node->value;

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;
        
        return; 
    }
}



void parseCallFunc(Node *node, OperNode *parent, bool isLeft) {
    if (node == NULL)
        return;

    OperNode *newOper = NULL;
    if (strcmp(node->type, "CALL") == 0) {
        parent->op = "ASM_CALL_Func";

        newOper = newOperNode();
        newOper->op = "ASM_FuncName";
        newOper->value = node->left->value;

        parent->left = newOper;

        parseCallFunc(node->right, parent, false);
    }else if (strcmp(node->type, "listExpr") == 0) {
        newOper = newOperNode();
        newOper->op = "ASM_Arg";
        newOper->value = node->left->value;
        //  добавил тип данных для ассемблера
        newOper->type = node->left->type;

        if (isLeft)
            parent->left = newOper;
        else
            parent->right = newOper;

        parseCallFunc(node->right,newOper, true);
    }else {
        parseCallFunc(node->left, parent, isLeft);
        parseCallFunc(node->right, parent, isLeft);
    }
}

//////////////////////////////////////////





bool findCall(Node *node) {
    if (node == NULL)
        return false;
    if (strcmp(node->type, "CALL") == 0)
        return true;
    
    bool left = findCall(node->left);
    bool right = findCall(node->right);

    return left || right;    
}




void parseTree(Node *node, Block *block, Block *backdoor) {
    if (node ==  NULL) {
        //printf("Return\n");
        return;
    }
    //printf("current node: %d\n", node->id);

    if (strcmp(node->type, "end") == 0) {
        Block *endBlock = createNewBlock();

        endBlock->type = "end"; 

        block->left = endBlock;
        //printf("Reached the end\n");
        return;
    }

    Block *newBlock = NULL;
    

    if (strcmp(node->type, "listStatement") == 0) {
        newBlock = createNewBlock();

        block->left = newBlock;
        newBlock->type =  strdup("ASM_Statement");
        newBlock->id = node->id;
        //printf("Created node %d, type:%s\n", newBlock->id, newBlock->type);
        //  паршу дерево
        parseTree(node->right, newBlock, backdoor);

        if (node->right == NULL) {
           newBlock->left = backdoor;
        }

        if (strcmp(node->left->type, "CALL") == 0)  {
            newBlock->type =  "ASM_CALL";
            parseCallFunc(node->left, newBlock->operations, true);
        }else  if (strcmp(node->left->type, "assignment") == 0) {
            newBlock->type = "ASM_Assignment";
        }


        // обрабатываю   левое поддерево стейтмента        
        parseTree(node->left, newBlock, backdoor);
        

    }else if (strcmp(node->type, "var") == 0) {
        block->type = strdup("ASM_Var");
        //  Если нашел  присвание результата из функции переменной
        if (findCall(node->right)) {
            block->type = "ASM_Var_CALL";

            newBlock = createNewBlock();
            newBlock->type = "ASM_Var";
            newBlock->left = block->left;
            block->left = newBlock;
            
            // паршу функцию
            parseCallFunc(node->right->left->right, block->operations, true);

            // паршу  переменную
            parseVarWithoutCall(node, newBlock->operations, true);

            return;

        }else {
            // просто делаю дерево операций
            // тут надо обработать еще return
            if (strcmp(node->left->value, "return") == 0)
                block->type = "ASM_Return";

            parseVarWithoutCall(node, block->operations, false);
        }

        return;
    }else if (strcmp(node->type, "funcSignature") == 0) {
        newBlock =  createNewBlock();
        
        Block *tmp = block->left;
        block->left = newBlock;
        newBlock->left  = tmp;

        newBlock->type = strdup("ASM_funcSignature");
        //newBlock->id = node->id;

        parseFuncSignature(node, newBlock->operations, true);

    }else if (strcmp(node->type, "dowhile") == 0) {
        block->type = strdup("ASM_Dowhile");

        newBlock = createNewBlock();
        newBlock->type = "ASM_END_Dowhile";
        newBlock->left = block->left;
        newBlock->right = block;
        block->left = NULL;

        parseTree(node->left, block, newBlock);

        parseIfStatement(node, newBlock->operations,  true);

    }else if (strcmp(node->type, "if") == 0) {
        if (findElseStatement(node->right)) {
            block->type = strdup("ASM_Ifelse");

            newBlock = createNewBlock();
            newBlock->type = "ASM_END_IfElse";
            newBlock->left = block->left;

            Block *ifBlock = createNewBlock();
            Block *elseBlock = createNewBlock();
            ifBlock->type = "ASM_ifelseif";
            elseBlock->type = "ASM_ifelseelse";

            block->left = ifBlock;
            block->right = elseBlock;

            parseTree(node->right->left, ifBlock, newBlock);
            parseTree(node->right->right, elseBlock, newBlock);

        }else {
            block->type = strdup("ASM_If");
            // тестик
            newBlock = createNewBlock();
            newBlock->type = "ASM_END_If";

            newBlock->left = block->left;
            block->left = NULL;
            block->right = newBlock;
            
            
            
            // не забыть в listStatement  проверить блок  на if
            // в  старой  версии было parseTree(node->right,  block, block->left);
            parseTree(node->right, block, newBlock);

        }
        parseIfStatement(node, block->operations, true);
        return;
    }else if (strcmp(node->type, "while") == 0) {
        block->type = strdup("ASM_while");

        newBlock = createNewBlock();
        newBlock->type = "ASM_END_While";
        newBlock->left = block->left;
        newBlock->right = block;

        parseTree(node->right, block, newBlock);

        parseIfStatement(node, block->operations, true);
        return;
    }else if ((strcmp(node->type, "assignment") == 0)) {
        parseVarWithoutCall(node, block->operations, true);
    }
   /*
        Добавил блок для импортируемых функций
    */

    else {
       // printf("Go to the right node\n");
        parseTree(node->right, block, backdoor);
       // printf("Go to the left node\n");
        parseTree(node->left, block,  backdoor);
    }

}



void generateDotStream(Block *block, FILE *file, DynamicArray *array) {
    if (block == NULL || contains(array, block->id)) {
        return;
    }

    addElement(array, block->id);

    fprintf(file, "  \"%d | %s\" [shape=ellipse];\n",
            block->id, block->type);

    if (block->left) {
        fprintf(file, "  \"%d | %s\" -> \"%d | %s\" [label=\" L \"];\n",
                block->id, block->type,
                block->left->id, block->left->type);
        generateDotStream(block->left, file, array);
    }

    if (block->right) {
        fprintf(file, "  \"%d | %s\" -> \"%d | %s\" [label=\" R \"];\n",
                block->id, block->type,
                block->right->id, block->right->type);
        generateDotStream(block->right, file, array);
    }

    
    if ( 
        (strcmp(block->type, "end") == 0)  ||
        (strcmp(block->type, "start") == 0) ||
        (strcmp(block->type, "ASM_Dowhile") == 0) ||
        //(strcmp(block->type, "ASM_END_Dowhile") == 0)  ||
        (strcmp(block->type, "ASM_END_If") == 0) ||
        (strcmp(block->type, "ASM_END_While") == 0) ||
        (strcmp(block->type, "ASM_END_IfElse") == 0) ||
        (strcmp(block->type, "ASM_ifelseif") == 0) ||
        (strcmp(block->type, "ASM_ifelseelse") == 0)
        ) {
            return;
        }

    if (block->operations) {
        fprintf(file, "  \"%d | %s\" -> \"%d | %s\\n%s\\n%s\"[shape=box];\n",
                block->id, block->type,
                block->operations->id, block->operations->op,  block->operations->type, block->operations->value);
        printOperations(block->operations, file);
    }
}

void printOperations(OperNode *node, FILE *file) {
    if (node == NULL) {
        return;
    }

    // Печать узла
    fprintf(file, "  \"%d | %s\\n%s\\n%s\" [shape=box];\n",
            node->id, node->op, node->type, node->value);

    // Печать левого поддерева
    if (node->left) {
        fprintf(file, "  \"%d | %s\\n%s\\n%s\" -> \"%d | %s\\n%s\\n%s\" [label=\" L \"];\n",
                node->id, node->op, node->type, node->value,
                node->left->id, node->left->op, node->left->type, node->left->value);
        printOperations(node->left, file);
    }

    // Печать правого поддерева
    if (node->right) {
        fprintf(file, "  \"%d | %s\\n%s\\n%s\" -> \"%d | %s\\n%s\\n%s\" [label=\" R \"];\n",
                node->id, node->op, node->type, node->value,
                node->right->id, node->right->op, node->right->type, node->right->value);
        printOperations(node->right, file);
    }
}


GraphArray *initGraphArray(int size) {
    GraphArray *array = malloc(sizeof(GraphArray));

    array->data = malloc(size * sizeof(StreamGraph));
    array->size = size;

    return array;
}


char *generateASMFileName(char *filename) {
    char *newFile = (char *)malloc(256 * sizeof(char)); // Выделение памяти для нового имени файла
    if (newFile == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (filename[i] != '\0') {
        if (filename[i] == 'd')
            break;
        newFile[i] = filename[i];
        i++;
    }

    newFile[i++] = 's';
    newFile[i] = '\0';

    return newFile; // Возвращаем указатель на выделенную память
}



void createDotStreamFile(Node *root, char *filename) {
    if (root == NULL){
        fprintf(stderr, "Root node in NULL\n");
        exit(EXIT_FAILURE);
    }
    
    FILE *file = fopen(filename,  "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }





    Node *current = root;
    int size = 0;
    while(current) {
        // Тестовый блок исключения  внешней функции
        if (current->right->right ==  NULL) {
            current = current->left;
            continue;
        }

        size++;
        printf("Node id: %d, type: %s, size: %d\n", current->id, current->type, size);
        bool addedEnd = false;
        addEndNode(current, &addedEnd);
        current = current->left;
    }
    // дебаг размера дерева
    printf("SIZE OF TREE: %d\n", size);

    GraphArray *testGraph = initGraphArray(size);
    current = root;
    for (int i = 0; i < size; i++) {
        printf("Parsee Tree Node id: %d, type: %s, size: %d\n", current->id, current->type, size);
        if (current ==  NULL)
            return;
        
        //  если функция - внешняя, то пропустить 
        if (current->right->right->id == 0) {
            printf("Block type %s and id:%d\n", current->right->right->type, current->right->right->id);
            current = current->left;
            continue;
        }
        

        StreamGraph *graph = malloc(sizeof(graph));
        graph = initStreamGraph();
        testGraph->data[i] = graph;
        
        //////////////////////////////////////////
        parseTree(current->right,  graph->start, NULL);
        

        current = current->left;
    }

    fprintf(file, "digraph G {\n");
    fprintf(file, "    node [shape=box];\n");    

    for (int  i = 0; i < size;  i++) {
        DynamicArray array;
        initArray(&array, 2);
        StreamGraph *current = testGraph->data[i];
        generateDotStream(current->start, file, &array);
    }

    fprintf(file, "}\n");
    fclose(file);





    // работаю с генерацией ASM
    char *newAsmFile = generateASMFileName(filename);
    printf("%s\n", newAsmFile);


    FILE *asmfile = fopen(newAsmFile,  "w");
    if (asmfile == NULL) {
        fprintf(stderr, "Error opening file %s\n", newAsmFile);
        exit(EXIT_FAILURE);
    }

    // устанавливаю глобальную переменную для output
    initFileOutput(asmfile);
    // Массив для хранения регистров
    initRegArray();
    
    for (int i = 0; i < size; i++) {
        // массив для хранения данных относительно sbp
        OffsetArray *array = initOffsetArray(1024);

        // массив для хранения меток
        MarkArray *markArray = initMarkArray(1024);


        StreamGraph *current = testGraph->data[i];
        // запуск генерации asm файла
        parseControlFlowGraph(current->start, i, array, markArray, false);
    }



    fclose(asmfile);
    /*

    for (int i = size - 1; i >= 0; i--) {
        StreamGraph *current = testGraph->data[i];
    }



    generateASMFileName(filename);
    FILE *asmfile = fopen(filename,  "w");
    if (asmfile == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
        exit(EXIT_FAILURE);
    }





    fclose(asmfile);

    */
    /*
        Что я хочу сделать:
        отсортировать все функции в обратном порядке, чтобы main была обработана в самом конце
        редактирую название файла  c .dot  на .s
        открываю его fopen
        вызываю функцию для присвоению глоабльной переменной этот файл
        дальше в файл прописываю пролог asm
        дальше запускаю функцию по  генерации ассемблера
    
    */
}

