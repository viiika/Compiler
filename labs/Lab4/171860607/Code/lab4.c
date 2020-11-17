
#include "lab4.h"
VAR varlist = NULL;
VAR vartail = NULL;
VAR memlist = NULL;
VAR memtail = NULL;
REG reg[32];
int last_modify_reg = -1;
int ifInFunc = 0;
int argNum = 0;
int overOFFSET = 0;
char *curFunc = NULL;
extern InterCode interCode;
extern InterCode interCodetail;
int ifgotopack = 0;
int varietyNum = 0;
void addoperand(Operand op)
{
    if (op->counted == 0)
    {
        if (op->kind == OP_VARIABLE || op->kind == OP_ADDRESS)
        {
            op->counted = 1;
            varietyNum += 1;
        }
    }
}
int curfreeRegNum = 9; //9,10
int pianyiliang = 0;

void preprocess(FILE *fp)
{
    for (int i = 0; i < 32; i++)
    {
        reg[i] = (REG)malloc(sizeof(struct REG_));
        reg[i]->free = 1;
    }
    //fig. 9
    reg[0]->name = "$zero";
    reg[1]->name = "$at";
    reg[2]->name = "$v0";
    reg[3]->name = "$v1";
    reg[4]->name = "$a0";
    reg[5]->name = "$a1";
    reg[6]->name = "$a2";
    reg[7]->name = "$a3";
    reg[8]->name = "$t0";
    reg[9]->name = "$t1";
    reg[10]->name = "$t2";
    reg[11]->name = "$t3";
    reg[12]->name = "$t4";
    reg[13]->name = "$t5";
    reg[14]->name = "$t6";
    reg[15]->name = "$t7";
    reg[16]->name = "$s0";
    reg[17]->name = "$s1";
    reg[18]->name = "$s2";
    reg[19]->name = "$s3";
    reg[20]->name = "$s4";
    reg[21]->name = "$s5";
    reg[22]->name = "$s6";
    reg[23]->name = "$s7";
    reg[24]->name = "$t8";
    reg[25]->name = "$t9";
    reg[26]->name = "$k0";
    reg[27]->name = "$k1";
    reg[28]->name = "$gp";
    reg[29]->name = "$sp";
    reg[30]->name = "$fp";
    reg[31]->name = "$ra";

    fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, ".globl main\n");
    InterCode tic = interCode;
    while (tic != NULL)
    {
        if (tic->kind == IC_DEC)
        {
            //fprintOperand(fp,tic->u.decop.op1);
            fprintf(fp, "t%d: .word %d\n", tic->u.decop.op1->u.var_no, tic->u.decop.size);
            //fprintf(fp, "t%d: .word %d\n",tic->u.decop.op1->u.var_no,tic->u.decop.size);
        }
        tic = tic->next;
    }
    tic = interCode;
    while (tic != NULL)
    {
        switch (tic->kind)
        {
        case IC_LABEL:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_FUNCTION:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_ASSIGN:
            tic->u.binop.left->counted = 0;
            tic->u.binop.right->counted = 0;
            break;
        case IC_ADD:
            tic->u.triop.result->counted = 0;
            tic->u.triop.op1->counted = 0;
            tic->u.triop.op2->counted = 0;
            break;
        case IC_SUB:
            tic->u.triop.result->counted = 0;
            tic->u.triop.op1->counted = 0;
            tic->u.triop.op2->counted = 0;
            break;
        case IC_MUL:
            tic->u.triop.result->counted = 0;
            tic->u.triop.op1->counted = 0;
            tic->u.triop.op2->counted = 0;
            break;
        case IC_DIV:
            tic->u.triop.result->counted = 0;
            tic->u.triop.op1->counted = 0;
            tic->u.triop.op2->counted = 0;
            break;
        case IC_GET_ADDR1:
            tic->u.binop.left->counted = 0;
            tic->u.binop.right->counted = 0;
            break;
        case IC_GET_ADDR2:
            tic->u.triop.result->counted = 0;
            tic->u.triop.op1->counted = 0;
            tic->u.triop.op2->counted = 0;
            break;
        case IC_GET_VAR:
            tic->u.binop.left->counted = 0;
            tic->u.binop.right->counted = 0;
            break;
        case IC_TO_VAR:
            tic->u.binop.left->counted = 0;
            tic->u.binop.right->counted = 0;
            break;
        case IC_GOTO:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_IFGOTO:
            tic->u.ifgotoop.op1->counted = 0;
            tic->u.ifgotoop.op2->counted = 0;
            tic->u.ifgotoop.result->counted = 0;
            break;
        case IC_RETURN:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_DEC:
            tic->u.decop.op1->counted = 0;
            break;
        case IC_ARG:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_CALL:
            tic->u.binop.left->counted = 0;
            tic->u.binop.right->counted = 0;
            break;
        case IC_PARAM:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_READ:
            tic->u.sinop.op1->counted = 0;
            break;
        case IC_WRITE:
            tic->u.sinop.op1->counted = 0;
            break;

        default:
            break;
        }
        //fprintf(fp, "\n");
        tic = tic->next;
    }
    tic = interCode;
    while (tic != NULL)
    {
        switch (tic->kind)
        {
        case IC_LABEL:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_FUNCTION:
            addoperand(tic->u.sinop.op1);

            break;
        case IC_ASSIGN:
            addoperand(tic->u.binop.left);
            addoperand(tic->u.binop.right);

            break;
        case IC_ADD:
        case IC_SUB:
        case IC_MUL:
        case IC_DIV:
            addoperand(tic->u.triop.result);
            addoperand(tic->u.triop.op1);
            addoperand(tic->u.triop.op2);
            break;
        case IC_GET_ADDR1:
            addoperand(tic->u.binop.left);
            addoperand(tic->u.binop.right);
            break;
        case IC_GET_ADDR2:
            addoperand(tic->u.triop.result);
            addoperand(tic->u.triop.op1);
            addoperand(tic->u.triop.op2);
            break;
        case IC_GET_VAR:
        case IC_TO_VAR:
            addoperand(tic->u.binop.left);
            addoperand(tic->u.binop.right);
            break;
        case IC_GOTO:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_IFGOTO:
        IC_DIV:
            addoperand(tic->u.ifgotoop.result);
            addoperand(tic->u.ifgotoop.op1);
            addoperand(tic->u.ifgotoop.op2);
            break;
        case IC_RETURN:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_DEC:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_ARG:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_CALL:
            addoperand(tic->u.binop.left);
            addoperand(tic->u.binop.right);
            break;
        case IC_PARAM:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_READ:
            addoperand(tic->u.sinop.op1);
            break;
        case IC_WRITE:
            addoperand(tic->u.sinop.op1);
            break;

        default:
            break;
        }
        //fprintf(fp, "\n");
        tic = tic->next;
    }

    fprintf(fp, "jbxxncm54: .word %d\n", varietyNum * 4);
    //fprintf(fp, "jbxxncm55: .word %d\n",20);
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n");
    fprintf(fp, "\n");
    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0, 1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0, $0\n");
    fprintf(fp, "  jr $ra\n");
}

