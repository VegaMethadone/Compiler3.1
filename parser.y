%{
    #include "node.h"
    #include "error.h"
    #include "parser.tab.h"
    
    #define YYDEBUG 1
%}

%error-verbose
%union{
    Node* node;
}
    
// Операторы
%token <node> PLUS MINUS MULTIPLY DIVIDE PERCENT EQUAL NOT_EQUAL // {"+", "-", "*", "/", "%", "==", "!="}
%token <node> LT GT LTE GTE // {"<", ">", "<=", ">="}

// Логические операторы
%token <node> AND OR NOT  // {"&&", "||", "!"}

// Идентификаторы
%token <node> IDENTIFIER // Идентификатор (имя переменной, функции и т.д.)

// Разделители и скобки
%token <node> COMMA       // ","
%token <node> SEMICOLON   // ";"
%token <node> LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET // {"(", ")", "{", "}", "[", "]"}

// Типы данных
%token <node> STR        // Строка
%token <node> CHAR       // Символ
%token <node> BIN HEX INT // Двоичное, Шестнадцатеричное, Целое число
%token <node> TYPEDEF    // Определение типа
%token <node> ARRAY_COMMAS // Запятые в массивах

// Булевы значения
%token <node> TRUE FALSE // Логическое значение: TRUE, FALSE

// Управляющие конструкции
%token <node> IF ELSE WHILE DO BREAK // Условные и циклические конструкции


// Исходный код
%type <node> source

// Общие конструкции
%type <node> typeRef              // Ссылка на тип
%type <node> funcSignature        // Подпись функции
%type <node> sourceItem           // Элемент исходного кода
%type <node> statement            // Утверждение
%type <node> expr                 // Выражение
%type <node> argDef               // Определение аргументов

// Управляющие конструкции
%type <node> if                   // Условный оператор IF
%type <node> block                // Блок кода
%type <node> while                // Цикл WHILE
%type <node> do                   // Цикл DO
%type <node> break                // Выход из цикла

// Переменные и литералы
%type <node> var                  // Переменная
%type <node> literal              // Литерал
%type <node> place                // Место (позиция)
%type <node> indexer              // Индексатор
%type <node> array                // Массив

// Функции и вызовы
%type <node> call                 // Вызов функции
%type <node> listArgDef           // Список аргументов функции

// Выражения
%type <node> expression           // Общее выражение
%type <node> unary                // Унарные операции
%type <node> binary               // Бинарные операции
%type <node> assignment           // Присваивание
%type <node> listExpr             // Список выражений
%type <node> optionalListExpr     // Опциональный список выражений

// Логические и управляющие конструкции
%type <node> optionalElseStatement // Опциональное ELSE выражение
%type <node> listStatement        // Список утверждений

// Структуры данных
%type <node> listVarDeclared      // Список объявленных переменных
%type <node> listVarDeclaredItem  // Элемент списка объявленных переменных
%type <node> optionalTypeRef      // Опциональная ссылка на тип
%type <node> builtin              // Встроенные функции или операторы
%type <node> custom               // Пользовательские функции или операторы
%type <node> braces               // Скобки (фигурные и круглые)


%%
// Определяет исходный код
source:
    {{ $$ = NULL; }}                                                  // Пустой исходный код
    | source sourceItem {{ $$ = createNode("source", $1, $2, "r"); }}  // Последовательность элементов исходного кода

// Определяет отдельный элемент исходного кода
sourceItem:
    funcSignature block {{ $$ = createNode("sourceItem", $1, $2, ""); }}          // Функция и блок кода
    | funcSignature SEMICOLON {{ $$ = createNode("sourceItem", $1, NULL, ""); }}  // Функция с завершающей точкой с запятой

// Определяет подпись функции
funcSignature:
    argDef LPAREN listArgDef RPAREN {{ $$ = createNode("funcSignature", $1, $3, ""); }}  // Определение аргументов и список аргументов

// Определяет список аргументов функции
listArgDef:
    {{ $$ = NULL; }}         // Пустой список аргументов
    | argDef {{ $$ = $1; }}  // Один аргумент
    | argDef COMMA listArgDef {{ $$ = createNode("listArgDef", $1, $3, ""); }}  // Аргументы, разделенные запятыми

