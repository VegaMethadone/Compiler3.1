#include "asmGeneration.h"

// #define DEBUG_ASM_FUNC
// #define DEBUG_ASM_VAR
// #define DEBUG_ASM_ASSIGN
// #define DEBUG_ASM_CALL
// #define DEBUG_MARK_ARRAY

FILE *out = NULL;
RegArray *regArray = NULL;


// Определение мнемоник
#define mnemonic_0(mnemonic) \
    fprintf(out, "\t%s\n", mnemonic);

#define mnemonic_call(mnemonic, name) \
    fprintf(out, "\t%s %s\n", mnemonic, name);

#define mnemonic_1(mnemonic, arg1) \
    fprintf(out, "\t%s %%%s\n", mnemonic, arg1);

#define mnemonic_2(mnemonic, arg1, arg2) \
    fprintf(out, "\t%s %%%s, %%%s\n", mnemonic, arg1, arg2);

#define mnemonic_2_int(mnemonic, arg1, arg2) \
    fprintf(out, "\t%s $%s, %%%s\n", mnemonic, arg1, arg2);

#define mnemonic_2_int_int(mnemonic, arg1, arg2) \
    fprintf(out, "\t%s $%d, %%%s\n", mnemonic, arg1, arg2);

#define mnemonic_2_set(mnemonic, arg1, offset, arg2) \
    fprintf(out, "\t%s %%%s, -%d(%%%s)\n", mnemonic, arg1, offset, arg2);


// Определение операций
#define mov(from, to) mnemonic_2("movq", from, to)
#define mov_i(what, to) mnemonic_2_int("movq", what, to)
#define push(reg) mnemonic_1("pushq", reg)
#define pop(reg) mnemonic_1("popq", reg)
#define add(from, to) mnemonic_2("addq", from, to)
#define add_i(what, to) mnemonic_2_int("addq", what, to)
#define sub(from, to) mnemonic_2("subq", from, to)
#define sub_i(what, to) mnemonic_2_int("subq", what, to)
#define sub_i_i(what, to) mnemonic_2_int_int("subq",  what, to)
#define mul(what) mnemonic_1("imulq", what)
#define div(what) mnemonic_1("divq", what)
#define rem(from, to) mnemonic_2("remq", from, to)
#define _and(from, to) mnemonic_2("andq", from, to)
#define _or(from, to) mnemonic_2("orq", from, to)
#define neg(op, to) mnemonic_2("negq", op, to)
#define _not(op, to) mnemonic_2("notq", op, to)

#define jumpeq(to) mnemonic_call("je", to)
#define jumpgt(to) mnemonic_call("jg", to)
#define jumpge(to) mnemonic_call("jge", to)
#define jumplt(to) mnemonic_call("jl", to)
#define jumple(to) mnemonic_call("jle", to)
#define jumpne(to) mnemonic_call("jne", to)
#define jump(to) mnemonic_call("jmp", to)

#define load(from, to) mnemonic_2("movq", from, to)
#define store(from, to) mnemonic_2("movq", from, to)

#define cmp(what, to) mnemonic_2("cmpq", what, to)

#define ret() mnemonic_0("ret")
#define call(label) mnemonic_call("call", label)

#define set(what, offset, to) mnemonic_2_set("movq", what, offset, to)

void initFileOutput(FILE *file) {
    out = file;
    return;
}


/*

ASM_FuncSignature   [+-] придумать как парсить аргументы
ASM_Var             [+] надо сделать строки и bool
ASM_Assignment      [+] тоже самое что и  ASM_Var
ASM_Condition       
ASM_Call            [-] сделать работу с регистрами
ASM_Return          [+-] пушу результат в регистр %rax



    Надо будет добавить еще 2 массива:
        1. Массив для хранения меток по принципу стека
        2. Массив-индикатор для передачи аргументов в функции и их чтения


    Метки
    1. While -> {loop_start_1, loop_end_1}
    2. DoWhile -> {do_loop_start_1, do_loop_start_1}
    3. If -> {if_condition_1, if_true_1, if_end_1 }
    4. IfElse -> {if_condition_1, if_true_1 , if_false_1 ,  if_end_1 }


*/

int markCounter = 1;