int saveReg(FILE *fp, struct Operand_ *op, char *name)
{
    //printf("savereg\n");
    for (int i = 11; i < 26; i++)
    {
        if (reg[i]->free == 1)
        {
            reg[i]->free = 0;
            VAR myvar = (VAR)malloc(sizeof(struct VAR_));
            myvar->next = NULL;
            myvar->op = op;
            myvar->reg = i;
            myvar->offset = -996;
            myvar->name = malloc(40);
            sprintf(myvar->name, "%s", name);
            if (varlist == NULL)
            {
                varlist = myvar;
                vartail = myvar;
            }
            else
            {
                vartail->next = myvar;
                vartail = myvar;
                //fprintf(fp,"%s\n",vartail->name);
            }

            return i;
        }
    }
    //not found free reg
    VAR curVar = varlist;
    while (curVar != NULL) //first delete the constant then delete the tmp var
    {
        if (curVar->op->kind == OP_CONSTANT && curVar->reg != last_modify_reg && curVar->reg > 11)
        {
            last_modify_reg = curVar->reg;
            if (curVar == varlist)
            {
                varlist = varlist->next;
            }
            else
            {
                VAR tmp = varlist;
                while (tmp != NULL && tmp->next != NULL && tmp->next != curVar)
                {
                    tmp = tmp->next;
                }
                tmp->next = curVar->next;
                //free(curVar);
            }
            VAR myvar = (VAR)malloc(sizeof(struct VAR_));
            myvar->next = NULL;
            myvar->op = op;
            myvar->reg = curVar->reg;
            myvar->offset = -996;
            myvar->name = name;
            //reg[curVar->reg]->name=name;
            reg[curVar->reg]->free = 0;
            if (varlist == NULL)
            {
                varlist = myvar;
                vartail = myvar;
            }
            else
            {
                vartail->next = myvar;
                vartail = myvar;
            }

            return curVar->reg;
        }
        curVar = curVar->next;
    }
    curVar = varlist;
    while (curVar != NULL)
    {
        if (curVar->op->kind != OP_CONSTANT && (curVar->op->kind == OP_TMP_VARIABLE || curVar->op->kind == OP_TMP_ADDRESS) && curVar->reg != last_modify_reg && curVar->reg > 11)
        {
            last_modify_reg = curVar->reg;
            if (curVar == varlist)
            {
                varlist = varlist->next;
            }
            else
            {
                VAR tmp = varlist;
                while (tmp != NULL && tmp->next != NULL && tmp->next != curVar)
                {
                    tmp = tmp->next;
                }
                tmp->next = curVar->next;
                //free(curVar);
            }
            VAR myvar = (VAR)malloc(sizeof(struct VAR_));
            myvar->next = NULL;
            myvar->op = op;
            myvar->reg = curVar->reg;
            myvar->offset = -996;
            myvar->name = name;
            //reg[curVar->reg]->name=name;
            reg[curVar->reg]->free = 0;
            if (varlist == NULL)
            {
                varlist = myvar;
                vartail = myvar;
            }
            else
            {
                vartail->next = myvar;
                vartail = myvar;
            }

            return curVar->reg;
        }
        curVar = curVar->next;
    }
    //all regs are full
    //printf("??? %d\n",curfreeRegNum);
    VAR ttt = varlist;
    while (ttt != NULL)
    {
        if (ttt->reg == curfreeRegNum)
        {
            //fprintf(fp,"here\n");
            if (ttt->op->kind == OP_CONSTANT)
            { //||ttt->op->kind==OP_TMP_ADDRESS||ttt->op->kind==OP_TMP_VARIABLE){
                if (ttt == varlist)
                {
                    varlist = varlist->next;
                }
                else
                {
                    VAR tmp = varlist;
                    while (tmp != NULL && tmp->next != NULL && tmp->next != ttt)
                    {
                        tmp = tmp->next;
                    }
                    tmp->next = ttt->next;
                    //free(curVar);
                }

                break;
            }
            else
            {
                if (ttt->offset < 0)
                {
                    ttt->offset = pianyiliang;
                    ttt->reg = -1;
                    fprintf(fp, "  la $t0, jbxxncm54\n");
                    fprintf(fp, "  sw %s, %d($t0)\n", reg[curfreeRegNum]->name, ttt->offset);
                    pianyiliang += 4;
                    break;
                }
                else
                {
                    //ttt->offset=pianyiliang;
                    ttt->reg = -1;
                    fprintf(fp, "  la $t0, jbxxncm54\n");
                    fprintf(fp, "  sw %s, %d($t0)\n", reg[curfreeRegNum]->name, ttt->offset);
                    //pianyiliang+=4;
                    break;
                }
            }
        }
        ttt = ttt->next;
    }
    //fprintf(fp,"here ");
    VAR myvar = (VAR)malloc(sizeof(struct VAR_));
    myvar->next = NULL;
    myvar->op = op;
    myvar->reg = curfreeRegNum;
    myvar->offset = pianyiliang;
    myvar->name = name;
    if (varlist == NULL)
    {
        varlist = myvar;
        vartail = myvar;
    }
    else
    {
        vartail->next = myvar;
        vartail = myvar;
    }
    pianyiliang += 4;
    curfreeRegNum = curfreeRegNum + 1;
    if (curfreeRegNum == 11)
    {
        curfreeRegNum = 9;
    }
    return myvar->reg;
}

