#include "lab2.h"
#include "syntax.tab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include"lab1.h"




int curFuncDep; //0 means no func,1 means main

//packing for samples which go against suppose 7, and curStructure's tail is it is para.
FieldList curStructure[FuncDepSize];
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
    //printf("%d\n",val);
    return val;
}

int deleteCurFuncDep(){//insert to head of the FuncDep
    FieldList f=FuncDep[curFuncDep];
    FieldList f2;
    while (f!=NULL)
    {
        f2=f;
        f=f->tail;
        //delete f2
        unsigned int hash = hash_pjw(f2->name) % HashTableSize;
        if(HashTable[hash]!=NULL &&strcmp(HashTable[hash]->name,f2->name)==0 &&HashTable[hash]->depth==curFuncDep){
            HashTable[hash]=HashTable[hash]->tail;
            //printf("delete %s\n",f2->name);
            //free(f2); //I donnot know why. for BUG B-1.
            
        }
    }
    

}
//var: name type 
//func:returnType paraNum, paraType
//
FieldList insertHashTable(FieldList f,int kind)
{
    //printf("insert %s\n",f->name);
    //printf("insert1\n");
    unsigned int hash = hash_pjw(f->name) % HashTableSize;
    //printf("%d\n",hash);
    //if(HashTable[hash]==NULL)
        //printf("insert1.5\n");
    //printf("%d",hash);
    FieldList hashf;
    //insert to head
    //printf("insert2\n");
    //printf("insert2.01\n");
    //hashf->tail = HashTable[hash];
    //if(HashTable[hash]==NULL){
    //    printf("WTF\n");
   // }
    //printf("insert2.12\n");
    if(kind==FUNCTION){
        curFuncDep=curFuncDep-1;
    }
    if(HashTable[hash]==NULL){
        
        //printf("!!!\n");
        HashTable[hash] = (FieldList)malloc(sizeof(struct FieldList_));
        //printf("%s\n",f->name);
        //strcpy()
        //char *id=malloc(40*sizeof(char));
        //strcpy(id,f->name);
        //HashTable[hash]->name=id;
        HashTable[hash]->name=f->name;
        //printf("???2\n");
        HashTable[hash]->type=f->type;
        //printf("???3\n");
        HashTable[hash]->tail=NULL;
        HashTable[hash]->depth=curFuncDep;
        
    }
    else 
    {
        //printf("???");
        hashf = (FieldList)malloc(sizeof(struct FieldList_));
        hashf->tail = HashTable[hash];

        HashTable[hash] = hashf;
        hashf->name = f->name;
        hashf->type=f->type;
        hashf->depth=curFuncDep;
       
    }
    FieldList depF=(FieldList)malloc(sizeof(struct FieldList_));
   
    //FuncDep[curFuncDep]=HashTable[hash];
    //not the same FieldList
    depF->tail=FuncDep[curFuncDep];
    depF->depth=curFuncDep;
    depF->name=HashTable[hash]->name;
    depF->type=HashTable[hash]->type;
    FuncDep[curFuncDep]=depF;

    if(kind==FUNCTION){
        curFuncDep=curFuncDep+1;
        HashTable[hash]->type->kind=FUNCTION;
    }
    
    //seems no use
    // no it is useful
    //temp useless becase we add it in another part vardec for para is function
    /*
    if(f->type->kind==FUNCTION){
        FieldList paraCheck=f->type->u.structure;
        
        while (paraCheck!=NULL)
        {
            FieldList fuck=searchHashTable(paraCheck->name,NULL,e_OptTag);
            if(fuck==NULL || fuck->depth<curFuncDep){
                //insert
                insertHashTable(paraCheck,BASIC);

                
            }
            else
            { //error type 3
                return fuck;
            }
            
        }
        
    }
    */
    
    //printf("insert2.5\n");
    return NULL;
}