void parseControlFlowGraph(Block *block, int curNum, OffsetArray *array, MarkArray *markArray, bool ifelse)  {
    if  (block == NULL)
        return;
    
    /*
        Тут работаю с нодами и проверяю на посещение
    */


    if (strcmp(block->type, "start") == 0)  {
        if (curNum == 0) {
            // пролог asm
            fprintf(out, ".section .text\n");
            fprintf(out, ".global _start\n\n");

        }
        

    }else if (strcmp(block->type, "end") == 0) {
        printf("THE END\n");
        if  (curNum == 0) {
            // печать конца функции main
            fprintf(out, "\n\n\t# End block\n");
            fprintf(out, "\tmovq $%d, %%%s\n", 60, "rax");
            fprintf(out, "\tmovq $%d, %%%s\n", 0, "rdi");
            fprintf(out, "\tsyscall\n\n\n");
        }else {
            fprintf(out, "\n");
            mov("rbp", "rsp");
            pop("rbp");
            ret();
            fprintf(out, "\n\n");
        }

    }else if (strcmp(block->type, "ASM_funcSignature") == 0) {
        // пару сигнатуру функции
        fprintf(out, "\t# ASM_funcSignature\n");
        printf("Doing ASM_funcSig\n");
        regArray->current = 0;
        ASM_FuncSignature(block->operations, array);
        fprintf(out, "\n");
        
    }else if (strcmp(block->type, "ASM_Assignment") == 0) {
        fprintf(out, "\t# ASM_Assignment\n");
        // паршу присвоение
        // протестить
        printf("Doing assign\n");
        ASM_Assignment(block->operations, array);
        fprintf(out, "\n");

    }else if  (strcmp(block->type, "ASM_Return") == 0) {
        // паршу return 
        fprintf(out, "\t# ASM_Return\n");
        ASM_Return(block->operations, array);

        if (strcmp(block->left->type, "end") != 0) {
            /*
                movq %rbp, %rsp
	            popq %rbp
	            ret
            */
           mov("rbp", "rsp");
           pop("rbp");
           ret();
        }

    }else if  (strcmp(block->type, "ASM_Var") == 0)  {
        // паршу инициализацию переменной
        fprintf(out, "\t# ASM_Var\n");
        printf("Doing ASM_Var\n");
        ASM_Var(block->operations, array);
        fprintf(out, "\n");

    }else if (strcmp(block->type, "ASM_Var_CALL") == 0)  {
        //  чищу регистры от предыдущеих функций
        fprintf(out, "\t# ASM_Var_CALL\n");
        printf("Doing ASM_Var\n");
        cleanRegArray(regArray);

        /*
         Блок для проверки регистров
        */
       printf("ASM_Var_CALL: check regArray\n");
       for (int i = 0; i < regArray->size;  i++) {
            char *isUsed  =  "false";
            if (regArray->data[i]->used == true)
                isUsed = "true";
            printf("index: %d, register: %s, used: %s\n", i, regArray->data[i]->reg, isUsed);
        }
        printf("Currend index in regArray: %d\n",  regArray->current);

        ASM_Call(block->operations, array);

        printf("ASM_Var_CALL after: check regArray\n");
       for (int i = 0; i < regArray->size;  i++) {
            char *isUsed  =  "false";
            if (regArray->data[i]->used == true)
                isUsed = "true";
            printf("index: %d, register: %s, used: %s\n", i, regArray->data[i]->reg, isUsed);
        }
        printf("Currend index in regArray: %d\n",  regArray->current);

        fprintf(out, "\n\n");
        // checkRegArray();
        // делаю  вызов функции


    }else if (strcmp(block->type, "ASM_CALL") == 0)  {
        //  чищу регистры от предыдущеих функций
        fprintf(out, "\t# ASM_CALL\n");
        printf("Doing ASM_CALL\n");
        cleanRegArray(regArray);

        /*
         Блок для проверки регистров
        */
       printf("ASM_CALL: check regArray\n");
       for (int i = 0; i < regArray->size;  i++) {
            char *isUsed  =  "false";
            if (regArray->data[i]->used == true)
                isUsed = "true";
            
            printf("index: %d, register: %s, used: %s\n", i, regArray->data[i]->reg, isUsed);

        }
        printf("Currend index in regArray: %d\n",  regArray->current);

        ASM_Call(block->operations, array);

        printf("ASM_CALL after: check regArray\n");
       for (int i = 0; i < regArray->size;  i++) {
            char *isUsed  =  "false";
            if (regArray->data[i]->used == true)
                isUsed = "true";
            printf("index: %d, register: %s, used: %s\n", i, regArray->data[i]->reg, isUsed);
        }
        printf("Currend index in regArray: %d\n",  regArray->current);

        fprintf(out, "\n\n");
        //checkRegArray();
        // делаю  вызов функции


    }else if (strcmp(block->type, "ASM_Dowhile") == 0) {
        // тут я  ставлю метку и заношу её в массив  меток
        fprintf(out, "\t# ASM_Dowhile\n");
        printf("Doing DoWhile\n");
        char mark[256];
        sprintf(mark, "do_while_%d", markCounter);
        markCounter++;
        fprintf(out, "%s:\n", mark);
        pushMarkArray(markArray, mark);

        /*
        char mark[256];
        sprintf(mark, "do_while_%s", markCounter++);
        pushMarkArray(markArray, mark);
        */
        
    } else if (strcmp(block->type, "ASM_END_Dowhile") == 0)  {
        // проверяю условие
        // достаю метку из массива
        fprintf(out, "\t# ASM_END_Dowhile\n");
        printf("Doing END_DoWhile\n");
        ASM_Condition(block->operations, array, markArray, "dowhile");
        fprintf(out, "\n\n");

    } else if (strcmp(block->type, "ASM_If") == 0)  {
        fprintf(out, "\t# ASM_If\n");
        printf("Doing if section\n");
        char  mark[256];
        sprintf(mark, "if_end_%d", markCounter);
        markCounter++;
        pushMarkArray(markArray, mark);
        pushMarkArray(markArray, mark);
        ASM_Condition(block->operations, array, markArray, "if");
        fprintf(out, "\n\n");
        // паршу кондицию
        // расставляю метки

    }else if (strcmp(block->type, "ASM_END_If") == 0)  {
        // расставляю метки
        fprintf(out, "\t# ASM_END_if\n");
        char *endMark = popMarkArray(markArray);
        fprintf(out, "%s:\n", endMark);
        fprintf(out, "\n\n");

    }else if (strcmp(block->type, "ASM_while") == 0)  {
        // паршу кондицию
        // расставляю метки
        printf("Doing While\n");
        char startMark[256];
        char endMark[256];    
        sprintf(startMark, "while_start_%d", markCounter);
        sprintf(endMark, "while_end_%d", markCounter);
        markCounter++;

        fprintf(out, "%s:\n", startMark);

        pushMarkArray(markArray, endMark);
        pushMarkArray(markArray, startMark);
        pushMarkArray(markArray, endMark);

        printf("TRYING ASM_CONDITION\n");
        ASM_Condition(block->operations, array, markArray, "while");
        fprintf(out, "\n\n");

    }else if (strcmp(block->type, "ASM_END_While") == 0) {
        printf("Reached END_While\n");
        char *startMark = popMarkArray(markArray);
        char *endMark = popMarkArray(markArray);
        jump(startMark);
        fprintf(out, "\n%s:\n", endMark);
        fprintf(out, "\n");

    }else if (strcmp(block->type, "ASM_Ifelse") == 0) {
        char else_IfElse[256];
        char end_IfElse[256];
        sprintf(else_IfElse, "else_ifelse_%d", markCounter);
        sprintf(end_IfElse, "end_IfElse_%d", markCounter);
        markCounter++;

        
        pushMarkArray(markArray, end_IfElse);
        pushMarkArray(markArray, else_IfElse);
        pushMarkArray(markArray, end_IfElse);
        pushMarkArray(markArray, else_IfElse);


        ASM_Condition(block->operations, array, markArray, "ifelse");

        parseControlFlowGraph(block->left, curNum, array, markArray, true);
        parseControlFlowGraph(block->right, curNum, array, markArray, false);

        return;

    }else if (strcmp(block->type, "ASM_END_IfElse") == 0) {
        char *end_ifelse = popMarkArray(markArray);
        fprintf(out, "%s:\n", end_ifelse);

    }else if (strcmp(block->type, "ASM_ifelseelse") == 0) {
        char *else_ifelse = popMarkArray(markArray);
        fprintf(out, "%s:\n", else_ifelse);
    }

    if (block->left !=  NULL && (strcmp(block->left->type, "ASM_END_IfElse") == 0) && ifelse) {
        // вытаскиваю end_ifelse_[id]
        char *end_ifelse = popMarkArray(markArray);
        jump(end_ifelse);
        fprintf(out, "\n\n");
        return;
    }


    // иду влево  по базовому случаю
    parseControlFlowGraph(block->left, curNum, array, markArray, ifelse);
    
    //  иду парво по базовому члучаю
   // parseControlFlowGraph(block->right, curNum, array, markArray);

}




