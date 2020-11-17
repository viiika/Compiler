#include "lab3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tmp_var_no = 1;
int var_no = 1;
int label_no = 1;

InterCode interCode = NULL;
InterCode interCodetail=NULL;

void insertInterCodes(InterCode ic)
{
    //printf("ic\n");
    
    if(interCode==NULL){
        ic->next=NULL;
        ic->prev=NULL;
        //interCodes=(InterCodes)malloc(sizeof(struct InterCodes_));
        //interCodes->code=ic;
        interCode=ic;
        interCodetail=ic;

        //curInterCodes=interCodes;
    }
    else{
        //InterCodes newInterCodes = (InterCodes)malloc(sizeof(struct InterCodes_));
        //newInterCodes->code=ic;
        //newInterCodes->next=NULL;
        //newInterCodes->prev=curInterCodes;
        //curInterCodes->next=newInterCodes;
        //curInterCodes=curInterCodes->next;
        ic->prev=interCodetail;
        ic->next=NULL;
        interCodetail->next=ic;
        interCodetail=ic;

    }
    
    // if (interCode == NULL)
    // {
    //     interCode = ic;
    //     interCode->prev = interCode; //head->prev=tail, however tail->next=NULL
    //     interCode->next = NULL;
    // }
    // else
    // {
    //     interCode->prev->next = ic;
    //     ic->prev = interCode->prev;
    //     ic->next = NULL;
    //     interCode->prev = ic;
    // }
}
void deleteInterCodes(InterCode ic)
{
    // if (ic == interCode)
    // {
    //     ic->next->prev = ic->prev;
    // }
    // else if (ic == interCode->prev)
    // {
    //     interCode->prev = ic->prev;
    //     ic->prev->next = NULL;
    // }
    // else
    // {
    //     ic->prev->next = ic->next;
    //     ic->next->prev = ic->prev;
    // }
    if (ic == interCode)
    {
        interCode=interCode->next;
        interCode->prev=NULL;
    }
    else if (ic == interCodetail)
    {
        interCodetail=interCodetail->prev;
        interCodetail->next=NULL;
    }
    else
    {
        ic->prev->next = ic->next;
        ic->next->prev = ic->prev;
    }
}