FieldList searchHashTable(char *name,Type type,int SpecialCode)
{
    //printf("7770\n");
    //printf("%s\n",name);
    unsigned int hash = hash_pjw(name) % HashTableSize;
    //printf("7770\n");
    FieldList f = HashTable[hash];
    //if(HashTable[hash]==NULL)
    //    printf("%s why\n",name);
    //if(HashTable[hash]!=NULL)
    //    printf("%s wuwuwu %d\n",HashTable[hash]->name,HashTable[hash]->type->kind);
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
           if(strcmp(name, f->name) == 0 &&f->type->kind!=FUNCTION &&curFuncDep<=f->depth){
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
            //printf("%s %s %d\n",name,f->name,f->type->kind);
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
    else 
    {
        while (f != NULL)
        {
            if (strcmp(name, f->name) == 0)
            {
                
                return f;
            }
            f = f->tail;
        }
       
    }
    

    return NULL;
}


int typeCheck(Type t1, Type t2)
{   
    //printf("%d %d",t1->kind,t2->kind);
    //i donnot know which is true
    //if(t1==NULL &&t2==NULL){
    //if(t1==NULL || t2==NULL){
    if(t1==NULL &&t2!=NULL || t2==NULL &&t1!=NULL ||t1==NULL &&t2==NULL)
    {    
        //return 0;
        return -1;
    }
    if (t1->kind != t2->kind)
    {
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
        //printf("%s %s \n",t1->structureName,t2->structureName);
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
    curFuncDep=0;
    i = 0;
    for (; i < FuncDepSize; i++)
    {
        FuncDep[i] = NULL;
    }
    //curStructDep=0;
    i = 0;
    for (; i < FuncDepSize; i++)
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
    //printf("Program\n");
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
    //printf("ExtDef\n");
    ExtDef(extdeflist->child);
    //printf("ExtDefList\n");
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
    //printf("Specifier\n");
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
        curFuncDep=curFuncDep+1;
        //printf("MAIN\n");
        f = FunDec(extdef->child->brother, retT); //ret is the type of function,no relation of para.
        //TODO
        //check
        //printf("MAIN\n");
        if(f!=NULL){
            //printf("emmm\n");
            //only cmp to the func and name
            f2=searchHashTable(f->name,f->type,e_ExtDef);
            //printf("search\n");
            if(f2!=NULL){
                //printError(4,extdef->child->brother->child->lineno,extdef->child->brother->child->nchar_data);
            }
            else
            {
                
                //printf("f2==NULL\n");
                //means no same func name
                FieldList fuckk=insertHashTable(f,FUNCTION);//TODO

                //temp useless
                //if(fuckk!=NULL){
                //    printError(3,extdef->child->brother->lineno,fuckk->name);
                //}
                
                
            }
            //printf("CompSt\n");
            CompSt(extdef->child->brother->brother, retT);
            deleteCurFuncDep();
            curFuncDep=curFuncDep-1;
        }

        //printf("emmm\n");
        
        break;
    default:
        break;
    }
    //another half in structspecifier
    //if(retT->kind==STRUCTURE){
    //    curStructDep=curStructDep-1;
    //    free(curStructure[curStructDep]);
    //}
    
}

void ExtDecList(struct TreeNode* extdeclist,Type type){
    // ExtDecList(extdef->child->brother);
    if(extdeclist->child->brother==NULL){//vardec
        VarDec(extdeclist->child,type,BASIC);//same as struct
    }
    else{//vardec comma extdeclist
         VarDec(extdeclist->child,type,BASIC);
         ExtDecList(extdeclist->child->brother->brother,type);
    }
    
}

Type Specifier(struct TreeNode *specifier)
{
    if(specifier==NULL){
        return NULL;
    }

    Type t;
    //printf("MAIN %d\n",specifier->child->int_name);
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
         //printf("emmm%d %d \n",specifier->child->int_kind,t->u.basic);
       
        break;
    case e_StructSpecifier:
        t = StructSpecifier(specifier->child);
        //printf("~z2zz %d zzz~\n",t);
        //if(t==NULL)
        //    printf("null\n");
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
                //insertHashTable(structure,STRUCTURE);
                //printf("~%s %d zzz~\n",f->name,f->type->kind);
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
        //printf("???\n");
        //printf("%s \n",structspecifier->child->brother->child->nchar_data);
        f=searchHashTable(structspecifier->child->brother->child->nchar_data,NULL,e_Tag);
        //printf("MAIN\n");
        if(f==NULL){
            //printf("MAIN\n");
            printError(17,structspecifier->child->brother->child->lineno,structspecifier->child->brother->child->nchar_data);
            return NULL; //TODO
        }

        //t=f->type;
        //t->structureName=f->name;
        //t->u.structure=f->tail;
        //printf("%s %d mmm\n",f->name,f->type->kind);
        return f->type;
        break;

    default:
        //error
        break;
    }
    //printf("~z1zz %d zzz~\n",t->kind);
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
    //printf("Def\n");
    if (f == NULL)
    {
        f = DefList(deflist->child->brother,structure);
        //printf("Deflist1\n");
    }
    else
    {
        //printf("Deflist2\n");
        FieldList tmp = f;
        while (tmp->tail != NULL)
        {
            tmp = tmp->tail;
        }
        tmp->tail = DefList(deflist->child->brother,structure);//wtf, need to be tmp->tail
        //printf("Deflist3\n");
    }
    return f;
}
FieldList Def(struct TreeNode *def,int structure)
{
    if(def==NULL){
         return NULL;
    }
    //printf("HHH Def\n");  

    //: Specifier DecList SEMI
    FieldList f;
    Type type = Specifier(def->child);
    if(type==NULL){
        return NULL; //pack in 0415
    }
    //printf("in HHH Def %d",type->kind);
    //printf("HHH\n");
    //printf("%d\n",FUNCTION);
    f = DecList(def->child->brother, type,structure);
    //printf("here\n");
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
FieldList Dec(struct TreeNode *dec, Type type,int structure)
{
    if(dec==NULL){
        return NULL;
    }
    //printf("HHH\n");
    FieldList f = VarDec(dec->child, type,structure);
    //printf("%d",f->type->kind);
    //printf("HHH,%s s\n",dec->child->child->nchar_data);
    if (dec->child->brother != NULL) //VarDec ASSIGNOP Exp
    {
        //check
       
        //errorType15 becase now it is initilizing.
        if(structure==STRUCTURE){
            printError(152,dec->child->lineno,dec->child->nchar_data);
            //return NULL;
        }
        Type t = Exp(dec->child->brother->brother);
        //check;
        //printf("here? %d\n",t==NULL);
        //if(typeCheck(t,type)!=0){
        if(typeCheck(t,f->type)!=0){
            printError(5,dec->child->brother->brother->lineno,dec->child->brother->brother->nchar_data);
            //return NULL;
        }
    }
    
    return f;
}

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
    //switch (tnode->int_name)
    //printf("HHH\n");
    switch (vardec->child->int_name)
    {
    case e_ID: //ID
        f = (FieldList)malloc(sizeof(struct FieldList_));
        f->name = vardec->child->nchar_data;
        //printf("%d",type->kind);
        f->type = type;
        f->tail = NULL;
        //printf("~~~%d zzzxc\n",type);
        //printf("~~~%s %d zxc\n",f->name,f->type->kind);
        //check
        //according to suppose 7, there is no need to check this
        //if(structure!=STRUCTURE){
        //}
        //printf("curs %d %s\n",curStructDep-1,vardec->child->nchar_data);
        switch (structure)
        {
            case BASIC:
                f2=searchHashTable(f->name,f->type,e_VarDec);//means no func,type depends on f->type
                //printf("here\n"); //it is structure's ID.
                if(f2!=NULL){
                    //printf("%d %d %d\n",curFuncDep,vardec->child->lineno,curStructDep);
                    if(curStructure[curStructDep]==NULL){
                         //    printf("NULL\n");
                        printError(3,vardec->child->lineno,vardec->child->nchar_data);
                        break;
                    }
                   
                    //packing for suppose 7



                    FieldList packForSuppose7=curStructure[curStructDep]->tail;
                    //printf("%s %d z\n",curStructure[curStructDep]->tail->name,curStructDep);
                    int flag=0;
                    while (packForSuppose7!=NULL)
                    {
                        //printf("pack%s %d %s\n",f->name,vardec->child->lineno,packForSuppose7->name);
                        if(strcmp(f->name,packForSuppose7->name)==0){
                           flag=1; //make sure not same with it is own data
                        }
                        packForSuppose7=packForSuppose7->tail;
                    }
                    
                    if(flag==0){
                        printError(3,vardec->child->lineno,vardec->child->nchar_data);
                        //return NULL;
                    }
                    else
                    {
                        //struct  { int b=0;} b;
                        //b can be inserted
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
                //printf("%s %d %d\n",f->name,vardec->child->lineno,f2==NULL);    
                if(f2!=NULL){
                    if(f2->type->value==LEFT_VALUE&&f->type->value==LEFT_VALUE){
                        if(curFuncDep<=f->depth){
                            printError(3,vardec->child->lineno,vardec->child->nchar_data);
                        }
                    }
                    else{
                        printError(3,vardec->child->lineno,vardec->child->nchar_data);
                    }
                    
                    //return NULL;
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
                //printf("z%s\n",pack7f1->name);

                f2=searchHashTable(f->name,f->type,e_VarDec);
                //printf("%s %d %d\n",f->name,vardec->child->lineno,f2==NULL);
                if(f2!=NULL){
                    //include suppose 7 
                    FieldList packForSuppose7=curStructure[curStructDep-1]->tail;
                    //printf("pack%s %d %s\n",f->name,vardec->child->lineno,packForSuppose7->name);
                    while (packForSuppose7->tail!=NULL)
                    {
                        //printf("pack%s %d %s\n",f->name,vardec->child->lineno,packForSuppose7->name);
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
    case e_VarDec: //VarDec LB INT RB
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = ARRAY;
        t->u.array.elem = type;
        //t->u.array.size = vardec->child->brother->brother->int_data;
        t->u.array.size = atoi( vardec->child->brother->brother->nchar_data);
        //printf("%d\n",structure);
        return VarDec(vardec->child, t,structure);
        break;
    default:
        break;
    }
    return f; //useless
}

FieldList FunDec(struct TreeNode *fundec, Type type)
{ //ID LP VarList RP | ID LP RP
    if(fundec==NULL){
        return NULL;
    }

    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
    Type ttt=(Type)malloc(sizeof(struct Type_));
    f->name = fundec->child->nchar_data;

    //check name

    FieldList f2=searchHashTable(f->name,NULL,e_ExtDef);
           // printf("search\n");
    if(f2!=NULL){
        printError(4,fundec->child->lineno,fundec->child->nchar_data);
    }
    //f->type = type;
    //printf("789\n");
    f->type=ttt;
    f->type->kind=FUNCTION;
    //printf("456\n");
    f->type->u.function.retType=type;
    f->type->u.function.para=NULL;
    //printf("123\n");
    f->tail = NULL;
    if (fundec->child->brother->brother->int_name == e_VarList)
    {
        //f->tail = VarList(fundec->child->brother->brother);
        f->type->u.function.para=VarList(fundec->child->brother->brother);
        //debug
        //FieldList ff1=f->tail;
        //while (ff1!=NULL)
        //{
        //    printf("%s \n",ff1->name);
        //    ff1=ff1->tail;
        //}
        
    }
    return f;
}

FieldList VarList(struct TreeNode *varlist)//VarList-> ParamDec | ParamDec COMMA VarList
{
    if(varlist==NULL){
        return NULL;
    }

    FieldList f = ParamDec(varlist->child);
    //printf("MAIN\n");
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
    //printf("MAIN\n");
    return VarDec(paramdec->child->brother, t,FUNCTION);
}

void CompSt(struct TreeNode *compst, Type type)
{ //LC DefList StmtList RC
    if(compst==NULL){
        return;
    }
    
    DefList(compst->child->brother, BASIC);
    //printf("StmtList\n");
    StmtList(compst->child->brother->brother,type);
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
    //printf("Stmt3\n");
    //printf("233%d\n",stmt->child->int_name);
    switch (stmt->child->int_name)
    {
       
    case e_Exp:
        
        Exp(stmt->child);
        break;
    case e_CompSt:
        curFuncDep=curFuncDep+1;
        CompSt(stmt->child, type);
        deleteCurFuncDep();
        curFuncDep=curFuncDep-1;
        break;
    case e_RETURN:
        //printf("return\n");
        t = Exp(stmt->child->brother);
        
        //check
        if(typeCheck(t,type)!=0){
           
            //printf("%d,%d\n",t->u.basic,type->u.basic);
            printError(8, stmt->child->lineno, stmt->child->nchar_data);
        }
        //printf("return finished\n");
        break;
    case e_IF:
        Exp(stmt->child->brother->brother);
        Stmt(stmt->child->brother->brother->brother->brother,type);
        //printf("if?\n");
        if (stmt->child->brother->brother->brother->brother->brother != NULL)
        {
            //printf("here\n");
            Stmt(stmt->child->brother->brother->brother->brother->brother->brother,type);
        }
        //printf("here\n");
        break;
        //printf("hello\n");
    case e_WHILE:
        Exp(stmt->child->brother->brother);
        Stmt(stmt->child->brother->brother->brother->brother,type);
        //printf("while\n");
        break;
    default:
        break;
    }
}
Type Exp(struct TreeNode *exp)
{
    //printf("%d\n",curFuncDep);
    FieldList f;
    Type t;
    Type t1;
    Type t2;
    //printf("%d\n",exp->child->int_name);
    switch (exp->child->int_name)
    {
    case e_Exp:
    {
        //printf("666\n");
        switch (exp->child->brother->int_name)
        {
        case e_ASSIGNOP:
            //check
            //printf("777\n");
            t1 = Exp(exp->child);
            
            t2 = Exp(exp->child->brother->brother);
            //printf("emm %d %d\n",t2->kind,t2->u.basic);
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
                //printf("emm\n");
                printError(5, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            return t1;
            break;
        case e_AND:

        case e_OR:

        case e_RELOP:

        case e_PLUS:

        case e_MINUS:

        case e_STAR:

        case e_DIV:
            t1 = Exp(exp->child);
            t2 = Exp(exp->child->brother->brother);
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
                //printf("here\n");
                return NULL;
            }
            break;

        case e_LB:
            
            t = Exp(exp->child);
            if (t == NULL)
            {
                return NULL;
            }
            //printf("%d\n",t->kind);
            if (t->kind != ARRAY)
            {
                printError(10, exp->child->lineno, exp->child->child->nchar_data); //BUG?
                return NULL;
            }
            
            Type t2 = Exp(exp->child->brother->brother);
            if (t2 == NULL)
            {
                return NULL;
            }
            if (t2->kind == BASIC && t2->u.basic== UBASIC_INT) //include other types?
            {
                Type finDebug=(Type)malloc(sizeof(struct Type_));
                memcpy(finDebug,t->u.array.elem,sizeof(struct Type_));
                finDebug->value = BOTH_VALUE;
                return finDebug;
            }
            else{
                
                //float tmpf = exp->child->brother->brother->child->float_data;
                //char important[20];
                //sprintf(important, "%f", tmpf);
                //printError(12, exp->child->lineno, important);
                printError(12, exp->child->lineno, exp->child->brother->brother->child->nchar_data);
                return NULL;
            }
            
            break;
        case e_DOT:
            
            t = Exp(exp->child);
            if (t == NULL)
            {
                return NULL;
            }
            //printf("{{{ %s\n",t->kind);
            if (t->kind != STRUCTURE)
            {
                //printf("%d\n",t->kind);
                printError(13, exp->child->lineno, exp->child->nchar_data);
                return NULL;
            }
            //printf("%s\n",t->structureName);
            f = t->u.structure->tail;
            //printf("???\n");
            while (f != NULL)
            {
                //printf("%s %s\n", f->name,exp->child->brother->brother->nchar_data);
                if (strcmp(f->name, exp->child->brother->brother->nchar_data) == 0)
                {
                    Type t2=(Type)malloc(sizeof(struct Type_));
                    memcpy(t2,f->type,sizeof(struct Type_));
                    t2->value=BOTH_VALUE;
                    //t->value = BOTH_VALUE;
                    return t2;
                }
                f = f->tail;
            }
            printError(14, exp->child->lineno, exp->child->brother->brother->nchar_data);
            return NULL;
            break;
        default:
            break;
        }
    }
    break;
    case e_LP:
        return Exp(exp->child->brother);
        break;
    case e_MINUS://same as e_NOT

        //break;
    case e_NOT: //suppose ! can be used both integer and float , so it can be shared with MINUS.
        t = Exp(exp->child->brother);
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

    case e_ID:// ID | ID LP Args RP | ID LP RP
    {
        //printf("779\n");
        //printf("%d %s\n",curFuncDep,exp->child->nchar_data);
        if (exp->child->brother == NULL) //ID
        {
            //printf("770\n");
            //printf("%s\n",exp->child->nchar_data);
             f = searchHashTable(exp->child->nchar_data,NULL,e_Exp);
             //struct a{ int b;}c; a and c 's type 's kind both is STRUCTURE. so we need to distinguish them by value left or right.
             //printf("%s %d %d\n",f->name,f->type->kind,f->type->value);
             //printf("787\n");
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
                return t;
            }
        }
        else //ID LP Args RP   //function using
        {
             f = searchHashTable(exp->child->nchar_data,NULL,e_NOT);
            if (f == NULL)
            {
                //printf("???\n");
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

                //FieldList tmpAF = f->type->u.structure->tail; //not structure ,itis func!!!!
                FieldList tmpAF = f->type->u.function.para;
                FieldList tmpBF = Args(exp->child->brother->brother);
                //printf("%d %d\n",tmpAF->type->kind,tmpBF->type->kind);
                int same = 1;
                while (tmpAF != NULL && tmpBF != NULL)
                {
                    //if (tmpAF->type == tmpBF->type)
                    if(typeCheck(tmpAF->type,tmpBF->type)==0)
                    {
                        //printf("???\n");
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
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->u.basic=UBASIC_INT;
        t->value = RIGHT_VALUE;
        return t;
        break;
    case e_FLOAT:
        t = (Type)malloc(sizeof(struct Type_));
        t->kind = BASIC;
        t->u.basic=UBASIC_FLOAT;
        t->value = RIGHT_VALUE;
        return t;
        break;
    default:
        break;
    }
}
FieldList Args(struct TreeNode *args)
{ //Exp COMMA Args | Exp
    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));
    Type finDebug=Exp(args->child);
    f->type=finDebug;
    f->name=NULL;
    //f->name = args->child->nchar_data;
    //f->type = Exp(args->child); //if Exp error, it will return NULL
    f->tail = NULL;
    if (args->child->brother != NULL) //exp COMMA args
    {
        if (f == NULL) //it seems impossible
        {
            f = Args(args->child->brother->brother);
        }
        else
        {
            FieldList tmp = f;
            while (tmp->tail != NULL)
            {
                tmp = tmp->tail;
            }
            tmp->tail = Args(args->child->brother->brother);
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