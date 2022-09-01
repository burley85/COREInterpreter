#ifndef STRUCTDEFS_H
#define STRUCTDEFS_H
#include <stdio.h>
#define MAX_INT 256
#define MAX_ID_LENGTH 32
#define MAX_SCOPE_NESTING 256

typedef enum tokenType{
    PROGRAM, BEGIN, END, NEW, DEFINE, EXTENDS, CLASS, INT, IF, THEN, ELSE, WHILE,
    ENDIF, SEMICOLON, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, ASSIGN, NEGATION, OR,
    EQUAL, LESS, LESSEQUAL, ADD, SUB, MULT, INPUT, OUTPUT, REF, SHARE, CONST,
    IDENTIFIER, EOS, ERROR, OTHER
} tokenType;

typedef enum {IntID, RefID, FuncID} idType;

typedef struct Scanner {
    FILE *file;
    int currentValue;
    char currentID[MAX_ID_LENGTH + 2];
    tokenType currentToken;
} Scanner;

// <assign> ::= id = input ( ) ; | id = <expr> ; | id = new class; | id = share id | id = share id + <expr> | id = share id - <expr> ;
typedef struct Assign
{
    struct ID *id;
    struct ID *id2;
    int inputAssignment;
    struct Expr *expr;
    int addOp;
} Assign;

// <cmpr> ::= <expr> == <expr> | <expr> < <expr> | <expr> <= <expr>
typedef struct Cmpr
{
    tokenType operator;
    struct Expr *expr1;
    struct Expr *expr2;
} Cmpr;


// <cond> ::= <cmpr> | ! ( <cond> ) | <cmpr> or <cond>
typedef struct Cond
{
    struct Cmpr *cmpr;
    struct Cond *cond;
} Cond;

// <decl> ::= <decl-int> | <decl-ref>
typedef struct Decl
{
    struct DeclInt *declInt;
    struct DeclRef *declRef;
} Decl;

// <decl-int> ::= int <id-list> ;
typedef struct DeclInt
{
    struct IDList *idList;
} DeclInt;

// <decl-ref> ::= ref <id-list> ;
typedef struct DeclRef
{
    struct IDList *idList;
} DeclRef;

// <decl-seq> ::= <decl> | <decl><decl-seq> | <func-decl> | <func-decl><decl-seq>
typedef struct DeclSeq
{
    struct Decl *decl;
    struct FuncDecl *funcDecl;
    struct DeclSeq *declSeq;
} DeclSeq;

// <expr> ::= <term> | <term> + <expr> | <term> – <expr>
typedef struct Expr
{
    struct Term *term;
    struct Expr* expr;
    int addOperator;
} Expr;

// <factor> ::= id | const | ( <expr> )
typedef struct Factor
{
    struct ID *id;
    struct Expr *expr;
    int constVal;
    
} Factor;

// <formals> ::= id | id , <formals>
typedef struct Formals
{
    struct ID *id;
    struct Formals *formals;
} Formals;

// <func-call> ::= begin id ( <formals> ) ;
typedef struct FuncCall
{
    struct ID *id;
    struct Formals *formals;
} FuncCall;

// <func-decl> ::= id ( ref <formals> ) { <stmt-seq> }
typedef struct FuncDecl
{
    struct ID *id;
    struct Formals *formals;
    struct StmtSeq *stmtSeq;
} FuncDecl;

// <id> ::=
typedef struct ID
{
    idType type;
    char *name;
} ID;

// <id-list> ::= id | id , <id-list>
typedef struct IDList
{
    struct ID *id;
    struct IDList *idList;
} IDList;

// <if> ::= if <cond> then { <stmt-seq> } | if <cond> then { <stmt-seq> } else { <stmt-seq> }
typedef struct If
{
    struct Cond *cond;
    struct StmtSeq *ifStmtSeq;
    struct StmtSeq *elseStmtSeq;
} If;

// <loop> ::= while <cond> { <stmt-seq> }
typedef struct Loop
{
    struct Cond *cond;
    struct StmtSeq *stmtSeq;
} Loop;

// <out> ::= output ( <expr> ) ;
typedef struct Out
{
    struct Expr *expr;
} Out;

// <prog> ::= program <decl-seq> begin <stmt-seq> end | program begin <stmt-seq> end
typedef struct Prog
{
    struct DeclSeq *declSeq;
    struct StmtSeq *stmtSeq;
    struct Memory *memory;
} Prog;

// <stmt> ::= <assign> | <if> | <loop>  | <out> | <decl> | <func-call>
typedef struct Stmt
{
    struct Assign *assign;
    struct If *ifStmt;
    struct Loop *loop;
    struct Out *out;
    struct Decl *decl;
    struct FuncCall *funcCall;

} Stmt;

// <stmt-seq> ::= <stmt> | <stmt><stmt-seq>
typedef struct StmtSeq
{
    struct Stmt *stmt;
    struct StmtSeq *stmtSeq;
} StmtSeq;

// <term> ::= <factor> | <factor> * <term> 
typedef struct Term
{
    struct Factor *factor;
    struct Term *term;
} Term;

#endif