void fprintInterCode(char *filename)
{
    //optimizeInterCode();
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        return;
    }
    InterCode curInterCode = interCode;
    while (curInterCode != NULL)
    {
        switch (curInterCode->kind)
        {
        case IC_LABEL:
            fprintf(fp, "LABEL ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            fprintf(fp, " : ");
            break;
        case IC_FUNCTION:
            fprintf(fp, "FUNCTION ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            fprintf(fp, " : ");
            break;
        case IC_ASSIGN:
            fprintOperand(fp, curInterCode->u.binop.left);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.binop.right);
            break;
        case IC_ADD:
            fprintOperand(fp, curInterCode->u.triop.result);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, " + ");
            fprintOperand(fp, curInterCode->u.triop.op2);
            break;
        case IC_SUB:
            fprintOperand(fp, curInterCode->u.triop.result);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, " - ");
            fprintOperand(fp, curInterCode->u.triop.op2);
            break;
        case IC_MUL:
            fprintOperand(fp, curInterCode->u.triop.result);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, " * ");
            fprintOperand(fp, curInterCode->u.triop.op2);
            break;
        case IC_DIV:
            fprintOperand(fp, curInterCode->u.triop.result);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, " / ");
            fprintOperand(fp, curInterCode->u.triop.op2);
            break;
        case IC_GET_ADDR1:
            fprintOperand(fp, curInterCode->u.binop.left);
            fprintf(fp, " := &");
            fprintOperand(fp, curInterCode->u.binop.right);
            break;
        case IC_GET_ADDR2:
            fprintOperand(fp, curInterCode->u.triop.result);
            fprintf(fp, " := &");
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, " + ");
            fprintOperand(fp, curInterCode->u.triop.op2);
            break;
        case IC_GET_VAR:
            fprintOperand(fp, curInterCode->u.binop.left);
            fprintf(fp, " := *");
            fprintOperand(fp, curInterCode->u.binop.right);
            break;
        case IC_TO_VAR:
            fprintf(fp, "*");
            fprintOperand(fp, curInterCode->u.binop.left);
            fprintf(fp, " := ");
            fprintOperand(fp, curInterCode->u.binop.right);
            break;
        case IC_GOTO:
            fprintf(fp, "GOTO ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;
        case IC_IFGOTO:
            fprintf(fp, "IF ");
            fprintOperand(fp, curInterCode->u.ifgotoop.op1);
            fprintf(fp, " %s ", curInterCode->u.ifgotoop.relop);
            fprintOperand(fp, curInterCode->u.ifgotoop.op2);
            fprintf(fp, " GOTO ");
            fprintOperand(fp, curInterCode->u.ifgotoop.result);
            break;
        case IC_RETURN:
            fprintf(fp, "RETURN ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;
        case IC_DEC:
            fprintf(fp, "DEC ");
            fprintOperand(fp, curInterCode->u.decop.op1);
            fprintf(fp, " %d ", curInterCode->u.decop.size);
            break;
        case IC_ARG:
            fprintf(fp, "ARG ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;
        case IC_CALL:
            fprintOperand(fp, curInterCode->u.binop.left);
            fprintf(fp, " := CALL ");
            fprintOperand(fp, curInterCode->u.binop.right);
            break;
        case IC_PARAM:
            fprintf(fp, "PARAM ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;
        case IC_READ:
            fprintf(fp, "READ ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;
        case IC_WRITE:
            fprintf(fp, "WRITE ");
            fprintOperand(fp, curInterCode->u.sinop.op1);
            break;

        default:
            break;
        }
        fprintf(fp, "\n");
        curInterCode = curInterCode->next;
    }
    fclose(fp);
}

void fprintOperand(FILE *fp, struct Operand_ *op)
{
    if (fp == NULL || op == NULL)
    {
        return;
    }
    switch (op->kind)
    {
    case OP_VARIABLE:
        fprintf(fp, "v%s", op->u.value);//add v
        break;
    case OP_CONSTANT:
        fprintf(fp, "#%s", op->u.value);
        break;
    case OP_ADDRESS:
        fprintf(fp, "*v%s", op->u.real_value->u.value);//add v
        break;
    case OP_TMP_VARIABLE:
        fprintf(fp, "t%d", op->u.var_no);
        break;
    case OP_TMP_ADDRESS:
        fprintf(fp, "*t%d", op->u.real_value->u.var_no);
        break;
    case OP_FUNCTION:
        fprintf(fp, "%s", op->u.value);
        break;
    case OP_LABEL:
        fprintf(fp, "label%d", op->u.var_no);
        break;
    default:
        break;
    }
}
void optimizeInterCode(){
    InterCode ic=interCode;
    for(;ic!=NULL;ic=ic->next){
        if(ic->kind==IC_IFGOTO){
            InterCode ic1=ic;
            InterCode ic2=ic->next;
            if(ic2==NULL){
                continue;
            }
            InterCode ic3=ic->next->next;
            if(ic3==NULL){
                continue;
            }
            if(ic2->kind==IC_GOTO&&ic3->kind==IC_LABEL&&ic1->u.ifgotoop.result==ic3->u.sinop.op1){
                ic1->u.ifgotoop.result=ic2->u.sinop.op1;
                deleteInterCodes(ic2);
                if(strcmp(ic1->u.ifgotoop.relop,"==")==0){
                    strcpy(ic1->u.ifgotoop.relop,"!=");
                }
                else if(strcmp(ic1->u.ifgotoop.relop,"!=")==0){
                    strcpy(ic1->u.ifgotoop.relop,"==");
                }
                else if(strcmp(ic1->u.ifgotoop.relop,">=")==0){
                    strcpy(ic1->u.ifgotoop.relop,"<");
                }
                else if(strcmp(ic1->u.ifgotoop.relop,"<=")==0){
                    strcpy(ic1->u.ifgotoop.relop,">");
                }
                else if(strcmp(ic1->u.ifgotoop.relop,">")==0){
                    strcpy(ic1->u.ifgotoop.relop,"<=");
                }
                else if(strcmp(ic1->u.ifgotoop.relop,"<")==0){
                    strcpy(ic1->u.ifgotoop.relop,">=");
                }
            }
        }
        else if(ic->kind==IC_GOTO){
            InterCode ic1=ic;
            InterCode ic2=ic->next;
            if(ic2==NULL){
                continue;
            }
            if(ic2->kind==IC_LABEL&&ic1->u.sinop.op1==ic2->u.sinop.op1){
                deleteInterCodes(ic1);
            }
        }
        else if(ic->kind==IC_ADD||ic->kind==IC_SUB||ic->kind==IC_MUL||ic->kind==IC_DIV){
            Operand op1=ic->u.triop.op1;
            Operand op2=ic->u.triop.op2;
            Operand result=ic->u.triop.result;

            if(result->kind==OP_TMP_VARIABLE&&op1->kind==OP_CONSTANT&&op2->kind==OP_CONSTANT){
                switch (ic->kind)
                {
                case IC_ADD:sprintf(result->u.value,"%d",atoi(op1->u.value)+atoi(op2->u.value));
                    break;
                case IC_SUB:sprintf(result->u.value,"%d",atoi(op1->u.value)-atoi(op2->u.value));
                    break;
                case IC_MUL:sprintf(result->u.value,"%d",atoi(op1->u.value)*atoi(op2->u.value));
                    break;
                case IC_DIV:
                    if(atoi(op2->u.value)==0){
                        sprintf(result->u.value,"%d",0);//undifined
                    }
                    else{
                        sprintf(result->u.value,"%d",atoi(op1->u.value)/atoi(op2->u.value));
                    }
                    break;
                default:
                    break;
                }
                result->kind=OP_CONSTANT;
                InterCode ic_tmp=ic;
                ic=ic->prev;
                deleteInterCodes(ic_tmp);
            }

        }

    }
    //delete some label
    Operand labelnum=(Operand)malloc(sizeof(struct Operand_));
    labelnum->u.var_no=-1;
    labelnum->next=NULL;
    Operand labelnumtail=labelnum;
    for(ic=interCode;ic!=NULL;ic=ic->next){
        if(ic->kind==IC_GOTO){
            Operand ln=(Operand)malloc(sizeof(struct Operand_));
            ln->u.var_no=ic->u.sinop.op1->u.var_no;
            ln->next=NULL;
            labelnumtail->next=ln;
            labelnumtail=ln;
        }
        else if(ic->kind==IC_IFGOTO){
            Operand ln=(Operand)malloc(sizeof(struct Operand_));
            ln->u.var_no=ic->u.ifgotoop.result->u.var_no;
            ln->next=NULL;
            labelnumtail->next=ln;
            labelnumtail=ln;
        }
    }
    for(ic=interCode;ic!=NULL;ic=ic->next){
        if(ic->kind==IC_LABEL){
            int no=ic->u.sinop.op1->u.var_no;
            Operand ln=labelnum;
            while(ln!=NULL){
                if(ln->u.var_no==no){
                    break;
                }
                ln=ln->next;
            }
            InterCode ic_tmp=ic;
            if(ln==NULL){
                ic=ic->prev;
                deleteInterCodes(ic_tmp);
            }
        }
    }
    labelnumtail=NULL;
    while (labelnum!=NULL)
    {
        Operand t=labelnum;
        labelnum=labelnum->next;
        free(t);
    }
    
}