#include "lab2.h"

#include "syntax.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define FASTER

int lab3struct=0;
//packing for samples which go against suppose 7, and curStructure's tail is it is para.
FieldList curStructure[0x3fff];
int curStructDep;

int curNonNameStructID;

unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}


//var: name type 
//func:returnType paraNum, paraType
//
FieldList insertHashTable(FieldList f,int kind)
{
    unsigned int hash = hash_pjw(f->name) % HashTableSize;

    FieldList hashf;

    if(HashTable[hash]==NULL){
        
        HashTable[hash] = (FieldList)malloc(sizeof(struct FieldList_));

        HashTable[hash]->name=f->name;
        HashTable[hash]->type=f->type;
        HashTable[hash]->tail=NULL;
        
    }
    else 
    {
        hashf = (FieldList)malloc(sizeof(struct FieldList_));
        hashf->tail = HashTable[hash];

        HashTable[hash] = hashf;
        hashf->name = f->name;
        hashf->type=f->type;
       
    }
    FieldList depF=(FieldList)malloc(sizeof(struct FieldList_));
   
    if(kind==FUNCTION){
        //curFuncDep=curFuncDep+1;
        HashTable[hash]->type->kind=FUNCTION;
    }
    
    return NULL;
}

FieldList searchHashTable(char *name,Type type,int SpecialCode)
{
    unsigned int hash = hash_pjw(name) % HashTableSize;
    FieldList f = HashTable[hash];
    //only cmp to the func and name,which means type is func is enough
    if(SpecialCode==e_ExtDef){
        while (f!=NULL)
        {
           if(strcmp(name, f->name) == 0 &&f->type->kind==FUNCTION){
               return f;
           }
           f=f->tail;
        }
        return NULL; //means no same function name in hashtable

    }
    else if(SpecialCode==e_OptTag){
        while (f!=NULL)
        {
           if(strcmp(name, f->name) == 0){
               if(f->type->kind==STRUCTURE || f->type->kind==BASIC|| f->type->kind==ARRAY){
                 return f;
               }
           }
           f=f->tail;
        }
        return NULL;//means no same structure or other basic orarray name in hashtable
        
    }
    else if(SpecialCode==e_Tag){//input type is useless and donnot use it
        while (f!=NULL)
        {
           //printf("MAIN %d\n",f->type->kind);
           if(strcmp(name, f->name) == 0 &&f->type->kind==STRUCTURE){
              return f;
           }
           f=f->tail;
        }
        return NULL;//means no same structure  name in hashtable
        
    }
    else if(SpecialCode==e_VarDec){//in fact, this if same as e_OptTag //now it is not same
        while (f!=NULL)
        {
           if(strcmp(name, f->name) == 0 &&f->type->kind!=FUNCTION){ //&&curFuncDep<=f->depth){
              return f;
           }

          if(strcmp(name, f->name) == 0 &&f->type->kind!=FUNCTION &&f->type->value==LEFT_VALUE){
              return f;
           }


           f=f->tail;
        }
        return NULL;
        
    }
    else if(SpecialCode==e_Exp){
        while (f!=NULL)
        {
           if(strcmp(name, f->name) == 0 && f->type->kind!=FUNCTION){
              return f;
           }
           f=f->tail;
        }
        return NULL;
    }
    else if(SpecialCode==e_NOT){
        while (f!=NULL)
        {
           if(strcmp(name, f->name) == 0){
              return f;
           }
           f=f->tail;
        }
        return NULL;
    }
    else //for SpecialCode=-1
    {
        while (f != NULL)
        {
            if (strcmp(name, f->name) == 0 &&f->type->kind!=FUNCTION)
            {
                
                return f;
            }
            f = f->tail;
        }
       return NULL;
    }
    

    return NULL;
}


int typeCheck(Type t1, Type t2)
{   
    if(t1==NULL || t2==NULL)
    {    
        //return 0;
       
        return -1;
    }
    if (t1->kind != t2->kind)
    {
        //printf("%d %d\n",t1->kind,t2->kind);
        return -1;
    }
    if(t1->kind==BASIC){
        if(t1->u.basic==t2->u.basic){
            return 0;
        }
        else
        {
            return -1;
        }
        
    }
    if(t1->kind==ARRAY){
        //well int a[10][2] is similar to int b[5][3]
        //but int a[10] not similar to 5

        //if(t1->u.array.size==t2->u.array.size){
            return typeCheck(t1->u.array.elem,t2->u.array.elem);
        //}
        //else
        //{
        //    return -1;
        //}
        
    }
    if (t1->kind == STRUCTURE)//for no 2.3 part students.
    {
        if(t1->u.structure->name==NULL && t2->u.structure->name==NULL && t1->curNonNameStruct==t2->curNonNameStruct){
            
            return 0;
        }
        if(t1->u.structure->name==NULL || t2->u.structure->name==NULL){
            return -1;
        }
        if (strcmp(t1->u.structure->name, t2->u.structure->name) == 0)
        //if(strcmp(t1->structureName,t2->structureName)==0)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    if(t1->kind==FUNCTION){
        if(typeCheck(t1->u.function.retType,t2->u.function.retType)==0 &&t1->u.function.paraNum==t2->u.function.paraNum){
            FieldList funF1=t1->u.function.para;
            FieldList funF2=t2->u.function.para;
            while (funF1!=NULL &&funF2!=NULL)
            {
                if(typeCheck(funF1->type,funF2->type)!=0){
                    return -1;
                }
                funF1=funF1->tail;
                funF2=funF2->tail;
            }
            return 0;
            
        }
        else
        {
            return -1;
        }
        
    }
    
    return 0;
}


void createHashTable()
{
    int i = 0;
    for (; i < HashTableSize; i++)
    {
        HashTable[i] = NULL;
    }

    //add in lab3
    FieldList funF1=(FieldList)malloc(sizeof(struct FieldList_));
    memset(funF1,0,sizeof(struct FieldList_));
    funF1->name=(char*)malloc(sizeof(char)*40);
    strcpy(funF1->name,"read");
    funF1->type=(Type)malloc(sizeof(struct Type_));
    memset(funF1->type,0,sizeof(struct Type_));
    funF1->tail=NULL;
    funF1->type->kind=FUNCTION;
    funF1->type->u.function.retType=(Type)malloc(sizeof(struct Type_));
    memset(funF1->type->u.function.retType,0,sizeof(struct Type_));
    funF1->type->u.function.retType->kind=BASIC;
    funF1->type->u.function.retType->u.basic=UBASIC_INT;
    funF1->type->u.function.para=NULL;
    funF1->type->u.function.paraNum=0;
    insertHashTable(funF1,READWRITE);

    FieldList funF2=(FieldList)malloc(sizeof(struct FieldList_));
    memset(funF2,0,sizeof(struct FieldList_));
    funF2->name=(char*)malloc(sizeof(char)*40);
    strcpy(funF2->name,"write");
    funF2->type=(Type)malloc(sizeof(struct Type_));
    memset(funF2->type,0,sizeof(struct Type_));
    funF2->tail=NULL;
    funF2->type->kind=FUNCTION;
    funF2->type->u.function.retType=(Type)malloc(sizeof(struct Type_));
    memset(funF2->type->u.function.retType,0,sizeof(struct Type_));
    funF2->type->u.function.retType->kind=BASIC;
    funF2->type->u.function.retType->u.basic=UBASIC_INT;
    funF2->type->u.function.para=(FieldList)malloc(sizeof(struct FieldList_));
    memset(funF2->type->u.function.para,0,sizeof(struct FieldList_));
    char* write=(char*)malloc(40*sizeof(char));
    strcpy(write,"wpara");
    funF2->type->u.function.para->name=write;
    funF2->type->u.function.para->type=(Type)malloc(sizeof(struct Type_));
    memset(funF2->type->u.function.para->type,0,sizeof(struct Type_));
    funF2->type->u.function.para->type->kind=BASIC;
    funF2->type->u.function.paraNum=1;
    insertHashTable(funF2,READWRITE);
    //add in lab3

    //curStructDep=0;
    i = 0;
    for (; i < 0x3fff; i++)
    {
        curStructure[i] = NULL;
    }
    curStructDep=1;
    curStructure[0]=(FieldList)malloc(sizeof(struct FieldList_));
    curStructure[0]->tail=NULL;
    curStructure[0]->name=NULL;
    curStructure[0]->type=NULL;

    curNonNameStructID=0;
}

void symbolTable(struct TreeNode *root)
{
    createHashTable();
    Program(root);
}
//
void Program(struct TreeNode *root)
{
    ExtDefList(root->child);
}