// Определяет аргумент функции
argDef:
    optionalTypeRef IDENTIFIER {{ $$ = createNode("argDef", $1, $2, ""); }}  // Тип и имя аргумента

// Определяет опциональную ссылку на тип
optionalTypeRef:
    {{ $$ = NULL; }}           // Отсутствие типа
    | typeRef {{ $$ = $1; }}  // Ссылка на тип

// Определяет ссылку на тип данных
typeRef:
    builtin {{ $$ = $1; }}   // Встроенный тип
    | custom {{ $$ = $1; }}  // Пользовательский тип
    | array {{ $$ = $1; }}   // Массив

// Определяет встроенные функции или типы
builtin:
    TYPEDEF {{ $$ = $1; }}  // Определение типа

// Определяет заявление (statement) в языке
statement:
    var {{ $$ = $1; }}       // Переменная
    | if {{ $$ = $1; }}      // Условный оператор IF
    | block {{ $$ = $1; }}   // Блок кода
    | while {{ $$ = $1; }}   // Цикл WHILE
    | do {{ $$ = $1; }}      // Цикл DO
    | break {{ $$ = $1; }}   // Выход из цикла
    | expression {{ $$ = $1; }}  // Выражение

// Определяет пользовательские функции
custom:
    IDENTIFIER {{ $$ = $1; }}  // Имя пользовательской функции

// Определяет массив
array:
    typeRef ARRAY_COMMAS {{ $$ = createNode("array", $1, NULL, $2->value); }}  // Тип и запятые в массиве

// Определяет список объявленных переменных
listVarDeclared:
    listVarDeclaredItem COMMA listVarDeclared {{ $$ = createNode("listVarDeclared", $1, $3, ""); }}  // Переменные, разделенные запятыми
    | listVarDeclaredItem {{ $$ = createNode("listVarDeclared", $1, NULL, ""); }}                    // Один элемент в списке

// Определяет элемент списка объявленных переменных
listVarDeclaredItem:
    IDENTIFIER {{ $$ = $1; }}    // Имя переменной
    | assignment {{ $$ = $1; }}  // Присваивание

// Определяет переменную
var:
    typeRef listVarDeclared SEMICOLON {{ $$ = createNode("var", $1, $2, ""); }}  // Тип и список переменных

// Определяет условный оператор IF
if:
    IF LPAREN expr RPAREN statement optionalElseStatement {{ $$ = createNode("if", $3, createNode("ifStatement", $5, $6, ""), ""); }}  // Условие и блок операторов

// Определяет опциональное ELSE выражение
optionalElseStatement:
    ELSE statement optionalElseStatement {{ $$ = createNode("else", $2, $3, ""); }}   // ELSE и следующее опциональное ELSE
    | ELSE statement {{ $$ = createNode("else", $2, NULL, ""); }}                     // Простое ELSE
    | {{ $$ = NULL; }}                                                                // Отсутствие ELSE

// Определяет список утверждений
listStatement:
    statement listStatement {{ $$ = createNode("listStatement", $1, $2, ""); }}  // Утверждения, разделенные друг от друга
    | {{ $$ = NULL; }}  // Пустой список

// Определяет блок кода
block:
    LBRACE listStatement RBRACE {{ $$ = createNode("block", $2, NULL, ""); }}  // Блок кода с утверждениями
    | LBRACE RBRACE {{ $$ = createNode("block", NULL, NULL, ""); }}            // Пустой блок

// Определяет цикл WHILE
while:
    WHILE LPAREN expr RPAREN statement {{ $$ = createNode("while", $3, $5, ""); }}  // Условие и блок кода

// Определяет цикл DO
do:
    DO block WHILE LPAREN expr RPAREN SEMICOLON {{ $$ = createNode("dowhile", $2, $5, ""); }}  // Блок кода и условие

// Определяет выход из цикла
break:
    BREAK SEMICOLON {{ $$ = createNode("break", NULL, NULL, ""); }}  // Команда BREAK

// Определяет выражение
expression:
    expr SEMICOLON {{ $$ = $1; }}  // Выражение с точкой с запятой

// Определяет присваивание
assignment:
    expr EQUAL expr {{ $$ = createNode("assignment", $1, $3, ""); }}  // Левое и правое выражение

