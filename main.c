//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;



typedef struct{
     char name[MAX_CHARNAME];
     int pos;
     int credit;
     int energy;
     int flag_graduated;
     int lab_target;
     int flag_lab;
        
}smm_player_t;

smm_player_t *smm_players;


void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void generatePlayers(int n, int initEnergy);
void printPlayerStatus(void); //print all player status at the beginning of each turn
//function prototypes
#if 0

smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)

#endif

float calcAverageGrade(int player) //calculate average grade of the player
{
    int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
    if (size == 0) return 0.0f; // no grade-> zero out

    int i;
    float sum = 0.0f;

    for (i = 0; i < size; i++)
    {
        void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);

        int grade = smmObj_getObjectGrade(ptr);  // bring grade value
        sum += grade;
    }

    return sum / size;
}


void printGrades(int player)//print all the grade history of the player
{
    int i;
    int len;
    void *ptr;

    len = smmdb_len(LISTNO_OFFSET_GRADE + player);

    printf("\n[Grades] Player: %s\n", smm_players[player].name);

    if (len == 0)
    {
        printf("  No grades.\n");
        return;
    }

    for (i = 0; i < len; i++)
    {
        ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);

        if (ptr != NULL)
        {
            printf("  %d. %s : %d\n",
                   i + 1,
                   smmObj_getObjectName(ptr),
                   smmObj_getObjectGrade(ptr));
        }
    }
}




void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
      int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
      int i;
      for(i=0;i<size;i++)
      {
         void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
         if (strcmp(smmObj_getObjectName(ptr), lectureName) == 0)
         {
           return ptr;                                
         }      
                                    
      }
      return NULL;
}


int isGraduated(void) //someone is graduated or not
{
    int i;
    for (i=0;i<smm_player_nr;i++)
    {
        if (smm_players[i].flag_graduated == 1)
           return 1 ;
    } 
    
    return 0;  
}




void goForward(int player, int step)
{
     int i;
     void *ptr;
     
 
     //player_pos[player] = player_pos[player]+step;
     ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
     printf("start from %i(%s)(%i)\n", smm_players[player].pos, smmObj_getObjectName(ptr), step);    
     for (i=0;i<step;i++)
     {
         
         smm_players[player].pos = (smm_players[player].pos+1)%smm_board_nr;
         ptr = smmdb_getData(LISTNO_NODE ,smm_players[player].pos);
         printf("=> moved to %i(%s)\n",smm_players[player].pos, smmObj_getObjectName(ptr));
        
       }         
 }

void printPlayerStatus(void)//print all player status at the beginning of each turn
{
      int i;
      void *ptr;
      printf("\n\n===================PLAYER STATUS=====================\n"); 
     for (i=0;i<smm_player_nr;i++)
       {
                ptr = smmdb_getData(LISTNO_NODE ,smm_players[i].pos);
                                
               printf("%s - position:%i(%s), credit:%i, energy:%i\n\n", 
               smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), smm_players[i].credit, smm_players[i].energy );
               
       }       
               
}



void generatePlayers(int n, int initEnergy)
{
     int i;
     
     smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
     
     
     for (i=0;i<n;i++)
       {
               smm_players[i].pos = 0;
               smm_players[i].credit = 0;
               smm_players[i].energy = initEnergy;
               smm_players[i].flag_graduated = 0;
               smm_players[i].flag_lab = 0;
               smm_players[i].lab_target = 0;
               
               printf("Input %i-th player name: ",i);
               scanf("%s", &smm_players[i].name[0]);
               fflush(stdin);
               
                     
        }              
}