void ExtDefList(struct TreeNode *extdeflist)
{
    if (extdeflist->int_name == e_NULL ||extdeflist==NULL || extdeflist->child == NULL)
    {
        return;
    }
    ExtDef(extdeflist->child);
    ExtDefList(extdeflist->child->brother);
}
void ExtDef(struct TreeNode *extdef)
{
    if (extdef == NULL)
    {
        return;
    }
    Type retT = Specifier(extdef->child);
    //if(retT==NULL){
        //printf("hello?\n");
    //    return ; //pack in 0415
                    //pack in 0416
                    //cannot return NULL because FunDec need it
    //}
    FieldList f = NULL;
    FieldList f2;
    switch (extdef->child->brother->int_name)
    {
    case e_ExtDecList: //Specifier ExtDecList SEMI
        ExtDecList(extdef->child->brother,retT);
        
        break;
    case e_SEMI: //Specifier SEMI
        return;
        break;
    case e_FunDec: //Specifier FunDec CompSt
        {
        f = FunDec(extdef->child->brother, retT); //ret is the type of function,no relation of para.
 
        if(f!=NULL){
            //only cmp to the func and name
            f2=searchHashTable(f->name,f->type,e_ExtDef);
            //printf("search\n");
            if(f2!=NULL){
                //printError(4,extdef->child->brother->child->lineno,extdef->child->brother->child->nchar_data);
            }
            else
            {
                
                FieldList fuckk=insertHashTable(f,FUNCTION);//TODO


                //lab3
                Operand op=(Operand)malloc(sizeof(struct Operand_));
                op->kind=OP_FUNCTION;
                strcpy(op->u.value,f->name);
                InterCode ic=(InterCode)malloc(sizeof(struct InterCode_));
                ic->kind=IC_FUNCTION;
                ic->u.sinop.op1=op;
                insertInterCodes(ic);
                FieldList flt=f->type->u.function.para; //suppose func's para store in there.
                while (flt!=NULL)
                {
                    Operand opp=(Operand)malloc(sizeof(struct Operand_));
                    opp->kind=OP_VARIABLE;
                    strcpy(opp->u.value,flt->name);
                    InterCode ic=(InterCode)malloc(sizeof(struct InterCode_));
                    ic->kind=IC_PARAM;
                    ic->u.sinop.op1=opp;
                    insertInterCodes(ic);
                    flt=flt->tail;
                }
            }
            CompSt(extdef->child->brother->brother, retT);

        }
        break;
        }
    default:
        break;
    }
    //another half in structspecifier

    
}

void ExtDecList(struct TreeNode* extdeclist,Type type){
    // ExtDecList(extdef->child->brother);

    FieldList l3=VarDec(extdeclist->child,type,BASIC);//same as struct
    if(l3!=NULL){
        if(l3->type->kind==ARRAY){
            Operand op=(Operand)malloc(sizeof(struct Operand_));
            op->kind=OP_TMP_VARIABLE;
            op->u.var_no=tmp_var_no;
            tmp_var_no+=1;
            InterCode ic=(InterCode)malloc(sizeof(struct InterCode_));
            ic->kind=IC_DEC;
            ic->u.decop.op1=op;
            ic->u.decop.size=arrayLen(l3->type,1);//here is for define
            insertInterCodes(ic);

            Operand opp=(Operand)malloc(sizeof(struct Operand_));
            opp->kind=OP_VARIABLE;
            strcpy(opp->u.value,l3->name);
            InterCode icc=(InterCode)malloc(sizeof(struct InterCode_));
            icc->kind=IC_GET_ADDR1;
            icc->u.binop.left=opp;
            icc->u.binop.right=op;
            insertInterCodes(icc);
            //printf("hello\n");

        }
    }


    if(extdeclist->child->brother==NULL){//vardec
        
    }
    else{//vardec comma extdeclist
         ExtDecList(extdeclist->child->brother->brother,type);
    }
    
}