/*

    Дальше идет генерация листинга на ассемблере GAS ASSEMBER под Linux 
    Разбиваю генерацию на базовые случаи и работают с базовыми случаями  отдельно

*/


void ASM_FuncSignature(OperNode *node, OffsetArray *array) {
    if (node == NULL) {
        #ifdef DEBUG_ASM_FUNC
            printf("return\n");
        #endif
        return;
    }

    #ifdef DEBUG_ASM_FUNC
        printf("current node is: %s\n", node->op);
    #endif
    
    #ifdef DEBUG_ASM_FUNC
        printf("go to the left\n");
    #endif
    ASM_FuncSignature(node->left, array);

    #ifdef DEBUG_ASM_FUNC
        printf("go to the right\n");
    #endif
    ASM_FuncSignature(node->right, array);

    if (strcmp(node->op, "ASM_ArgDef") == 0)  {
        if (strcmp(node->left->value, "int") == 0) {
            // беру из массива аргументов для функции переменные
            // заношу в мой массив индификатор переменной и место в памяти относительно sbp (stack base pointer)
            // заношу в стек функции эту переменную
            array->currentOffset = array->currentOffset + 8;
            sub_i_i(8, "rsp");
        }
        if (strcmp(node->right->type, "IDENTIFIER") == 0) {
            if (regArray->data[regArray->current]->used == false) {
                printf("expected to find %s, current pos:%d\n", regArray->data[regArray->current]->reg, regArray->current);
                
                for (int i = 0; i < regArray->size;  i++) {
                    char *isUsed  =  "false";
                    if (regArray->data[i]->used == true)
                        isUsed = "true";
                    printf("index: %d, register: %s, used: %s\n", i, regArray->data[i]->reg, isUsed);
                }

                fprintf(stderr, "Not enough value in regArray\n");
                exit(EXIT_FAILURE);
            }
            addOffsetArray(array, node->right->value, array->currentOffset);
            set(regArray->data[regArray->current]->reg, array->currentOffset, "rbp");

            regArray->data[regArray->current]->used = true; // Добавлено: установка флага used
            regArray->current += 1;
        }

    }

    if (strcmp(node->op, "ASM_FuncDef") == 0)  {
            #ifdef DEBUG_ASM_FUNC
                printf("current op: %s\n", node->op);
            #endif
            if (strcmp(node->left->value, "void") != 0 ) {
                // обрабатываю все случаи, кроме void когда мне нужно взять определенной значение и положить его на стек
                // а может и не надо, просто в регистр %RAX убрать значение
            }
            // тут надо создать индефикатор моей функции для ассемблера. Пример -> main:
            if (strcmp(node->right->value, "main") == 0)  {
               fprintf(out, "%s:\n", "_start"); 
            }else {
                fprintf(out, "%s:\n", node->right->value);
            }
            // сохроняю указатели на стек
            fprintf(out, "\tpushq %rbp\n");
            fprintf(out, "\tmovq %rsp, %rbp\n\n\n");

            return;
    }
}