// Определяет выражения
expr:
    unary     {{ $$ = $1; }}    // Унарные выражения
    | binary  {{ $$ = $1; }}    // Бинарные выражения
    | braces  {{ $$ = $1; }}    // Выражения в скобках
    | call    {{ $$ = $1; }}    // Вызов функции
    | indexer {{ $$ = $1; }}    // Индексатор
    | place   {{ $$ = $1; }}    // Местоположение
    | literal {{ $$ = $1; }}    // Литерал

// Определяет бинарные операции
binary:
    assignment  // Присваивание является бинарной операцией
    | expr PLUS expr {{ $$ = createNode("PLUS", $1, $3, ""); }}            // Операция сложения
    | expr MINUS expr {{ $$ = createNode("MINUS", $1, $3, ""); }}          // Операция вычитания
    | expr MULTIPLY expr {{ $$ = createNode("TIMES", $1, $3, ""); }}       // Операция умножения
    | expr DIVIDE expr {{ $$ = createNode("DIVIDE", $1, $3, ""); }}        // Операция деления
    | expr PERCENT expr {{ $$ = createNode("PERCENT", $1, $3, ""); }}      // Операция остатка
    | expr EQUAL EQUAL expr {{ $$ = createNode("EQUAL", $1, $4, ""); }}    // Операция равенства
    | expr NOT_EQUAL expr {{ $$ = createNode("NOT_EQUAL", $1, $3, ""); }}  // Операция неравенства
    | expr LT expr {{ $$ = createNode("LESS", $1, $3, ""); }}              // Меньше
    | expr GT expr {{ $$ = createNode("GREATER", $1, $3, ""); }}           // Больше
    | expr LTE expr {{ $$ = createNode("LESS_EQ", $1, $3, ""); }}          // Меньше или равно
    | expr GTE expr {{ $$ = createNode("GREATER_EQ", $1, $3, ""); }}       // Больше или равно
    | expr AND expr {{ $$ = createNode("AND", $1, $3, ""); }}              // Логическое И
    | expr OR expr {{ $$ = createNode("OR", $1, $3, ""); }}                // Логическое ИЛИ

// Определяет унарные операции
unary:
    PLUS expr {{ $$ = createNode("PLUS", $2, NULL, ""); }}      // Унарный плюс
    | MINUS expr {{ $$ = createNode("MINUS", $2, NULL, ""); }}  // Унарный минус
    | NOT expr {{ $$ = createNode("NOT", $2, NULL, ""); }}      // Логическое НЕ

// Определяет выражения в скобках
braces:
    LPAREN expr RPAREN {{ $$ = createNode("braces", $2, NULL, ""); }}  // Скобки вокруг выражения

// Определяет вызов функции
call:
    IDENTIFIER LPAREN optionalListExpr RPAREN {{ $$ = createNode("CALL", $1, $3, ""); }}  // Имя функции и список аргументов

// Определяет опциональный список выражений
optionalListExpr:
    listExpr {{ $$ = createNode("optionalListExpr", $1, NULL, ""); }}   // Список выражений
    | {{ $$ = createNode("optionalListExpr", NULL, NULL, ""); }}        // Пустой список выражений

// Определяет список выражений
listExpr:
    expr COMMA listExpr {{ $$ = createNode("listExpr", $1, $3, ""); }}  // Выражения, разделенные запятыми
    | expr {{ $$ = createNode("listExpr", $1, NULL, ""); }}             // Одно выражение

// Определяет индексатор
indexer:
    expr LBRACKET listExpr RBRACKET {{ $$ = createNode("indexer", $1, $3, ""); }}  // Выражение в квадратных скобках

// Определяет местоположение (переменная или функция)
place:
    IDENTIFIER {{ $$ = $1; }}  // Имя идентификатора

// Определяет литералы
literal:
    TRUE {{ $$ = $1; }}     // Логическое значение TRUE
    | FALSE {{ $$ = $1; }}  // Логическое значение FALSE
    | STR {{ $$ = $1; }}    // Строка
    | CHAR {{ $$ = $1; }}   // Символ
    | HEX {{ $$ = $1; }}    // Шестнадцатеричное число
    | BIN {{ $$ = $1; }}    // Двоичное число
    | INT {{ $$ = $1; }}    // Целое число

%%