Type Specifier(struct TreeNode *specifier)
{
    if(specifier==NULL){
        return NULL;
    }

    Type t;
    switch (specifier->child->int_name)
    {
    case e_TYPE:
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        //t->u.basic = specifier->child->int_kind; //only for k_int or k_float,no other choices
        //if(specifier->child->int_kind==k_int){
        char *tmp="int";
        if(strcmp(specifier->child->nchar_data,tmp)==0){
            t->u.basic=UBASIC_INT;
            
        }
        else{
            t->u.basic=UBASIC_FLOAT;
        }
        t->value=RIGHT_VALUE;
        break;
    case e_StructSpecifier:
        printf("Cannot translate: Code contains variables or parameters of structure type.\n");
        lab3struct=1;
        exit(0);
        t = StructSpecifier(specifier->child);
        break;

    default:
        //error
        break;
    }
    return t;
}
Type StructSpecifier(struct TreeNode *structspecifier)
{
    if(structspecifier==NULL){
        return NULL;
    }
    //struct TreeNode tnode= structspecifier ->child->brother; //there must be a 'STRUCT'
    Type t = (Type)malloc(sizeof(struct Type_));
    Type t2=(Type)malloc(sizeof(struct Type_));
    FieldList f;
    //printf("MAIN %d\n",structspecifier->child->brother->int_name);
    switch (structspecifier->child->brother->int_name)
    {
    case e_NULL: //OptTag -> null
       
        t->curNonNameStruct=curNonNameStructID;
        curNonNameStructID=curNonNameStructID+1;

    case e_OptTag: //STRUCT OptTag LC DefList RC
        //printf("here\n");
        
        t->kind = STRUCTURE;
        t->value=LEFT_VALUE;
        f = (FieldList)malloc(sizeof(struct FieldList_));
        t->u.structure = f;
        
        f->tail=NULL;
        //OptTag -> ID | null
        if (structspecifier->child->brother->int_name==e_NULL|| structspecifier->child->brother->child->int_name==e_NULL)           // == NULL)
        {
            //t->u.structure->name = NULL;
            f->name=NULL;
           
        }
        else
        {
            //check //no need check here
            //t->u.structure->name = structspecifier->child->brother->child->nchar_data;
            f->name=structspecifier->child->brother->child->nchar_data;
            
        }
        t->structureName=f->name;
        //packing for samples which go against suppose 7
        //printf("%d\n",curStructDep);
        curStructure[curStructDep]=(FieldList)malloc(sizeof(struct FieldList_));
        //reconnection here, before here, we  give the curStructure a new structure, at last we give it the f.
        curStructure[curStructDep]->tail=NULL;

        //t->u.structure->tail=NULL;
        //need to exist for checking more error
        //t->u.structure->tail = DefList(structspecifier->child->brother->brother->brother,STRUCTURE);
        curStructDep=curStructDep+1;
        f->tail = DefList(structspecifier->child->brother->brother->brother,STRUCTURE);//store it is para
        //another half in extdef
        //curStructure=f;
        //f->tail=t->u.structure;
        //f->type=t;
        f->type=t;
        if(f->name!=NULL){
            FieldList f2=searchHashTable(f->name,NULL,e_OptTag);
            if(f2!=NULL ){//&&curFuncDep<=f->depth){ //2.2 wonnot protect the struct type
                //means there is the same
                
                printError(16,structspecifier->child->brother->child->lineno,structspecifier->child->brother->child->nchar_data);
                //return NULL;
                return f->type;
            }
            else
            {
                insertHashTable(f,STRUCTURE);
            }
            
        }
        //clear for next suppose 7
        curStructDep=curStructDep-1;
        //free(curStructure[curStructDep]); //without free, we can read it in extdef, and next part we will fugai it.

        memcpy(t2,t,sizeof(struct Type_));
        //return t;
        return t2;
        break;
    case e_Tag: //STRUCT Tag
        //check
        f=searchHashTable(structspecifier->child->brother->child->nchar_data,NULL,e_Tag);
        if(f==NULL){
            printError(17,structspecifier->child->brother->child->lineno,structspecifier->child->brother->child->nchar_data);
            return NULL; //TODO
        }
        return f->type;
        break;

    default:
        //error
        break;
    }
    memcpy(t2,t,sizeof(struct Type_));
    //return t;
    return t2;
}
FieldList DefList(struct TreeNode *deflist,int structure)//->int_name==e_NULL
{
    //if (deflist == NULL || deflist->child != NULL)
    if(deflist->int_name==e_NULL ||deflist==NULL||deflist->child==NULL)
    {
        return NULL;
    }
    //Def DefList
    FieldList f = Def(deflist->child,structure);
    if (f == NULL)
    {
        f = DefList(deflist->child->brother,structure);
    }
    else
    {
        FieldList tmp = f;
        while (tmp->tail != NULL)
        {
            tmp = tmp->tail;
        }
        tmp->tail = DefList(deflist->child->brother,structure);//wtf, need to be tmp->tail
    }
    return f;
}
FieldList Def(struct TreeNode *def,int structure)
{
    if(def==NULL){
         return NULL;
    }
    //: Specifier DecList SEMI
    FieldList f;
    Type type = Specifier(def->child);
    if(type==NULL){
        return NULL; //pack in 0415
    }
    f = DecList(def->child->brother, type,structure);
    return f;
}
FieldList DecList(struct TreeNode *declist, Type type,int structure)
{
    if(declist==NULL ||declist->child==NULL){
        return NULL;
    }

    FieldList f = Dec(declist->child, type,structure); //if Dec error, it will return NULL
    //printf("HHH %s\n",f->name);
    if (declist->child->brother != NULL)     //Dec COMMA DecList
    {
        if (f == NULL)
        {
            f = DecList(declist->child->brother->brother, type,structure);
        }
        else
        {
            FieldList tmp = f;
            while (tmp->tail != NULL)
            {
                tmp = tmp->tail;
            }
            tmp->tail = DecList(declist->child->brother->brother, type,structure);
        }
    }
    return f;
}
//choose==1 array from DEC, full size ; 2 from EXP[] ,part size
int arrayLen(Type t,int choose){
    if (t==NULL)
    {
        return 0;
    }
    //choose=1;
    if(choose==1){
        if(t->kind==BASIC){
            return 4;
        }
        else{
            int basic=4;
            while (t->u.array.elem->kind!=BASIC)
            {
                basic=t->u.array.size*basic;
                t=t->u.array.elem;
            }
            basic=t->u.array.size*basic;
            if(t->u.array.elem->kind==BASIC){
                return basic;
            }
            else{
                return 1;
            }
        }
    }
    if(choose==2){
        if(t->u.array.elem->kind==BASIC){
            return 4;
        }
        else{
            int basic=4;
            t=t->u.array.elem;
            while (t->u.array.elem->kind!=BASIC)
            {
                basic=t->u.array.size*basic;
                t=t->u.array.elem;
            }
            basic=t->u.array.size*basic;
            if(t->u.array.elem->kind==BASIC){
                return basic;
            }
            else{
                return 1;
            }
            
        }
        // else{
            
        //     int basic=4;
        //     while (t->u.array.elem->kind!=BASIC)
        //     {
        //         basic=t->u.array.size*basic;
        //         if(t->u.array.elem->u.array.elem->kind==BASIC){
        //             return basic;
        //         }
        //         t=t->u.array.elem;
        //     }
        //     basic=t->u.array.size*basic;
        //     return basic;
        // }
    }
    return 1;
}
FieldList Dec(struct TreeNode *dec, Type type,int structure)
{
    if(dec==NULL){
        return NULL;
    }
    FieldList f = VarDec(dec->child, type,structure);

    #ifdef FASTER
    if(f->type->kind==ARRAY &&structure==BASIC){
        Operand zop=(Operand)malloc(sizeof(struct Operand_));
        zop->kind=OP_TMP_VARIABLE;
        zop->u.var_no=tmp_var_no;
        tmp_var_no+=1;
        //zop->kind=OP_VARIABLE;
        //strcpy(zop->u.value,f->name);

        InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
        zic->kind=IC_DEC;
        zic->u.decop.op1=zop;
        zic->u.decop.size=arrayLen(f->type,1);
        insertInterCodes(zic);

        Operand zzop=(Operand)malloc(sizeof(struct Operand_));
        zzop->kind=OP_VARIABLE;
        strcpy(zzop->u.value,f->name);

        InterCode zzic=(InterCode)malloc(sizeof(struct InterCode_));
        zzic->kind=IC_GET_ADDR1;
        zzic->u.binop.left=zzop;
        zzic->u.binop.right=zop;
        insertInterCodes(zzic);
    }
    if (dec->child->brother != NULL){
        //errorType15 becase now it is initilizing.
        if(structure==STRUCTURE){
            printError(152,dec->child->lineno,dec->child->nchar_data);
            //return NULL;
        }


        Operand xop=(Operand)malloc(sizeof(struct Operand_));
        //xop->kind=OP_TMP_VARIABLE;
        //xop->u.var_no=tmp_var_no;
        //tmp_var_no+=1;
        xop->kind=OP_VARIABLE;
        strcpy(xop->u.value,f->name);
        Type t = Exp(dec->child->brother->brother,xop);
        if(typeCheck(t,f->type)!=0){
            printError(5,dec->child->brother->brother->lineno,dec->child->brother->brother->nchar_data);
            return NULL;
        }
        if(xop->kind!=OP_VARIABLE||strcmp(xop->u.value,f->name)!=0){
            Operand oop=(Operand)malloc(sizeof(struct Operand_));
            oop->kind=OP_VARIABLE;
            strcpy(oop->u.value,f->name);

            InterCode zi=(InterCode)malloc(sizeof(struct InterCode_));
            zi->kind=IC_ASSIGN;
            zi->u.binop.left=oop; //f
            zi->u.binop.right=xop;//f

            //newpack for    b[4]=a[3]=c[4];
            FieldList packfzo=searchHashTable(oop->u.value,NULL,-1);  
            FieldList packfzx=searchHashTable(xop->u.value,NULL,-1); 
            
                insertInterCodes(zi);
            //}
            
        }
    }
    #else

    #endif

    // if (dec->child->brother != NULL) //VarDec ASSIGNOP Exp
    // {
    //     //check  
    //     //errorType15 becase now it is initilizing.
    //     if(structure==STRUCTURE){
    //         printError(152,dec->child->lineno,dec->child->nchar_data);
    //         //return NULL;
    //     }
    //     Type t = Exp(dec->child->brother->brother,place);
    //     if(typeCheck(t,f->type)!=0){
    //         printError(5,dec->child->brother->brother->lineno,dec->child->brother->brother->nchar_data);
    //         //return NULL;
    //     }
    // }
    
    return f;
}
//vardec->ID |vardec LB[ int RB]
FieldList VarDec(struct TreeNode *vardec, Type type2,int structure)
{
    if(vardec==NULL){
        return NULL;
    }

    FieldList f;
    FieldList f2;
    FieldList pack7f1;
    FieldList pack7f2;
    Type type=(Type)malloc(sizeof(struct Type_));
    memcpy(type,type2,sizeof(struct Type_));
    type->value=BOTH_VALUE;
    Type t;
    switch (vardec->child->int_name)
    {
    case e_ID: //ID
        {
        f = (FieldList)malloc(sizeof(struct FieldList_));
        f->name = vardec->child->nchar_data;
        f->type = type;
        f->tail = NULL;
        //check
        //according to suppose 7, there is no need to check this
        //if(structure!=STRUCTURE){
        //}
        switch (structure)
        {
            case BASIC:
                f2=searchHashTable(f->name,f->type,e_VarDec);//means no func,type depends on f->type
                if(f2!=NULL){
                    if(curStructure[curStructDep]==NULL){
                        printError(3,vardec->child->lineno,vardec->child->nchar_data);
                        break;
                    }                   
                    //packing for suppose 7
                    FieldList packForSuppose7=curStructure[curStructDep]->tail;
                    int flag=0;
                    while (packForSuppose7!=NULL)
                    {
                        if(strcmp(f->name,packForSuppose7->name)==0){
                           flag=1; //make sure not same with it is own data
                        }
                        packForSuppose7=packForSuppose7->tail;
                    }
                    
                    if(flag==0){
                        printError(3,vardec->child->lineno,vardec->child->nchar_data);
                    }
                    else
                    {
                        insertHashTable(f,BASIC); 
                    }
                    
                    //return NULL;
                }
                else
                {
                    insertHashTable(f,BASIC);//BASIC ==STRUCTURE ==ARRAY
                }

                break;
            case FUNCTION://cause function para is useless. A(int x1,int x2);
                            // no it is useful
                            //TODO
                       
                f2=searchHashTable(f->name,f->type,e_VarDec);   
                if(f2!=NULL){
                    printError(3,vardec->child->lineno,vardec->child->nchar_data);
                }
                else
                {
                    insertHashTable(f,BASIC);//BASIC ==STRUCTURE ==ARRAY
                }
                
                //return f;
                break;
            case STRUCTURE:
                //packing for suppose 7
                pack7f1= (FieldList)malloc(sizeof(struct FieldList_));
                pack7f2=curStructure[curStructDep-1];
                while(pack7f2->tail!=NULL){
                    pack7f2=pack7f2->tail;
                }
                pack7f2->tail=pack7f1;
                pack7f1->tail=NULL;
                pack7f1->name=f->name;
                pack7f1->type=f->type;
                //packing for suppose 7
                f2=searchHashTable(f->name,f->type,e_VarDec);
                if(f2!=NULL){
                    //include suppose 7 
                    FieldList packForSuppose7=curStructure[curStructDep-1]->tail;
                    while (packForSuppose7->tail!=NULL)
                    {
                        if(strcmp(f->name,packForSuppose7->name)==0){
                            printError(15,vardec->child->lineno,vardec->child->nchar_data);
                            break;
                            //return NULL;
                        }
                        packForSuppose7=packForSuppose7->tail;
                    }
                }
                else
                {
                    insertHashTable(f,STRUCTURE);//STRUCTURE is same as BASIC, they both means no FUNCTION
                }
                break;

                
            default:
                break;
        }
        
        return f;
        break;
        }
    case e_VarDec: //VarDec LB INT RB
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = ARRAY;
        t->u.array.elem = type;
        t->u.array.size = atoi( vardec->child->brother->brother->nchar_data);
        //printf("%d\n",t->u.array.size);
        return VarDec(vardec->child, t,structure);
        break;
    default:
        break;
    }
    return f; //useless
}

FieldList FunDec(struct TreeNode *fundec, Type type)//checked no error
{ //ID LP VarList RP | ID LP RP
    if(fundec==NULL){
        return NULL;
    }

    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
    Type ttt=(Type)malloc(sizeof(struct Type_));
    f->name = fundec->child->nchar_data;

    //check name

    FieldList f2=searchHashTable(f->name,NULL,e_ExtDef);
    if(f2!=NULL){
        printError(4,fundec->child->lineno,fundec->child->nchar_data);
    }
    f->type=ttt;
    f->type->kind=FUNCTION;
    f->type->u.function.retType=type;
    f->type->u.function.para=NULL;
    f->tail = NULL;
    
    if (fundec->child->brother->brother->int_name == e_VarList)
    {   
        f->type->u.function.para=VarList(fundec->child->brother->brother);
    }
    return f;
}