void ASM_Var(OperNode *node, OffsetArray *array) {
    if (node ==  NULL) {
        #ifdef DEBUG_ASM_VAR
            printf("return\n");
        #endif
        return;
    }

    #ifdef DEBUG_ASM_VAR
        printf("Current node: %s, %d\n", node->op, node->id);
    #endif

    if (strcmp(node->op, "ASM_Value") == 0) {
        // работаю с int,  не ебу пока что со str
        if  (strcmp(node->type, "INT") == 0) {
            // заношу на значение  в регистр
            // пушу регистр на стек
            mov_i(node->value, "rax");
            push("rax");

            return;
        }else if (strcmp(node->type, "IDENTIFIER") == 0) {
            // выяснять где находится на стеке значение
            // заношу его в регистр
            // пушу регистр на стек
            if (array->size >  0) {
                int pos = containsOffsetArray(array, node->value);
                if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                }  

                // работаю с позицией
                fprintf(out, "\t%s -%d(%rbp), %%%s\n", "movq", pos, "rax");
                push("rax");


            }else {
                fprintf(out, "ERROR: no such value: %s\n", node->value);
            }
            
            return;
        }
    }
    
    if (node->left !=  NULL) {
        #ifdef DEBUG_ASM_VAR
            printf("Go the left\n");
        #endif
        ASM_Var(node->left, array); 
    }
    
    if (node->right !=  NULL) {
        #ifdef DEBUG_ASM_VAR
            printf("Go the right\n");
        #endif
        ASM_Var(node->right, array); 
    }



    if (strcmp(node->op, "ASM_Write") == 0) {
        // заношу левую ноду на стек данные опрделенного типа под этим индефикатором 
        addOffsetArray(array, node->left->value, array->currentOffset);

        // случай возврата значения из функции. Просто беру сразу из регистра значение
        if (strcmp(node->right->op, "ASM_PopStack") != 0) {
            pop("rax");
        }
        //  достаю со  стека переменную
        //  присваю по адресу относительно SPB Пример: movq $20, -8(%sbp)
        set("rax", array->currentOffset, "rbp");
        fprintf(out, "\n\n");

        return;
    }else if (strcmp(node->op, "ASM_Type") == 0) {
        if (strcmp(node->value,  "int") == 0) {
            array->currentOffset += 8;
            sub_i_i(8, "rsp");
            //fprintf(out, "\n");
        }


        return;
    }

    if (strcmp(node->op, "TIMES") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rdi");
        pop("rax");
        mul("rdi");
        push("rax");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "MINUS") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rcx");
        pop("rdi");
        sub("rcx", "rdi");
        push("rdi");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "PLUS") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rcx");
        pop("rdi");
        add("rcx", "rdi");
        push("rdi");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "DEVIDE") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rax");
        pop("rcx");
        mov_i("0", "rdx");
        div("rcx");
        push("rax");
        fprintf(out,"\n");
        return;
    }



}


void ASM_Assignment(OperNode *node, OffsetArray *array) {
    if (node == NULL) {
        #ifdef DEBUG_ASM_ASSIGN
            printf("return\n");
        #endif
        return;
    }

    #ifdef DEBUG_ASM_ASSIGN
        printf("current node: %s, %d\n", node->op, node->id);
    #endif

    if (strcmp(node->op, "ASM_Value") == 0) {
        // работаю с int,  не ебу пока че со str
        if  (strcmp(node->type, "INT") == 0) {
            // заношу на значение  в регистр
            // пушу регистр на стек
            mov_i(node->value, "rax");
            push("rax");

            return;
        }else if (strcmp(node->type, "IDENTIFIER") == 0) {
            // выяснять где находится на стеке значение
            // заношу его в регистр
            // пушу регистр на стек
            if (array->size >  0) {
                int pos = containsOffsetArray(array, node->value);
                if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                }  

                // работаю с позицией
                fprintf(out, "\t%s -%d(%rbp), %%%s\n", "movq", pos, "rax");
                push("rax");
                
                
            }else {
                fprintf(out, "ERROR: no such value: %s\n", node->value);
            }
            
            return;
        }
    }


    if (node->left !=  NULL) {
        #ifdef DEBUG_ASM_ASSIGN
            printf("go to the left\n");
        #endif
        ASM_Assignment(node->left, array);
    }

    if (node->right != NULL) {
        #ifdef DEBUG_ASM_ASSIGN
            printf("go to the right\n");
        #endif
        ASM_Assignment(node->right, array);
    }


    if (strcmp(node->op, "ASM_Write") == 0) {
        //иду налево и  читаю, куда буду присваивать значение
        // иду право
        // достаю со  стека
        // присваиваю

        pop("rax");
        if (array->size > 0) {
            int pos = containsOffsetArray(array, node->left->value);
            if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                } 

            // работаю с позицией
            set("rax", pos, "rbp");
            fprintf(out, "\n\n");

        }else  {
            fprintf(out, "ERROR: no such value: %s\n", node->value);
        }

        return;
    }

    if (strcmp(node->op, "TIMES") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rdi");
        pop("rax");
        mul("rdi");
        push("rax");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "MINUS") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rcx");
        pop("rdi");
        sub("rcx", "rdi");
        push("rdi");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "PLUS") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rcx");
        pop("rdi");
        add("rcx", "rdi");
        push("rdi");
        fprintf(out,"\n");
        return;
    }else if (strcmp(node->op, "DEVIDE") == 0) {
        // снимаю со стека 2 значения
        // делаю операцию
        // сохроняю в регистр
        // пушу регистр на стек
        pop("rax");
        pop("rcx");
        mov_i("0", "rdx");
        div("rcx");
        push("rax");
        fprintf(out,"\n");
        return;
    }

}

