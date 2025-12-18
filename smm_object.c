//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR    100
#define MAX_NODETYPE    7


#define SMMNODE_TYPE_LECTURE      0
#define SMMNODE_TYPE_RESTAURANT   1
#define SMMNODE_TYPE_LABORATORY   2
#define SMMNODE_TYPE_HOME         3
#define SMMNODE_TYPE_GOTOLAB      4
#define SMMNODE_TYPE_FOODCHANGE   5
#define SMMNODE_TYPE_FESTIVAL     6



#define GRADE_AP                  0
#define GRADE_AZ                  1
#define GRADE_AM                  2
#define GRADE_BP                  3
#define GRADE_BZ                  4
#define GRADE_BM                  5
#define GRADE_CP                  6
#define GRADE_CZ                  7
#define GRADE_CM                  8
#define GRADE_DP                  9
#define GRADE_DZ                 10
#define GRADE_DM                 11
#define GRADE_F                  12




static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A+",
       "A0",
       "A-",
       "B+",
       "B0",
       "B-",
       "C+",
       "C0",
       "C-",
       "D+",
       "D0",
       "D-",
       "F"
       
};
       



typedef struct {
    char name[MAX_CHARNAME];
    int objType;
    int type;
    int credit;
    int energy;
    int grade;
} smmObj_object_t;


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
    smmObj_object_t* ptr;
    ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
    strcpy(ptr->name,name); 
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->objType = objType;
    ptr->grade = grade;
    
    
    return ((void*)ptr);
    
}



//member retrieving
char* smmObj_getObjectName(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return(objPtr->name);
}

int smmObj_getObjectType(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      return(objPtr->type);
}
int smmObj_getObjectCredit(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      return(objPtr->credit);
}
int smmObj_getObjectEnergy(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      
      return(objPtr->energy);
}
char* smmObj_getTypeName(void *ptr)
{
      smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
      return(smmObj_nodeName[objPtr->type]);
}
int smmObj_getObjectGrade(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->grade);
}


#if 0
//element to string

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif
