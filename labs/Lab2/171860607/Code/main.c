#include<stdio.h>
#include "syntax.tab.h"
#include "lab2.h"

extern FILE* yyin;
extern int yylineno;
extern int iferror;
extern void yyrestart(FILE*);
extern struct TreeNode * root;
extern void printTree(struct TreeNode * tree,int depth);
int main(int argc, char** argv) {
    if (argc <= 1) { return 1;}
    FILE* f=fopen(argv[1],"r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yylineno=1;
    yyrestart(f);
    yyparse();
    if(!iferror){
        //printTree(root,0);
        symbolTable(root);
    }
    return 0;
}