void ASM_Condition(OperNode *node, OffsetArray *array, MarkArray *markArray, char *option) {
    if (node ==  NULL)
        return;
    
    // потом надо будет фиксить для случаев, когда надо вместитьб больше  условий, чем 1
    if (strcmp(node->op, "ASM_Value") == 0) {
        // работаю с int,  не ебу пока че со str
        if  (strcmp(node->type, "INT") == 0) {
            // заношу на значение  в регистр
            // пушу регистр на стек
            mov_i(node->value, "rax");
            push("rax");
            return;
        }else if (strcmp(node->type, "IDENTIFIER") == 0) {
            // выяснять где находится на стеке значение
            // заношу его в регистр
            // пушу регистр на стек
            if (array->size >  0) {
                int pos = containsOffsetArray(array, node->value);
                if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                }  

                // работаю с позицией
                fprintf(out, "\t%s -%d(%rbp), %%%s\n", "movq", pos, "rax");
                push("rax");
                
                
            }else {
                fprintf(out, "ERROR: no such value: %s\n", node->value);
            }
            

            return;
        }else if (strcmp(node->type, "BOOL") == 0) {
            // заношу на значение  в регистр
            // пушу регистр на стек
        }
    }

    ASM_Condition(node->left, array, markArray,  option);
    ASM_Condition(node->right, array, markArray, option);


    //снимаю со со стека 2  значения
    // кладу их в  регистры  

    if (strcmp(node->op, "LESS") == 0) {
        // LESS (a < b): команда на GAS будет `jl` (jump if less).
        // Перед этим обычно используется команда `cmp`, которая сравнивает два значения.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumplt(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumpge(mark);
        }

    } else if  (strcmp(node->op, "EQUAL") == 0) {
        // EQUAL (a == b): команда на GAS будет `je` (jump if equal).
        // Для этого также используется команда `cmp` перед проверкой.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumpeq(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumpne(mark);
        }

    } else if  (strcmp(node->op, "NOT_EQUAL") == 0) {
        // NOT_EQUAL (a != b): команда на GAS будет `jne` (jump if not equal).
        // Опять же используется команда `cmp`.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumpne(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumpeq(mark);
        }

    } else if  (strcmp(node->op, "NOT") == 0) {
        // NOT (логическое отрицание): команда на GAS будет `not`.
        // Эта команда инвертирует все биты в регистре.
        pop("rdi"); // r 
        pop("rax"); // l

    } else if  (strcmp(node->op, "GREATER") == 0) {
        // GREATER (a > b): команда на GAS будет `jg` (jump if greater).
        // Перед этим также используется `cmp`.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumpgt(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumple(mark);
        }

    } else if  (strcmp(node->op, "LESS_EQ") == 0) {
        // LESS_EQ (a <= b): команда на GAS будет `jle` (jump if less or equal).
        // Необходимо использовать команду `cmp` перед этим.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumple(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumpgt(mark);
        }

    } else if  (strcmp(node->op, "GREATER_EQ") == 0) {
        // GREATER_EQ (a >= b): команда на GAS будет `jge` (jump if greater or equal).
        // Используется `cmp` для сравнения значений.
        pop("rdi"); // r 
        pop("rax"); // l
        char *mark = popMarkArray(markArray);
        cmp("rdi", "rax");
        if (strcmp(option, "dowhile") == 0) {
            jumple(mark);
        }else if ((strcmp(option, "while") == 0) || (strcmp(option, "if") == 0)  || (strcmp(option, "ifelse") == 0) ) {
            jumplt(mark);
        }

    } else if  (strcmp(node->op, "AND") == 0) {
        // AND (логическое И): команда на GAS будет `and`.
        // Эта команда выполняет побитовое "И" между двумя регистрами.
        pop("rdi"); // r  
        pop("rax"); // l

    } else if  (strcmp(node->op, "OR") == 0) {
        // OR (логическое ИЛИ): команда на GAS будет `or`.
        // Эта команда выполняет побитовое "ИЛИ" между двумя регистрами.
        pop("rdi"); // r 
        pop("rax"); // l
    }


    // беру справа значение

}