int findReg(FILE *fp, struct Operand_ *op)
{
    if (op->kind == OP_CONSTANT)
    {
        int i = saveReg(fp, op, op->u.value);

        fprintf(fp, "  li %s, %d\n", reg[i]->name, atoi(op->u.value));
        return i;
    }
    char *name = malloc(40);
    ;
    if (op->kind == OP_TMP_VARIABLE)
    {
        //name =
        memset(name, 0, sizeof(name));
        sprintf(name, "t%d", op->u.var_no);
    }
    else if (op->kind == OP_TMP_ADDRESS)
    {
        //name = malloc(40);
        memset(name, 0, sizeof(name));
        sprintf(name, "t%d", op->u.real_value->u.var_no);
    }
    else if (op->kind == OP_ADDRESS)
    {
        //name = malloc(40);
        memset(name, 0, sizeof(name));
        sprintf(name, "v%s", op->u.real_value->u.value);
        //name = op->u.real_value->u.value;
    }
    else if (op->kind == OP_VARIABLE)
    {
        //name = malloc(40);
        memset(name, 0, sizeof(name));
        sprintf(name, "v%s", op->u.value);
    }
    else
    {
        name = op->u.value;
    }
    VAR curVar = varlist;
    while (curVar != NULL)
    {
        if (strcmp(name, curVar->name) == 0)
        {
            if (curVar->reg > 0) //means in regs
            {
                return curVar->reg;
            }
            else
            {
                //fprintf(fp,"here\n");
                VAR ttt = varlist;
                while (ttt != NULL)
                {
                    if (ttt->reg == curfreeRegNum)
                    {
                        if (ttt->op->kind == OP_CONSTANT || ttt->op->kind == OP_TMP_ADDRESS || ttt->op->kind == OP_TMP_VARIABLE)
                        {
                            if (ttt == varlist)
                            {
                                varlist = varlist->next;
                            }
                            else
                            {
                                VAR tmp = varlist;
                                while (tmp != NULL && tmp->next != NULL && tmp->next != ttt)
                                {
                                    tmp = tmp->next;
                                }
                                tmp->next = ttt->next;
                                //free(curVar);
                            }

                            break;
                        }
                        else
                        {
                            if (ttt->offset < 0)
                            {
                                ttt->offset = pianyiliang;
                                ttt->reg = -1;
                                fprintf(fp, "  la $t0, jbxxncm54\n");
                                fprintf(fp, "  sw %s, %d($t0)\n", reg[curfreeRegNum]->name, ttt->offset);
                                pianyiliang += 4;
                                break;
                            }
                            else
                            {
                                //ttt->offset=pianyiliang;
                                ttt->reg = -1;
                                fprintf(fp, "  la $t0, jbxxncm54\n");
                                fprintf(fp, "  sw %s, %d($t0)\n", reg[curfreeRegNum]->name, ttt->offset);
                                //pianyiliang+=4;
                                break;
                            }
                        }
                    }
                    ttt = ttt->next;
                }

                fprintf(fp, "  la $t0, jbxxncm54\n");
                fprintf(fp, "  lw %s, %d($t0)\n", reg[curfreeRegNum]->name, curVar->offset);
                curVar->reg = curfreeRegNum;
                curfreeRegNum = curfreeRegNum + 1;
                if (curfreeRegNum == 11)
                {
                    curfreeRegNum = 9;
                }
                return curVar->reg;
            }
        }
        curVar = curVar->next;
    }
    //fprintf(fp,"not found\n");
    int i = saveReg(fp, op, name);
    return i;
}

