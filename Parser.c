#include <stdlib.h>
#include <string.h>
#include "StructDefs.h"
#include "Scanner.h"
#include "Parser.h"


// Function declarations

    DeclSeq *parseDeclSeq(Scanner *scanner, IDListStack *ids);
    Decl *parseDecl(Scanner *scanner, IDListStack *ids);
    DeclInt *parseDeclInt(Scanner *scanner, IDListStack *ids);
    DeclRef *parseDeclRef(Scanner *scanner, IDListStack *ids);
    IDList *parseIDList(Scanner *scanner, IDListStack *ids, idType type);
    StmtSeq *parseStmtSeq(Scanner *scanner, IDListStack *ids);
    Stmt *parseStmt(Scanner *scanner, IDListStack *ids);
    If *parseIf(Scanner *scanner, IDListStack *ids);
    Loop *parseLoop(Scanner *scanner, IDListStack *ids);
    Cond *parseCond(Scanner *scanner, IDListStack *ids);
    Cmpr *parseCmpr(Scanner *scanner, IDListStack *ids);
    Assign *parseAssign(Scanner *scanner, IDListStack *ids);
    Expr *parseExpr(Scanner *scanner, IDListStack *ids);
    Term *parseTerm(Scanner *scanner, IDListStack *ids);
    Factor *parseFactor(Scanner *scanner, IDListStack *ids);
    Out *parseOut(Scanner *scanner, IDListStack *ids); 
    FuncDecl *parseFuncDecl(Scanner *scanner, IDListStack *ids);
    Formals *parseFormals(Scanner *scanner, IDListStack *ids);
    FuncCall *parseFuncCall(Scanner *scanner, IDListStack *ids);

//----------------------------------------------------------------

void addScopeToIDListStack(IDListStack *ids){
    if (ids->stackSize >= MAX_SCOPE_NESTING - 1){
        printf("ERROR: Nested scope limit exceeded \n");
        exit(1);
    }
    ids->stack[ids->stackSize] = malloc(sizeof(IDList));
    ids->stack[ids->stackSize]->id = malloc(sizeof(ID));
    ids->stack[ids->stackSize]->id->name = malloc(MAX_ID_LENGTH * sizeof(char));
    memset(ids->stack[ids->stackSize]->id->name, 0, sizeof(MAX_ID_LENGTH * sizeof(char)));
    
    ids->stack[ids->stackSize]->idList = NULL;
    ids->stackSize++;

}

IDListStack *initIDListStack(){
    IDListStack *ids = malloc(sizeof(IDListStack));
    *ids->stack = malloc(MAX_SCOPE_NESTING * sizeof(IDListStack *));
    ids->stackSize = 0;
    addScopeToIDListStack(ids);
    return ids;
}

void freeIDList(IDList *idList){
    if(idList->idList) freeIDList(idList->idList);
    free(idList);
}

void removeScopeFromIDListStack(IDListStack *ids){
    if (ids->stackSize == 0){
        printf("ERROR: call to removeScopeFromIDListStack with stack size 0.\n");
        exit(1);
    }
    
    ids->stackSize--;
    freeIDList(ids->stack[ids->stackSize]);
}

ID *searchIDList(IDList *idList, char* idName, int isFuncID){
    ID *id = NULL;
    if (idList && idList->id){
        if(!strcmp(idList->id->name, idName) && ((isFuncID && idList->id->type == FuncID) || (!isFuncID && idList->id->type != FuncID))) id = idList->id;
        else id = searchIDList(idList->idList, idName, isFuncID);
    }
    return id;
}

ID *searchIDListStack(IDListStack *ids, char *idName, int isFuncID){
    ID *id = NULL;
    for(int i = ids->stackSize - 1; i >= 0; i--){
        id = searchIDList(ids->stack[i], idName, isFuncID);
        if(id) return id;
    }
    return id;
}

void addIDtoIDList(IDList **idList, ID *id){
    IDList *newIDList;
    newIDList = malloc(sizeof(IDList));
    newIDList->idList = *idList;
    newIDList->id = id;
    *idList = newIDList;
}

void addIDtoIDListStack(IDListStack *ids, ID *id){
    addIDtoIDList(&(ids->stack[ids->stackSize - 1]), id);
}