void ASM_Call(OperNode *node, OffsetArray *array) {
    if (node == NULL) {
        #ifdef DEBUG_ASM_CALL
            printf("return\n");
        #endif

        return;
    }

    #ifdef DEBUG_ASM_CALL
        printf("current node: %s, %d\n", node->op, node->id);
    #endif

    // Проверка типа узла и выполнение соответствующих действий
    if (strcmp(node->op, "ASM_Arg") == 0) {
        if (strcmp(node->type, "INT") == 0) {
            // Заносим значение в регистр
            mov_i(node->value, "rax");            
        } else if (strcmp(node->type, "IDENTIFIER") == 0) {
            // Выясняем, где находится значение на стеке
            // Заносим его в регистр
            // Пушим регистр на стек
            if (array->size >  0) {
                int pos = containsOffsetArray(array, node->value);
                if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                }  

                // работаю с позицией
                fprintf(out, "\t%s -%d(%rbp), %%%s\n", "movq", pos, "rax");
                
                
            }else {
                fprintf(out, "ERROR: no such value: %s\n", node->value);
            }

        }
        /*
            // любое значение кладу в регистр %rax и %rax в свободный из 6 regArray
            mov("rax", regArray->data[regArray->current]->reg);
            regArray->data[regArray->current]->used = true;
            regArray->current += 1;
        */
        
        // любое значение кладу в регистр %rax и %rax в свободный из 6 regArray
        regArray->current += 1;
        mov("rax", regArray->data[regArray->current-1]->reg);
        regArray->data[regArray->current-1]->used = true;

        printf("===============================\n");
        printf("Place: %s in  register: %s, index: %d\n", node->value, regArray->data[regArray->current-1]->reg, regArray->current-1);
        printf("===============================\n");

    }   

    // Рекурсивный вызов для левого и правого поддеревьев
    ASM_Call(node->left, array);
    ASM_Call(node->right, array);

    if (strcmp(node->op, "ASM_CALL_Func") == 0) {
        // Слева название функции, которую нужно вызвать
        // Вызов функции
        #ifdef DEBUG_ASM_CALL
            printf("call %s\n", node->left->value);
        #endif

        call(node->left->value);
    }

}

void ASM_Return(OperNode *node, OffsetArray *array) {
    if (node ==  NULL)
        return;

    ASM_Return(node->left, array);
    ASM_Return(node->right, array);

    if  (strcmp(node->op, "ASM_Value") == 0) {
        if  (strcmp(node->type, "INT") == 0) {
            // заношу на значение  в регистр %EAX
            mov(node->value, "rax");
            //push("rax");
            return;
        }else if (strcmp(node->type, "IDENTIFIER") == 0) {
            // выяснять где находится на стеке значение
            // заношу его в регистр %EAX
            if (array->size >  0) {
                int pos = containsOffsetArray(array, node->value);
                if  (pos == -1) {
                    fprintf(out, "ERROR: no such value: %s\n", node->value);
                    return;
                }  

                // работаю с позицией
                fprintf(out, "\t%s -%d(%rbp), %%%s\n", "movq", pos, "rax");
                //push("rax");
                
                
            }else {
                fprintf(out, "ERROR: no such value: %s\n", node->value);
            }

            return;
        }
    }

}



/*

    Массив для хранения данных с offset-ами относительно sbp

*/