void fprintFinalCode(FILE *fp)
{

    preprocess(fp);

    InterCode curInterCode = interCode;
    while (curInterCode != NULL)
    {
        switch (curInterCode->kind)
        {
        case IC_LABEL:
        {
            //printf("LABEL\n");
            //fprintOperand(fp, curInterCode->u.sinop.op1);//only this label
            fprintf(fp, "label%d", curInterCode->u.sinop.op1->u.var_no);
            fprintf(fp, " : ");
            fprintf(fp, "\n");
            break;
        }
        case IC_FUNCTION:
        {
            //TODO
            //printf("IC_FUNCTION\n");
            overOFFSET = 0;
            fprintf(fp, "\n%s:\n", curInterCode->u.sinop.op1->u.value);
            for (int i = 11; i < 26; i++)
            {
                reg[i]->free = 1;
            }
            varlist = NULL;
            vartail = NULL;
            memlist = NULL;
            memtail = NULL;
            if (strcmp(curInterCode->u.sinop.op1->u.value, "main") != 0)
            {
                ifInFunc = 1;
                curFunc = curInterCode->u.sinop.op1->u.value;
                FieldList fl = searchHashTable(curInterCode->u.sinop.op1->u.value, NULL, e_ExtDef);
                FieldList tfl = searchHashTable(curFunc, NULL, e_ExtDef);
                int para = 0;
                InterCode tic = curInterCode->next;
                while (tic != NULL && tic->kind == IC_PARAM)
                {
                    //printf("while %d\n",para);
                    //overOFFSET-=4;
                    if (para < 4)
                    {
                        VAR va = (VAR)malloc(sizeof(struct VAR_));
                        va->op = tic->u.sinop.op1;
                        va->offset = -996;
                        va->reg = 4 + para;
                        //va->name=tic->u.sinop.op1->u.value;
                        char *packnight = malloc(40);
                        va->name = packnight;
                        if (tic->u.sinop.op1->kind == OP_VARIABLE || tic->u.sinop.op1->kind == OP_ADDRESS)
                        {
                            sprintf(va->name, "v%s", tic->u.sinop.op1->u.value);
                        }
                        else if (tic->u.sinop.op1->kind == OP_TMP_ADDRESS || tic->u.sinop.op1->kind == OP_TMP_VARIABLE)
                        {
                            sprintf(va->name, "t%s", tic->u.sinop.op1->u.value);
                        }

                        va->next = NULL;
                        //printf("var init %s %s\n",va->name,reg[va->reg]->name);
                        if (varlist == NULL)
                        {
                            varlist = va;
                            vartail = va;
                        }
                        else
                        {
                            vartail->next = va;
                            vartail = va;
                        }
                    }
                    else
                    {
                        int re = findReg(fp, tic->u.sinop.op1);
                        FieldList ddd = fl->type->u.function.para;

                        int countdebugtfl = 0;
                        while (ddd != NULL)
                        {
                            countdebugtfl += 1;
                            ddd = ddd->tail;
                        }

                        fprintf(fp, "  lw %s, %d($fp)\n", reg[re]->name, (countdebugtfl - 1 - para) * 4); //TODO

                        //already insert in findReg

                        // VAR va = (VAR)malloc(sizeof(struct VAR_));
                        // va->op=tic->u.sinop.op1;
                        // va->reg=re;
                        // va->next=NULL;
                        // if(varlist==NULL)
                        // {
                        //     varlist=va;
                        //     vartail=va;
                        // }
                        // else
                        // {
                        //     vartail->next=va;
                        //     vartail=va;
                        // }
                    }
                    //printf("end?\n");
                    tic = tic->next;
                    para = para + 1;
                }
            }
            else
            {
                ifInFunc = 0;
                curFunc = curInterCode->u.sinop.op1->u.value; //also can be assigned as 'main'
            }
            //printf("end!\n");
            break;
        }
        case IC_ASSIGN:
        {   //seems only constant or var or tmpvar
            //printf("ASSIGN\n");
            if (curInterCode->u.binop.left->kind == OP_VARIABLE || curInterCode->u.binop.left->kind == OP_TMP_VARIABLE)
            {
                //printf("1\n");
                if (curInterCode->u.binop.right->kind == OP_CONSTANT)
                {
                    //x:=#y
                    //printf("11\n");
                    int l = findReg(fp, curInterCode->u.binop.left);
                    //printf("findreg?\n");
                    //printf("  li %s, %d\n", reg[l]->name, atoi(curInterCode->u.binop.right->u.value));
                    fprintf(fp, "  li %s, %d\n", reg[l]->name, atoi(curInterCode->u.binop.right->u.value));
                }
                else if (curInterCode->u.binop.right->kind == OP_VARIABLE || curInterCode->u.binop.right->kind == OP_TMP_VARIABLE)
                {
                    //x:=y
                    //printf("12\n");
                    int l = findReg(fp, curInterCode->u.binop.left);
                    int r = findReg(fp, curInterCode->u.binop.right);
                    fprintf(fp, "  move %s, %s\n", reg[l]->name, reg[r]->name);
                }
                else if (curInterCode->u.binop.right->kind == OP_ADDRESS || curInterCode->u.binop.right->kind == OP_TMP_ADDRESS)
                {
                    //printf("13\n");
                    //x:=*y
                    int l = findReg(fp, curInterCode->u.binop.left);
                    int r = findReg(fp, curInterCode->u.binop.right);
                    fprintf(fp, "  lw %s, 0(%s)\n", reg[l]->name, reg[r]->name);
                }
            }
            else if (curInterCode->u.binop.left->kind == OP_ADDRESS || curInterCode->u.binop.left->kind == OP_TMP_ADDRESS)
            {
                //printf("2\n");
                if (curInterCode->u.binop.right->kind == OP_CONSTANT)
                {
                    //*x:=#y
                    //int l=findReg(fp,curInterCode->u.binop.left);
                    //fprintf(fp,"  li %s, %d\n", reg[l]->name, atoi(curInterCode->u.binop.right->u.value));
                    int l = findReg(fp, curInterCode->u.binop.left);
                    int r = findReg(fp, curInterCode->u.binop.right);
                    fprintf(fp, "  li %s, %s\n", reg[r]->name, curInterCode->u.binop.right->u.value);
                    fprintf(fp, "  sw %s, 0(%s)\n", reg[r]->name, reg[l]->name); //bbugg
                }
                else if (curInterCode->u.binop.right->kind == OP_VARIABLE || curInterCode->u.binop.right->kind == OP_TMP_VARIABLE)
                {
                    //*x:=y
                    int l = findReg(fp, curInterCode->u.binop.left);
                    int r = findReg(fp, curInterCode->u.binop.right);
                    fprintf(fp, "  sw %s, 0(%s)\n", reg[r]->name, reg[l]->name); //error
                }
                else if (curInterCode->u.binop.right->kind == OP_ADDRESS || curInterCode->u.binop.right->kind == OP_TMP_ADDRESS)
                {
                    //*x:=*y
                    //
                    //*x=t,t=*y
                    int l = findReg(fp, curInterCode->u.binop.left);
                    int r = findReg(fp, curInterCode->u.binop.right);
                    Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
                    ifgototmp1->kind = OP_VARIABLE;
                    ifgototmp1->next = NULL;
                    ifgototmp1->prev = NULL;
                    sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
                    ifgotopack += 1;
                    int pack1 = findReg(fp, ifgototmp1);
                    fprintf(fp, "  lw %s, 0(%s)\n", reg[pack1]->name, reg[r]->name);

                    fprintf(fp, "  sw %s, 0(%s)\n", reg[pack1]->name, reg[l]->name); //error
                    //fprintf(fp, "  lw %s, 0(%s)\n", reg[l]->name, reg[r]->name);
                }
            }

            // if(curInterCode->u.binop.right->kind==OP_CONSTANT){
            //     int l=findReg(fp,curInterCode->u.binop.left);
            //     fprintf(fp,"  li %s, %d\n", reg[l]->name, atoi(curInterCode->u.binop.right->u.value));
            // }
            // else{
            //     int l=findReg(fp,curInterCode->u.binop.left);
            //     int r=findReg(fp,curInterCode->u.binop.right);
            //     fprintf(fp, "  move %s, %s\n", reg[l]->name, reg[r]->name);
            // }
            break;
        }
        case IC_ADD:
        {
            //printf("ADD\n");
            if (curInterCode->u.triop.op1->kind == OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int res = findReg(fp, curInterCode->u.triop.result);
                int check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }

                fprintf(fp, "  li %s, %d\n", reg[res]->name, atoi(curInterCode->u.triop.op1->u.value) + atoi(curInterCode->u.triop.op2->u.value));
            }
            else if (curInterCode->u.triop.op1->kind != OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  addi %s, %s, %d\n", reg[res]->name, reg[l]->name, atoi(curInterCode->u.triop.op2->u.value));
            }
            else
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int r = findReg(fp, curInterCode->u.triop.op2);
                check = staynightpack(fp, r, curInterCode->u.triop.op2);
                if (check != -1)
                {
                    r = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  add %s, %s, %s\n", reg[res]->name, reg[l]->name, reg[r]->name);
            }

            break;
        }
        case IC_SUB:
        {
            if (curInterCode->u.triop.op1->kind == OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int res = findReg(fp, curInterCode->u.triop.result);
                int check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  li %s, %d\n", reg[res]->name, atoi(curInterCode->u.triop.op1->u.value) - atoi(curInterCode->u.triop.op2->u.value));
            }
            else if (curInterCode->u.triop.op1->kind != OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  addi %s, %s, %d\n", reg[res]->name, reg[l]->name, -1 * atoi(curInterCode->u.triop.op2->u.value));
            }
            else
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int r = findReg(fp, curInterCode->u.triop.op2);
                check = staynightpack(fp, r, curInterCode->u.triop.op2);
                if (check != -1)
                {
                    r = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  sub %s, %s, %s\n", reg[res]->name, reg[l]->name, reg[r]->name);
            }
            break;
        }
        case IC_MUL:
        {
            if (curInterCode->u.triop.op1->kind == OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int res = findReg(fp, curInterCode->u.triop.result);
                int check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  li %s, %d\n", reg[res]->name, atoi(curInterCode->u.triop.op1->u.value) * atoi(curInterCode->u.triop.op2->u.value));
            }
            else
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int r = findReg(fp, curInterCode->u.triop.op2);
                check = staynightpack(fp, r, curInterCode->u.triop.op2);
                if (check != -1)
                {
                    r = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  mul %s, %s, %s\n", reg[res]->name, reg[l]->name, reg[r]->name);
            }
            break;
        }
        case IC_DIV:
        {
            //printf("DIV\n");
            if (curInterCode->u.triop.op1->kind == OP_CONSTANT && curInterCode->u.triop.op2->kind == OP_CONSTANT)
            {
                int res = findReg(fp, curInterCode->u.triop.result);
                int check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  li %s, %d\n", reg[res]->name, atoi(curInterCode->u.triop.op1->u.value) / atoi(curInterCode->u.triop.op2->u.value));
            }
            else
            {
                int l = findReg(fp, curInterCode->u.triop.op1);
                int check = staynightpack(fp, l, curInterCode->u.triop.op1);
                if (check != -1)
                {
                    l = check;
                }
                int r = findReg(fp, curInterCode->u.triop.op2);
                check = staynightpack(fp, r, curInterCode->u.triop.op2);
                if (check != -1)
                {
                    r = check;
                }
                int res = findReg(fp, curInterCode->u.triop.result);
                check = staynightpack(fp, res, curInterCode->u.triop.result);
                if (check != -1)
                {
                    res = check;
                }
                fprintf(fp, "  div %s, %s\n", reg[l]->name, reg[r]->name);
                fprintf(fp, "  mflo %s\n", reg[res]->name);
            }
            break;
        }
        case IC_GET_ADDR1:
        {
            //printf("GETADDR\n");
            int l = findReg(fp, curInterCode->u.binop.left);
            int r = findReg(fp, curInterCode->u.binop.right);
            fprintf(fp, "  la %s, ", reg[r]->name);
            fprintOperand(fp, curInterCode->u.binop.right);
            fprintf(fp, "\n  move %s, %s\n", reg[l]->name, reg[r]->name);
            break;
        }
        case IC_GET_ADDR2:
        {   //my add
            //printf("GETADDR2\n");
            int l = findReg(fp, curInterCode->u.triop.op1);
            //int r=findReg(fp,curInterCode->u.triop.op2); op2 is constant
            int res = findReg(fp, curInterCode->u.triop.result);
            //fprintf(fp, "  la %s, %s\n", reg[l]->name, curInterCode->u.triop.op1->u.value);
            fprintf(fp, "  la %s, ", reg[l]->name);
            fprintOperand(fp, curInterCode->u.triop.op1);
            fprintf(fp, "\n  addi %s, %s, %d\n", reg[res]->name, reg[l]->name, atoi(curInterCode->u.triop.op2->u.value));
            break;
        }
        case IC_GET_VAR:
        {
            //printf("GETVAR\n");
            int l = findReg(fp, curInterCode->u.binop.left);
            int r = findReg(fp, curInterCode->u.binop.right);
            fprintf(fp, "  lw %s, 0(%s)\n", reg[r]->name, reg[l]->name);
            break;
        }
        case IC_TO_VAR:
        {
            //printf("TOVAR\n");
            int l = findReg(fp, curInterCode->u.binop.left);
            int r = findReg(fp, curInterCode->u.binop.right);
            fprintf(fp, "  sw %s, 0(%s)\n", reg[r]->name, reg[l]->name);
            break;
        }
        case IC_GOTO:
        {
            fprintf(fp, "  j label%d\n", curInterCode->u.sinop.op1->u.var_no);
            break;
        }
        case IC_IFGOTO:
        {
            //printf("IFGOTO\n");
            int l = findReg(fp, curInterCode->u.ifgotoop.op1);
            int r = findReg(fp, curInterCode->u.ifgotoop.op2);
            int pack1 = l;
            int pack2 = r;

            if (curInterCode->u.ifgotoop.op1->kind == OP_TMP_ADDRESS || curInterCode->u.ifgotoop.op1->kind == OP_ADDRESS)
            {
                // fprintf(fp, "0(%s), ",reg[l]->name);
                Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
                ifgototmp1->kind = OP_VARIABLE;
                ifgototmp1->next = NULL;
                ifgototmp1->prev = NULL;
                sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
                ifgotopack += 1;
                pack1 = findReg(fp, ifgototmp1);
                fprintf(fp, "  lw %s 0(%s)\n", reg[pack1]->name, reg[l]->name);
            }
            else
            {
                //fprintf(fp, "%s, ",reg[l]->name);
            }
            if (curInterCode->u.ifgotoop.op2->kind == OP_TMP_ADDRESS || curInterCode->u.ifgotoop.op2->kind == OP_ADDRESS)
            {
                //fprintf(fp, "0(%s), ",reg[r]->name);

                Operand ifgototmp2 = (Operand)malloc(sizeof(struct Operand_));
                ifgototmp2->kind = OP_VARIABLE;
                ifgototmp2->next = NULL;
                ifgototmp2->prev = NULL;
                sprintf(ifgototmp2->u.value, "ifgotopack%d", ifgotopack);
                ifgotopack += 1;
                pack2 = findReg(fp, ifgototmp2);
                fprintf(fp, "  lw %s 0(%s)\n", reg[pack2]->name, reg[r]->name);
            }
            else
            {
                //fprintf(fp, "%s, ",reg[r]->name);
            }
            if (strcmp(curInterCode->u.ifgotoop.relop, "==") == 0)
            {
                fprintf(fp, "  beq ");
            }
            else if (strcmp(curInterCode->u.ifgotoop.relop, "!=") == 0)
            {
                fprintf(fp, "  bne ");
            }
            else if (strcmp(curInterCode->u.ifgotoop.relop, ">") == 0)
            {
                fprintf(fp, "  bgt ");
            }
            else if (strcmp(curInterCode->u.ifgotoop.relop, "<") == 0)
            {
                fprintf(fp, "  blt ");
            }
            else if (strcmp(curInterCode->u.ifgotoop.relop, ">=") == 0)
            {
                fprintf(fp, "  bge ");
            }
            else if (strcmp(curInterCode->u.ifgotoop.relop, "<=") == 0)
            {
                fprintf(fp, "  ble ");
            }
            // if(curInterCode->u.ifgotoop.op1->kind==OP_TMP_ADDRESS||curInterCode->u.ifgotoop.op1->kind==OP_ADDRESS){
            //      fprintf(fp, "  lw %s 0(%s)\n",curInterCode->u.sinop.op1->u.var_no,reg[l]->name);
            // }

            //fprintf(fp, "label%d\n", curInterCode->u.ifgotoop.result->u.var_no);
            //fprintf(fp, "%s, %s, label%d\n",reg[l]->name, reg[r]->name, curInterCode->u.ifgotoop.result->u.var_no);
            fprintf(fp, "%s, %s, label%d\n", reg[pack1]->name, reg[pack2]->name, curInterCode->u.ifgotoop.result->u.var_no);
            break;
        }
        case IC_RETURN:
        {
            //printf("IC_RETURN\n");
            if (curInterCode->u.sinop.op1->kind == OP_CONSTANT && atoi(curInterCode->u.sinop.op1->u.value) == 0)
            {
                //fprintf(fp, "  move $v0, $%d\n",atoi(curInterCode->u.sinop.op1->u.value));
                fprintf(fp, "  move $v0, $0\n");
            }
            else
            {
                int l = findReg(fp, curInterCode->u.sinop.op1);
                fprintf(fp, "  move $v0, %s\n", reg[l]->name);
            }
            fprintf(fp, "  jr $ra\n");
            break;
        }
        case IC_DEC:
            //TODO
            //done in preprocess
            //fprintf(fp, "DEC ");
            //fprintOperand(fp, curInterCode->u.decop.op1);
            //fprintf(fp, " %d ", curInterCode->u.decop.size);
            break;
        case IC_ARG:
        {
            //DONE IN CALL
            argNum += 1;
            break;
        }

        case IC_CALL:
        {
            //printf("IC_CALL\n");
            fprintf(fp, "  addi $sp, $sp, -4\n");
            fprintf(fp, "  sw $ra, 0($sp)\n");
            //fprintf(fp, "  addi $sp, $sp, -72\n");
            fprintf(fp, "  addi $sp, $sp, -60\n");
            for (int i = 11; i < 26; i++)
            {
                fprintf(fp, "  sw %s, %d($sp)\n", reg[i]->name, (i - 11) * 4);
            }
            int l = findReg(fp, curInterCode->u.binop.left);
            FieldList tfl = searchHashTable(curFunc, NULL, e_ExtDef);
            FieldList debugtfl = searchHashTable(curInterCode->u.binop.right->u.value, NULL, e_ExtDef);
            FieldList ddd = debugtfl->type->u.function.para;
            FieldList ddd2 = tfl->type->u.function.para;
            int countdebugtfl = 0;
            while (ddd != NULL)
            {
                countdebugtfl += 1;
                ddd = ddd->tail;
            }
            int counttfl = 0;
            while (ddd2 != NULL)
            {
                counttfl += 1;
                ddd2 = ddd2->tail;
            }

            if (ifInFunc) //save cur func's $a0-$a3
            {
                //fprintf(fp, "  addi $sp, $sp, -%d\n", argNum*4);
                fprintf(fp, "  addi $sp, $sp, -%d\n", countdebugtfl * 4);

                //for(int i=0;i<argNum;i++)
                for (int i = 0; i < countdebugtfl; i++)
                {
                    if (i >= counttfl)
                    {
                        break;
                    }
                    if (i < 4)
                    {
                        fprintf(fp, "  sw $a%d, %d($sp)\n", i, i * 4);
                        VAR p = varlist;
                        while (p != NULL)
                        {
                            if (p->op->kind != OP_CONSTANT && p->reg == 4 + i)
                            {
                                break;
                            }
                            p = p->next;
                        }
                        Operand op = p->op;
                        if (p == varlist)
                        {
                            varlist = varlist->next;
                        }
                        else
                        {
                            VAR va = varlist;
                            while (va != NULL)
                            {
                                if (va->next == p)
                                {
                                    break;
                                }
                                va = va->next;
                            }
                            if (vartail == p)
                            {
                                vartail = va;
                            }
                            va->next = p->next;
                            p->next = NULL;
                        }
                        VAR mva = (VAR)malloc(sizeof(struct VAR_));
                        mva->op = op;
                        mva->offset = -996;
                        mva->next = NULL;
                        if (memlist == NULL)
                        {
                            memlist = mva;
                            memtail = mva;
                        }
                        else
                        {
                            memtail->next = mva;
                            memtail = mva;
                        }
                        fprintf(fp, "  move %s, %s\n", reg[findReg(fp, op)]->name, reg[4 + i]->name);
                    }
                }
            }
            //IC_ARG
            InterCode tmp = curInterCode->prev;
            int curPara = 0;
            while (tmp != NULL && tmp->kind == IC_ARG)
            {
                int reg_n = findReg(fp, tmp->u.sinop.op1);
                int pack1 = reg_n;
                if (tmp->u.sinop.op1->kind == OP_TMP_ADDRESS || tmp->u.sinop.op1->kind == OP_ADDRESS)
                {
                    Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
                    ifgototmp1->kind = OP_VARIABLE;
                    ifgototmp1->next = NULL;
                    ifgototmp1->prev = NULL;
                    sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
                    ifgotopack += 1;
                    pack1 = findReg(fp, ifgototmp1);
                    fprintf(fp, "  lw %s, 0(%s)\n", reg[pack1]->name, reg[reg_n]->name);
                }

                if (curPara < 4)
                {
                    fprintf(fp, "  move %s, %s\n", reg[curPara + 4]->name, reg[pack1]->name);
                    curPara++;
                }
                else
                {
                    fprintf(fp, "  addi $sp, $sp, -4\n");
                    fprintf(fp, "  sw %s, 0($sp)\n", reg[pack1]->name);
                    fprintf(fp, "  move $fp, $sp\n");
                    curPara++;
                }
                tmp = tmp->prev;
            }

            fprintf(fp, "  jal %s\n", curInterCode->u.binop.right->u.value);
            //move to the sub func
            //xxx
            argNum = 0;
            //return to this func
            if (curPara > 4)
            {
                fprintf(fp, "  addi $sp, $sp, %d\n", 4 * (curPara - 4));
            }

            if (ifInFunc)
            {
                //FieldList  ddbugfl=searchHashTable(curFunc,NULL,e_ExtDef);
                //for(int i=0;i<fl->type->u.function.paraNum;i++){
                for (int i = 0; i < countdebugtfl; i++)
                {
                    if (i >= counttfl)
                    {
                        break;
                    }
                    if (i < 4)
                    {
                        fprintf(fp, "  lw %s, %d($sp)\n", reg[4 + i]->name, i * 4);
                        VAR va = varlist;
                        while (va != NULL)
                        {
                            if (va->op->kind != OP_CONSTANT && strcmp(memlist->op->u.value, va->op->u.value) == 0)
                            {
                                break;
                            }
                            va = va->next;
                        }
                        if (va != NULL)
                        {
                            reg[va->reg]->free = 1;
                            va->reg = 4 + i;
                        }
                        else
                        {
                            VAR vva = (VAR)malloc(sizeof(struct VAR_));
                            vva->op = memlist->op;
                            vva->reg = 4 + i;
                            vva->offset = -996;
                            vva->next = NULL;
                            if (varlist == NULL)
                            {
                                varlist = vva;
                                vartail = vva;
                            }
                            else
                            {
                                vartail->next = vva;
                                vartail = vva;
                            }
                        }
                        memlist = memlist->next;
                    }
                }
                //FieldList  ffl=searchHashTable(curFunc,NULL,e_ExtDef);
                fprintf(fp, "  addi $sp, $sp, %d\n", countdebugtfl * 4);
            }
            for (int i = 11; i < 26; i++)
            {
                fprintf(fp, "  lw %s, %d($sp)\n", reg[i]->name, (i - 11) * 4);
            }
            //fprintf(fp, "  addi $sp, $sp, 72\n");
            fprintf(fp, "  addi $sp, $sp, 60\n");

            fprintf(fp, "  lw $ra, 0($sp)\n");
            fprintf(fp, "  addi $sp, $sp, 4\n");
            fprintf(fp, "  move %s, $v0\n", reg[l]->name);

            break;
        }
        case IC_PARAM:
            //TODO
            //done in IC_FUNCTION
            break;
        case IC_READ:
        {
            fprintf(fp, "  addi $sp, $sp, -4\n");
            fprintf(fp, "  sw $ra, 0($sp)\n");
            fprintf(fp, "  jal read\n");
            fprintf(fp, "  lw $ra, 0($sp)\n");
            fprintf(fp, "  addi $sp, $sp, 4\n");
            int l = findReg(fp, curInterCode->u.sinop.op1);
            fprintf(fp, "  move %s, $v0\n", reg[l]->name);
            break;
        }
        case IC_WRITE:
            if (ifInFunc == 0)
            {
                int l = findReg(fp, curInterCode->u.sinop.op1);
                int pack1 = l;
                if (curInterCode->u.sinop.op1->kind == OP_TMP_ADDRESS || curInterCode->u.sinop.op1->kind == OP_ADDRESS)
                {
                    Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
                    ifgototmp1->kind = OP_VARIABLE;
                    ifgototmp1->next = NULL;
                    ifgototmp1->prev = NULL;
                    sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
                    ifgotopack += 1;
                    pack1 = findReg(fp, ifgototmp1);
                    fprintf(fp, "  lw %s, 0(%s)\n", reg[pack1]->name, reg[l]->name);
                }

                fprintf(fp, "  move $a0, %s\n", reg[pack1]->name);
                fprintf(fp, "  addi $sp, $sp, -4\n");
                fprintf(fp, "  sw $ra, 0($sp)\n");
                fprintf(fp, "  jal write\n");
                fprintf(fp, "  lw $ra, 0($sp)\n");
                fprintf(fp, "  addi $sp, $sp, 4\n");
            }
            else
            {
                int l = findReg(fp, curInterCode->u.sinop.op1);
                int pack1 = l;
                if (curInterCode->u.sinop.op1->kind == OP_TMP_ADDRESS || curInterCode->u.sinop.op1->kind == OP_ADDRESS)
                {
                    Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
                    ifgototmp1->kind = OP_VARIABLE;
                    ifgototmp1->next = NULL;
                    ifgototmp1->prev = NULL;
                    sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
                    ifgotopack += 1;
                    pack1 = findReg(fp, ifgototmp1);
                    fprintf(fp, "  lw %s, 0(%s)\n", reg[pack1]->name, reg[l]->name);
                }

                fprintf(fp, "  addi $sp, $sp, -8\n");
                fprintf(fp, "  sw $a0, 0($sp)\n");
                fprintf(fp, "  sw $ra, 4($sp)\n");
                fprintf(fp, "  move $a0, %s\n", reg[pack1]->name);
                fprintf(fp, "  jal write\n");
                fprintf(fp, "  lw $a0, 0($sp)\n");
                fprintf(fp, "  lw $ra, 4($sp)\n");
                fprintf(fp, "  addi $sp, $sp, 8\n");
            }
            break;
        default:
            break;
        }

        curInterCode = curInterCode->next;
    }
}

int staynightpack(FILE *fp, int start, Operand op)
{
    if (op->kind == OP_TMP_ADDRESS || op->kind == OP_ADDRESS)
    {
        Operand ifgototmp1 = (Operand)malloc(sizeof(struct Operand_));
        ifgototmp1->kind = OP_VARIABLE;
        ifgototmp1->next = NULL;
        ifgototmp1->prev = NULL;
        sprintf(ifgototmp1->u.value, "ifgotopack%d", ifgotopack);
        ifgotopack += 1;
        int pack1 = findReg(fp, ifgototmp1);
        fprintf(fp, "  lw %s, 0(%s)\n", reg[pack1]->name, reg[start]->name);
        return pack1;
    }
    else
    {
        return -1;
    }
}