Prog *parseProg(Scanner *scanner){
    Prog *p = malloc(sizeof(Prog));
    IDListStack *ids = initIDListStack(&ids);
    p->declSeq = NULL;

    if (currentToken(scanner) != PROGRAM){
        printf("ERROR: Expected PROGRAM token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    if (nextToken(scanner) != BEGIN){
        p->declSeq = parseDeclSeq(scanner, ids);
        
    }
    if (currentToken(scanner) != BEGIN){
        printf("ERROR: Expected BEGIN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume BEGIN
    nextToken(scanner);

    addScopeToIDListStack(ids);
    

    p->stmtSeq = parseStmtSeq(scanner, ids);

    if (currentToken(scanner) != END){
        printf("ERROR: Expected END token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume END
    nextToken(scanner);

    //Check EOS
    if (currentToken(scanner) != EOS){
        printf("ERROR: Expected EOS but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    
    return p;
}

DeclSeq *parseDeclSeq(Scanner *scanner, IDListStack *ids){
    DeclSeq *d = malloc(sizeof(DeclSeq));
    d->decl = NULL;
    d->declSeq = NULL;
    d->funcDecl = NULL;

    if (currentToken(scanner) == INT || currentToken(scanner) == REF){
        d->decl = parseDecl(scanner, ids);
    }
    else {
        d->funcDecl = parseFuncDecl(scanner, ids);
    }
    if (currentToken(scanner) != BEGIN) d->declSeq = parseDeclSeq(scanner, ids);

    return d;
}

Decl *parseDecl(Scanner *scanner, IDListStack *ids){
    Decl *d = malloc(sizeof(Decl));
    d->declInt = NULL;
    d->declRef = NULL;

    if (currentToken(scanner) == INT) d->declInt = parseDeclInt(scanner, ids);
    
    else d->declRef = parseDeclRef(scanner, ids);
    
    return d;
}

DeclInt *parseDeclInt(Scanner *scanner, IDListStack *ids){
    DeclInt *d = malloc(sizeof(DeclInt));
    d->idList = NULL;

    if(currentToken(scanner) != INT){
        printf("ERROR: Expected INT token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume INT
    nextToken(scanner);

    d->idList = parseIDList(scanner, ids, IntID);

    if(currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume SEMICOLON
    nextToken(scanner);

    return d;
}

DeclRef *parseDeclRef(Scanner *scanner, IDListStack *ids){
    DeclRef *d = malloc(sizeof(DeclRef));
    d->idList = NULL;

    if(currentToken(scanner) != REF){
        printf("ERROR: Expected REF token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume REF
    nextToken(scanner);

    d->idList = parseIDList(scanner, ids, RefID);

    if(currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    //Consume SEMICOLON
    nextToken(scanner);

    return d;
}

IDList *parseIDList(Scanner *scanner, IDListStack *ids, idType type){
    IDList *idList = malloc((sizeof(IDList)));
    idList->idList = NULL;
    idList->id = malloc(sizeof(ID));
    idList->id->name = malloc(MAX_ID_LENGTH * sizeof(char));

    if(currentToken(scanner) != IDENTIFIER){
        printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }
    const char *name = getID(scanner);
    strcpy(idList->id->name, name);
    idList->id->type = type;

    addIDtoIDListStack(ids, idList->id);
    //Consume ID
    nextToken(scanner);

    if(currentToken(scanner) != COMMA && currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected COMMA or SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(0);
    }

    if(currentToken(scanner) == COMMA){
        //Consume COMMA
        nextToken(scanner);

        idList->idList = parseIDList(scanner, ids, type);
    }
    return idList;
}

StmtSeq *parseStmtSeq(Scanner *scanner, IDListStack *ids){
    StmtSeq *s = malloc(sizeof(StmtSeq));
    s->stmtSeq = NULL;
    
    s->stmt = parseStmt(scanner, ids);

    if (currentToken(scanner) != END && currentToken(scanner) != RBRACE){
        s->stmtSeq = parseStmtSeq(scanner, ids);
    }

    return s;
}

Stmt *parseStmt(Scanner *scanner, IDListStack *ids){
    Stmt *s = malloc(sizeof(Stmt));
    s->assign = NULL;
    s->decl = NULL;
    s->funcCall = NULL;
    s->ifStmt = NULL;
    s->loop = NULL;
    s->out = NULL;

    tokenType current = currentToken(scanner);
    
    switch (current){
    case IDENTIFIER:
        s->assign = parseAssign(scanner, ids);

        break;
    case INT:
    case REF:
        s->decl = parseDecl(scanner, ids);
        break;
    case BEGIN:
        s->funcCall = parseFuncCall(scanner, ids);
        break;
    case IF:
        s->ifStmt = parseIf(scanner, ids);
        break;
    case WHILE:
        s->loop = parseLoop(scanner, ids);
        break;
    case OUTPUT:
        s->out = parseOut(scanner, ids);
        break;
    default:
        printf("ERROR: Expected ID, INT, REF, BEGIN, IF, WHILE, or OUT token but got %s.\n", tokenToString(current));
        break;
    }

    return s;
}

Assign *parseAssign(Scanner *scanner, IDListStack *ids){
    Assign *a = malloc(sizeof(Assign));
    a->expr = NULL;
    a->id2 = NULL;
    a->inputAssignment = 0;
    a->id = malloc(sizeof(ID));
    a->addOp = 0;
    

    if (currentToken(scanner) != IDENTIFIER){
        printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }

    a->id = searchIDListStack(ids, getID(scanner), 0);
    if(a->id == NULL){
        printf("ERROR: ID %s not declared.\n", getID(scanner));
        exit(1);
    }

    //Consume ID
    nextToken(scanner);

    if(currentToken(scanner) != ASSIGN){
        printf("ERROR: Expected ASSIGN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }

    //Consume ASSIGN
    nextToken(scanner);

    tokenType token = currentToken(scanner);
    switch (token){
    case INPUT:
        a->inputAssignment = 1;
        //Consume INPUT
        nextToken(scanner);
        if(currentToken(scanner) != LPAREN){
            printf("ERROR: Expected LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume LPAREN
        nextToken(scanner);
        if(currentToken(scanner) != RPAREN){
            printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume RPAREN
        nextToken(scanner);
        break;
    case NEW:
        if(a->id->type != RefID){
            printf("ERROR: Assignment of INT variable, %s, to new class.\n", a->id2->name);
            exit(1);
        }

        //Consume NEW
        nextToken(scanner);
        if(currentToken(scanner) != CLASS){
            printf("ERROR: Expected CLASS token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume CLASS
        nextToken(scanner);

        break;
    case SHARE:
        if(a->id->type != RefID){
            printf("ERROR: Share assignment on INT variable, %s.\n", a->id->name);
            exit(1);
        }

        //Consume SHARE
        nextToken(scanner);
        if(currentToken(scanner) != IDENTIFIER){
            printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        a->id2 = searchIDListStack(ids, getID(scanner), 0);
        if(a->id2 == NULL){
            printf("ERROR: ID %s not declared.\n", getID(scanner));
            exit(1);
        }
        if(a->id->type != RefID){
            printf("ERROR: Share assignment from INT variable, %s.\n", a->id2->name);
            exit(1);
        }
        //Consume ID
        nextToken(scanner);
        
        if(currentToken(scanner) == ADD || currentToken(scanner) == SUB){
            if(currentToken(scanner) == ADD) a->addOp = 1;
            //Consume operator
            nextToken(scanner);

            a->expr = parseExpr(scanner, ids);
        }

        break;
    default:
        a->expr = parseExpr(scanner, ids);
        break;
    }
    if(currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume SEMICOLON
    nextToken(scanner);
    return a;
}

If *parseIf(Scanner *scanner, IDListStack *ids){
    If *i = malloc(sizeof(If));
    i->elseStmtSeq = NULL;
    
    if (currentToken(scanner) != IF){
        printf("ERROR: Expected IF token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume IF
    nextToken(scanner);

    i->cond = parseCond(scanner, ids);

    if (currentToken(scanner) != THEN){
        printf("ERROR: Expected THEN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume THEN
    nextToken(scanner);

    if (currentToken(scanner) != LBRACE){
        printf("ERROR: Expected LBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LBRACE
    nextToken(scanner);

    addScopeToIDListStack(ids);
    i->ifStmtSeq = parseStmtSeq(scanner, ids);
    removeScopeFromIDListStack(ids);
    
    if (currentToken(scanner) != RBRACE){
        printf("ERROR: Expected RBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RBRACE
    nextToken(scanner);

    if (currentToken(scanner) == ELSE){
        //Consume ELSE
        nextToken(scanner);
        if (currentToken(scanner) != LBRACE){
            printf("ERROR: Expected LBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume LBRACE
        nextToken(scanner);

        addScopeToIDListStack(ids);
        i->elseStmtSeq = parseStmtSeq(scanner, ids);
        removeScopeFromIDListStack(ids);

        if (currentToken(scanner) != RBRACE){
            printf("ERROR: Expected RBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume RBRACE
        nextToken(scanner);
    }
    return i;
}

Loop *parseLoop(Scanner *scanner, IDListStack *ids){
    Loop *l = malloc(sizeof(Loop));
    
    if (currentToken(scanner) != WHILE){
        printf("ERROR: Expected WHILE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume WHILE
    nextToken(scanner);

    l->cond = parseCond(scanner, ids);

    if (currentToken(scanner) != LBRACE){
        printf("ERROR: Expected LBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LBRACE
    nextToken(scanner);

    addScopeToIDListStack(ids);
    l->stmtSeq = parseStmtSeq(scanner, ids);
    removeScopeFromIDListStack(ids);
    
    if (currentToken(scanner) != RBRACE){
        printf("ERROR: Expected RBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RBRACE
    nextToken(scanner);

    return l;
}

Cond *parseCond(Scanner *scanner, IDListStack *ids){
    Cond *c = malloc(sizeof(Cond));
    c->cmpr = NULL;
    c->cond = NULL;

    if(currentToken(scanner) == NEGATION){
        //Consume NEGATION
        nextToken(scanner);

        if (currentToken(scanner) != LPAREN){
            printf("ERROR: Expected LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume LPAREN
        nextToken(scanner);

        c->cond = parseCond(scanner, ids);

        if (currentToken(scanner) != RPAREN){
            printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume RPAREN
        nextToken(scanner);
    }
    else{
        c->cmpr = parseCmpr(scanner, ids);
        
        if (currentToken(scanner) == OR){
            //Consume OR
            nextToken(scanner);

            c->cond = parseCond(scanner, ids);
        }
    }
    return c;
}

Cmpr *parseCmpr(Scanner *scanner, IDListStack *ids){
    Cmpr *c = malloc(sizeof(Cmpr));
    c->expr1 = parseExpr(scanner, ids);

    if(currentToken(scanner) != LESS && currentToken(scanner) != LESSEQUAL && currentToken(scanner) != EQUAL){
        printf("ERROR: Expected LESS, LESSEQUAL, or EQUAL token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    c->operator = currentToken(scanner);
    //Consume operator
    nextToken(scanner);

    c->expr2 = parseExpr(scanner, ids);
    return c;
}

Expr *parseExpr(Scanner *scanner, IDListStack *ids){
    Expr *e = malloc(sizeof(Expr));
    e->expr = NULL;
    e->addOperator = 0;
    e->term = parseTerm(scanner, ids);

    if(currentToken(scanner) == ADD || currentToken(scanner) == SUB){
        if(currentToken(scanner) == ADD) e->addOperator = 1;
        //Consume ADD or SUB
        nextToken(scanner);
        e->expr = parseExpr(scanner, ids);
    }
    return e;
}

Term *parseTerm(Scanner *scanner, IDListStack *ids){
    Term *t = malloc(sizeof(Term));
    t->term = NULL;
    t->factor = parseFactor(scanner, ids);

    if(currentToken(scanner) == MULT){
        //Consume MULT
        nextToken(scanner);
        t->term = parseTerm(scanner, ids);
    }
    return t;
}

Factor *parseFactor(Scanner *scanner, IDListStack *ids){
    Factor *f = malloc(sizeof(Factor));
    f->constVal = 0;
    f->expr = NULL;
    f->id = NULL;

    tokenType token = currentToken(scanner);
    switch (token){
    case IDENTIFIER:
        f->id = searchIDListStack(ids, getID(scanner), 0);
        if(f->id == NULL){
            printf("ERROR: ID %s not declared.\n", getID(scanner));
            exit(1);
        }
        //Consume ID
        nextToken(scanner);
        break;
    case CONST:
        f->constVal = getCONST(scanner);
        //Consume CONST
        nextToken(scanner);
        break;
    case LPAREN:
        //Consume LPAREN
        nextToken(scanner);
        f->expr = parseExpr(scanner, ids);
        if(currentToken(scanner) != RPAREN){
            printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
            exit(1);
        }
        //Consume RPAREN
        nextToken(scanner);
        break;
    default:
        printf("ERROR: Expected ID, CONST, or LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
        break;
    }
    return f;
}

Out *parseOut(Scanner *scanner, IDListStack *ids){
    Out *o = malloc(sizeof(Out));
    if(currentToken(scanner) != OUTPUT){
        printf("ERROR: Expected OUTPUT token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume OUTPUT
    nextToken(scanner);
    
    if(currentToken(scanner) != LPAREN){
        printf("ERROR: Expected LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LPAREN
    nextToken(scanner);

    o->expr = parseExpr(scanner, ids);

    if(currentToken(scanner) != RPAREN){
        printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RPAREN
    nextToken(scanner);
    
    if(currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume SEMICOLON
    nextToken(scanner);
    return o;
}

FuncDecl *parseFuncDecl(Scanner *scanner, IDListStack *ids){
    FuncDecl *f = malloc(sizeof(FuncDecl));
    f->id = malloc(sizeof(ID));
    f->id->name = malloc(MAX_ID_LENGTH * sizeof(char));

    if(currentToken(scanner) != IDENTIFIER){
        printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    const char *name = getID(scanner);
    strcpy(f->id->name, name);
    
    f->id->type = FuncID;
    //Consume IDENTIFIER
    nextToken(scanner);

    addIDtoIDListStack(ids, f->id);

    if(currentToken(scanner) != LPAREN){
        printf("ERROR: Expected LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LPAREN
    nextToken(scanner);

    if(currentToken(scanner) != REF){
        printf("ERROR: Expected REF token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume REF
    nextToken(scanner);

    f->formals = parseFormals(scanner, ids);

    if(currentToken(scanner) != RPAREN){
        printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RPAREN
    nextToken(scanner);
    

    if(currentToken(scanner) != LBRACE){
        printf("ERROR: Expected LBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LBRACE
    nextToken(scanner);

    addScopeToIDListStack(ids);
    f->stmtSeq = parseStmtSeq(scanner, ids);
    removeScopeFromIDListStack(ids);

    if(currentToken(scanner) != RBRACE){
        printf("ERROR: Expected RBRACE token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RBRACE
    nextToken(scanner);  

    return f;
}

Formals *parseFormals(Scanner *scanner, IDListStack *ids){
    Formals *f = malloc(sizeof(Formals));
    f->formals = NULL;
    f->id = malloc(sizeof(ID));
    f->id->name = malloc(MAX_ID_LENGTH * sizeof(char));

    if(currentToken(scanner) != IDENTIFIER){
        printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }

    const char *name = getID(scanner);
    strcpy(f->id->name, name);
    f->id->type = RefID;
    addIDtoIDListStack(ids, f->id);

    //Consume ID
    nextToken(scanner);


    if(currentToken(scanner) == COMMA){
        //Consume COMMA
        nextToken(scanner);

        f->formals = parseFormals(scanner, ids);
    }

    return f;
}

FuncCall *parseFuncCall(Scanner *scanner, IDListStack *ids){
    FuncCall *f = malloc(sizeof(FuncCall));
    
    if(currentToken(scanner) != BEGIN){
        printf("ERROR: Expected BEGIN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume BEGIN
    nextToken(scanner);

    if(currentToken(scanner) != IDENTIFIER){
        printf("ERROR: Expected ID token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    f->id = searchIDListStack(ids, getID(scanner), 1);
    if(!f->id){
        printf("ERROR: function %s not defined.\n", getID(scanner));
        exit(1);
    }
    
    //Consume ID
    nextToken(scanner);

    if(currentToken(scanner) != LPAREN){
        printf("ERROR: Expected LPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume LPAREN
    nextToken(scanner);

    f->formals = parseFormals(scanner, ids);

    if(currentToken(scanner) != RPAREN){
        printf("ERROR: Expected RPAREN token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume RPAREN
    nextToken(scanner);

    if(currentToken(scanner) != SEMICOLON){
        printf("ERROR: Expected SEMICOLON token but got %s.\n", tokenToString(currentToken(scanner)));
        exit(1);
    }
    //Consume SEMICOLON
    nextToken(scanner);

    return f;
}