OffsetArray *initOffsetArray(int capacity) {
    OffsetArray *array = malloc(sizeof(OffsetArray));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    array->data = malloc(capacity * sizeof(Place));
    if (array->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    array->cap = capacity;
    array->size = 0;
    array->currentOffset = 0;

    printf("array is inited %d\n", array->cap);

    return array;
}

void addOffsetArray(OffsetArray *array, const char *identifier, int offset) {
    if (array->size >= array->cap) {
        fprintf(stderr, "Array is full\n");
        return;
    }

    array->data[array->size].identifier = strdup(identifier);
    if (array->data[array->size].identifier == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    array->data[array->size].offset = offset;

    printf("Added: %s, %d\n", array->data[array->size].identifier, array->data[array->size].offset);

    array->size++;
}

int containsOffsetArray(OffsetArray *array, const char *value) {
    for (int i = 0; i < array->size; i++) {
        if (strcmp(array->data[i].identifier, value) == 0) {
            return array->data[i].offset; 
        }
    }
    return -1; 
}

void ASMfreeArray(OffsetArray *array) {
    for (int i = 0; i < array->size; i++) {
        free(array->data[i].identifier); 
    }
    free(array->data);  
    free(array);  
}



/*

    Массив для хранения меток

*/


MarkArray *initMarkArray(int cap) {
    MarkArray *array = malloc(sizeof(MarkArray));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    array->mark = malloc(cap * sizeof(char *));

    array->cap = cap;
    array->size = 0;

    return array;
}


void pushMarkArray(MarkArray *array, char *value) {
    if (array->size < array->cap) {
        array->mark[array->size++] = value;
    } else {
        fprintf(stderr, "Array is full, cannot add more elements\n");
    }

    #ifdef DEBUG_MARK_ARRAY
        printf("Added value: %s, size: %d\n", array->mark[array->size-1], array->size);
    #endif
}

char *popMarkArray(MarkArray *array) {
    if (array->size == 0)  {
        fprintf(stderr, "No value in mark array to pop\n");
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG_MARK_ARRAY
        printf("Extracted value: %s, size: %d\n", array->mark[array->size-1], array->size-1);
    #endif

    char *newPlace = array->mark[array->size - 1]; 
    array->size -= 1;

    return newPlace;
}

void freeMarkArray(MarkArray *array) {
    if (array == NULL) {
        return;
    }

    if (array->mark != NULL) {
        free(array->mark);
    }

    free(array);
}


/*

    Массив для работы с аргументами через регистры:
    1. %rdi
    2. %rsi
    3. %rdx
    4. %rcx
    5. %r8
    6. %r9

*/

Pair *getNewPair(char *reg) {
    Pair *newPair = malloc(sizeof(Pair));
    if (newPair == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    newPair->reg = reg;
    newPair->used = false;
    return newPair;
}

void initRegArray() {
    regArray = malloc(sizeof(RegArray));
    if (regArray == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    regArray->size = 6;
    regArray->current = 0;
    regArray->data = malloc(regArray->size * sizeof(Pair *));  

    if (regArray->data == NULL) {
        fprintf(stderr, "Memory allocation for data regArray failed\n");
        free(regArray);  
        exit(EXIT_FAILURE);
    }

    regArray->data[0] = getNewPair("rdi");
    regArray->data[1] = getNewPair("rsi");
    regArray->data[2] = getNewPair("rdx");
    regArray->data[3] = getNewPair("rcx");
    regArray->data[4] = getNewPair("r8");
    regArray->data[5] = getNewPair("r9");

}

void cleanRegArray(RegArray *array) {
    for (int i = 0; i < array->size; i++) {
        array->data[i]->used = false;
        printf("pos: %d,  register: %s  is cleaned!\n", i, array->data[i]->reg);
    }
    //Добавил, чтобы отсчет всегда начинался с 0 в каждом вызове функции
    array->current = 0;
}

void freeRegArray(RegArray *array) {
    if (array == NULL) {
        return;
    }

    if (array->data != NULL) {
        for (int i = 0; i < array->size; i++) {
            free(array->data[i]); 
        }
        free(array->data); 
    }

    free(array); 
}



void checkRegArray() {
    for (int i = 0; i < regArray->size; i++) {
        printf("Register: %s, used: %s\n", regArray->data[i]->reg, 
               regArray->data[i]->used ? "true" : "false");
    }
}