FieldList VarList(struct TreeNode *varlist)//VarList-> ParamDec | ParamDec COMMA VarList
{
    if(varlist==NULL){
        return NULL;
    }
    FieldList f = ParamDec(varlist->child);
    if (varlist->child->brother != NULL)
    {
        
        //check if f is null ?
        if(f==NULL){
            
            f=VarList(varlist->child->brother->brother);
        }
        else
        {
            
            
            f->tail = VarList(varlist->child->brother->brother);
        }
        
        
    }
    return f;
}

FieldList ParamDec(struct TreeNode *paramdec)
{
    if(paramdec==NULL){
        return NULL;
    }
    Type t = Specifier(paramdec->child);
    if(t==NULL){
        return NULL; //pack in 0415
    }
    return VarDec(paramdec->child->brother, t,FUNCTION);
}

void CompSt(struct TreeNode *compst, Type type)
{ //LC DefList StmtList RC
    if(compst==NULL){
        return;
    }
    
    DefList(compst->child->brother, BASIC);
    StmtList(compst->child->brother->brother,type);
    //printf("hello\n");
}
void StmtList(struct TreeNode *stmtlist,Type type)//->int_name==e_NULL
{ //Stmt StmtList
    //if (stmtlist == NULL || stmtlist->child == NULL)
    if(stmtlist->int_name==e_NULL ||stmtlist==NULL||stmtlist->child==NULL)
    {
        return;
    }
    Stmt(stmtlist->child,type);
    //printf("Stmt2\n");
    StmtList(stmtlist->child->brother,type);
}
/*
Stmt : Exp SEMI 
    | CompSt 
    | RETURN Exp SEMI
    | IF LP Exp RP  Stmt %prec LOWER_THAN_ELSE 
    | IF LP Exp RP Stmt ELSE Stmt 
    | WHILE LP Exp RP Stmt 
*/
void Stmt(struct TreeNode *stmt,Type type)
{
    if(stmt==NULL){
        return;
    }
    Type t;
    switch (stmt->child->int_name)
    {
       
    case e_Exp:
        
        Exp(stmt->child,NULL);
        //printf("hello\n");
        break;
    case e_CompSt:
        CompSt(stmt->child, type);
        break;
    case e_RETURN:
        {
        #ifdef FASTER
        Operand zop=(Operand)malloc(sizeof(struct Operand_));
        zop->kind=OP_TMP_VARIABLE;
        zop->u.var_no=tmp_var_no;
        tmp_var_no+=1;
        #else
        #endif


        t = Exp(stmt->child->brother,zop);
        
        //check
        if(typeCheck(t,type)!=0){
            printError(8, stmt->child->lineno, stmt->child->nchar_data);
        }
        #ifdef FASTER
            InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
            zic->kind=IC_RETURN;
            zic->u.sinop.op1=zop;
            insertInterCodes(zic);
        #else
        #endif


        break;
        }
    case e_IF:
        {
        #ifdef FASTER
            Operand aop1=(Operand)malloc(sizeof(struct Operand_));
            aop1->kind=OP_LABEL;
            aop1->u.var_no=label_no;
            label_no+=1;
            Operand aop2=(Operand)malloc(sizeof(struct Operand_));
            aop2->kind=OP_LABEL;
            aop2->u.var_no=label_no;
            label_no+=1;

            translate_cond(stmt->child->brother->brother,aop1,aop2);
            //label1
            InterCode laic=(InterCode)malloc(sizeof(struct InterCode_));
            laic->kind=IC_LABEL;
            laic->u.sinop.op1=aop1;
            insertInterCodes(laic);

            Stmt(stmt->child->brother->brother->brother->brother,type);

            if(stmt->child->brother->brother->brother->brother->brother == NULL) //if () xxx  and no else xxx
            {
                //label2
                InterCode laic2=(InterCode)malloc(sizeof(struct InterCode_));
                laic2->kind=IC_LABEL;
                laic2->u.sinop.op1=aop2;
                insertInterCodes(laic2);
            }
            else
            {
                Operand aop3=(Operand)malloc(sizeof(struct Operand_));
                aop3->kind=OP_LABEL;
                aop3->u.var_no=label_no;
                label_no+=1;

                InterCode goic=(InterCode)malloc(sizeof(struct InterCode_));
                goic->kind=IC_GOTO;
                goic->u.sinop.op1=aop3;
                insertInterCodes(goic);

                //label2
                InterCode laic2=(InterCode)malloc(sizeof(struct InterCode_));
                laic2->kind=IC_LABEL;
                laic2->u.sinop.op1=aop2;
                insertInterCodes(laic2);

                Stmt(stmt->child->brother->brother->brother->brother->brother->brother,type);

                //label3
                InterCode laic3=(InterCode)malloc(sizeof(struct InterCode_));
                laic3->kind=IC_LABEL;
                laic3->u.sinop.op1=aop3;
                insertInterCodes(laic3);
            }
            

        #else
        #endif


        //comment in lab3 but they are useful
        // Exp(stmt->child->brother->brother);
        // Stmt(stmt->child->brother->brother->brother->brother,type);
        // if (stmt->child->brother->brother->brother->brother->brother != NULL)
        // {
        //     Stmt(stmt->child->brother->brother->brother->brother->brother->brother,type);
        // }
        break;
        }
    case e_WHILE:
        {
        #ifdef FASTER
            Operand aop1=(Operand)malloc(sizeof(struct Operand_));
    
            aop1->kind=OP_LABEL;
            aop1->u.var_no=label_no;
            label_no+=1;
            Operand aop2=(Operand)malloc(sizeof(struct Operand_));
            aop2->kind=OP_LABEL;
            aop2->u.var_no=label_no;
            label_no+=1;
            Operand aop3=(Operand)malloc(sizeof(struct Operand_));
            aop3->kind=OP_LABEL;
            aop3->u.var_no=label_no;
            label_no+=1;

            //label1
            InterCode laic=(InterCode)malloc(sizeof(struct InterCode_));
            laic->kind=IC_LABEL;
            laic->u.sinop.op1=aop1;
            insertInterCodes(laic);

            translate_cond(stmt->child->brother->brother,aop2,aop3);
            
            //label2
            InterCode laic2=(InterCode)malloc(sizeof(struct InterCode_));
            laic2->kind=IC_LABEL;
            laic2->u.sinop.op1=aop2;
            insertInterCodes(laic2);

            Stmt(stmt->child->brother->brother->brother->brother,type);

            InterCode goic=(InterCode)malloc(sizeof(struct InterCode_));
            goic->kind=IC_GOTO;
            goic->u.sinop.op1=aop1;
            insertInterCodes(goic);

            //label3
            InterCode laic3=(InterCode)malloc(sizeof(struct InterCode_));
            laic3->kind=IC_LABEL;
            laic3->u.sinop.op1=aop3;
            insertInterCodes(laic3);
        #else
        #endif

        //comment in lab3 but they are useful
        // Exp(stmt->child->brother->brother);
        // Stmt(stmt->child->brother->brother->brother->brother,type);
        break;
        }
    default:
        break;
    }
}
Type Exp(struct TreeNode *exp,Operand place)
{
    //printf("%d\n",1);
    FieldList f;
    Type t;
    Type t1;
    Type t2;
    switch (exp->child->int_name)
    {
    case e_Exp:
        {
            switch (exp->child->brother->int_name)
            {
            case e_ASSIGNOP:
                {
                #ifdef FASTER
                    Operand opt1=(Operand)malloc(sizeof(struct Operand_));
                    opt1->kind=OP_TMP_VARIABLE;
                    opt1->u.var_no=tmp_var_no;
                    tmp_var_no=tmp_var_no+1;
                    Operand opt2=(Operand)malloc(sizeof(struct Operand_));
                    opt2->kind=OP_TMP_VARIABLE;
                    opt2->u.var_no=tmp_var_no;
                    tmp_var_no=tmp_var_no+1;
                    int ron=opt2->u.var_no;
                #else
                    
                #endif
                t1 = Exp(exp->child,opt1);
                
                t2 = Exp(exp->child->brother->brother,opt2);
                if (t1 == NULL || t2 == NULL)
                {
                    return NULL;
                }
                if (t1->value == RIGHT_VALUE)
                {
                    printError(6, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                
                if (typeCheck(t1, t2) != 0)
                {
                    printError(5, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                
                #ifdef FASTER
                    if(!(t1->kind==ARRAY &&t2->kind==ARRAY)){
                        InterCode com=(InterCode)malloc(sizeof(struct InterCode_));
                        com->kind=IC_ASSIGN;
                        com->u.binop.left=opt1;
                        com->u.binop.right=opt2;
                        insertInterCodes(com);
                        if(place!=NULL){
                            InterCode com2=(InterCode)malloc(sizeof(struct InterCode_));
                            com2->kind=IC_ASSIGN;
                            com2->u.binop.left=place;
                            com2->u.binop.right=opt1;
                            insertInterCodes(com2);
                        }
                    }
                    else{ //int a[5],b[5],c[5]; a[5]=b[5]=c[5];
                    
                        //int copyNum=t2->u.array.size;
                        //if(t1->u.array.size<copyNum){
                        //    copyNum=t1->u.array.size;
                        //}
                        //a[5][4] b[4][5]
                        int copyNum=arrayLen(t2,1)/4;                 
                        if(arrayLen(t1,1)/4<copyNum){
                           copyNum=arrayLen(t1,1)/4;
                        }
                        
                        for(int i=0;i<copyNum;i++){//copy from opt2 to opt1
                            //Operand pack0value=(Operand)malloc(sizeof(struct Operand_));
                            //pack0value->kind=OP_TMP_VARIABLE;
                            //pack0value->u.var_no=tmp_var_no;
                            //tmp_var_no+=1;
                            Operand pack0=(Operand)malloc(sizeof(struct Operand_));
                            //pack0->kind=OP_TMP_ADDRESS;
                            pack0->kind=OP_TMP_VARIABLE;
                            pack0->u.var_no=tmp_var_no;
                            //pack0->u.real_value=pack0value;
                            tmp_var_no+=1;

                            Operand pack1=(Operand)malloc(sizeof(struct Operand_));
                            pack1->kind=OP_TMP_VARIABLE;
                            pack1->u.var_no=tmp_var_no;
                            tmp_var_no+=1;

                            //Operand pack2value=(Operand)malloc(sizeof(struct Operand_));
                            //pack2value->kind=OP_TMP_VARIABLE;
                            //pack2value->u.var_no=tmp_var_no;
                            //tmp_var_no+=1;
                            Operand pack2=(Operand)malloc(sizeof(struct Operand_));
                            //pack2->kind=OP_TMP_ADDRESS;
                            pack2->kind=OP_TMP_VARIABLE;
                            pack2->u.var_no=tmp_var_no;
                            //pack2->u.real_value=pack2value;
                            tmp_var_no+=1;
                            //distance
                            Operand pack3=(Operand)malloc(sizeof(struct Operand_));
                            pack3->kind=OP_CONSTANT;
                            sprintf(pack3->u.value,"%d",i*4);
                            // &a+20
                            InterCode packzi0=(InterCode)malloc(sizeof(struct InterCode_));
                            packzi0->kind=IC_ADD;
                            packzi0->u.triop.op1=opt1;
                            packzi0->u.triop.op2=pack3;
                            packzi0->u.triop.result=pack0;
                            insertInterCodes(packzi0);
                            // &a+20
                            InterCode packzi=(InterCode)malloc(sizeof(struct InterCode_));
                            packzi->kind=IC_ADD;
                            packzi->u.triop.op1=opt2;
                            packzi->u.triop.op2=pack3;
                            packzi->u.triop.result=pack2;
                            insertInterCodes(packzi);
                            //  t1=*(&b+20)
                            InterCode packzi2=(InterCode)malloc(sizeof(struct InterCode_));
                            packzi2->kind=IC_GET_VAR;
                            packzi2->u.binop.left=pack1;
                            packzi2->u.binop.right=pack2;
                            insertInterCodes(packzi2);
                            //  *(&a+20)=t1
                            InterCode packzi3=(InterCode)malloc(sizeof(struct InterCode_));
                            packzi3->kind=IC_TO_VAR;
                            packzi3->u.binop.left=pack0;
                            packzi3->u.binop.right=pack1;
                            insertInterCodes(packzi3);


                        }
                        
                        if(place!=NULL){
                            InterCode com2=(InterCode)malloc(sizeof(struct InterCode_));
                            com2->kind=IC_ASSIGN;
                            com2->u.binop.left=place;
                            com2->u.binop.right=opt1;
                            insertInterCodes(com2);
                        }
                        
                    }
                    
                #else
                    
                    
                #endif



                return t1;
                break;
                }
            case e_PLUS:

            case e_MINUS:

            case e_STAR:

            case e_DIV:
                {
                #ifdef FASTER
                    Operand opt1=(Operand)malloc(sizeof(struct Operand_));
                    opt1->kind=OP_TMP_VARIABLE;
                    opt1->u.var_no=tmp_var_no;
                    tmp_var_no=tmp_var_no+1;
                    Operand opt2=(Operand)malloc(sizeof(struct Operand_));
                    opt2->kind=OP_TMP_VARIABLE;
                    opt2->u.var_no=tmp_var_no;
                    tmp_var_no=tmp_var_no+1;
                    int ron=opt2->u.var_no;
                #else
                    
                #endif
                t1 = Exp(exp->child,opt1);
                t2 = Exp(exp->child->brother->brother,opt2);
                if (t1 == NULL || t2 == NULL)
                {
                    return NULL;
                }
                if (t1->kind == BASIC && t2->kind == BASIC && t1->u.basic==t2->u.basic)
                {
                    t1->value = RIGHT_VALUE;
                    if(place!=NULL){
                        #ifdef FASTER
                            if(place->kind==OP_TMP_VARIABLE&&opt1->kind==OP_CONSTANT&&opt2->kind==OP_CONSTANT){
                                int res=0;
                                switch(exp->child->brother->int_name)
                                {
                                    case e_PLUS:
                                        res=atoi(opt1->u.value)+atoi(opt2->u.value);
                                        break;
                                    case e_MINUS:
                                        res=atoi(opt1->u.value)-atoi(opt2->u.value);
                                        break;
                                    
                                    case e_STAR:
                                        res=atoi(opt1->u.value)*atoi(opt2->u.value);
                                        break;
                                    case e_DIV:
                                        if(atoi(opt2->u.value)==0){
                                            //undifined
                                        }else{
                                            res=atoi(opt1->u.value)/atoi(opt2->u.value);
                                        }
                                        
                                        break;
                                    default:
                                        break;
                                }
                                place->kind=OP_CONSTANT;
                                sprintf(place->u.value,"%d",res);
                            }
                            else{
                                InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
                                zic->next=NULL;
                                zic->prev=NULL;
                                switch (exp->child->brother->int_name)
                                {
                                case e_PLUS:
                                    zic->kind=IC_ADD;
                                    break;
                                case e_MINUS:
                                zic->kind=IC_SUB;
                                break;
                                case e_STAR:
                                zic->kind=IC_MUL;
                                break;
                                case e_DIV:
                                zic->kind=IC_DIV;
                                break;
                                default:
                                    break;
                                }
                                zic->u.triop.op1=opt1;
                                zic->u.triop.op2=opt2;
                                zic->u.triop.result=place;
                                insertInterCodes(zic);
                            }

                        #else
                        #endif
                    }

                    
                    return t1;
                }
                else
                {
                    printError(7, exp->child->lineno, exp->child->nchar_data);
                    //printf("here\n");
                    return NULL;
                }
                break;
                }
            case e_AND:

            case e_OR:

            case e_RELOP:
                {
                #ifdef FASTER
                Operand aop1=(Operand)malloc(sizeof(struct Operand_));
                aop1->kind=OP_LABEL;
                aop1->u.var_no=label_no;
                label_no+=1;
                Operand aop2=(Operand)malloc(sizeof(struct Operand_));
                aop2->kind=OP_LABEL;
                aop2->u.var_no=label_no;
                label_no+=1;
                //place:=#0
                if(place!=NULL){
                    InterCode aic=(InterCode)malloc(sizeof(struct InterCode_));
                    aic->kind=IC_ASSIGN;
                    aic->u.binop.left=place;

                    Operand aop0=(Operand)malloc(sizeof(struct Operand_)); //could be emit
                    aop0->kind=OP_CONSTANT;
                    sprintf(aop0->u.value,"%d",0);
                    aic->u.binop.right=aop0;
                    insertInterCodes(aic);
                }

                Type lab3t1=translate_cond(exp,aop1,aop2); //a serious bug!!!

                InterCode laic=(InterCode)malloc(sizeof(struct InterCode_));
                laic->kind=IC_LABEL;
                laic->u.sinop.op1=aop1;
                insertInterCodes(laic);
                //place:=#1
                if(place!=NULL){
                    InterCode aic=(InterCode)malloc(sizeof(struct InterCode_));
                    aic->kind=IC_ASSIGN;
                    aic->u.binop.left=place;

                    Operand aop0=(Operand)malloc(sizeof(struct Operand_)); //could be emit
                    aop0->kind=OP_CONSTANT;
                    sprintf(aop0->u.value,"1");
                    aic->u.binop.right=aop0;
                    insertInterCodes(aic);
                }

                InterCode laic2=(InterCode)malloc(sizeof(struct InterCode_));
                laic2->kind=IC_LABEL;
                laic2->u.sinop.op1=aop2;
                insertInterCodes(laic2);

                #else

                #endif
                return lab3t1;
                break;
                }
            case e_LB://like a[5]
                {
                Operand base=(Operand)malloc(sizeof(struct Operand_));
                base->kind=OP_TMP_VARIABLE;
                base->u.var_no=tmp_var_no;
                tmp_var_no+=1;

                t = Exp(exp->child,base);
                if (t == NULL)
                {
                    return NULL;
                }
                if (t->kind != ARRAY)
                {
                    printError(10, exp->child->lineno, exp->child->child->nchar_data); //BUG?
                    return NULL;
                }
                
                //int simplelen=1;
                Operand sub=(Operand)malloc(sizeof(struct Operand_));
                sub->kind=OP_TMP_VARIABLE;
                sub->u.var_no=tmp_var_no;
                tmp_var_no+=1;
                
                Type t2 = Exp(exp->child->brother->brother,sub);



                if (t2 == NULL)
                {
                    return NULL;
                }
                if (t2->kind == BASIC && t2->u.basic== UBASIC_INT) //include other types?
                {
                    
                    Operand width=(Operand)malloc(sizeof(struct Operand_));
                    width->kind=OP_TMP_VARIABLE;
                    width->u.var_no=tmp_var_no;
                    tmp_var_no+=1;
                    //if(atoi(sub->u.value)!=0)
                        Operand wop=(Operand)malloc(sizeof(struct Operand_));
                        wop->kind=OP_CONSTANT;
                        //sprintf(wop->u.value,"%d",t->u.array.size);
                        sprintf(wop->u.value,"%d",arrayLen(t,2));
                        //printf("%d %d %d\n",t->u.array.size,t->u.array.elem->u.array.size,t->u.array.elem->u.array.elem->u.array.size);
                        
                        InterCode wic=(InterCode)malloc(sizeof(struct InterCode_));
                        wic->kind=IC_MUL;
                        wic->u.triop.op1=sub;
                        wic->u.triop.op2=wop;
                        wic->u.triop.result=width;


                        if(sub->kind==OP_CONSTANT&&wop->kind==OP_CONSTANT){
                            int widthoutput=atoi(sub->u.value)*atoi(wop->u.value);
                            width->kind=OP_CONSTANT;
                            sprintf(width->u.value,"%d",widthoutput);
                        }
                        else if(sub->kind==OP_CONSTANT&&atoi(sub->u.value)==0 || wop->kind==OP_CONSTANT&&atoi(wop->u.value)==0){
                            width->kind=OP_CONSTANT;
                            sprintf(width->u.value,"0");
                        }
                        else{
                            insertInterCodes(wic);
                        }

                        // InterCode wic2=(InterCode)malloc(sizeof(struct InterCode_));
                        // wic2->kind=IC_GET_ADDR2;
                        // wic2->u.triop.op1=base;
                        // wic2->u.triop.op2=width;
                        // wic2->u.triop.result=place;
                        if(place!=NULL){

                        
                            InterCode wic2=(InterCode)malloc(sizeof(struct InterCode_));
                            //wic2->kind=IC_ADD;
                            wic2->kind=IC_GET_ADDR2;
                            wic2->u.triop.op1=base;
                            wic2->u.triop.op2=width;
                            wic2->u.triop.result=place;
                            if(t->u.array.elem->kind==BASIC){
                                //wic2->kind=IC_GET_ADDR2;
                                Operand tmp=(Operand)malloc(sizeof(struct Operand_));
                                tmp->kind=OP_TMP_VARIABLE;
                                tmp->u.var_no=tmp_var_no;
                                tmp_var_no+=1;
                                wic2->u.triop.result=tmp;
                                place->kind=OP_TMP_ADDRESS;
                                place->u.real_value=tmp;
                            }
                            else{
                                place->kind=OP_TMP_VARIABLE;
                                place->u.var_no=tmp_var_no;
                                tmp_var_no+=1;
                            }

                            //pack
                            wic2->kind=IC_ADD;
                            // if(exp->child->child->int_name==e_ID){
                            //      FieldList lab3f=searchHashTable(base->u.value,NULL,-1);
                            //      if(lab3f->is_in_params==1)
                            //        wic->kind=IC_ADD;
                            // }
                            // else{
                            //     wic2->kind=IC_ADD;
                            // }
                            insertInterCodes(wic2);
                            

                        }




                    Type finDebug=(Type)malloc(sizeof(struct Type_));
                    memcpy(finDebug,t->u.array.elem,sizeof(struct Type_));
                    finDebug->value = BOTH_VALUE;
                    return finDebug;
                }
                else{
                    printError(12, exp->child->lineno, exp->child->brother->brother->child->nchar_data);
                    return NULL;
                }
                

                break;
                }
            case e_DOT://no structure so no possible to here
                {
                t = Exp(exp->child,place);
                if (t == NULL)
                {
                    return NULL;
                }
                if (t->kind != STRUCTURE)
                {
                    printError(13, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                f = t->u.structure->tail;
                while (f != NULL)
                {
                    if (strcmp(f->name, exp->child->brother->brother->nchar_data) == 0)
                    {
                        Type t2=(Type)malloc(sizeof(struct Type_));
                        memcpy(t2,f->type,sizeof(struct Type_));
                        t2->value=BOTH_VALUE;
                        return t2;
                    }
                    f = f->tail;
                }
                printError(14, exp->child->lineno, exp->child->brother->brother->nchar_data);
                return NULL;
                break;
                }
            default:
                break;
            }
        }
        break;
    case e_LP:
        return Exp(exp->child->brother,place);
        break;
    case e_MINUS://same as e_NOT //now it is different in lab 3
        {
        #ifdef FASTER
            Operand zop=(Operand)malloc(sizeof(struct Operand_));
            zop->next=NULL;
            zop->prev=NULL;
            zop->kind=OP_TMP_VARIABLE;
            zop->u.var_no=tmp_var_no;
            tmp_var_no+=1;
        #else
        #endif

        t = Exp(exp->child->brother,zop);
        if (t == NULL)
        {
            return NULL;
        }
        //if(t->kind==ARRAY||t->kind==STRUCTURE){
        if (t->kind != BASIC)//_INT && t->kind != BASIC_FLOAT)
        {
            printError(7, exp->child->lineno, exp->child->nchar_data);
            return NULL;
        }
        t->value = RIGHT_VALUE;

        #ifdef FASTER
            if(place!=NULL){
                if(zop->kind==OP_CONSTANT){
                    place->kind=OP_CONSTANT;
                    int zk=atoi(zop->u.value);
                    zk=0-zk;
                    sprintf(place->u.value,"%d",zk);
                }
                else{
                    Operand zxc=(Operand)malloc(sizeof(struct Operand_));
                    zxc->next=NULL;
                    zxc->prev=NULL;
                    zxc->kind=OP_CONSTANT;
                    sprintf(zxc->u.value,"0");

                    InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
                    zic->next=NULL;
                    zic->prev=NULL;
                    zic->kind=IC_SUB;
                    zic->u.triop.op1=zxc;
                    zic->u.triop.op2=zop;
                    zic->u.triop.result=place;
                    insertInterCodes(zic);
                }
            }
        #else
            if(place!=NULL){
               
                Operand zxc=(Operand)malloc(sizeof(struct Operand_));
                zxc->next=NULL;
                zxc->prev=NULL;
                zxc->kind=OP_CONSTANT;
                sprintf(zxc->u.value,"0");

                InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
                zic->next=NULL;
                zic->prev=NULL;
                zic->kind=IC_SUB;
                zic->u.triop.op1=zxc;
                zic->u.triop.op2=zop;
                zic->u.triop.result=place;
                insertInterCodes(zic);
                
            }
        #endif
        }

        return t;

        
        break;
    case e_NOT: //suppose ! can be used both integer and float , so it can be shared with MINUS. //now it is different in lab 3
        {
         #ifdef FASTER
            Operand aop1=(Operand)malloc(sizeof(struct Operand_));
            aop1->next=NULL;
            aop1->prev=NULL;
            aop1->kind=OP_LABEL;
            aop1->u.var_no=label_no;
            label_no+=1;
            Operand aop2=(Operand)malloc(sizeof(struct Operand_));
            aop2->next=NULL;
            aop2->prev=NULL;
            aop2->kind=OP_LABEL;
            aop2->u.var_no=label_no;
            label_no+=1;
            //place:=#0
            if(place!=NULL){
                InterCode aic=(InterCode)malloc(sizeof(struct InterCode_));
                aic->next=NULL;
                aic->prev=NULL;
                aic->kind=IC_ASSIGN;
                aic->u.binop.left=place;

                Operand aop0=(Operand)malloc(sizeof(struct Operand_)); //could be emit
                aop0->next=NULL;
                aop0->prev=NULL;
                aop0->kind=OP_CONSTANT;
                sprintf(aop2->u.value,"0");
                aic->u.binop.right=aop0;
                insertInterCodes(aic);
            }

            Type lab3t1=translate_cond(exp->child,aop1,aop2);

            InterCode laic=(InterCode)malloc(sizeof(struct InterCode_));
            laic->next=NULL;
            laic->prev=NULL;
            laic->kind=IC_LABEL;
            laic->u.sinop.op1=aop1;
            insertInterCodes(laic);
            //place:=#1
            if(place!=NULL){
                InterCode aic=(InterCode)malloc(sizeof(struct InterCode_));
                aic->next=NULL;
                aic->prev=NULL;
                aic->kind=IC_ASSIGN;
                aic->u.binop.left=place;

                Operand aop0=(Operand)malloc(sizeof(struct Operand_)); //could be emit
                aop0->next=NULL;
                aop0->prev=NULL;
                aop0->kind=OP_CONSTANT;
                sprintf(aop2->u.value,"1");
                aic->u.binop.right=aop0;
                insertInterCodes(aic);
            }

            InterCode laic2=(InterCode)malloc(sizeof(struct InterCode_));
            laic2->next=NULL;
            laic2->prev=NULL;
            laic2->kind=IC_LABEL;
            laic2->u.sinop.op1=aop2;
            insertInterCodes(laic2);

            #else

            #endif

        return lab3t1;
        }
        break;

    case e_ID:// ID | ID LP Args RP | ID LP RP
    {
        if (exp->child->brother == NULL) //ID
        {
             f = searchHashTable(exp->child->nchar_data,NULL,e_Exp);
             //struct a{ int b;}c; a and c 's type 's kind both is STRUCTURE. so we need to distinguish them by value left or right.
            
            //if (f == NULL || f->type->kind != VARIABLE) // concern about whether variable or function
            if(f==NULL ||f->type->kind==STRUCTURE&&f->type->value==LEFT_VALUE)//struct a{ int b;}c; a and c 's type 's kind both is STRUCTURE. so we need to distinguish them by value left or right.
            {
                printError(1, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            else
            {
                t = (Type)malloc(sizeof(struct Type_));
                memcpy(t,f->type,sizeof(struct Type_));
                //t->kind = f->type->kind;
                //t->u = f->type->u;
                //t->value = LEFT_VALUE;
                t->value=BOTH_VALUE;
                
                //lab3
                #ifdef FASTER
                //sprintf(place->u.value,"%d",0);
                if(place!=NULL){
                     strcpy(place->u.value,exp->child->nchar_data);
                
                     place->kind=OP_VARIABLE;
                }
               
                #else
                strcpy(place->u.value,exp->child->nchar_data);
                place->kind=OP_VARIABLE;
                #endif

                return t;
            }
        }
        else if (exp->child->brother->brother->brother == NULL) //ID LP RP
        {
            f = searchHashTable(exp->child->nchar_data,NULL,e_NOT);
            if (f == NULL)
            {
                printError(2, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            else if (f->type->kind != FUNCTION)
            {
                //printError(11, exp->child->lineno, exp->child->nchar_data);
                printError(11, exp->child->lineno, f->name);
                return NULL;
            }
            //else if (f->type->u.structure->tail != NULL)
            else if(f->type->u.function.para!=NULL)
            {
                printError(9, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            else
            {
                t = (Type)malloc(sizeof(struct Type_));
                memcpy(t,f->type->u.function.retType,sizeof(struct Type_));
                //t->kind = f->type->kind;
                //t->u = f->type->u;
                //t->value = LEFT_VALUE;
                t->value=RIGHT_VALUE;

                //lab3
                if(strcmp(f->name,"read")==0){
                    if(place!=NULL){
                        InterCode newIC=(InterCode)malloc(sizeof(struct InterCode_));
                        newIC->kind=IC_READ;
                        newIC->u.sinop.op1=place;
                        insertInterCodes(newIC);
                    }
                   
                }
                else
                {
                    Operand newOP=(Operand)malloc(sizeof(struct Operand_));
                    newOP->kind=OP_FUNCTION;
                    strcpy(newOP->u.value,f->name);

                    if(place!=NULL){
                        InterCode newIC=(InterCode)malloc(sizeof(struct InterCode_));
                        newIC->kind=IC_CALL;
                        newIC->u.binop.left=place;
                        newIC->u.binop.right=newOP;
                        insertInterCodes(newIC);
                    }
                    else
                    {
                        Operand newplace=(Operand)malloc(sizeof(struct Operand_));
                        newplace->kind=OP_TMP_VARIABLE;
                        newplace->u.var_no=tmp_var_no;
                        tmp_var_no=tmp_var_no+1;

                        InterCode newIC=(InterCode)malloc(sizeof(struct InterCode_));
                        newIC->kind=IC_CALL;
                        newIC->u.binop.left=newplace;
                        newIC->u.binop.right=newOP;
                        insertInterCodes(newIC);
                    }
                    
                }
                
                
                return t;
            }
        }
        else //ID LP Args RP   //function using
        {
             f = searchHashTable(exp->child->nchar_data,NULL,e_NOT);
            if (f == NULL)
            {
                printError(2, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            else if (f->type->kind != FUNCTION)
            {
                //printError(11, exp->child->lineno, exp->child->nchar_data);
                printError(11, exp->child->lineno, f->name);
                return NULL;
            }
            else
            { //check if the parameters are consistent
                
                 //lab3
                Operand newOP=(Operand)malloc(sizeof(struct Operand_));
                newOP->next=NULL;
                newOP->prev=NULL;//useless

                //FieldList tmpAF = f->type->u.structure->tail; //not structure ,itis func!!!!
                FieldList tmpAF = f->type->u.function.para;
                FieldList tmpBF = Args(exp->child->brother->brother,newOP);
                //printf("%d %d\n",tmpAF->type->kind,tmpBF->type->kind);
                int same = 1;
                while (tmpAF != NULL && tmpBF != NULL)
                {
                    //if (tmpAF->type == tmpBF->type)
                    if(typeCheck(tmpAF->type,tmpBF->type)==0)
                    {
                        tmpAF = tmpAF->tail;
                        tmpBF = tmpBF->tail;
                    }
                    else
                    {
                        same = 0;
                        break;
                    }
                }
                if (tmpAF == NULL && tmpBF == NULL && same == 1)
                {
                    t = (Type)malloc(sizeof(struct Type_));
                    memcpy(t,f->type->u.function.retType,sizeof(struct Type_));
                    t->value=RIGHT_VALUE;


                    //lab3
                    if(strcmp(f->name,"write")==0){
                        //if(place!=NULL){
                        InterCode newIC=(InterCode)malloc(sizeof(struct InterCode_));
                        newIC->kind=IC_WRITE;
                        newIC->u.sinop.op1=newOP->next;
                        insertInterCodes(newIC);
                        if(place!=NULL){
                            //below is to make [place:=#0] which means return 0;
                            Operand newOP2=(Operand)malloc(sizeof(struct Operand_));
                            newOP2->next=NULL;
                            newOP2->prev=NULL;//useless
                            newOP2->kind=OP_CONSTANT;
                            //newOP2->u.value=0;
                            strcpy(newOP2->u.value,"0");

                            InterCode newIC2=(InterCode)malloc(sizeof(struct InterCode_));
                            newIC2->kind=IC_ASSIGN;
                            newIC2->u.binop.left=place;
                            newIC2->u.binop.right=newOP2;
                            insertInterCodes(newIC2);
                        }
                        
                        //}
                    }
                    else{
                        
                        Operand zOP=newOP->next; //=(Operand)malloc(sizeof(struct Operand_));
                        while (zOP!=NULL)
                        {
                            InterCode newIC=(InterCode)malloc(sizeof(struct InterCode_));
                            newIC->kind=IC_ARG;
                            newIC->u.sinop.op1=zOP;
                            insertInterCodes(newIC);
                            zOP=zOP->next;
                        }

                        //below is to make [place:=CALL function.name] 
                        Operand newOP2=(Operand)malloc(sizeof(struct Operand_));
                        newOP2->next=NULL;
                        newOP2->prev=NULL;//useless
                        newOP2->kind=OP_FUNCTION;
                        strcpy(newOP2->u.value,f->name);
                        
                        if(place!=NULL){
                            InterCode newIC2=(InterCode)malloc(sizeof(struct InterCode_));
                            newIC2->kind=IC_CALL;
                            newIC2->u.binop.left=place;
                            newIC2->u.binop.right=newOP2;
                            insertInterCodes(newIC2);
                                
                        }
                        else
                        {
                            Operand znewplace=(Operand)malloc(sizeof(struct Operand_));
                            znewplace->kind=OP_TMP_VARIABLE;
                            znewplace->u.var_no=tmp_var_no;
                            tmp_var_no=tmp_var_no+1;


                            InterCode newIC2=(InterCode)malloc(sizeof(struct InterCode_));
                            newIC2->kind=IC_CALL;
                            newIC2->u.binop.left=znewplace;
                            newIC2->u.binop.right=newOP2;
                            newIC2->next=NULL;
                            newIC2->prev=NULL;
                            insertInterCodes(newIC2);
                        }
                        
                    }
                    



                    return t;
                }
                else
                {
                    printError(9, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
            }
        }
    }
    break;
    case e_INT:
        {
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->u.basic=UBASIC_INT;
        t->value = RIGHT_VALUE;


        //lab3
        if(place!=NULL){
            #ifdef FASTER
            strcpy(place->u.value,exp->child->nchar_data);
            place->kind=OP_CONSTANT;
            //printf("here\n");
            #else
            #endif
        }
       
        return t;
        break;
        }
    case e_FLOAT:
        {
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->u.basic=UBASIC_FLOAT;
        t->value = RIGHT_VALUE;

        if(place!=NULL){
            #ifdef FASTER
            strcpy(place->u.value,exp->child->nchar_data);
            place->kind=OP_CONSTANT;
            #else
            #endif
        }
        return t;
        break;
        }
    default:
        break;
    }
}
FieldList Args(struct TreeNode *args,Operand place)
{ //Exp COMMA Args | Exp
    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));

    //lab3
    Operand newOP=(Operand)malloc(sizeof(struct Operand_));
    newOP->kind=OP_TMP_VARIABLE;
    newOP->u.var_no=tmp_var_no;
    tmp_var_no=tmp_var_no+1;


    Type finDebug=Exp(args->child,newOP);
    f->type=finDebug;
    f->name=NULL;
    //f->name = args->child->nchar_data;
    //f->type = Exp(args->child); //if Exp error, it will return NULL
    f->tail = NULL;


    //insert to tail
    // if(place->next==NULL){
    //     place->next=newOP;
    //     newOP->next=NULL;
    // }
    // else
    // {
    //     Operand tmp=place;
    //     while(tmp->next!=NULL){
    //         tmp=tmp->next;
    //     }
    //     tmp->next=newOP;
    //     newOP->next=NULL;
    // }
    //insert to head
    newOP->next=place->next;
    place->next=newOP; 
    


    if (args->child->brother != NULL) //exp COMMA args
    {
        if (f == NULL) //it seems impossible
        {
            f = Args(args->child->brother->brother,place);
        }
        else
        {
            FieldList tmp = f;
            while (tmp->tail != NULL)
            {
                tmp = tmp->tail;
            }
            tmp->tail = Args(args->child->brother->brother,place);
        }
    }
    return f;
}

void printError(int errorType, int line, char *name)
{
    switch (errorType)
    {
    case 1:
        printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", line, name);
        break;
    case 2:
        printf("Error type 2 at Line %d: Undefined function \"%s\".\n", line, name);
        break;
    case 3:
        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", line, name);
        break;
    case 4:
        printf("Error type 4 at Line %d: Redefined function \"%s\".\n", line, name);
        break;
    case 5:
        printf("Error type 5 at Line %d: Type mismatched for assignment.\n", line);
        break;
    case 6:
        printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", line);
        break;
    case 7:
        printf("Error type 7 at Line %d: Type mismatched for operands.\n", line);
        break;
    case 8:
        printf("Error type 8 at Line %d: Type mismatched for return.\n", line);
        break;
    case 9:
        printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments.\n", line,name);
        break;
    case 10:
        printf("Error type 10 at Line %d: \"%s\" is not an array.\n", line, name);
        break;
    case 11:
        printf("Error type 11 at Line %d: \"%s\" is not a function.\n", line, name);
        break;
    case 12:
        printf("Error type 12 at Line %d: \"%s\" is not an integer.\n", line, name);
        break;
    case 13:
        printf("Error type 13 at Line %d: Illegal use of \".\".\n", line);
        break;
    case 14:
        printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", line, name);
        break;
    case 15:
        printf("Error type 15 at Line %d: Redefined field \"%s\".\n", line, name);
        break;
    case 16:
        printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", line, name);
        break;
    case 17:
        printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", line, name);
        break;
    case 152:
        printf("Error type 15 at Line %d: Cannot initialize field when to define the structure.\n", line);
        break;
    default:
        break;
    }
}

Type translate_cond(struct TreeNode* exp,Operand trueop,Operand falseop){
    switch (exp->child->int_name)
    {
    case e_Exp:
        {
            switch (exp->child->brother->int_name)
            {
            case e_AND:
                {
                Operand aop1=(Operand)malloc(sizeof(struct Operand_));
                aop1->kind=OP_LABEL;
                aop1->u.var_no=label_no;
                label_no+=1;
                Type t1 = translate_cond(exp->child,aop1,falseop);

                InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
                zz->kind=IC_LABEL;
                zz->u.sinop.op1=aop1;
                insertInterCodes(zz);

                Type t2 = translate_cond(exp->child->brother->brother,trueop,falseop);

                if (t1 == NULL || t2 == NULL)
                {
                    return NULL;
                }
                if (t1->kind == BASIC && t2->kind == BASIC && t1->u.basic==t2->u.basic)
                {
                    t1->value = RIGHT_VALUE;
                    return t1;
                }
                else
                {
                    printError(7, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                break;
                }
            case e_OR:
                {
                Operand aop1=(Operand)malloc(sizeof(struct Operand_));
                aop1->kind=OP_LABEL;
                aop1->u.var_no=label_no;
                label_no+=1;
                Type t1 = translate_cond(exp->child,trueop,aop1);

                InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
                zz->kind=IC_LABEL;
                zz->u.sinop.op1=aop1;
                insertInterCodes(zz);

                Type t2 = translate_cond(exp->child->brother->brother,trueop,falseop);
                
                if (t1 == NULL || t2 == NULL)
                {
                    return NULL;
                }
                if (t1->kind == BASIC && t2->kind == BASIC && t1->u.basic==t2->u.basic)
                {
                    t1->value = RIGHT_VALUE;
                    return t1;
                }
                else
                {
                    printError(7, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                break;
                }
            case e_RELOP:
                {
                Operand aop1=(Operand)malloc(sizeof(struct Operand_));
                aop1->kind=OP_TMP_VARIABLE;
                aop1->u.var_no=tmp_var_no;
                tmp_var_no+=1;
                Operand aop2=(Operand)malloc(sizeof(struct Operand_));
                aop2->kind=OP_TMP_VARIABLE;
                aop2->u.var_no=tmp_var_no;
                tmp_var_no+=1;
                Type lab3t1=Exp(exp->child,aop1);
                Type lab3t2=Exp(exp->child->brother->brother,aop2);
                //printf("%d",aop2->kind);
           
                if (lab3t1 == NULL || lab3t2 == NULL)
                {
                    return NULL;
                }
                if (lab3t1->kind == BASIC && lab3t2->kind == BASIC && lab3t1->u.basic==lab3t2->u.basic)
                {
                    lab3t1->value = RIGHT_VALUE;
                    InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
                    zic->kind=IC_IFGOTO;
                    zic->u.ifgotoop.op1=aop1;
                    zic->u.ifgotoop.op2=aop2;
                    zic->u.ifgotoop.result=trueop;
                    strcpy(zic->u.ifgotoop.relop,exp->child->brother->nchar_data);
                    insertInterCodes(zic);

                    InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
                    zz->kind=IC_GOTO;
                    zz->u.sinop.op1=falseop;
                    insertInterCodes(zz);
                    return lab3t1;
                }
                else
                {
                    printError(7, exp->child->lineno, exp->child->nchar_data);
                    return NULL;
                }
                break;
                }
            default:
                {
                Operand aop1=(Operand)malloc(sizeof(struct Operand_));
                aop1->kind=OP_TMP_VARIABLE;
                aop1->u.var_no=tmp_var_no;
                tmp_var_no+=1;

                Type lab3t1=Exp(exp,aop1);//exp not exp->child

                Operand aop2=(Operand)malloc(sizeof(struct Operand_));
                aop2->kind=OP_CONSTANT;
                strcpy(aop2->u.value,"0");
                

                InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
                zic->kind=IC_IFGOTO;
                zic->u.ifgotoop.op1=aop1;
                zic->u.ifgotoop.op2=aop2;
                zic->u.ifgotoop.result=trueop;
                strcpy(zic->u.ifgotoop.relop,"!=");
                insertInterCodes(zic);

                InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
                zz->kind=IC_GOTO;
                zz->u.sinop.op1=falseop;
                insertInterCodes(zz);
                return lab3t1;
                }
            break;
            }
        }
        break;    
    case e_LP:{
        return translate_cond(exp->child->brother,trueop,falseop);
        break;
    }
    
    case e_MINUS:{
        
            if(atoi(exp->child->brother->child->nchar_data)==0){
                //printf("here\n");
            }
            else{
                InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
                zz->kind=IC_GOTO;
                zz->u.sinop.op1=falseop;
                insertInterCodes(zz);
            }
            //printf("hello\n");
            Type t=translate_cond(exp->child->brother,falseop,trueop);
            if (t == NULL)
            {
                return NULL;
            }
            //if(t->kind==ARRAY||t->kind==STRUCTURE){
            if (t->kind != BASIC)//_INT && t->kind != BASIC_FLOAT)
            {
                printError(7, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            t->value = RIGHT_VALUE;
            return t;
            break;
        
        }
        break;
    
    
    case e_INT:{
        Type t=Exp(exp,NULL);
        
        if(atoi(exp->child->nchar_data)!=0){//if(1)
            // InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
            // zz->kind=IC_GOTO;
            // zz->u.sinop.op1=trueop;
            // insertInterCodes(zz);
        }
        else{//if(0)
            InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
            zz->kind=IC_GOTO;
            zz->u.sinop.op1=falseop;
            insertInterCodes(zz);
        }
        return t;
        break;
    } 
    
    case e_NOT:
        {
        Type t=translate_cond(exp->child->brother,falseop,trueop);//exp not
        if (t == NULL)
        {
            return NULL;
        }
        //if(t->kind==ARRAY||t->kind==STRUCTURE){
        if (t->kind != BASIC)//_INT && t->kind != BASIC_FLOAT)
        {
            printError(7, exp->child->lineno, exp->child->nchar_data);
            return NULL;
        }
        t->value = RIGHT_VALUE;
        return t;
        break;
        }
    
    
    
    default:
        {
        Operand aop1=(Operand)malloc(sizeof(struct Operand_));
        aop1->kind=OP_TMP_VARIABLE;
        aop1->u.var_no=tmp_var_no;
        tmp_var_no+=1;

        Type lab3t1=Exp(exp,aop1);//exp not exp->child

        Operand aop2=(Operand)malloc(sizeof(struct Operand_));
        aop2->kind=OP_CONSTANT;
        strcpy(aop2->u.value,"0");
        

        InterCode zic=(InterCode)malloc(sizeof(struct InterCode_));
        zic->kind=IC_IFGOTO;
        zic->u.ifgotoop.op1=aop1;
        zic->u.ifgotoop.op2=aop2;
        zic->u.ifgotoop.result=trueop;
        strcpy(zic->u.ifgotoop.relop,"!=");
        insertInterCodes(zic);

        InterCode zz=(InterCode)malloc(sizeof(struct InterCode_));
        zz->kind=IC_GOTO;
        zz->u.sinop.op1=falseop;
        insertInterCodes(zz);
        return lab3t1;

        break;
        }
    }
}