int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
    if (c == 'g')
    {
        printGrades(player);

        printf("\n Press any key to roll a die: ");
        getchar();
        fflush(stdin);
    }

    
    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
     void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
     
    int type = smmObj_getObjectType(ptr);
    int credit = smmObj_getObjectCredit(ptr);
    int energy = smmObj_getObjectEnergy(ptr);
    int grade;
    void *gradePtr;
    
    printf(" \n\n--> player: %s  pos :%i,   type : %s,   credit : %i,   energy : %i\n", 
              smm_players[player].name, smm_players[player].pos, smmObj_getTypeName(ptr), credit, energy );
    switch(type)
     {
         case SMMNODE_TYPE_LECTURE:
         if (findGrade(player, smmObj_getObjectName(ptr)) == NULL ) // do not take duplicate classes  
         {    char choice[10];
              printf(" \n->Lecture %s , credit (%i), energy (%i) starts! Are you going to join or drop? : ", smmObj_getObjectName(ptr), credit, energy);
              scanf("%9s", &choice);
              
              if (strcmp(choice, "join") == 0)  //choice - join or drop     
              {
                    if (smm_players[player].energy < energy)
                      {
                        printf("\n-> %s is too hungry to take the lecture %s.\n", smm_players[player].name, smmObj_getObjectName(ptr));//energy unavailable
                       }
                    else 
                      {              
                          smm_players[player].credit += credit;
                          smm_players[player].energy -= energy;
              
                          grade =  rand()%SMMNODE_MAX_GRADE;
                          gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE, type, credit, energy, grade);
                          smmdb_addTail(LISTNO_OFFSET_GRADE+player, gradePtr);
                       }
                 }
              else
              {
                  printf("\n-> you chose to drop the lecture %s.\n", smmObj_getObjectName(ptr));
              }      
                      
         }                                                        
              break;
              
              
         case SMMNODE_TYPE_RESTAURANT:
              
              smm_players[player].energy += energy;
              break;
              
         case SMMNODE_TYPE_LABORATORY:
            {  
               int dice;                       
               if (smm_players[player].flag_lab == 0) // target declare 
               {  
                  smm_players[player].lab_target = rand() % 6 + 1;
                  smm_players[player].flag_lab = 1;
                  printf("%s enters the laboratory.\n", smm_players[player].name);
                  printf(" Experiment target value: %d\n", smm_players[player].lab_target);
                }
               else // when player still in the laboratory - try again
               {
                  printf("%s is still in the laboratory.\n", smm_players[player].name );   
                  printf("Experiment target value: %d\n", smm_players[player].lab_target);
               }
               
               printf(" Press any key to roll a die for experiment: ");
               getchar();
               getchar();
               
               dice = rand() % 6 + 1;
               printf(" Rolled dice: %d\n", dice);

               if (dice == smm_players[player].lab_target) // when player got the same value  
                 {
                     printf(" Experiment succeeded! %s escapes the laboratory.\n",
                     smm_players[player].name);
  
                     smm_players[player].flag_lab = 0;
                     smm_players[player].lab_target = 0;

                      smm_players[player].credit += credit;
                      smm_players[player].energy -= energy;
                  }
              else
                  {
                     printf(" Experiment failed. %s remains in the laboratory.\n", smm_players[player].name);
                  }

              
                  break;
           }
                  
                    
         case SMMNODE_TYPE_HOME:
              
              smm_players[player].energy += energy;
              if (smm_players[player].credit >= GRADUATE_CREDIT) //the condition of graduation
                {
                        smm_players[player].flag_graduated = 1;
                        printf(" Total credit: %d\n", smm_players[player].credit);
                }                         
              break;
              
         case SMMNODE_TYPE_GOTOLAB:
               smm_players[player].flag_lab = 1;
               smm_players[player].lab_target = rand() % MAX_DIE + 1;

             printf("%s entered the lab.\n", smm_players[player].name);
             printf(" Experiment target value is set to %d\n", smm_players[player].lab_target);
   

              
              break;
              
         case SMMNODE_TYPE_FOODCHANGE:
              {int idx;
              void *foodPtr;
              
              printf("%s  gets a food chance! press any key to pick a food card: ",smm_players[player].name); //choosing food card
              getchar();
              
              idx= rand()%smm_food_nr;
              foodPtr = smmdb_getData(LISTNO_FOODCARD, idx);
              int foodEnergy = smmObj_getObjectEnergy(foodPtr);
              printf(" \n Food card: %s !! you got %i energy !!\n", smmObj_getObjectName(foodPtr), foodEnergy); //get food energy
              
              smm_players[player].energy += foodEnergy;
             


              
              break;}
              
         case SMMNODE_TYPE_FESTIVAL:
              { int idx;
              void* festPtr;

               printf("%s participates to Snow Festival! press any key to pick a festival card: ",smm_players[player].name);//choosing festival card
               getchar();   

               idx = rand() % smm_festival_nr;
               festPtr = smmdb_getData(LISTNO_FESTCARD, idx);

               if (festPtr != NULL)
                   {
                     printf("\n MISSION: %s\n", smmObj_getObjectName(festPtr)); // showing the mission
                   }

                break;
                }
              
         default:
            break;
    }
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        printf("%s %i %i %i\n", name, type, credit, energy);
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, ptr);
        smm_board_nr = smmdb_len(LISTNO_NODE);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
     
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp,"%s %i", name, &energy ) == 2) //read a food parameter set
    {
          void* ptr;
          printf("%s %i\n", name,energy);
          ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0,0, energy, 0);
          smmdb_addTail(LISTNO_FOODCARD, ptr);
          smm_food_nr = smmdb_len(LISTNO_FOODCARD);
          
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fgets(name, sizeof(name), fp) != NULL) //read a festival card string
    {
          void* ptr;
          printf("%s", name);
          ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0,0,0,0);
          smmdb_addTail(LISTNO_FESTCARD, ptr);
          smm_festival_nr = smmdb_len(LISTNO_FESTCARD); //length of festcard
          
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", smm_festival_nr);
     
    
    //2. Player configuration ---------------------------------------------------------------------------------

    do
    {
         printf("\n\nInput player number: ");
        //input player number to player_nr
        scanf("%i", &smm_player_nr);
        fflush(stdin);
        
        
        if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
            printf("Invalid player number!\n");
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);
    
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData(LISTNO_NODE,0)));
    
    

   
    turn = 0;
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
      
        //4-3. go forward
        goForward(turn, die_result);
        //pos = pos+2;
        //pos = (pos + rand()%6+1)%board_nr;
  
		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        // 4-5. next turn
       //confirm graduation conditon
         if (smm_players[turn].credit >= GRADUATE_CREDIT &&
                 smmObj_getObjectType(smmdb_getData(LISTNO_NODE, smm_players[turn].pos)) == SMMNODE_TYPE_HOME)
         {
              smm_players[turn].flag_graduated = 1;
              printf("\n=== %s has graduated! ===\n", smm_players[turn].name);
              printf("Total credit: %d\n", smm_players[turn].credit);

             //print name, credit, gradeavg,...of graduated player 
               int i;
               int size = smmdb_len(LISTNO_OFFSET_GRADE + turn);
               for (i = 0; i < size; i++)
                 {
                            void* gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + turn, i);
                            char* lectureName = smmObj_getObjectName(gradePtr);
                            int credit = smmObj_getObjectCredit(gradePtr);  // credit of every lectures
                            int grade = smmObj_getObjectGrade(gradePtr);    // grade of every lectures
                            printf("Lecture: %s, Credit: %d, Grade: %d\n", lectureName, credit, grade); //print each 
                    }

            //entire grade avg.
            float avgGrade = calcAverageGrade(turn);
            printf("Average Grade: %.2f\n", avgGrade); // print avg grade

            printf("=== Game Over ===\n");
             break; // 게임 종료
          }

        //4-5. next turn
        turn = (turn +1)%smm_player_nr;       
    }
    
    
    free(smm_players);
  
    system("PAUSE");
    return 0;
}
