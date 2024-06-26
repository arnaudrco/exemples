//ESP32 chess engine 1.0
//Sergey Urusov, ususovsv@gmail.com

const signed char fp=1; //pawn
const signed char fn=2; //knight
const signed char fb=3; //bishop
const signed char fr=4; //rook
const signed char fq=5; //queen
const signed char fk=6; //king
const int fig_weight[]={0,100,320,330,500,900,0};
const char fig_symb[]="  NBRQK";
const char fig_symb1[]=" pNBRQK";

int countin=0,countall=0;

const int MAXSTEPS=150;
const int MAXDEPTH=30;

typedef struct {
  signed char f1,f2; //    
  signed char c1,c2; //     
  signed char check; //  
  signed char type; //  1-  ,2- ,3- ,4-5-6-7-  ,,,
  short weight; // , 
} step_t;

int level;
int fdepth;

struct position_t {  
  byte w; //  1,  - 0
  byte wrk, wrq, brk, brq; // 
  byte pp; //   1..63      
  step_t steps[MAXSTEPS+1]; //     
  int n_steps; //   0..MAXSTEPS  
  int cur_step; //     0..MAXSTEPS  
  step_t best;  
  int check_on_table;    
  short weight_w; //  
  short weight_b; //  
  short weight_s; //  ,    +
};

step_t steps2[MAXSTEPS]; //    
int n_steps2; //     

struct packed_t {
  int q[8];
};
packed_t polep;

String fenstr;
const int MAXEPD=5;
int bestcount=0;
step_t bestmove[MAXEPD]; //   
boolean bestsolved=0;
boolean zero=0;

position_t pos[MAXDEPTH]; //   


int TRACE=0;
short pole[64];

unsigned long timelimith=1*60*1000; //    (1 ) 
unsigned long starttime;

int nullmove=1;
int multipov=0;
int futility=1;
int lazyeval=1;

int depth=0;
int nulldepth;
int lazy;
boolean endspiel=0;
boolean stats=1;
int lastbestdepth=0;
step_t lastbeststep;
boolean halt=0;

step_t bufsteps[MAXSTEPS+1]; //   

step_t game_steps[1000]; // 
position_t game_pos; // 
int game_ply; // 
boolean game_w; // 
short game_pole[64];

const short column[64]={
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8,
  1, 2, 3, 4, 5, 6, 7, 8  
};
const short row[64]={
  8, 8, 8, 8, 8, 8, 8, 8,
  7, 7, 7, 7, 7, 7, 7, 7,
  6, 6, 6, 6, 6, 6, 6, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  4, 4, 4, 4, 4, 4, 4, 4,
  3, 3, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1  
};
const short diag1[64]={
  1, 2, 3, 4, 5, 6, 7, 8,
  2, 3, 4, 5, 6, 7, 8, 9,
  3, 4, 5, 6, 7, 8, 9,10,
  4, 5, 6, 7, 8, 9,10,11,
  5, 6, 7, 8, 9,10,11,12,
  6, 7, 8, 9,10,11,12,13,
  7, 8, 9,10,11,12,13,14,
  8, 9,10,11,12,13,14,15  
};
const short diag2[64]={
  8, 7, 6, 5, 4, 3, 2, 1,
  9, 8, 7, 6, 5, 4, 3, 2,
 10, 9, 8, 7, 6, 5, 4, 3,
 11,10, 9, 8, 7, 6, 5, 4,
 12,11,10, 9, 8, 7, 6, 5,
 13,12,11,10, 9, 8, 7, 6,
 14,13,12,11,10, 9, 8, 7,
 15,14,13,12,11,10, 9, 8  
};


int poswk=0; //  
int posbk=0; //  

unsigned long count=0;

int task_start=0;
int task_check=0;
int task_l;
short task_s;
unsigned long task_time,task_execute;

const int stat_weightw[7][64]={
  { 0,  0,  0,  0,  0,  0,  0,  0, //pawn
  100,100,100,100,100,100,100,100,  // 50, 50, 50, 50, 50, 50, 50, 50,
   20, 30, 40, 50, 50, 40, 30, 20,  // 10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,  //  5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0},
  
 {-50,-40,-30,-30,-30,-30,-40,-50, //knife
  -40,-20,  0,  0,  0,  0,-20,-40,
  -30,  0, 10, 15, 15, 10,  0,-30,
  -30,  5, 15, 20, 20, 15,  5,-30,
  -30,  0, 15, 20, 20, 15,  0,-30,
  -30,  5, 10, 15, 15, 10,  5,-30,
  -40,-20,  0,  5,  5,  0,-20,-40,
  -50,-40,-30,-30,-30,-30,-40,-50}, 
  
 {-20,-10,-10,-10,-10,-10,-10,-20, //bishop
  -10,  0,  0,  0,  0,  0,  0,-10,
  -10,  0,  5, 10, 10,  5,  0,-10,
  -10,  5,  5, 10, 10,  5,  5,-10,
  -10,  0, 10, 10, 10, 10,  0,-10,
  -10, 10, 10, 10, 10, 10, 10,-10,
  -10,  5,  0,  0,  0,  0,  5,-10,
  -20,-10,-10,-10,-10,-10,-10,-20},  

 {  0,  0,  0,  0,  0,  0,  0,  0, //rook
    5, 10, 10, 10, 10, 10, 10,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0},
  
 {-20,-10,-10, -5, -5,-10,-10,-20, //queen
  -10,  0,  0,  0,  0,  0,  0,-10,
  -10,  0,  5,  5,  5,  5,  0,-10,
   -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
  -10,  5,  5,  5,  5,  5,  0,-10,
  -10,  0,  5,  0,  0,  0,  0,-10,
  -20,-10,-10, -5, -5,-10,-10,-20},  

 {-30,-40,-40,-50,-50,-40,-40,-30, //kingw
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -20,-30,-30,-40,-40,-30,-30,-20,
  -10,-20,-20,-20,-20,-20,-20,-10,
   10, 10,-10,-10,-10,-10, 10, 10,  
   10, 40, 30,  0,  0,  0, 50, 10}, 

 {-50,-40,-30,-20,-20,-30,-40,-50, //king endspiel
  -30,-20,-10,  0,  0,-10,-20,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-30,  0,  0,  0,  0,-30,-30,
  -50,-30,-30,-30,-30,-30,-30,-50}  

};

const int stat_weightb[7][64]={
  { 0,  0,  0,  0,  0,  0,  0,  0, //pawn
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,  
   20, 30, 40, 50, 50, 40, 30, 20,  
  100,100,100,100,100,100,100,100,  
    0,  0,  0,  0,  0,  0,  0,  0},
  
 {-50,-40,-30,-30,-30,-30,-40,-50, //knife
  -40,-20,  0,  0,  0,  0,-20,-40,
  -30,  5, 10, 15, 15, 10,  5,-30,
  -30,  0, 15, 20, 20, 15,  0,-30,
  -30,  5, 15, 20, 20, 15,  5,-30, 
  -30,  0, 10, 15, 15, 10,  0,-30,
  -40,-20,  0,  5,  5,  0,-20,-40,
  -50,-40,-30,-30,-30,-30,-40,-50}, 
  
 {-20,-10,-10,-10,-10,-10,-10,-20, //bishop
  -10,  5,  0,  0,  0,  0,  5,-10,
  -10, 10, 10, 10, 10, 10, 10,-10,
  -10,  0, 10, 10, 10, 10,  0,-10,
  -10,  5,  5, 10, 10,  5,  5,-10,
  -10,  0,  5, 10, 10,  5,  0,-10, 
  -10,  0,  0,  0,  0,  0,  0,-10,
  -20,-10,-10,-10,-10,-10,-10,-20},  

 {  0,  0,  0,  5,  5,  0,  0,  0, //rook     
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,   
    0,  0,  0,  0,  0,  0,  0,  0},
  
 {-20,-10,-10, -5, -5,-10,-10,-20, //queen 
  -10,  0,  5,  0,  0,  0,  0,-10,
  -10,  5,  5,  5,  5,  5,  0,-10,
    0,  0,  5,  5,  5,  5,  0, -5,
   -5,  0,  5,  5,  5,  5,  0, -5,
  -10,  0,  5,  5,  5,  5,  0,-10,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -20,-10,-10, -5, -5,-10,-10,-20},  
 
 { 10, 40, 30,  0,  0,  0, 50, 10, //kingb
  10, 10,-10,-10,-10,-10, 10, 10, 
  -10,-20,-20,-20,-20,-20,-20,-10,
  -20,-30,-30,-40,-40,-30,-30,-20,  
  -30,-40,-40,-50,-50,-40,-40,-30, 
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30,
  -30,-40,-40,-50,-50,-40,-40,-30},

 {-50,-30,-30,-30,-30,-30,-30,-50,  //kingb endspiel
  -30,-30,  0,  0,  0,  0,-30,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 30, 40, 40, 30,-10,-30,
  -30,-10, 20, 30, 30, 20,-10,-30,
  -30,-20,-10,  0,  0,-10,-20,-30,
  -50,-40,-30,-20,-20,-30,-40,-50}   
 
};


//88 - end of raw
//99 - end of all
const byte diag_step[64][17] {
 {9,18,27,36,45,54,63,99}, //0
 {10,19,28,37,46,55,88,8,99}, //1
 {11,20,29,38,47,88,9,16,99}, //2
 {12,21,30,39,88,10,17,24,99}, //3 
 {13,22,31,88,11,18,25,32,99}, //4 
 {14,23,88,12,19,26,33,40,99}, //5 
 {15,88,13,20,27,34,41,48,99}, //6 
 {14,21,28,35,42,49,56,99}, //7 
 {17,26,35,44,53,62,88,1,99}, //8 
 {18,27,36,45,54,63,88,16,88,0,88,2,99}, //9 
 {19,28,37,46,55,88,17,24,88,1,88,3,99}, //10 
 {20,29,38,47,88,18,25,32,88,2,88,4,99}, //11 
 {21,30,39,88,19,26,33,40,88,3,88,5,99}, //12 
 {22,31,88,20,27,34,41,48,88,4,88,6,99}, //13 
 {23,88,21,28,35,42,49,56,88,5,88,7,99}, //14 
 {22,29,36,43,50,57,88,6,99}, //15 
 {25,34,43,52,61,88,9,2,99}, //16 
 {26,35,44,53,62,88,24,88,8,88,10,3,99}, //17 
 {27,36,45,54,63,88,25,32,88,9,0,88,11,4,99}, //18 
 {28,37,46,55,88,26,33,40,88,10,1,88,12,5,99}, //19 
 {29,38,47,88,27,34,41,48,88,11,2,88,13,6,99}, //20 
 {30,39,88,28,35,42,49,56,88,12,3,88,14,7,99}, //21 
 {31,88,29,36,43,50,57,88,13,4,88,15,99}, //22 
 {30,37,44,51,58,88,14,5,99}, //23 
 {33,42,51,60,88,17,10,3,99}, //24 
 {34,43,52,61,88,32,88,16,88,18,11,4,99}, //25 
 {35,44,53,62,88,33,40,88,17,8,88,19,12,5,99}, //26 
 {36,45,54,63,88,34,41,48,88,18,9,0,88,20,13,6,99}, //27 
 {37,46,55,88,35,42,49,56,88,19,10,1,88,21,14,7,99}, //28 
 {38,47,88,36,43,50,57,88,20,11,2,88,22,15,99}, //29 
 {39,88,37,44,51,58,88,21,12,3,88,23,99}, //30 
 {38,45,52,59,88,22,13,4,99}, //31 
 {41,50,59,88,25,18,11,4,99}, //32 
 {42,51,60,88,40,88,24,88,26,19,12,5,99}, //33 
 {43,52,61,88,41,48,88,25,16,88,27,20,13,6,99}, //34 
 {44,53,62,88,42,49,56,88,26,17,8,88,28,21,14,7,99}, //35 
 {45,54,63,88,43,50,57,88,27,18,9,0,88,29,22,15,99}, //36 
 {46,55,88,44,51,58,88,28,19,10,1,88,30,23,99}, //37 
 {47,88,45,52,59,88,29,20,11,2,88,31,99}, //38 
 {46,53,60,88,30,21,12,3,99}, //39 
 {49,58,88,33,26,19,12,5,99}, //40
 {50,59,88,48,88,32,88,34,27,20,13,6,99}, //41 
 {51,60,88,49,56,88,33,24,88,35,28,21,14,7,99}, //42 
 {52,61,88,50,57,88,34,25,16,88,36,29,22,15,99}, //43 
 {53,62,88,51,58,88,35,26,17,8,88,37,30,23,99}, //44 
 {54,63,88,52,59,88,36,27,18,9,0,88,38,31,99}, //45 
 {55,88,53,60,88,37,28,19,10,1,88,39,99}, //46 
 {54,61,88,38,29,20,11,2,99}, //47 
 {57,88,41,34,27,20,13,6,99}, //48 
 {58,88,56,88,40,88,42,35,28,21,14,7,99}, //49 
 {59,88,57,88,41,32,88,43,36,29,22,15,99}, //50
 {60,88,58,88,42,33,24,88,44,37,30,23,99}, //51 
 {61,88,59,88,43,34,25,16,88,45,38,31,99}, //52 
 {62,88,60,88,44,35,26,17,8,88,46,39,99}, //53 
 {63,88,61,88,45,36,27,18,9,0,88,47,99}, //54 
 {62,88,46,37,28,19,10,1,99}, //55 
 {49,42,35,28,21,14,7,99}, //56 
 {48,88,50,43,36,29,22,15,99}, //57 
 {49,40,88,51,44,37,30,23,99}, //58 
 {50,41,32,88,52,45,38,31,99}, //59 
 {51,42,33,24,88,53,46,39,99}, //60   
 {52,43,34,25,16,88,54,47,99}, //61 
 {53,44,35,26,17,8,88,55,99}, //62 
 {54,45,36,27,18,9,0,99} //63  ++++++
};

const byte stra_step[64][18] {
 {1,2,3,4,5,6,7,88,8,16,24,32,40,48,56,99}, //0
 {2,3,4,5,6,7,88,9,17,25,33,41,49,57,88,0,99}, //1
 {3,4,5,6,7,88,10,18,26,34,42,50,58,88,1,0,99}, //2
 {4,5,6,7,88,11,19,27,35,43,51,59,88,2,1,0,99}, //3
 {5,6,7,88,12,20,28,36,44,52,60,88,3,2,1,0,99}, //4
 {6,7,88,13,21,29,37,45,53,61,88,4,3,2,1,0,99}, //5
 {7,88,14,22,30,38,46,54,62,88,5,4,3,2,1,0,99}, //6
 {15,23,31,39,47,55,63,88,6,5,4,3,2,1,0,99}, //7
 {9,10,11,12,13,14,15,88,16,24,32,40,48,56,88,0,99}, //8
 {10,11,12,13,14,15,88,17,25,33,41,49,57,88,8,88,1,99}, //9
 {11,12,13,14,15,88,18,26,34,42,50,58,88,9,8,88,2,99}, //10
 {12,13,14,15,88,19,27,35,43,51,59,88,10,9,8,88,3,99}, //11
 {13,14,15,88,20,28,36,44,52,60,88,11,10,9,8,88,4,99}, //12
 {14,15,88,21,29,37,45,53,61,88,12,11,10,9,8,88,5,99}, //13
 {15,88,22,30,38,46,54,62,88,13,12,11,10,9,8,88,6,99}, //14
 {23,31,39,47,55,63,88,14,13,12,11,10,9,8,88,7,99}, //15
 {17,18,19,20,21,22,23,88,24,32,40,48,56,88,8,0,99}, //16
 {18,19,20,21,22,23,88,25,33,41,49,57,88,16,88,9,1,99}, //17
 {19,20,21,22,23,88,26,34,42,50,58,88,17,16,88,10,2,99}, //18
 {20,21,22,23,88,27,35,43,51,59,88,18,17,16,88,11,3,99}, //19
 {21,22,23,88,28,36,44,52,60,88,19,18,17,16,88,12,4,99}, //20
 {22,23,88,29,37,45,53,61,88,20,19,18,17,16,88,13,5,99}, //21
 {23,88,30,38,46,54,62,88,21,20,19,18,17,16,88,14,6,99}, //22
 {31,39,47,55,63,88,22,21,20,19,18,17,16,88,15,7,99}, //23
 {25,26,27,28,29,30,31,88,32,40,48,56,88,16,8,0,99}, //24
 {26,27,28,29,30,31,88,33,41,49,57,88,24,88,17,9,1,99}, //25
 {27,28,29,30,31,88,34,42,50,58,88,25,24,88,18,10,2,99}, //26
 {28,29,30,31,88,35,43,51,59,88,26,25,24,88,19,11,3,99}, //27
 {29,30,31,88,36,44,52,60,88,27,26,25,24,88,20,12,4,99}, //28
 {30,31,88,37,45,53,61,88,28,27,26,25,24,88,21,13,5,99}, //29
 {31,88,38,46,54,62,88,29,28,27,26,25,24,88,22,14,6,99}, //30
 {39,47,55,63,88,30,29,28,27,26,25,24,88,23,15,7,99}, //31
 {33,34,35,36,37,38,39,88,40,48,56,88,24,16,8,0,99}, //32
 {34,35,36,37,38,39,88,41,49,57,88,32,88,25,17,9,1,99}, //33
 {35,36,37,38,39,88,42,50,58,88,33,32,88,26,18,10,2,99}, //34
 {36,37,38,39,88,43,51,59,88,34,33,32,88,27,19,11,3,99}, //35
 {37,38,39,88,44,52,60,88,35,34,33,32,88,28,20,12,4,99}, //36
 {38,39,88,45,53,61,88,36,35,34,33,32,88,29,21,13,5,99}, //37
 {39,88,46,54,62,88,37,36,35,34,33,32,88,30,22,14,6,99}, //38
 {47,55,63,88,38,37,36,35,34,33,32,88,31,23,15,7,99}, //39
 {41,42,43,44,45,46,47,88,48,56,88,32,24,16,8,0,99}, //40
 {42,43,44,45,46,47,88,49,57,88,40,88,33,25,17,9,1,99}, //41
 {43,44,45,46,47,88,50,58,88,41,40,88,34,26,18,10,2,99}, //42
 {44,45,46,47,88,51,59,88,42,41,40,88,35,27,19,11,3,99}, //43
 {45,46,47,88,52,60,88,43,42,41,40,88,36,28,20,12,4,99}, //44
 {46,47,88,53,61,88,44,43,42,41,40,88,37,29,21,13,5,99}, //45
 {47,88,54,62,88,45,44,43,42,41,40,88,38,30,22,14,6,99}, //46
 {55,63,88,46,45,44,43,42,41,40,88,39,31,23,15,7,99}, //47
 {49,50,51,52,53,54,55,88,56,88,40,32,24,16,8,0,99}, //48
 {50,51,52,53,54,55,88,57,88,48,88,41,33,25,17,9,1,99}, //49
 {51,52,53,54,55,88,58,88,49,48,88,42,34,26,18,10,2,99}, //50
 {52,53,54,55,88,59,88,50,49,48,88,43,35,27,19,11,3,99}, //51
 {53,54,55,88,60,88,51,50,49,48,88,44,36,28,20,12,4,99}, //52
 {54,55,88,61,88,52,51,50,49,48,88,45,37,29,21,13,5,99}, //53
 {55,88,62,88,53,52,51,50,49,48,88,46,38,30,22,14,6,99}, //54
 {63,88,54,53,52,51,50,49,48,88,47,39,31,23,15,7,99}, //55
 {57,58,59,60,61,62,63,88,48,40,32,24,16,8,0,99}, //56
 {58,59,60,61,62,63,88,56,88,49,41,33,25,17,9,1,99}, //57
 {59,60,61,62,63,88,57,56,88,50,42,34,26,18,10,2,99}, //58
 {60,61,62,63,88,58,57,56,88,51,43,35,27,19,11,3,99}, //59
 {61,62,63,88,59,58,57,56,88,52,44,36,28,20,12,4,99}, //60
 {62,63,88,60,59,58,57,56,88,53,45,37,29,21,13,5,99}, //61
 {63,88,61,60,59,58,57,56,88,54,46,38,30,22,14,6,99}, //62
 {62,61,60,59,58,57,56,88,55,47,39,31,23,15,7,99} //63
};

const byte knight_step[64][9] {
 {10,17,99}, //0
 {11,18,16,99}, //1
 {12,19,17,8,99}, //2
 {13,20,18,9,99}, //3
 {14,21,19,10,99}, //4
 {15,22,20,11,99}, //5
 {23,21,12,99}, //6
 {22,13,99}, //7
 {2,18,25,99}, //8
 {3,19,26,24,99}, //9
 {4,20,27,25,16,0,99}, //10
 {5,21,28,26,17,1,99}, //11
 {6,22,29,27,18,2,99}, //12
 {7,23,30,28,19,3,99}, //13
 {31,29,20,4,99}, //14
 {30,21,5,99}, //15
 {1,10,26,33,99}, //16
 {2,11,27,34,32,0,99}, //17
 {3,12,28,35,33,24,8,1,99}, //18
 {4,13,29,36,34,25,9,2,99}, //19
 {5,14,30,37,35,26,10,3,99}, //20
 {6,15,31,38,36,27,11,4,99}, //21
 {7,39,37,28,12,5,99}, //22
 {38,29,13,6,99}, //23
 {9,18,34,41,99}, //24
 {10,19,35,42,40,8,99}, //25
 {11,20,36,43,41,32,16,9,99}, //26
 {12,21,37,44,42,33,17,10,99}, //27
 {13,22,38,45,43,34,18,11,99}, //28
 {14,23,39,46,44,35,19,12,99}, //29
 {15,47,45,36,20,13,99}, //30
 {46,37,21,14,99}, //31
 {17,26,42,49,99}, //32
 {18,27,43,50,48,16,99}, //33
 {19,28,44,51,49,40,24,17,99}, //34
 {20,29,45,52,50,41,25,18,99}, //35
 {21,30,46,53,51,42,26,19,99}, //36
 {22,31,47,54,52,43,27,20,99}, //37
 {23,55,53,44,28,21,99}, //38
 {54,45,29,22,99}, //39
 {25,34,50,57,99}, //40
 {26,35,51,58,56,24,99}, //41
 {27,36,52,59,57,48,32,25,99}, //42
 {28,37,53,60,58,49,33,26,99}, //43
 {29,38,54,61,59,50,34,27,99}, //44
 {30,39,55,62,60,51,35,28,99}, //45
 {31,63,61,52,36,29,99}, //46
 {62,53,37,30,99}, //47
 {33,42,58,99}, //48
 {34,43,59,32,99}, //49
 {35,44,60,56,40,33,99}, //50
 {36,45,61,57,41,34,99}, //51
 {37,46,62,58,42,35,99}, //52
 {38,47,63,59,43,36,99}, //53
 {39,60,44,37,99}, //54
 {61,45,38,99}, //55
 {41,50,99}, //56
 {40,42,51,99}, //57
 {43,52,48,41,99}, //58
 {44,53,49,42,99}, //59
 {45,54,50,43,99}, //60
 {46,55,51,44,99}, //61
 {47,52,45,99}, //62
 {53,46,99} //63  +++++
};

const byte king_step[64][9] {
 {1,9,8,99}, //0
 {2,10,9,8,0,99}, //1
 {3,11,10,9,1,99}, //2
 {4,12,11,10,2,99}, //3
 {5,13,12,11,3,99}, //4
 {6,14,13,12,4,99}, //5
 {7,15,14,13,5,99}, //6
 {15,14,6,99}, //7
 {1,9,17,16,0,99}, //8
 {2,10,18,17,16,8,0,1,99}, //9
 {3,11,19,18,17,9,1,2,99}, //10
 {4,12,20,19,18,10,2,3,99}, //11
 {5,13,21,20,19,11,3,4,99}, //12
 {6,14,22,21,20,12,4,5,99}, //13
 {7,15,23,22,21,13,5,6,99}, //14
 {23,22,14,6,7,99}, //15
 {9,17,25,24,8,99}, //16
 {10,18,26,25,24,16,8,9,99}, //17
 {11,19,27,26,25,17,9,10,99}, //18
 {12,20,28,27,26,18,10,11,99}, //19
 {13,21,29,28,27,19,11,12,99}, //20
 {14,22,30,29,28,20,12,13,99}, //21
 {15,23,31,30,29,21,13,14,99}, //22
 {31,30,22,14,15,99}, //23
 {17,25,33,32,16,99 }, //24
 {18,26,34,33,32,24,16,17,99}, //25
 {19,27,35,34,33,25,17,18,99}, //26
 {20,28,36,35,34,26,18,19,99}, //27
 {21,29,37,36,35,27,19,20,99}, //28
 {22,30,38,37,36,28,20,21,99}, //29
 {23,31,39,38,37,29,21,22,99}, //30
 {39,38,30,22,23,99}, //31
 {25,33,41,40,24,99}, //32
 {26,34,42,41,40,32,24,25,99}, //33
 {27,35,43,42,41,33,25,26,99}, //34
 {28,36,44,43,42,34,26,27,99}, //35
 {29,37,45,44,43,35,27,28,99}, //36
 {30,38,46,45,44,36,28,29,99}, //37
 {31,39,47,46,45,37,29,30,99}, //38
 {47,46,38,30,31,99}, //39
 {33,41,49,48,32,99}, //40
 {34,42,50,49,48,40,32,33,99}, //41
 {35,43,51,50,49,41,33,34,99}, //42
 {36,44,52,51,50,42,34,35,99}, //43
 {37,45,53,52,51,43,35,36,99}, //44
 {38,46,54,53,52,44,36,37,99}, //45
 {39,47,55,54,53,45,37,38,99}, //46
 {55,54,46,38,39,99}, //47
 {41,49,57,56,40,99}, //48
 {42,50,58,57,56,48,40,41,99}, //49
 {43,51,59,58,57,49,41,42,99}, //50
 {44,52,60,59,58,50,42,43,99}, //51
 {45,53,61,60,59,51,43,44,99}, //52
 {46,54,62,61,60,52,44,45,99}, //53
 {47,55,63,62,61,53,45,46,99}, //54
 {63,62,54,46,47,99}, //55
 {49,57,48,99}, //56
 {50,58,56,48,49,99}, //57
 {51,59,57,49,50,99}, //58
 {52,60,58,50,51,99}, //59
 {53,61,59,51,52,99}, //60
 {54,62,60,52,53,99}, //61
 {55,63,61,53,54,99}, //62
 {62,54,55,99} //63  +++++
};

          
//****************************
void setup() {   
  Serial.begin(115200);  
  while (!Serial) ;
  Serial.println(F("Start"));         
  xTaskCreate(       taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */      

//  hash =(hash_t *) malloc(MAXHASH*sizeof(hash_t));
//  Serial.print("Heap after malloc: ");
//Serial.println(ESP.getFreeHeap());                 
                    
}
//****************************
String load_usb() {
 String ss=""; 
 int count=0;
 char s='x'; 
  Serial.println("Wait for FEN or command (WAC,WAC n,STOP,TIME):");
  while (ss=="") ss=Serial.readString();
  ss.trim();
  return(ss);

  
  while (s!=';') {
    s=Serial.read();       
    if (s=='/') count++;
    if (s!=255) ss=ss+s;          
    delay(20);    
    if (count>=7&&Serial.available()==0) break;
  }  
  return ss;  
}
//****************************
String fenout(int l) {
 String s="";
 for (int r=0;r<8;r++) {   
  if (r>0) s=s+"/";
  int empty=0;
  for (int c=0;c<8;c++) {    
    int f=pole[c+r*8];    
    if (f==0) empty++; 
    if (f!=0||c==7)
     if (empty>0) { s=s+String(empty); empty=0;  }    
    switch (f) {
    case fp: s=s+"P"; break;
    case -fp: s=s+"p"; break;
    case fn: s=s+"N"; break;
    case -fn: s=s+"n"; break;
    case fb: s=s+"B"; break;
    case -fb: s=s+"b"; break;
    case fr: s=s+"R"; break;
    case -fr: s=s+"r"; break;
    case fq: s=s+"Q"; break;
    case -fq: s=s+"q"; break;
    case fk: s=s+"K"; break;
    case -fk: s=s+"k"; break;    
    }
  }    
 }
 if (pos[l].w==1) s=s+" w "; else s=s+" b ";
 if (pos[l].wrk+pos[l].wrq+pos[l].brk+pos[l].brq==0) s=s+"-";
  else {
   if (pos[l].wrk) s=s+"K"; 
   if (pos[l].wrq) s=s+"Q"; 
   if (pos[l].brk) s=s+"k"; 
   if (pos[l].brq) s=s+"q";        
 }
 if (pos[l].pp!=0) s=s+" "+str_pole(pos[l].pp); else s=s+" -";   
 return s;  
}
//****************************
boolean fen(String ss) {
 char s='x',i=0,j=0;  boolean load=false;   
  for (int i=0;i<64;i++) pole[i]=0;                               
  pos[0].w=1; 
  pos[0].wrk=0; pos[0].wrq=0;  pos[0].brk=0; pos[0].brq=0;
  pos[0].pp=0; 
  pos[0].cur_step=0; pos[0].n_steps=0;
  int spaces=0;      
  for (int c=0; c<ss.length(); c++) {
    s=ss[c];       
    if (i>7) { i=0; j++; }        
    if (spaces==3&&int(s)>='a'&&int(s)<='h') {      
      c++;
      char s1=ss[c];
      if (int(s1)>='1'&&int(s1)<='8') {
        pos[0].pp=8*(7-(int(s1)-int('1')))+int(s)-int('a');        
      }
    } else {
    int l=j*8+i;  
    switch (s) {      
    case '/': i=0; break;  
    case 'p': pole[l]=-fp; i++; break;
    case 'P': pole[l]=fp; i++; break;
    case 'n': pole[l]=-fn; i++; break;
    case 'N': pole[l]=fn; i++; break;
    case 'b': if (spaces==0) { pole[l]=-fb; i++; } else
              if (spaces==1) { pos[0].w=0; load=true; }              
              break;    
    case 'B': pole[l]=fb; i++; break;
    case 'r': pole[l]=-fr; i++; break;
    case 'R': pole[l]=fr; i++; break;
    case 'q': if (spaces==0) { pole[l]=-fq; i++; } else pos[0].brq=1; 
              break;
    case 'Q': if (spaces==0) { pole[l]=fq;  i++; } else pos[0].wrq=1;
              break;
    case 'k': if (spaces==0) { pole[l]=-fk;  i++; } else pos[0].brk=1; 
              break;
    case 'K': if (spaces==0) { pole[l]=fk;  i++; } else pos[0].wrk=1;
              break;
    case '1': i++; break;
    case '2': i+=2; break;
    case '3': i+=3; break;
    case '4': i+=4; break;
    case '5': i+=5; break;
    case '6': i+=6; break;
    case '7': i+=7; break;
    case '8': i=0; j++; break;    
    case ' ': spaces++; break;    
    case 'w': if (spaces==1) { pos[0].w=1; load=true; } break;        
    }       
    }    
    if (spaces==4) break;    
  }  
  if (!load) Serial.println(F("Error")); else fenstr=ss;      
  return load;  
}
//****************************
void getbm(int n,String ep) {  
  ep.trim(); 
  if (ep=="0-0") {
    for (int i=0;i<pos[0].n_steps;i++) 
     if (pos[0].steps[i].type==2)   { // 
        bestmove[n]=pos[0].steps[i]; 
        return;        
     }
  } else if (ep=="0-0-0") {
    for (int i=0;i<pos[0].n_steps;i++) 
     if (pos[0].steps[i].type==3)   { // 
        bestmove[n]=pos[0].steps[i]; 
        return;        
     }
  } else if (ep.indexOf("=")>-1) { // 
    char fi=ep.indexOf(ep.length()-1);  
    int type=7;
    if (fi=='N') type =4; else if (fi=='B') type=5; else if (fi=='R') type=6; 
    for (int i=0;i<pos[0].n_steps;i++) 
     if (pos[0].steps[i].type==type)   { //
        bestmove[n]=pos[0].steps[i]; 
        return;        
     }
  } else {  
   int posp1=ep.indexOf("+"); 
   int posp2=ep.indexOf("#"); 
   if (posp1>=0||posp2>=0) ep=ep.substring(0,ep.length()-1);  
   char co=ep.charAt(ep.length()-2);
   char ro=ep.charAt(ep.length()-1);  
   int c2=8*(7-(int(ro)-int('1')))+int(co)-int('a');  
   char fi=ep.charAt(0);       
  // int found=0;      
   for (int i=0;i<pos[0].n_steps;i++) {    
     if (pos[0].steps[i].c2==c2) {                
       if (fig_symb1[abs(pos[0].steps[i].f1)]==fi) {                
        if (ep.length()==3||ep.length()==4&&ep.charAt(1)=='x'){                  
         bestmove[n]=pos[0].steps[i];         
         return;  
        } else if (int(ep.charAt(1))-int('a')==column[pos[0].steps[i].c1]-1) {           
         bestmove[n]=pos[0].steps[i];                  
         return;            
        }
       } else if (ep.length()==2&&abs(pos[0].steps[i].f1)==fp) {        
        bestmove[n]=pos[0].steps[i];         
        return;  
       }        
       if (str_step(pos[0].steps[i])==ep) bestmove[n]=pos[0].steps[i]; 
        else {
          String st=str_step(pos[0].steps[i]);
          st=st.substring(0,1)+st.substring(2,st.length());         
          if (pos[0].steps[i].f2!=0&&ep.charAt(1)=='x'&&st==ep) 
           bestmove[n]=pos[0].steps[i];
        }
     }    
   }
  }
}  
//****************************
void epd() {
 for (int i=0;i<MAXEPD;i++) {
   bestmove[i].c1=-1; 
   bestmove[i].c2=-1; 
   bestmove[i].type=-1; 
 }
 String ep;
 int posbm=fenstr.indexOf("bm"); 
 if (posbm>-1) {
  int posp=fenstr.indexOf(";",posbm+2); 
  if (posp==-1) posp=fenstr.length();
  String ep=fenstr.substring(posbm+3,posp);  
  kingpositions();
  generate_steps(0);
  //show_steps(0);
  
  posp=ep.indexOf(" "); 
  if (posp>-1) {
    getbm(0,ep.substring(0,posp));
    ep=ep.substring(posp+1);    
    posp=ep.indexOf(" "); 
    if (posp>-1) {
     getbm(1,ep.substring(0,posp));
     ep=ep.substring(posp+1);          
     posp=ep.indexOf(" "); 
     if (posp>-1) {
      getbm(2,ep.substring(0,posp));            
      ep=ep.substring(posp+1);          
      posp=ep.indexOf(" "); 
      if (posp>-1) {
       getbm(3,ep.substring(0,posp));            
       ep=ep.substring(posp+1);          
       posp=ep.indexOf(" "); 
       if (posp>-1) {
        getbm(4,ep.substring(0,posp));                    
       } else getbm(4,ep);
      } else getbm(3,ep);
     } else getbm(2,ep);
    } else getbm(1,ep);
  } else getbm(0,ep);
  if (posp==-1) posp=ep.length();
  String bm=ep.substring(0,posp);
 } 
 bestcount=0;
 for (int i=0;i<MAXEPD;i++) if (bestmove[i].c1!=-1) bestcount++;
 //for (int i=0;i<MAXEPD;i++) if (bestmove[i].c1!=-1) Serial.print(str_step(bestmove[i])+" ");
 //Serial.println();
} 
//****************************
void show_position() {    
  for (int i=0;i<8;i++) {
     Serial.println("-----------------------------------------");       
      Serial.print("|");       
      for (int j=0;j<8;j++)    {        
        signed char f=pole[i*8+j];
        if (f>=0) { Serial.print("  "); Serial.print(fig_symb1[f]); Serial.print(" |"); }                     
         else { Serial.print(" -"); Serial.print(fig_symb1[-f]); Serial.print(" |"); }                     
      }   
      Serial.print("  ");              
      Serial.println(8-i);              
  }    
  Serial.println("-----------------------------------------");       
  Serial.println("   a    b    c    d    e    f    g    h");       
  if (pos[0].w==0) Serial.print("Black move"); else Serial.print("White move");     
  //Serial.print("  rok="+String(pos[0].wrk)+" "+String(pos[0].wrq)+" "+String(pos[0].brk)+" "+String(pos[0].brq));     
  //if (pos[0].pp!=0) Serial.print("  Prohod:"+String(pos[0].pp));             
  Serial.println();
}
//****************************
String str_pole(int i) {
  return String(char('a'+i%8)+String(8-i/8));  
}
//********************************** 
String str_steps(step_t st) { //  
  String s="";
   if (st.f1==0) return s;
   if (st.type==2) s="0-0";
    else if (st.type==3) s="0-0-0";
   else  {    
    if (abs(st.f1)>1) s=s+fig_symb[abs(st.f1)];   
    if (abs(st.f1)==2||abs(st.f1)==4) {
     s=s+str_pole(st.c1);      
     if (st.f2==0) s=s+"-";    
    }
    if (st.f2!=0) {
      if (abs(st.f1)==1) s=String(char('a'+st.c1%8));
      s=s+"x";    
      if (abs(st.f2)>1) s=s+fig_symb[abs(st.f2)];
    }    
    s=s+str_pole(st.c2);     
   } 
   if (st.type>3) s=s+"="+fig_symb[st.type-2];
   if (st.check==1) s=s+"+"; else 
    if (st.check==2) s=s+"#";
   return s;
}
//********************************** 
String str_step(step_t st) {
  String s="";
   if (st.f1==0) return s;
   if (st.type==2) s="0-0";
    else if (st.type==3) s="0-0-0";
   else  {    
    if (abs(st.f1)>1) s=s+fig_symb[abs(st.f1)];     
    s=s+str_pole(st.c1);      
    if (st.f2==0) s=s+"-";      
    if (st.f2!=0) s=s+"x";    
    s=s+str_pole(st.c2);     
   } 
   if (st.type>3) s=s+"="+fig_symb[st.type-2];
   if (st.check==1) s=s+"+"; else 
   if (st.check==2) s=s+"#";
   return s;
}
//****************************
void show_steps(int l) {
 Serial.println("Steps="+String(pos[l].n_steps));
 for (int i=0;i<pos[l].n_steps;i++) { 
  Serial.print(str_step(pos[l].steps[i])); Serial.print(" "); Serial.print(pos[l].steps[i].weight); Serial.print("    ");
  if ((i+1)%3==0) Serial.println(); 
  }
} 
//****************************  
void movepos(int l, step_t &s) {     
  pos[l+1].wrk=pos[l].wrk; pos[l+1].wrq=pos[l].wrq; 
  pos[l+1].brk=pos[l].brk; pos[l+1].brq=pos[l].brq;
  pos[l+1].pp=0;    
  if (pos[l].w) { // 
   if (pos[l].wrk||pos[l].wrq) {
    if (s.c1==60) {
      pos[l+1].wrk=0; pos[l+1].wrq=0;
    } else if (s.c1==63) pos[l+1].wrk=0;
    else if (s.c1==56) pos[l+1].wrq=0;
   }
   if (s.type==0&&s.f1==fp&&s.c2==s.c1-16)     
    if (column[s.c2]>1&&pole[s.c2-1]==-fp||column[s.c2]<8&&pole[s.c2+1]==-fp) pos[l+1].pp=s.c1-8;      
   pos[l+1].weight_w=pos[l].weight_w; pos[l+1].weight_b=pos[l].weight_b;
   if (s.f2!=0) pos[l+1].weight_b-=fig_weight[-s.f2];
   if (s.type>3) pos[l+1].weight_w+=fig_weight[s.type-2]-100;       
   if (stats) {
     if (s.f1==fk&&endspiel)  
      pos[l+1].weight_s=pos[l].weight_s+stat_weightw[6][s.c2]-stat_weightw[6][s.c1];
     else  
      pos[l+1].weight_s=pos[l].weight_s+stat_weightw[s.f1-1][s.c2]-stat_weightw[s.f1-1][s.c1];
     if (s.f2!=0) pos[l+1].weight_s+=stat_weightb[-s.f2-1][s.c2];
   }
  } else { // 
   if (pos[l].brk||pos[l].brq) {
    if (s.c1==4) {
      pos[l+1].brk=0; pos[l+1].brq=0;
    } else if (s.c1==7) pos[l+1].brk=0;
    else if (s.c1==0) pos[l+1].brq=0;
   }   
   if (s.type==0&&s.f1==-fp&&s.c2==s.c1+16) 
    if (column[s.c2]>1&&pole[s.c2-1]==fp||column[s.c2]<8&&pole[s.c2+1]==fp) pos[l+1].pp=s.c1+8;    
   pos[l+1].weight_w=pos[l].weight_w; pos[l+1].weight_b=pos[l].weight_b;
   if (s.f2!=0) pos[l+1].weight_w-=fig_weight[s.f2];
   if (s.type>3) pos[l+1].weight_b+=fig_weight[s.type-2]-100;         
   if (stats) {
     if (s.f1==-fk&&endspiel)  
      pos[l+1].weight_s=pos[l].weight_s-stat_weightb[6][s.c2]+stat_weightb[6][s.c1];
     else
      pos[l+1].weight_s=pos[l].weight_s-stat_weightb[-s.f1-1][s.c2]+stat_weightb[-s.f1-1][s.c1];
     if (s.f2!=0) pos[l+1].weight_s-=stat_weightw[s.f2-1][s.c2];
   }
  }  
  count++; //                     
}
//****************************  
void checks(int l, step_t &s) {
  if (pole[poswk]!=fk) {      
    Serial.println("wight king lost!");
    for (int i=0;i<64;i++) if (pole[i]==fk) {  poswk=i; break; }  
    if (pole[poswk]!=fk)  Serial.println("!!w!!"+String(l)+" "+str_step(pos[l-1].steps[pos[l-1].cur_step]));
  } 
  if (pole[posbk]!=-fk) {      
    Serial.println("black king lost!");
    for (int i=0;i<64;i++) if (pole[i]==-fk) {  posbk=i; break; }      
    if (pole[posbk]!=-fk)  {
      Serial.println("!!b!!"+String(l)+" "+str_step(pos[l].steps[pos[l].cur_step]));
      Serial.print(str_step(pos[l-1].steps[pos[l-1].cur_step])); Serial.println(" "+String(pos[l-1].steps[pos[l-1].cur_step].type));
      Serial.println(str_step(pos[l-2].steps[pos[l-2].cur_step]));
      Serial.println(str_step(pos[l-3].steps[pos[l-3].cur_step]));
      Serial.println(str_step(pos[l-4].steps[pos[l-4].cur_step]));
      Serial.println(str_step(pos[l-5].steps[pos[l-5].cur_step]));
      Serial.println(str_step(pos[l-6].steps[pos[l-6].cur_step]));
     show_position();
     delay(1000000);
    } 
   }  
 if (abs(s.f2)==fk) { Serial.println("--king eat--"+str_step(s)+" level="+String(l)); 
  if (0&&s.c1>63||s.c1<0||s.c2>63||s.c2<0) { Serial.println("!!field out!! c1="+String(s.c1)+" c2="+String(s.c2)+" "+str_step(s)+" level="+String(l));
     Serial.println(str_step(pos[l-1].steps[pos[l-1].cur_step])); 
     Serial.println(str_step(pos[l-2].steps[pos[l-2].cur_step]));
      Serial.println(str_step(pos[l-3].steps[pos[l-3].cur_step]));
      Serial.println(str_step(pos[l-4].steps[pos[l-4].cur_step]));
      Serial.println(str_step(pos[l-5].steps[pos[l-5].cur_step]));
      Serial.println(str_step(pos[l-6].steps[pos[l-6].cur_step]));
      show_position();
     backstep(l-1,pos[l-1].steps[pos[l-1].cur_step]);
      show_position();
      movestep(l-1,pos[l-1].steps[pos[l-1].cur_step]);
      show_position();
      halt=1;
      Serial.println(get_time((millis()-starttime)/1000));
     if (zero) Serial.println("NULL");  
     delay(10000000);
  }  
  if (pos[l].w&s.f1<0||!pos[l].w&s.f1>0) { Serial.println("--opposite!---"+str_step(s)+" level="+String(l)); 
      show_position();
     delay(1000000);
  } 
 }
}
//****************************  
void movestep(int l, step_t &s) {           
  //checks(l,s);   
  pole[s.c1]=0;
  pole[s.c2]=s.f1;          
  if (pos[l].w) { // 
   if (s.f1==fk) poswk=s.c2; 
   switch (s.type) {   
   case 0:     
    return;
   case 1: //  
    pole[s.c2+8]=0;  
    break;
   case 2: // 
    pole[60]=0;
    pole[61]=fr;
    pole[62]=fk;
    pole[63]=0;   
    poswk=62;    
    break;
   case 3: // 
    pole[60]=0;
    pole[59]=fr;
    pole[58]=fk;
    pole[57]=0;
    pole[56]=0;  
    poswk=58;     
    break;
   default: 
    pole[s.c2]=s.type-2;   
   } 
  } else { //    
   if (s.f1==-fk) posbk=s.c2; 
   switch (s.type) {   
   case 0:     
    return; 
   case 1: //  
    pole[s.c2-8]=0;
    break;
   case 2: // 
    pole[4]=0;
    pole[5]=-fr;
    pole[6]=-fk;
    pole[7]=0;
    posbk=6;       
    break;
   case 3: // 
    pole[4]=0;
    pole[3]=-fr;
    pole[2]=-fk;
    pole[1]=0;
    pole[0]=0;   
    posbk=2;    
    break;
   default: 
    pole[s.c2]=2-s.type;
   }
  }      
}
//****************************  
void backstep(int l, step_t &s) {     
  pole[s.c1]=s.f1;
  pole[s.c2]=s.f2;  
  if (pos[l].w) { // 
   if (s.f1==fk) poswk=s.c1; 
   switch (s.type) {
   case 0: return;
   case 1: //  
    pole[s.c2]=0;
    pole[s.c2+8]=-fp;
    break;
   case 2: // 
    pole[60]=fk;
    pole[61]=0;
    pole[62]=0;
    pole[63]=fr;
    poswk=60;    
    break;
   case 3: // 
    pole[60]=fk;
    pole[59]=0;
    pole[58]=0;
    pole[57]=0;
    pole[56]=fr;
    poswk=60;    
    break;
   }
  } else { //  
  if (s.f1==-fk) posbk=s.c1;
   switch (s.type) {
   case 0: return;
   case 1: //  
    pole[s.c2]=0;
    pole[s.c2-8]=fp;
    break;
   case 2: // 
    pole[4]=-fk;
    pole[5]=0;
    pole[6]=0;
    pole[7]=-fr;
    posbk=4;    
    break;
   case 3: // 
    pole[4]=-fk;
    pole[3]=0;
    pole[2]=0;
    pole[1]=0;
    pole[0]=-fr;
    posbk=4;    
    break;
   }
  }             
}
//****************************  
void add_king2(int l, int i) {
  signed char f1=pole[i];
  signed char f2;
  int j=0;
  while (king_step[i][j]!=99) {                
    f2=pole[king_step[i][j]];
    if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
     steps2[n_steps2].type=0;      
     steps2[n_steps2].c1=i;      
     steps2[n_steps2].c2=king_step[i][j];      
     steps2[n_steps2].f1=f1;      
     steps2[n_steps2].f2=f2;           
     n_steps2++;            
    }             
    j++;
  }    
}
//****************************
void add_king(int l, int i) {
  signed char f1=pole[i];
  signed char f2;
  int j=0;
  while (king_step[i][j]!=99) {                
    f2=pole[king_step[i][j]];
    if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
     pos[l].steps[pos[l].n_steps].type=0;      
     pos[l].steps[pos[l].n_steps].c1=i;      
     pos[l].steps[pos[l].n_steps].c2=king_step[i][j];      
     pos[l].steps[pos[l].n_steps].f1=f1;      
     pos[l].steps[pos[l].n_steps].f2=f2;           
     pos[l].n_steps++;            
    }             
    j++;
  }    
}
//****************************
void add_knight(int l, int i) {
  signed char f1=pole[i];
  signed char f2;
  int j=0;
  while (knight_step[i][j]!=99) {                
    f2=pole[knight_step[i][j]];
    if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
     pos[l].steps[pos[l].n_steps].type=0;      
     pos[l].steps[pos[l].n_steps].c1=i;      
     pos[l].steps[pos[l].n_steps].c2=knight_step[i][j];      
     pos[l].steps[pos[l].n_steps].f1=f1;      
     pos[l].steps[pos[l].n_steps].f2=f2;           
     pos[l].n_steps++;            
    }             
    j++;
  }    
}
//****************************
void add_stra(int l, int i) {
  signed char f1=pole[i];
  signed char f2=0;
  int j=0;
  while (stra_step[i][j]!=99) {      
    if (stra_step[i][j]==88) f2=0;   
    //Serial.print(str_pole(i)); Serial.print(" "); Serial.println(stra_step[i][j]);
    else if (f2==0) {      
     f2=pole[stra_step[i][j]];
     if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
      pos[l].steps[pos[l].n_steps].type=0;      
      pos[l].steps[pos[l].n_steps].c1=i;      
      pos[l].steps[pos[l].n_steps].c2=stra_step[i][j];      
      pos[l].steps[pos[l].n_steps].f1=f1;      
      pos[l].steps[pos[l].n_steps].f2=f2;           
      //Serial.print(str_pole(i));
      //Serial.print("-"); Serial.println(str_pole(stra_step[i][j]));
      pos[l].n_steps++;            
     }          
    }
    j++;
  }
}
//****************************
void add_diag(int l, int i) {
  signed char f1=pole[i];
  signed char f2=0;
  int j=0;
  while (diag_step[i][j]!=99) {                
    if (diag_step[i][j]==88) f2=0; 
    else if (f2==0) {     
     f2=pole[diag_step[i][j]];
     if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
      pos[l].steps[pos[l].n_steps].type=0;      
      pos[l].steps[pos[l].n_steps].c1=i;      
      pos[l].steps[pos[l].n_steps].c2=diag_step[i][j];      
      pos[l].steps[pos[l].n_steps].f1=f1;      
      pos[l].steps[pos[l].n_steps].f2=f2;           
      pos[l].n_steps++;            
     }         
    }
    j++;
  }    
}
//****************************
void add_one(int l, int c1, int c2) {
  steps2[n_steps2].type=0;      
  steps2[n_steps2].c1=c1;      
  steps2[n_steps2].c2=c2;      
  steps2[n_steps2].f1=pole[c1];      
  steps2[n_steps2].f2=pole[c2];           
  n_steps2++;                
}
//****************************
boolean checkd_w() {
  signed char f2=0;
  int j=0;
  while (diag_step[poswk][j]!=99) {                
    if (diag_step[poswk][j]==88) f2=0; 
    else if (f2==0) {     
     f2=pole[diag_step[poswk][j]];
     if (f2==-fb||f2==-fq) return(true);
    }
    j++;
  }
  f2=0;  j=0;
  while (stra_step[poswk][j]!=99) {                     
    if (stra_step[poswk][j]==88) f2=0; 
    else if (f2==0) {         
     f2=pole[stra_step[poswk][j]];
     if (f2==-fr||f2==-fq) return(true);   
     if (j==0||stra_step[poswk][j]==88)
      if (f2==-fk) return(true);    
    }
    j++;
  }  
  return(false);  
}
//****************************
boolean checkd_b() {  
  signed char f2=0;
  int j=0;
  while (diag_step[posbk][j]!=99) {                
    if (diag_step[posbk][j]==88) f2=0; 
    else if (f2==0) {     
     f2=pole[diag_step[posbk][j]];
     if (f2==fb||f2==fq) return(true);    
    }
    j++;
  }
  f2=0;  j=0;
  while (stra_step[posbk][j]!=99) {                     
    if (stra_step[posbk][j]==88) f2=0; 
    else if (f2==0) {         
     f2=pole[stra_step[posbk][j]];
     if (f2==fr||f2==fq) return(true);    
     if (j==0||stra_step[posbk][j]==88)
      if (f2==fk) return(true);    
    }
    j++;
  }  
  return(false);  
}
//****************************
boolean check_w() {
  signed char f2=0;
  int j=0;
  if (pole[poswk]!=fk) {        
    for (int i=0;i<64;i++) if (pole[i]==fk) {  poswk=i; break; }      
  }  
  while (diag_step[poswk][j]!=99) {                
    if (diag_step[poswk][j]==88) f2=0; 
    else if (f2==0) {     
     f2=pole[diag_step[poswk][j]];
     if (f2==-fb||f2==-fq) return(true);
    }
    j++;
  }
  f2=0;  j=0;
  while (stra_step[poswk][j]!=99) {                     
    if (stra_step[poswk][j]==88) f2=0; 
    else if (f2==0) {         
     f2=pole[stra_step[poswk][j]];
     if (f2==-fr||f2==-fq) return(true);   
     if (j==0||stra_step[poswk][j]==88)
      if (f2==-fk) return(true);    
    }
    j++;
  }
  j=0;
  while (knight_step[poswk][j]!=99) {                
    if (pole[knight_step[poswk][j]]==-fn) return(true);                 
    j++;
  }
  if (row[poswk]<7) {
    if (column[poswk]>1&&pole[poswk-9]==-fp) return(true);
    if (column[poswk]<8&&pole[poswk-7]==-fp) return(true);
  }
  j=0;
  while (king_step[poswk][j]!=99) {                
    if (pole[king_step[poswk][j]]==-fk) return(true);                 
    j++;
  }  
  return(false);  
}

//****************************
boolean check_b() {  
  signed char f2=0;
  int j=0;
  if (pole[posbk]!=-fk) {        
    for (int i=0;i<64;i++) if (pole[i]==-fk) {  posbk=i; break; }      
  }    
  while (diag_step[posbk][j]!=99) {                
    if (diag_step[posbk][j]==88) f2=0; 
    else if (f2==0) {     
     f2=pole[diag_step[posbk][j]];
     if (f2==fb||f2==fq) return(true);    
    }
    j++;
  }
  f2=0;  j=0;
  while (stra_step[posbk][j]!=99) {                     
    if (stra_step[posbk][j]==88) f2=0; 
    else if (f2==0) {         
     f2=pole[stra_step[posbk][j]];
     if (f2==fr||f2==fq) return(true);    
     if (j==0||stra_step[posbk][j]==88)
      if (f2==fk) return(true);    
    }
    j++;
  }
  j=0;
  while (knight_step[posbk][j]!=99) {                
    if (pole[knight_step[posbk][j]]==fn) return(true);                 
    j++;
  }    
  if (row[posbk]>2) {
    if (column[posbk]>1&&pole[posbk+7]==fp) return(true);
    if (column[posbk]<8&&pole[posbk+9]==fp) return(true);
  }
  j=0;
  while (king_step[posbk][j]!=99) {                
    if (pole[king_step[posbk][j]]==fk) return(true);                 
    j++;
  }    
  return(false);  
}
//****************************
void sort_steps(int l) { //      
step_t buf;  
   for (int i=0;i<pos[l].n_steps-1;i++) { //          
    int maxweight=pos[l].steps[i].weight;
    int maxj=i;
    for (int j=i+1;j<pos[l].n_steps;j++)  {
     if (pos[l].steps[j].weight>maxweight) {
      maxweight=pos[l].steps[j].weight; maxj=j;
     }      
    }    
    if (maxweight==0&&l>0) return;    
    if (maxj==i) continue;    
    buf=pos[l].steps[i];
    pos[l].steps[i]=pos[l].steps[maxj];
    pos[l].steps[maxj]=buf;    
   }    
}
//****************************
int evaluate(int l) { //                
  
  if (!stats) {  
    if (pos[l].w) return pos[l].weight_w-pos[l].weight_b; else return pos[l].weight_b-pos[l].weight_w;                
  } else {    
   if (pos[l].w) return 5000*(pos[l].weight_w-pos[l].weight_b+pos[l].weight_s)/(pos[l].weight_w+pos[l].weight_b+2000); 
   else return 5000*(pos[l].weight_b-pos[l].weight_w-pos[l].weight_s)/(pos[l].weight_w+pos[l].weight_b+2000);        
  }
} 
//****************************
void taskOne( void * parameter )
{
   task_time=millis();
   int l,j,check,f;
   signed char f2;
   unsigned long task_tik;
   unsigned long task_count=0;
   do { // 
    if (task_start==2) {      
    // task_tik=micros(); 
     l=task_l;
     if (pole[poswk]!=fk) {      
      for (int i=0;i<64;i++) if (pole[i]==fk) {  poswk=i; break; }     
     } 
     if (pole[posbk]!=-fk) {      
      for (int i=0;i<64;i++) if (pole[i]==-fk) {  posbk=i; break; }        
     }
     if (l>0) {
      if (pos[l-1].steps[pos[l-1].cur_step].check==0) {           
       if (pos[l].w) pos[l].check_on_table=check_w(); else pos[l].check_on_table=check_b();        
       pos[l-1].steps[pos[l-1].cur_step].check=pos[l].check_on_table;          
      } else pos[l].check_on_table=1; 
     } else if (pos[0].w) pos[0].check_on_table=check_w(); else pos[0].check_on_table=check_b();        
     n_steps2=0;
     for (int ii=0;ii<64;ii++) {
      int i;
      if (pos[l].w) i=ii; else i=63-ii;
      f=pole[i];          
      if (f==0||f<0&&pos[l].w||f>0&&!pos[l].w) continue;            
      if (f==fp) {
       if (row[i]<7&&pole[i-8]==0) add_one(l,i,i-8); 
       if (row[i]==2&&pole[i-8]==0&&pole[i-16]==0) add_one(l,i,i-16); 
       if (row[i]==7) {
        if (pole[i-8]==0) {
         add_one(l,i,i-8); steps2[n_steps2-1].type=4;      
         add_one(l,i,i-8); steps2[n_steps2-1].type=5;      
         add_one(l,i,i-8); steps2[n_steps2-1].type=6;      
         add_one(l,i,i-8); steps2[n_steps2-1].type=7;      
        } 
        if (column[i]>1&&pole[i-9]<0) {
         add_one(l,i,i-9); steps2[n_steps2-1].type=4;      
         add_one(l,i,i-9); steps2[n_steps2-1].type=5;      
         add_one(l,i,i-9); steps2[n_steps2-1].type=6;      
         add_one(l,i,i-9); steps2[n_steps2-1].type=7;              
        }
        if (column[i]<8&&pole[i-7]<0) {
         add_one(l,i,i-7); steps2[n_steps2-1].type=4;      
         add_one(l,i,i-7); steps2[n_steps2-1].type=5;      
         add_one(l,i,i-7); steps2[n_steps2-1].type=6;      
         add_one(l,i,i-7); steps2[n_steps2-1].type=7;              
        }     
      } else {
       if (column[i]>1&&pole[i-9]<0) add_one(l,i,i-9); 
       if (column[i]<8&&pole[i-7]<0) add_one(l,i,i-7);        
      }
    } else if (f==-fp) {
      if (row[i]>2&&pole[i+8]==0) add_one(l,i,i+8); 
      if (row[i]==7&&pole[i+8]==0&&pole[i+16]==0) add_one(l,i,i+16); 
      if (row[i]==2) {
        if (pole[i+8]==0) {
         add_one(l,i,i+8); steps2[n_steps2-1].type=4;      
         add_one(l,i,i+8); steps2[n_steps2-1].type=5;      
         add_one(l,i,i+8); steps2[n_steps2-1].type=6;      
         add_one(l,i,i+8); steps2[n_steps2-1].type=7;      
        } 
        if (column[i]>1&&pole[i+7]>0) {
         add_one(l,i,i+7); steps2[n_steps2-1].type=4;      
         add_one(l,i,i+7); steps2[n_steps2-1].type=5;      
         add_one(l,i,i+7); steps2[n_steps2-1].type=6;      
         add_one(l,i,i+7); steps2[n_steps2-1].type=7;              
        }
        if (column[i]<8&&pole[i+9]>0) {
         add_one(l,i,i+9); steps2[n_steps2-1].type=4;      
         add_one(l,i,i+9); steps2[n_steps2-1].type=5;      
         add_one(l,i,i+9); steps2[n_steps2-1].type=6;      
         add_one(l,i,i+9); steps2[n_steps2-1].type=7;              
        }     
      } else {
       if (column[i]>1&&pole[i+7]>0) add_one(l,i,i+7); 
       if (column[i]<8&&pole[i+9]>0) add_one(l,i,i+9);            
      }
     } else if (!endspiel&&abs(f)==fk) add_king2(l,i);      
    } //    
    if (pos[l].pp!=0&&pole[pos[l].pp]==0) { //   !!!
     if (pos[l].w==1) {
      if (column[pos[l].pp]>1&&pole[pos[l].pp+7]==fp) { add_one(l,pos[l].pp+7,pos[l].pp); steps2[n_steps2-1].type=1; steps2[n_steps2-1].f2=-fp;}
      if (column[pos[l].pp]<8&&pole[pos[l].pp+9]==fp) { add_one(l,pos[l].pp+9,pos[l].pp); steps2[n_steps2-1].type=1; steps2[n_steps2-1].f2=-fp;}
     } else {
      if (column[pos[l].pp]>1&&pole[pos[l].pp-9]==-fp) { add_one(l,pos[l].pp-9,pos[l].pp); steps2[n_steps2-1].type=1; steps2[n_steps2-1].f2=fp;}
      if (column[pos[l].pp]<8&&pole[pos[l].pp-7]==-fp) { add_one(l,pos[l].pp-7,pos[l].pp); steps2[n_steps2-1].type=1; steps2[n_steps2-1].f2=fp;}
     }
    }
  //   task_execute+=micros()-task_tik;
     task_start=0;             
    } //if task_start   
    task_count++;
    if (task_count>7000000) {
     task_count=0;
     if (Serial.available()) {
      String s=Serial.readString();
      s.trim();     
      s.toUpperCase();   
      if (s=="STOP") { halt=1; Serial.println("STOPPED"); } //timelimith=millis(); }     
     } else delay(1);     
    }   
  } while (1);   
}
//****************************
int action(step_t& s) { //  
int j;      
  signed char f2;
  if (s.f2!=0||s.type>0||abs(s.f1)>fr) return 0; //       
  switch (s.f1) {  
  case fp: 
     if (row[s.c2]>5) return 1; // 
     if (column[s.c2]>1&&pole[s.c2-9]<-fp||column[s.c2]<8&&pole[s.c2-7]<-fp) return 1; // 
     return 0;     
  case -fp: 
     if (row[s.c2]<4) return 1; // 
     if (column[s.c2]>1&&pole[s.c2+7]>fp||column[s.c2]<8&&pole[s.c2+9]>fp) return 1; // 
     return 0;          
  case fn:      
    j=0;
    while (knight_step[s.c2][j]!=99) {                    
     if (pole[knight_step[s.c2][j]]<-fb) return 1; //                
     j++;
    }      
    return 0;     
  case -fn:         
    j=0;
    while (knight_step[s.c2][j]!=99) {                    
     if (pole[knight_step[s.c2][j]]>fb) return 1; //               
     j++;
    }      
    return 0;     
  case fb: 
    j=0; f2=0;
    while (diag_step[s.c2][j]!=99) {                
     if (diag_step[s.c2][j]==88) f2=0; 
     else if (f2==0) {     
       f2=pole[diag_step[s.c2][j]];
       if (diag1[s.c1]!=diag1[stra_step[s.c2][j]]&&diag2[s.c1]!=diag2[stra_step[s.c2][j]])
        if (f2<-fb) return 1;              
     }
     j++;
    }   
    return 0;
  case -fb: 
    j=0; f2=0;
    while (diag_step[s.c2][j]!=99) {                
     if (diag_step[s.c2][j]==88) f2=0; 
     else if (f2==0) {     
       f2=pole[diag_step[s.c2][j]];
       if (diag1[s.c1]!=diag1[stra_step[s.c2][j]]&&diag2[s.c1]!=diag2[stra_step[s.c2][j]])
        if (f2>fb) return 1;              
     }
     j++;
    }       
    return 0;
  case fr: 
    j=0; f2=0;
    while (stra_step[s.c2][j]!=99) {                
     if (stra_step[s.c2][j]==88) f2=0; 
     else if (f2==0) {     
       f2=pole[stra_step[s.c2][j]];
       if (row[s.c1]!=row[stra_step[s.c2][j]]&&column[s.c1]!=column[stra_step[s.c2][j]])
        if (f2<-fr) return 1;              
     }
     j++;
    }           
    return 0;
  case -fr: 
    j=0; f2=0;
    while (stra_step[s.c2][j]!=99) {                
     if (stra_step[s.c2][j]==88) f2=0; 
     else if (f2==0) {     
       f2=pole[stra_step[s.c2][j]];
       if (row[s.c1]!=row[stra_step[s.c2][j]]&&column[s.c1]!=column[stra_step[s.c2][j]])
        if (f2>fr) return 1;       
     }              
     j++;
    }               
    return 0;  
  } //switch     
  return 0; 
}
//****************************  
void generate_steps(int l) {
 pos[l].cur_step=0; pos[l].n_steps=0; 
 int check;
 signed char f;  
 task_l=l;
 task_start=2; 
  
 for (int ii=0;ii<64;ii++) {
  int i;
  if (pos[l].w) i=ii; else i=63-ii;
  f=pole[i];    
  if (f==0||f<0&&pos[l].w||f>0&&!pos[l].w) continue;
  switch (abs(f)) {   
   case fn: add_knight(l,i); break;   
   case fb: add_diag(l,i); break;
   case fr: add_stra(l,i); break;   
   case fq: add_stra(l,i); add_diag(l,i); break;   
   case fk: if (endspiel) add_king(l,i); break;           
  }   
 } //     
  //int in=0;
  while (task_start==2) {  
   delayMicroseconds(0);
   //in=1;
  }  
  //if (in) countin++;
  //countall++;
 if (pos[l].w==1&&!pos[l].check_on_table) { //   
  if (pos[l].wrk) { //     
   if (pole[60]==fk&&pole[61]==0&&pole[62]==0&&pole[63]==fr) {
    pole[60]=0; pole[61]=fk; poswk=61; check=check_w(); pole[60]=fk; poswk=60; pole[61]=0;    
    if (!check) {    
     pos[l].steps[pos[l].n_steps].type=2;      
     pos[l].steps[pos[l].n_steps].c1=60;      
     pos[l].steps[pos[l].n_steps].c2=62;      
     pos[l].steps[pos[l].n_steps].f1=fk;      
     pos[l].steps[pos[l].n_steps].f2=0;           
     pos[l].n_steps++;                      
    } 
   }
  }  
  if (pos[l].wrq) { // 
   if (pole[60]==fk&&pole[59]==0&&pole[58]==0&&pole[57]==0&&pole[56]==fr) {
    pole[60]=0; pole[59]=fk; poswk=59; check=check_w(); pole[60]=fk; poswk=60; pole[59]=0;    
    if (!check) {        
     pos[l].steps[pos[l].n_steps].type=3;      
     pos[l].steps[pos[l].n_steps].c1=60;      
     pos[l].steps[pos[l].n_steps].c2=58;      
     pos[l].steps[pos[l].n_steps].f1=fk;      
     pos[l].steps[pos[l].n_steps].f2=0;           
     pos[l].n_steps++;                      
    } 
   }
  }
 } else if (pos[l].w==0&&!pos[l].check_on_table) { //   
  if (pos[l].brk) { // 
   if (pole[4]==-fk&&pole[5]==0&&pole[6]==0&&pole[7]==-fr) {
    pole[4]=0; pole[5]=-fk; posbk=5; check=check_b(); pole[4]=-fk; posbk=4; pole[5]=0;   
    if (!check) {    
     pos[l].steps[pos[l].n_steps].type=2;      
     pos[l].steps[pos[l].n_steps].c1=4;      
     pos[l].steps[pos[l].n_steps].c2=6;      
     pos[l].steps[pos[l].n_steps].f1=-fk;      
     pos[l].steps[pos[l].n_steps].f2=0;           
     pos[l].n_steps++;                      
    } 
   }
  }
  if (pos[l].brq) { //
   if (pole[4]==-fk&&pole[3]==0&&pole[2]==0&&pole[1]==0&&pole[0]==-fr) {    
    pole[4]=0; pole[3]=-fk; posbk=3; check=check_b(); pole[4]=-fk; posbk=4; pole[3]=0;   
    if (!check) {    
     pos[l].steps[pos[l].n_steps].type=3;      
     pos[l].steps[pos[l].n_steps].c1=4;      
     pos[l].steps[pos[l].n_steps].c2=2;      
     pos[l].steps[pos[l].n_steps].f1=-fk;      
     pos[l].steps[pos[l].n_steps].f2=0;           
     pos[l].n_steps++;                      
    }
   }  
  }
 }  
 for (int i=0;i<n_steps2;i++) {       
  pos[l].steps[pos[l].n_steps]=steps2[i];
  pos[l].n_steps++;
 }
 for (int i=0;i<pos[l].n_steps;i++) {       
  pos[l].steps[i].check=0;
  pos[l].steps[i].weight=abs(pos[l].steps[i].f2);  
  if (pos[l].steps[i].type>3) pos[l].steps[i].weight+=fig_weight[pos[l].steps[i].type-2];    
  pos[l].steps[i].weight<<=2;
  if (l>0) { //       
       if (pos[l].best.c2==pos[l].steps[i].c2&&pos[l].best.c1==pos[l].steps[i].c1) pos[l].steps[i].weight+=5; //                         
       if (pos[l].steps[i].c2==pos[l-1].steps[pos[l-1].cur_step].c2) pos[l].steps[i].weight+=8; //                      
  }
  //if (l<level) {
   //if (action(pos[l].steps[i])) {
   //  pos[l].steps[i].weight=1;
   //  show_position();   
   //    Serial.println("="+str_step(pos[l].steps[i]));      
   //   delay(20000);
   //}
  //}
 

 } //i     
  //Serial.println("TIME GENER="+String(micros()-sta)); 
 //show_steps(0);
 //delay(100000000); 
 sort_steps(l); 
 //halt=1;
 //show_steps(0);   
 
}
//****************************  
int draw_repeat(int l) {
 if (l<=12||zero) return 0;
 for (int i=0;i<4;i++) {
  int li=l-i; 
  if (pos[li].steps[pos[li].cur_step].c1!=pos[li-4].steps[pos[li-4].cur_step].c1||
      pos[li].steps[pos[li].cur_step].c1!=pos[li-8].steps[pos[li-8].cur_step].c1||
      pos[li].steps[pos[li].cur_step].c2!=pos[li-4].steps[pos[li-4].cur_step].c2||
      pos[li].steps[pos[li].cur_step].c2!=pos[li-8].steps[pos[li-8].cur_step].c2) return 0;
 }
 if (TRACE>0) Serial.println("repeat!");
 return 1;
}
//****************************  
int active(step_t& s) { //     
int j;  
  if (s.f2!=0||s.type>3) return 1; //  
  if (abs(s.f2)==fk) return -1; //   ,   ..
  switch (s.f1) {  
  case fp: 
     if (row[s.c2]>5) return 1; // 
     if (column[s.c2]>1&&posbk==s.c2-9||column[s.c2]<8&&posbk==s.c2-7) return 1; // 
     return -1;     
  case -fp: 
     if (row[s.c2]<4) return 1; // 
     if (column[s.c2]>1&&poswk==s.c2+7||column[s.c2]<8&&posbk==s.c2+9) return 1; // 
     return -1;          
  case fn:      
    j=0;
    while (knight_step[s.c2][j]!=99) {                    
     if (pole[knight_step[s.c2][j]]==-fk) return 1; //             
     j++;
    }      
    return 0;     
  case -fn:         
    j=0;
    while (knight_step[s.c2][j]!=99) {                    
     if (pole[knight_step[s.c2][j]]==fk) return 1; //             
     j++;
    }      
    return 0;     
  case fb: 
    if (diag1[s.c2]!=diag1[posbk]&&diag2[s.c2]!=diag2[posbk]) return -1; //  
    return 0;
  case -fb: 
    if (diag1[s.c2]!=diag1[poswk]&&diag2[s.c2]!=diag2[poswk]) return -1; //    
    return 0;
  case fr: 
    if (row[s.c2]!=row[posbk]&&column[s.c2]!=column[posbk]) return -1; // - 
    return 0;
  case -fr: 
    if (row[s.c2]!=row[poswk]&&column[s.c2]!=column[poswk]) return -1; // - 
    return 0;
  case fq: 
    if (diag1[s.c2]!=diag1[posbk]&&diag2[s.c2]!=diag2[posbk]&&        //    
        row[s.c2]!=row[posbk]&&column[s.c2]!=column[posbk]) return -1; // - 
    return 0;
  case -fq: 
    if (diag1[s.c2]!=diag1[poswk]&&diag2[s.c2]!=diag2[poswk]&&        //    
        row[s.c2]!=row[poswk]&&column[s.c2]!=column[poswk]) return -1; // - 
    return 0;
  } //switch     
  return 0; 
}
//****************************  
int quiescence(int l, int alpha, int beta, int depthleft) {     
  if (depthleft<=0) { 
    if (l>depth) depth=l;
    return evaluate(l);
  }
  int score=-20000;             
  generate_steps(l);  
  if (!pos[l].check_on_table) {   
    int weight=evaluate(l); 
    if (weight >= score) score=weight; 
    if (score>alpha) alpha=score;    
    if (alpha>=beta) return alpha;                
  }                      
  int check,checked,act; 
  for (int i=0;i<pos[l].n_steps;i++) {            
   act=1;
   if (!pos[l].check_on_table) {
    act=active(pos[l].steps[i]);
    if (act==-1) continue;    
   }
   movestep(l,pos[l].steps[i]);        
   check=0;   
   if (act==0) {
    if (pos[l].w) check=checkd_b(); else check=checkd_w();       
    pos[l].steps[i].check=check;
     if (!check) { backstep(l,pos[l].steps[i]); continue; }      
   }   
   if (pos[l].w) checked=check_w(); else checked=check_b();     
   if (checked) { backstep(l,pos[l].steps[i]);  continue; } //  -            
   
   if (check&&depthleft==1&&l<MAXDEPTH-1) depthleft++;     
   
   pos[l].cur_step=i;                       
   
   movepos(l,pos[l].steps[i]);   
   int tmp=-quiescence(l+1,-beta,-alpha,depthleft-1);   
   backstep(l,pos[l].steps[i]);
   if (draw_repeat(l)) tmp=0;    
   if (tmp>score) score=tmp;
   if (score>alpha) {
    alpha=score;      
    pos[l].best=pos[l].steps[i];          
   }
   if (alpha>=beta ) return alpha;    
  } 
  if (score==-20000) {
    if (pos[l].check_on_table) {
      score=-10000+l; pos[l-1].steps[pos[l-1].cur_step].check=2; 
    }
  }       
  return score;
}
//****************************  
int alphaBeta(int l, int alpha, int beta, int depthleft) {
 int score=-20000, check, ext, tmp;      
   if (depthleft<=0) {
      int fd=fdepth; //4-6-8         
      if (pos[l-1].steps[pos[l-1].cur_step].f2!=0) fd+=2;       ; //                 
      return quiescence(l,alpha,beta,fd);                    
   }   
   if (l>0) generate_steps(l);   
   if (l>=nulldepth&&zero==0&&depthleft>2)      //2
    if (!pos[l].check_on_table&&pos[l-1].steps[pos[l-1].cur_step].f2==0)  {
     zero=1;  
     pos[l+1].wrk=pos[l].wrk; pos[l+1].wrq=pos[l].wrq; 
     pos[l+1].brk=pos[l].brk; pos[l+1].brq=pos[l].brq;
     pos[l+1].weight_w=pos[l].weight_w; pos[l+1].weight_b=pos[l].weight_b; pos[l+1].weight_s=pos[l].weight_s;
     pos[l+1].pp=0;    
     pos[l].cur_step=MAXSTEPS;
     pos[l].steps[MAXSTEPS].f2=0;     
     int tmpz=-alphaBeta(l+1,-beta,-beta+1,depthleft-3);   //3
     zero=0;    
     if (tmpz>=beta) return beta;  
    }    
   if (l>4&&!zero&&depthleft<=2&&futility&&!pos[l].check_on_table&&pos[l-1].steps[pos[l-1].cur_step].f2==0) { //futility pruning
     int weight=evaluate(l); 
     if (weight-200>=beta) return beta;
   }   
   for (int i=0;i<pos[l].n_steps;i++) {       
    ext=0;
    if (l==0) { 
      depth=depthleft;                      
      if (level<7) if (pos[0].steps[pos[0].cur_step].check) ext=2; // 
    }
    movestep(l,pos[l].steps[i]);        
    if (pos[l].w) check=check_w(); else check=check_b();        
    if (check) { backstep(l,pos[l].steps[i]);  continue; } //  -      
    pos[l].cur_step=i;
    movepos(l,pos[l].steps[i]);    
    if (TRACE>0) {
     if (l==0) {              
       Serial.print(str_step(pos[0].steps[i])); Serial.print("  ");
       Serial.print(i+1); Serial.print("/"); Serial.print(pos[0].n_steps);                        
       //if (pos[0].steps[i].weight<-9000) { Serial.println(F(" checkmate")); continue; }        
     } else if (TRACE>l) {  
       Serial.println();             
       for (int ll=0;ll<l;ll++) Serial.print("      "); 
       Serial.print(String(l+1)+"- "+str_step(pos[l].steps[i]));       
     }
    } //TRACE         
    if (l>2&&!lazy&&!zero&&lazyeval&&!pos[l].steps[i].f2&&!pos[0].steps[pos[0].cur_step].check&&evaluate(l+1)+100<=alpha&&
     (pos[l].w&&!check_b()||!pos[l].w&&!check_w())) {
      lazy=1;
      if (-alphaBeta(l+1,-beta,-alpha,depthleft-3)<=alpha) tmp=alpha; 
       else { lazy=0; tmp=-alphaBeta(l+1,-beta,-alpha,depthleft-1+ext);   }
      lazy=0;
    } else tmp=-alphaBeta(l+1,-beta,-alpha,depthleft-1+ext);   
    backstep(l,pos[l].steps[i]);    
    if (draw_repeat(l)) tmp=0;
    if (tmp>score) score=tmp;
    pos[l].steps[i].weight=tmp;    
    if (score>alpha) {
      alpha=score;      
      pos[l].best=pos[l].steps[i];                  
      if (l==0&&level>3) //    
       if (print_best(depthleft)) return alpha;
    }
    if (l==0) {         
        if (TRACE>0) Serial.println("        "+String(tmp));            
        //if (alpha==9999||alpha==-5000) break;                                                           
    } else if (TRACE>l) {         
        Serial.print(" = "+String(tmp));                
    }             
    if (alpha>=beta) return alpha;                
    if (halt||(l<3&&millis()-starttime>timelimith))  //   
      return score;                        
   }   
   if (score==-20000) {
    if (pos[l].check_on_table) {
      score=-10000+l; pos[l-1].steps[pos[l-1].cur_step].check=2; 
    } else score=0;
   }       
   return score;
}
//****************************  
boolean is_draw() { //   
  boolean draw=false;
  int cn=0,cbw=0,cbb=0,co=0,cb=0,cw=0;
  for (int i=0;i<64;i++) {  
    if (abs(pole[i])==1) co++;  
    if (abs(pole[i])>3&&abs(pole[i])<6) co++; // , ,   
    if (abs(pole[i])==6) continue;  // 
    if (abs(pole[i])==2) cn++; // 
    if (abs(pole[i])==3&&(column[i]+row[i])%2==0) cbb++; //  
    if (abs(pole[i])==3&&(column[i]+row[i])%2==1) cbw++; //   
    if (pole[i]==3) cw++; //     
    if (pole[i]==-3) cb++; //     
  }
  if (cn==1&&co+cbb+cbw==0) draw=true;  // 
  if (cbb+cbw==1&&co+cn==0) draw=true;  // 
  if (co+cn+cbb==0||co+cn+cbw==0) draw=true; //    
  if (co+cn==0&&cb==1&&cw==1) draw=true; //    
  return draw;  
}
//****************************
String get_time(long tim) {  
  char sz[10];      
  if (tim>360000) tim=0;      
  sprintf(sz, "%02d:%02d:%02d", tim/3600,(tim%3600)/60,tim%60);      
  return String(sz);
}
//****************************
boolean print_best(int dep) {
  if (halt||millis()-starttime>timelimith) return false;
  if (lastbestdepth==dep&&pos[0].best.type==lastbeststep.type&&
   pos[0].best.c1==lastbeststep.c1&&pos[0].best.c2==lastbeststep.c2) return false;
  boolean ret=false; 
  if (pos[0].best.type==lastbeststep.type&&pos[0].best.c1==lastbeststep.c1&&pos[0].best.c2==lastbeststep.c2) {   
   for (int i=0;i<MAXEPD;i++) {    
    if (lastbeststep.c2==bestmove[i].c2&&lastbeststep.c1==bestmove[i].c1&&lastbeststep.type==bestmove[i].type) {
      ret=true; bestsolved=1; break;
    }
   }
  }
  lastbestdepth=dep;
  lastbeststep=pos[0].best;  
  if (pos[0].w==0) Serial.print("1..."); else Serial.print("1.");
  String st=str_step(pos[0].best);
  Serial.print(st);
  for (int i=0;i<10-st.length();i++) Serial.print(" ");  
  String depf="/"+String(depth+1)+" ";
  long tim=(millis()-starttime)/1000;
  String wei=String(pos[0].best.weight/100.,2);
  if (pos[0].best.weight>9000) wei="+M"+String((10001-pos[0].best.weight)/2);
  
  Serial.println("("+wei+") Depth: "+String(dep+depf)+get_time(tim)+" "+String(count/1000)+"kN");  
  return ret;
}         
//****************************
void kingpositions() {
  for (int i=0;i<64;i++) { //  
    if (pole[i]==fk) poswk=i;  
    if (pole[i]==-fk) posbk=i;
  } 
}  
//****************************  
boolean solve_step() {
int score; 
  boolean solved=0;   
  count=0;
  countin=0; countall=0;    
  zero=0; lazy=0;
  for (int i=1;i<MAXDEPTH;i++) {
   if (i%2) pos[i].w=!pos[0].w; else pos[i].w=pos[0].w;    
   pos[i].pp=0;
  } 
  starttime=millis();  
  task_execute=0;
  if (is_draw()) { Serial.println(" DRAW!"); return 1; } //  
  lastbestdepth=-1;
  lastbeststep.type=0; lastbeststep.c1=-1; lastbeststep.c2=-1;     
  bestsolved=0;    
  
  pos[0].weight_b=0; pos[0].weight_w=0; 
  for (int i=0;i<64;i++) { //    
    if (pole[i]<0) {
      pos[0].weight_b+=fig_weight[-pole[i]];      
    } else if (pole[i]>0) { 
      pos[0].weight_w+=fig_weight[pole[i]]; //   8000           
    }        
  }    
  if (pos[0].weight_w+pos[0].weight_b<3500) endspiel=true; else endspiel=false;    //3500?
  //Serial.println("endspiel="+String(endspiel));
  
  pos[0].weight_s=0; 
  for (int i=0;i<64;i++) { //   
   int f=pole[i];
   if (!f) continue;
   if (abs(f)==fk&&endspiel) {
     if (f<0) 
       pos[0].weight_s-=stat_weightb[6][i];                               
     else 
       pos[0].weight_s+=stat_weightw[6][i];                                 
   } else {                
     if (f<0) 
      pos[0].weight_s-=stat_weightb[-f-1][i];                               
     else 
      pos[0].weight_s+=stat_weightw[f-1][i];                                     
   }      
  }  

  kingpositions(); //  
  
  if (TRACE>0) Serial.println(" start score="+evaluate(0));

  generate_steps(0);     
  
  int legal=0;
  int check;
  int samebest=0;  
  for (int i=0;i<pos[0].n_steps;i++) {     //       
    movestep(0,pos[0].steps[i]);   
    if (pos[0].w) check=check_w();  else check=check_b();           
    pos[0].check_on_table=check;
    if (!check) legal++;         
    if (!check) pos[0].steps[i].weight=0; else pos[0].steps[i].weight=-30000;
    backstep(0,pos[0].steps[i]);
  }  
  if (legal==0) 
   if (pos[0].check_on_table) { Serial.println(" CHECKMATE!"); return 1; } // 
    else { Serial.println(" PAT!"); return 1; } // 
    
  sort_steps(0);
  pos[0].n_steps=legal; //    
  
  int ALPHA=-20000; 
  int BETA=20000; 
  level=2; 
  if (timelimith>300000) level=4;      
  for (int x=0;x<MAXDEPTH;x++) { 
    pos[x].best.f1=0; pos[x].best.c2=-1;     
  } //             

  stats=1;
  
  while (level<=20) {
   if (TRACE>0) {
    Serial.print(F("******* LEVEL=")); Serial.print(level);
    Serial.println();       
   }      
   for (int x=1;x<MAXDEPTH;x++) { 
    pos[x].best.f1=0; pos[x].best.c2=-1;     
   } //    0 
   for (int i=0;i<pos[0].n_steps;i++) {     //       
    movestep(0,pos[0].steps[i]);   
    if (pos[0].w) pos[0].steps[i].check=check_b(); 
     else pos[0].steps[i].check=check_w();           
    pos[0].steps[i].weight+=evaluate(0)+pos[0].steps[i].check*500;    
    if (pos[0].steps[i].f2!=0) pos[0].steps[i].weight-=pos[0].steps[i].f1;        
    backstep(0,pos[0].steps[i]);
   }  
   pos[0].steps[0].weight+=10000; // -  
   sort_steps(0);     
   for (int i=0;i<pos[0].n_steps;i++) pos[0].steps[i].weight=-8000;  //            
   if (nullmove) nulldepth=3; else  nulldepth=93;       
   //BETA=10000; ALPHA=9900; 
   //int sec=(millis()-starttime)/1000;   
   fdepth=4;   
   score=alphaBeta(0,ALPHA,BETA,level);   
   unsigned long tim=millis()-starttime;        
   boolean out=0;
   if (score>=BETA) out=1;
   if (multipov||samebest>2||out) { 
    samebest=0;          
    ALPHA=-20000;     
    BETA=20000;      
   } else {
    ALPHA=score-100;
    BETA=score+100;      
   }          
   if (tim>timelimith*0.2&&!out) { 
    stats=0;         
    ALPHA=score-300;     
    BETA=score+300;     
   }       
   
   sort_steps(0);    //                      
   if (print_best(level)||bestsolved||score>9900) { solved=1; break; }     
   if (tim>timelimith||halt) break;       
   if (pos[0].best.type==lastbeststep.type&&pos[0].best.c1==lastbeststep.c1&&pos[0].best.c2==lastbeststep.c2) 
    { samebest++; } else samebest=0;              
   
   level++; 
   
   //Serial.println(level);
   //Serial.println(tim/1000);
  } //while level    
  //Serial.println(String(countin)+"/"+String(countall));     
  //Serial.println("Task load: "+String(0.1*task_execute/(millis()-starttime))+"%");     
  return solved;
}
//****************************
unsigned int getpacked() { //    hash  
 int c=0; unsigned int hash=0;
 for (int i=0;i<8;i++) {
  polep.q[i]=0; 
  for (int j=0;j<8;j++) {      
   polep.q[i]<<=4;   
   polep.q[i]+=pole[c]&B1111;             
   //hash^=polep.q[i];   
   c++;   
  }  
  hash^=polep.q[i];
 }
 //return hash%MAXHASH;
}
//****************************
void printBits(int myInt){
 unsigned int mask=1;
 mask<<=31; 
 for (int i=0;i<32;i++) {   
   if(mask  & myInt)
       Serial.print('1');
   else
       Serial.print('0');
   mask=mask>>1;    
 }
 Serial.println();
}
//****************************
void elo(int numelo=0) { //ELOmeter tests
 String elos[76]={   
  "7k/1K6/8/4q3/8/2B5/2P5/8 w - -", //Bc3xe5+
  "R7/5qpk/1p6/2p1rb1p/2Q5/2P3P1/1P3P1P/7K w - -", //Qc4xf7
  "k7/8/3r4/8/4N3/5K2/5P2/8 w - -", //Ne4xd6
  "8/8/8/5K2/8/4R3/p2k4/8 w - -", //Re3-a3
  "8/8/2N5/8/8/4k3/p6K/8 w - -", //Nc6-b4
  "8/1q4k1/8/8/8/6K1/8/1R6 w - -", //Rb1xb7+
  "7k/q5pp/1P6/8/8/8/6PP/7K w - -", //b6xa7
  "rr3b1k/2p2p2/b4N1p/q3p3/2P5/8/PP3P2/KN1R2R1 w - -", //Rg1-g8#
  "r3k2r/5ppp/p3p3/1p1p4/1PpP4/2P1P3/P3KPPP/RR6 w - -", //Ke2-f1
  "8/8/8/1Pk5/8/8/2P3K1/8 w - -", //c2-c4
  "5rk1/2b2ppp/pp3n2/2p1p1B1/4P3/2NP4/PPP2PPP/5RK1 w - -", //Bg5xf6
  "r3q2k/5Q1p/1pb2bp1/5p2/2B5/PP3N2/K1P3P1/8 w - -", //Qf7xf6#
  "8/R7/1p2k3/2p1q1p1/2P1Q3/1P2K1P1/7r/8 w - -", //Ra7-e7+
  "1b3rk1/2q3p1/p7/2p3N1/2p4P/2P3n1/1PQ3P1/4R1K1 w - -", //Qc2-h7#
  "8/8/8/6p1/5kP1/7K/8/8 w - -" //Kh3-g2
  "2b5/ppp1k1pp/2n1p3/1BNp1p2/3P3P/PP2P1P1/1P1K1P2/8 w - -", //Bb5xc6
  "2r4k1/1ppq1pp1/p1n2n1p/8/3P4/1PBQ1N1P/P4PP1/3R2K1 w - -", //d4-d5
  "r2qk2r/ppp2ppp/1n1p1nb1/8/2PP4/2NB2P1/PP3PP1/R1BQ1RK1 w - -", //Qd1-e2+
  "8/2P5/3K4/5b2/1p6/6k1/8/8 w - -", //Kd6-e5
  "2r3r1/4Nppk/5b2/qppP4/8/1P3P2/P1P3P1/1K1RR3 w - -", //Re1-h1+
  "3R1rk1/p5pp/1p1Q4/5qP1/4Nn2/1P6/P5PP/7K w - -", //Ne4-f6+
  "r2qr2k/6pp/pp1p4/3Pn1N1/8/1P4P1/P2Q3P/R3R1K1 w - -", //Re1xe5
  "r2q1rk1/1ppn1ppp/p2np3/3p4/B2P4/2P1PN2/P1P1QPPP/R4RK1 w - -", //Ba4-b3
  "5q2/1p2k3/p2p4/5bp1/1B6/P2P1P2/2K3P1/1Q6 w - -", //Bb4xd6+
  "1r3rk1/5ppp/8/1pbN3Q/2p1P3/2Bn1P2/1P3qPP/RR5K w - -", //Nd5-f6+
  "3r1r1k/p3bpR1/1p1p1n1p/4pP1q/2N1P3/1P2BPQP/P4K2/6R1 w - -", //Rg7-h7+
  "rr3nk1/6q1/2p1pRP1/3pP1Qp/6p1/2P3P1/N6P/K1R5 w - -",//Rf6-f7
  "1r2r3/p1q2p1k/R5p1/3p3b/7Q/1P4R1/3B1PPP/6K1 w - -", //Qh4xh5+
  "4br2/p1q1p1k1/4Q1p1/1pN2n2/1P1b4/8/P3B1PP/4BR1K w - -", //Qe6xf5
  "8/p7/8/PP1k4/1K6/8/8/8 w - -", //a5-a6
  "6r1/pkpq3p/6p1/2P5/2NPnP2/1P5R/7K/3Q4 w - -", //c5-c6+
  "r4rk1/2qnppb1/4p1p1/4P1N1/1p1P1P2/1P6/1KP1N3/3R2QR w - -", //Rh1-h8+
  "1rb1k2r/2p2ppp/2p5/4p3/2P1n1q1/3Q4/PPPB2PP/2KR2NR w - -", //Qd3-d8+
  "4r1k1/p4p1p/1p3qpB/3b4/1P1R4/P1Q5/5PPP/6K1 w - -", //Rd4-e4
  "r4r1k/1p1bbppp/1qp1pn2/4B3/3P3P/1P1B1QR1/P3NPP1/1K1R4 w - -", //Bd3-c2?
  "3k1r2/4R3/2pB4/pqP2b1p/3P4/2Pp2P1/8/2K1Q3 w - -", //Re7-d7+
  "r3r1k1/4b1pp/3p3P/6P1/2p1b1q1/4B3/PP1Q2B1/K5RR w - -", //Qd2-d5+
  "rr4k1/R1Q2ppp/4pq2/8/7n/3P4/2P2NPP/5R1K w - -", //Qc7-b7
  "7R/8/8/8/3kp3/8/r7/4K3 w - -", //Rh8-d8+
  "3B2k1/1b3p1p/p5p1/1p5q/3Q4/1P3P2/P1r3PP/3R2K1 w - -", //Qd4-h8+
  "8/8/3p4/2k5/4P3/8/8/1K6 w - -", //Kb1-a2
  "r2qr1k1/1bpn1p1p/1p3bp1/p3p3/P1P5/4PN1P/1PQ1BPPB/R2R2K1 w - -", //e3-e4
  "3rkn2/1Q2b2p/2p1p3/p2q4/n3NP2/2P1K1R1/P2B3P/2N5 w - -", //Qb7xe7+
  "8/p7/Pp1kB3/1Ppn2K1/2P5/8/8/8 w - -", //Be6xd5
  "r1bqk2r/pp1n1ppp/2p2n2/3p4/P2Pp3/2P1P3/2PN1PPP/R1BQKB1R w - -", //Bc1-a3
  "r2k3r/pp3pb1/3p3p/1BpP2p1/Q7/P1P2q2/1P3P2/R3R1K1 w - -", //Bb5-e8
  "8/6P1/5K1r/8/8/8/8/3k4 w - -", //Kf6-f5 !
  "r4r2/2k2qpp/pRbbR3/P2p1P2/P1pP2P1/2B2Q2/2B5/6K1 w - -", //Rb6xc6+
  "rn2k2r/1bQ1qpp1/p3p3/2bp4/P2N3p/2PB4/1P3PPP/R1B1R1K1 w - -", //Nd4xe6
  "8/8/5k2/8/p7/8/1PK5/8 w - -", //Kc3
  "rr4k1/1q3ppp/1bQRp3/6P1/4P3/P4N2/1P6/K3R3 w - -", //Rd6-d8+
  "r3kb1r/pp4p1/2p1p1p1/4p3/8/2PqPQ1P/PP1N2P1/1R2K2R w - -", //Rh1-f1
  "2r2rk1/1R3pp1/n3pP1n/q2p4/p2P1NP1/R2Q1P1p/2N4P/6K1 w - -", //Qd3-g6
  "r3k1nr/pp1nq1p1/1bp1b2p/3pPp2/3P1B2/2PB1N2/PP1NQ1PP/R3K2R w - -", //Qe2-f1
  "2k4r/p1b2p2/Pp1r1q1p/1Pp1p3/4P1pP/2P1R1P1/5PB1/1RQ3K1 w - -", //c3-c4
  "r1bq1rk1/ppp2pp1/2np1n1p/3Np1NQ/8/1B1P4/PPP2PP1/2KR3R w - -", //Qh5-g6
  "r2q1rk1/pbp1bppp/1p2pn2/6B1/3P4/3B1N2/PPP1QPPP/R4RK1 w - -", //Rf1-d1
  "3kN2b/2p4P/2p2p2/2P2P1K/8/8/8/8 w - -", //Ne8xc7
  "r2qrnk1/pp2bppp/2p1bn2/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - -" //Qc2-b3
  "rnbq1rk1/ppp4p/3p2p1/3Pp2n/2P1Pp1b/2N2P2/PP1QNBPP/2KR1B1R w - -", //Bf2xh4
  "8/8/5p2/5p2/5P2/3p3B/5k1P/3K4 w - -", //Kd1-d2
  "5rk1/R6p/3pp3/2p1n2r/2q5/2B5/1PQ2PPP/5RK1 w - -", //Ra7-g7+
  "r6k/1p4pp/p2q4/1r3P2/2npQ2P/6P1/R1PB1P2/2K1R3 w - -", //Ra2xa6
  "5N2/4P3/7k/6r1/8/8/8/4K3 w - -", //Nf8-g6
  "8/8/8/8/3p1B2/4p3/5p2/5K1k w - -", //Bf4-g3
  "r4r2/pppqbpk1/2n1b2p/4p1p1/3pP3/3P2PP/PPPN1PBK/R2Q1RN1 w - -", //66
  "4r2k/5Qpp/8/2N5/3n2p1/8/2P3PP/4qR1K w - -",
  "rn2k2r/p4pp1/1p2p2p/1P1pPn1P/2qP4/5N2/2PB1PP1/RQ2K2R w - -",
  "8/8/8/4p1p1/8/5P2/6K1/3k4 w - -", //69
  "r4rk1/pb1qn1pp/1pnNp3/3pPp2/PP1P1P2/5N2/3Q2PP/R3KB1R w - -",
  "3rr1k1/3nbppp/p1R2n2/qp1Bp1B1/4P3/5N1P/PP3PP1/2RQ2K1 w - -", //71
  "4r1k1/1b4pp/p1p1r3/2Pp1qb1/PP2p3/2N3B1/4RPPP/3RQ1K1 w - -", //???
  "6k1/p1P5/P1r5/2p1K3/8/8/2R5/8 w - -",
  "r4r2/6kp/2pqppp1/pbR5/3P4/4QN2/PP3PPP/2R3K1 w - -",
  "4k2r/1p3ppp/p1n5/2r1p3/4P3/2N5/PP2KPPP/2RR4 w - -", //75
  "2r5/pp2kp2/3q1p1Q/3Pp3/6b1/1B6/P1P3PP/1K3R2 w - -" 
  
 };
}
//****************************
void WAC(int numwac=0) { //WAC tests

String wacs[300]={   
"2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - bm Qg6; id ""WAC.001""; d5+m2",
"8/7p/5k2/5p2/p1p2P2/Pr1pPK2/1P1R3P/8 b - - bm Rxb2; id ""WAC.002""; d18+4",
"5rk1/1ppb3p/p1pb4/6q1/3P1p1r/2P1R2P/PP1BQ1P1/5RKN w - - bm Rg3; id ""WAC.003""; d5+3",
"r1bq2rk/pp3pbp/2p1p1pQ/7P/3P4/2PB1N2/PP3PPR/2KR4 w - - bm Qxh7+; id ""WAC.004""; d5+m2",
"5k2/6pp/p1qN4/1p1p4/3P4/2PKP2Q/PP3r2/3R4 b - - bm Qc4+; id ""WAC.005""; d5+m2",
"7k/p7/1R5K/6r1/6p1/6P1/8/8 w - - bm Rb7; id ""WAC.006""; d5+7",
"rnbqkb1r/pppp1ppp/8/4P3/6n1/7P/PPPNPPP1/R1BQKBNR b KQkq - bm Ne3; id ""WAC.007""; d5+3",
"r4q1k/p2bR1rp/2p2Q1N/5p2/5p2/2P5/PP3PPP/R5K1 w - - bm Rf7; id ""WAC.008""; d5+7 d11+m8",
"3q1rk1/p4pp1/2pb3p/3p4/6Pr/1PNQ4/P1PB1PP1/4RRK1 b - - bm Bh2+; id ""WAC.009""; d5+m5",
"2br2k1/2q3rn/p2NppQ1/2p1P3/Pp5R/4P3/1P3PPP/3R2K1 w - - bm Rxh7; id ""WAC.010""; d5+4",
"r1b1kb1r/3q1ppp/pBp1pn2/8/Np3P2/5B2/PPP3PP/R2Q1RK1 w kq - bm Bxc6; id ""WAC.011""; d5+3",
"4k1r1/2p3r1/1pR1p3/3pP2p/3P2qP/P4N2/1PQ4P/5R1K b - - bm Qxf3+; id ""WAC.012""; d5+m2",
"5rk1/pp4p1/2n1p2p/2Npq3/2p5/6P1/P3P1BP/R4Q1K w - - bm Qxf8+; id ""WAC.013""; d5+1.5",
"r2rb1k1/pp1q1p1p/2n1p1p1/2bp4/5P2/PP1BPR1Q/1BPN2PP/R5K1 w - - bm Qxh7+; id ""WAC.014""; d5+m4",
"1R6/1brk2p1/4p2p/p1P1Pp2/P7/6P1/1P4P1/2R3K1 w - - bm Rxb7; id ""WAC.015""; d5+7",
"r4rk1/ppp2ppp/2n5/2bqp3/8/P2PB3/1PP1NPPP/R2Q1RK1 w - - bm Nc3; id ""WAC.016""; d5+2",
"1k5r/pppbn1pp/4q1r1/1P3p2/2NPp3/1QP5/P4PPP/R1B1R1K1 w - - bm Ne5; id ""WAC.017""; d5+1",
"R7/P4k2/8/8/8/8/r7/6K1 w - - bm Rh8; id ""WAC.018""; d6+6",
"r1b2rk1/ppbn1ppp/4p3/1QP4q/3P4/N4N2/5PPP/R1B2RK1 w - - bm c6; id ""WAC.019""; d6+0.7",    
"r2qkb1r/1ppb1ppp/p7/4p3/P1Q1P3/2P5/5PPP/R1B2KNR b kq - bm Bb5; id ""WAC.020""; d6+7",
"5rk1/1b3p1p/pp3p2/3n1N2/1P6/P1qB1PP1/3Q3P/4R1K1 w - - bm Qh6; id ""WAC.021""; d5+6",
"r1bqk2r/ppp1nppp/4p3/n5N1/2BPp3/P1P5/2P2PPP/R1BQK2R w KQkq - bm Ba2 Nxf7; id ""WAC.022""; Nxf7 d5+0.5",
"r3nrk1/2p2p1p/p1p1b1p1/2NpPq2/3R4/P1N1Q3/1PP2PPP/4R1K1 w - - bm g4; id ""WAC.023""; d5+4",
"6k1/1b1nqpbp/pp4p1/5P2/1PN5/4Q3/P5PP/1B2B1K1 b - - bm Bd4; id ""WAC.024""; d5+7",
"3R1rk1/8/5Qpp/2p5/2P1p1q1/P3P3/1P2PK2/8 b - - bm Qh4+; id ""WAC.025""; d5+12",
"3r2k1/1p1b1pp1/pq5p/8/3NR3/2PQ3P/PP3PP1/6K1 b - - bm Bf5; id ""WAC.026""; d5+1.2",
"7k/pp4np/2p3p1/3pN1q1/3P4/Q7/1r3rPP/2R2RK1 w - - bm Qf8+; id ""WAC.027""; d5+m2",
"1r1r2k1/4pp1p/2p1b1p1/p3R3/RqBP4/4P3/1PQ2PPP/6K1 b - - bm Qe1+; id ""WAC.028""; d5+3",
"r2q2k1/pp1rbppp/4pn2/2P5/1P3B2/6P1/P3QPBP/1R3RK1 w - - bm c6; id ""WAC.029""; d5+1",
"1r3r2/4q1kp/b1pp2p1/5p2/pPn1N3/6P1/P3PPBP/2QRR1K1 w - - bm Nxd6; id ""WAC.030""; d7+0.7",
"rb3qk1/pQ3ppp/4p3/3P4/8/1P3N2/1P3PPP/3R2K1 w - - bm Qxa8 d6 dxe6 g3; id ""WAC.031""; d6 d5+3 g3 d10+3.3",
"6k1/p4p1p/1p3np1/2q5/4p3/4P1N1/PP3PPP/3Q2K1 w - - bm Qd8+; id ""WAC.032""; d5+1.7",
"8/p1q2pkp/2Pr2p1/8/P3Q3/6P1/5P1P/2R3K1 w - - bm Qe5+ Qf4; id ""WAC.033""; d6+5",
"7k/1b1r2p1/p6p/1p2qN2/3bP3/3Q4/P5PP/1B1R3K b - - bm Bg1; id ""WAC.034""; d5+2.7",
"r3r2k/2R3pp/pp1q1p2/8/3P3R/7P/PP3PP1/3Q2K1 w - - bm Rxh7+; id ""WAC.035""; d5+m4",
"3r4/2p1rk2/1pQq1pp1/7p/1P1P4/P4P2/6PP/R1R3K1 b - - bm Re1+; id ""WAC.036""; d5+4",
"2r5/2rk2pp/1pn1pb2/pN1p4/P2P4/1N2B3/nPR1KPPP/3R4 b - - bm Nxd4+; id ""WAC.037""; d5+1",
"4k3/p4prp/1p6/2b5/8/2Q3P1/P2R1PKP/4q3 w - - bm Qd3 Rd8+; id ""WAC.038""; Rd8+ 5+1.5 Qd3 13+1.8", 
"r1br2k1/pp2bppp/2nppn2/8/2P1PB2/2N2P2/PqN1B1PP/R2Q1R1K w - - bm Na4; id ""WAC.039""; d5+2.3",
"3r1r1k/1p4pp/p4p2/8/1PQR4/6Pq/P3PP2/2R3K1 b - - bm Rc8; id ""WAC.040""; d5+3.8",
"1k6/5RP1/1P6/1K6/6r1/8/8/8 w - - bm Ka5 Kc5 b7; id ""WAC.041""; Ka5 d6+16",
"r1b1r1k1/pp1n1pbp/1qp3p1/3p4/1B1P4/Q3PN2/PP2BPPP/R4RK1 w - - bm Ba5; id ""WAC.042""; d5+3",
"r2q3k/p2P3p/1p3p2/3QP1r1/8/B7/P5PP/2R3K1 w - - bm Be7 Qxa8; id ""WAC.043""; Be7 d6+12",
"3rb1k1/pq3pbp/4n1p1/3p4/2N5/2P2QB1/PP3PPP/1B1R2K1 b - - bm dxc4; id ""WAC.044""; d6+2",
"7k/2p1b1pp/8/1p2P3/1P3r2/2P3Q1/1P5P/R4qBK b - - bm Qxa1; id ""WAC.045""; d6+5",
"r1bqr1k1/pp1nb1p1/4p2p/3p1p2/3P4/P1N1PNP1/1PQ2PP1/3RKB1R w K - bm Nb5; id ""WAC.046""; d5+0.5",
"r1b2rk1/pp2bppp/2n1pn2/q5B1/2BP4/2N2N2/PP2QPPP/2R2RK1 b - - bm Nxd4; id ""WAC.047""; d5+0.3",
"1rbq1rk1/p1p1bppp/2p2n2/8/Q1BP4/2N5/PP3PPP/R1B2RK1 b - - bm Rb4; id ""WAC.048""; d5+1.7",
"2b3k1/4rrpp/p2p4/2pP2RQ/1pP1Pp1N/1P3P1P/1q6/6RK w - - bm Qxh7+; id ""WAC.049""; d7+m6",
"k4r2/1R4pb/1pQp1n1p/3P4/5p1P/3P2P1/r1q1R2K/8 w - - bm Rxb6+; id ""WAC.050""; d5+m3",
"r1bq1r2/pp4k1/4p2p/3pPp1Q/3N1R1P/2PB4/6P1/6K1 w - - bm Rg4+; id ""WAC.051""; d5+9 d11+m8",
"r1k5/1p3q2/1Qpb4/3N1p2/5Pp1/3P2Pp/PPPK3P/4R3 w - - bm Re7 c4; id ""WAC.052""; Re7 d5+4",
"6k1/6p1/p7/3Pn3/5p2/4rBqP/P4RP1/5QK1 b - - bm Re1; id ""WAC.053""; d5+4",
"r3kr2/1pp4p/1p1p4/7q/4P1n1/2PP2Q1/PP4P1/R1BB2K1 b q - bm Qh1+; id ""WAC.054"";",
"r3r1k1/pp1q1pp1/4b1p1/3p2B1/3Q1R2/8/PPP3PP/4R1K1 w - - bm Qxg7+; id ""WAC.055""; d8+m4",
"r1bqk2r/pppp1ppp/5n2/2b1n3/4P3/1BP3Q1/PP3PPP/RNB1K1NR b KQkq - bm Bxf2+; id ""WAC.056""; d5+4",
"r3q1kr/ppp5/3p2pQ/8/3PP1b1/5R2/PPP3P1/5RK1 w - - bm Rf8+; id ""WAC.057""; d5+m3",
"8/8/2R5/1p2qp1k/1P2r3/2PQ2P1/5K2/8 w - - bm Qd1+; id ""WAC.058""; d5+7",
"r1b2rk1/2p1qnbp/p1pp2p1/5p2/2PQP3/1PN2N1P/PB3PP1/3R1RK1 w - - bm Nd5; id ""WAC.059""; d5+4",
"rn1qr1k1/1p2np2/2p3p1/8/1pPb4/7Q/PB1P1PP1/2KR1B1R w - - bm Qh8+; id ""WAC.060""; d5+m2",
"3qrbk1/ppp1r2n/3pP2p/3P4/2P4P/1P3Q2/PB6/R4R1K w - - bm Qf7+; id ""WAC.061""; d5+m2",
"6r1/3Pn1qk/p1p1P1rp/2Q2p2/2P5/1P4P1/P3R2P/5RK1 b - - bm Rxg3+; id ""WAC.062""; d6+0",
"r1brnbk1/ppq2pp1/4p2p/4N3/3P4/P1PB1Q2/3B1PPP/R3R1K1 w - - bm Nxf7; id ""WAC.063""; d5+2",
"8/6pp/3q1p2/3n1k2/1P6/3NQ2P/5PP1/6K1 w - - bm g4+; id ""WAC.064""; d5+m2",
"1r1r1qk1/p2n1p1p/bp1Pn1pQ/2pNp3/2P2P1N/1P5B/P6P/3R1RK1 w - - bm Ne7+; id ""WAC.065""; d5+9",
"1k1r2r1/ppq5/1bp4p/3pQ3/8/2P2N2/PP4P1/R4R1K b - - bm Qxe5; id ""WAC.066""; d5+4",
"3r2k1/p2q4/1p4p1/3rRp1p/5P1P/6PK/P3R3/3Q4 w - - bm Rxd5; id ""WAC.067""; d5+4",
"6k1/5ppp/1q6/2b5/8/2R1pPP1/1P2Q2P/7K w - - bm Qxe3; id ""WAC.068""; d5+6",
"2k5/pppr4/4R3/4Q3/2pp2q1/8/PPP2PPP/6K1 w - - bm f3 h3; id ""WAC.069""; f3 d7+6",
"2kr3r/pppq1ppp/3p1n2/bQ2p3/1n1PP3/1PN1BN1P/1PP2PP1/2KR3R b - - bm Na2+; id ""WAC.070""; d5+4",
"2kr3r/pp1q1ppp/5n2/1Nb5/2Pp1B2/7Q/P4PPP/1R3RK1 w - - bm Nxa7+; id ""WAC.071""; d8+3",
"r3r1k1/pp1n1ppp/2p5/4Pb2/2B2P2/B1P5/P5PP/R2R2K1 w - - bm e6; id ""WAC.072""; d5+2",
"r1q3rk/1ppbb1p1/4Np1p/p3pP2/P3P3/2N4R/1PP1Q1PP/3R2K1 w - - bm Qd2; id ""WAC.073""; d5+3.5",
"5r1k/pp4pp/2p5/2b1P3/4Pq2/1PB1p3/P3Q1PP/3N2K1 b - - bm Qf1+; id ""WAC.074""; d5+7",
"r3r1k1/pppq1ppp/8/8/1Q4n1/7P/PPP2PP1/RNB1R1K1 b - - bm Qd6; id ""WAC.075""; d5+4",
"r1b1qrk1/2p2ppp/pb1pnn2/1p2pNB1/3PP3/1BP5/PP2QPPP/RN1R2K1 w - - bm Bxf6; id ""WAC.076""; d5+2",
"3r2k1/ppp2ppp/6q1/b4n2/3nQB2/2p5/P4PPP/RN3RK1 b - - bm Ng3; id ""WAC.077""; d5+8",
"r2q3r/ppp2k2/4nbp1/5Q1p/2P1NB2/8/PP3P1P/3RR1K1 w - - bm Ng5+; id ""WAC.078""; d5+4.5",
"r3k2r/pbp2pp1/3b1n2/1p6/3P3p/1B2N1Pq/PP1PQP1P/R1B2RK1 b kq - bm Qxh2+; id ""WAC.079""; d5+m3",
"r4rk1/p1B1bpp1/1p2pn1p/8/2PP4/3B1P2/qP2QP1P/3R1RK1 w - - bm Ra1; id ""WAC.080""; d9+3",
"r4rk1/1bR1bppp/4pn2/1p2N3/1P6/P3P3/4BPPP/3R2K1 b - - bm Bd6; id ""WAC.081""; d5+0.5",
"3rr1k1/pp3pp1/4b3/8/2P1B2R/6QP/P3q1P1/5R1K w - - bm Bh7+; id ""WAC.082""; d5+8",
"3rr1k1/ppqbRppp/2p5/8/3Q1n2/2P3N1/PPB2PPP/3R2K1 w - - bm Qxd7; id ""WAC.083""; d5+3.5",
"r2q1r1k/2p1b1pp/p1n5/1p1Q1bN1/4n3/1BP1B3/PP3PPP/R4RK1 w - - bm Qg8+; id ""WAC.084""; d5+m2",
"kr2R3/p4r2/2pq4/2N2p1p/3P2p1/Q5P1/5P1P/5BK1 w - - bm Na6; id ""WAC.085""; d5+6 d7+10",
"8/p7/1ppk1n2/5ppp/P1PP4/2P1K1P1/5N1P/8 b - - bm Ng4+; id ""WAC.086""; d5+0.5",
"8/p3k1p1/4r3/2ppNpp1/PP1P4/2P3KP/5P2/8 b - - bm Rxe5; id ""WAC.087""; d8+6",
"r6k/p1Q4p/2p1b1rq/4p3/B3P3/4P3/PPP3P1/4RRK1 b - - bm Rxg2+; id ""WAC.088""; d5+m5",
"1r3b1k/p4rpp/4pp2/3q4/2ppbPPQ/6RK/PP5P/2B1NR2 b - - bm g5; id ""WAC.089""; d5+7",
"3qrrk1/1pp2pp1/1p2bn1p/5N2/2P5/P1P3B1/1P4PP/2Q1RRK1 w - - bm Nxg7; id ""WAC.090""; d5+2.5",
"2qr2k1/4b1p1/2p2p1p/1pP1p3/p2nP3/PbQNB1PP/1P3PK1/4RB2 b - - bm Be6; id ""WAC.091""; d6+1",
"r4rk1/1p2ppbp/p2pbnp1/q7/3BPPP1/2N2B2/PPP4P/R2Q1RK1 b - - bm Bxg4; id ""WAC.092""; d9+0.7",
"r1b1k1nr/pp3pQp/4pq2/3pn3/8/P1P5/2P2PPP/R1B1KBNR w KQkq - bm Bh6; id ""WAC.093""; d5+1",
"8/k7/p7/3Qp2P/n1P5/3KP3/1q6/8 b - - bm e4+; id ""WAC.094""; d5+9",
"2r5/1r6/4pNpk/3pP1qp/8/2P1QP2/5PK1/R7 w - - bm Ng4+; id ""WAC.095""; d5+9",
"r1b4k/ppp2Bb1/6Pp/3pP3/1qnP1p1Q/8/PPP3P1/1K1R3R w - - bm Qd8+ b3; id ""WAC.096""; d5+2 d14+m9",
"6k1/5p2/p5np/4B3/3P4/1PP1q3/P3r1QP/6RK w - - bm Qa8+; id ""WAC.097"";",
"1r3rk1/5pb1/p2p2p1/Q1n1q2p/1NP1P3/3p1P1B/PP1R3P/1K2R3 b - - bm Nxe4; id ""WAC.098""; d5+5",
"r1bq1r1k/1pp1Np1p/p2p2pQ/4R3/n7/8/PPPP1PPP/R1B3K1 w - - bm Rh5; id ""WAC.099""; d5+m2",
"8/k1b5/P4p2/1Pp2p1p/K1P2P1P/8/3B4/8 w - - bm Be3 b6+; id ""WAC.100""; Be3 d14+2.6 b6+ d17+4",
"5rk1/p5pp/8/8/2Pbp3/1P4P1/7P/4RN1K b - - bm Bc3; id ""WAC.101""; d10+1.5",
"2Q2n2/2R4p/1p1qpp1k/8/3P3P/3B2P1/5PK1/r7 w - - bm Qxf8+; id ""WAC.102""; d5+m3",
"6k1/2pb1r1p/3p1PpQ/p1nPp3/1q2P3/2N2P2/PrB5/2K3RR w - - bm Qxg6+; id ""WAC.103""; d5+m4",
"b4r1k/pq2rp2/1p1bpn1p/3PN2n/2P2P2/P2B3K/1B2Q2N/3R2R1 w - - bm Qxh5; id ""WAC.104""; d5+m3",
"r2r2k1/pb3ppp/1p1bp3/7q/3n2nP/PP1B2P1/1B1N1P2/RQ2NRK1 b - - bm Bxg3 Qxh4; id ""WAC.105""; Qxh4 d7+m8 d8+m6",
"4rrk1/pppb4/7p/3P2pq/3Qn3/P5P1/1PP4P/R3RNNK b - - bm Nf2+; id ""WAC.106""; d5+9",
"5n2/pRrk2p1/P4p1p/4p3/3N4/5P2/6PP/6K1 w - - bm Nb5; id ""WAC.107""; d5+3",
"r5k1/1q4pp/2p5/p1Q5/2P5/5R2/4RKPP/r7 w - - bm Qe5; id ""WAC.108""; d5+3",
"rn2k1nr/pbp2ppp/3q4/1p2N3/2p5/QP6/PB1PPPPP/R3KB1R b KQkq - bm c3; id ""WAC.109""; d7+0.5",
"2kr4/bp3p2/p2p2b1/P7/2q5/1N4B1/1PPQ2P1/2KR4 b - - bm Be3; id ""WAC.110""; d5+7",
"6k1/p5p1/5p2/2P2Q2/3pN2p/3PbK1P/7P/6q1 b - - bm Qf1+; id ""WAC.111""; d5+10",
"r4kr1/ppp5/4bq1b/7B/2PR1Q1p/2N3P1/PP3P1P/2K1R3 w - - bm Rxe6; id ""WAC.112""; d5+2.5",
"rnbqkb1r/1p3ppp/5N2/1p2p1B1/2P5/8/PP2PPPP/R2QKB1R b KQkq - bm Qxf6; id ""WAC.113""; d5+1",
"r1b1rnk1/1p4pp/p1p2p2/3pN2n/3P1PPq/2NBPR1P/PPQ5/2R3K1 w - - bm Bxh7+; id ""WAC.114""; d5+2.5",
"4N2k/5rpp/1Q6/p3q3/8/P5P1/1P3P1P/5K2 w - - bm Nd6; id ""WAC.115""; d5+2.5",
"r2r2k1/2p2ppp/p7/1p2P1n1/P6q/5P2/1PB1QP1P/R5RK b - - bm Rd2; id ""WAC.116""; d8+2",
"3r1rk1/q4ppp/p1Rnp3/8/1p6/1N3P2/PP3QPP/3R2K1 b - - bm Ne4; id ""WAC.117""; d5+6",
"r5k1/pb2rpp1/1p6/2p4q/5R2/2PB2Q1/P1P3PP/5R1K w - - bm Rh4; id ""WAC.118""; d5+4",
"r2qr1k1/p1p2ppp/2p5/2b5/4nPQ1/3B4/PPP3PP/R1B2R1K b - - bm Qxd3; id ""WAC.119""; d5+4",
"r4rk1/1bn2qnp/3p1B1Q/p2P1pP1/1pp5/5N1P/PPB2P2/2KR3R w - - bm Rhg1 g6; id ""WAC.120""; g6 d7+3",
"6k1/5p1p/2bP2pb/4p3/2P5/1p1pNPPP/1P1Q1BK1/1q6 b - - bm Bxf3+; id ""WAC.121""; d5+11",
"1k6/ppp4p/1n2pq2/1N2Rb2/2P2Q2/8/P4KPP/3r1B2 b - - bm Rxf1+; id ""WAC.122""; d5+12",
"6k1/1b2rp2/1p4p1/3P4/PQ4P1/2N2q2/5P2/3R2K1 b - - bm Bxd5 Rc7 Re6; id ""WAC.123""; Rc7 d5+2 Bxd5 d10+4 Re6 d17+6",
"6k1/3r4/2R5/P5P1/1P4p1/8/4rB2/6K1 b - - bm g3; id ""WAC.124""; d5+2",
"r1bqr1k1/pp3ppp/1bp5/3n4/3B4/2N2P1P/PPP1B1P1/R2Q1RK1 b - - bm Bxd4+; id ""WAC.125""; d5+3",
"r5r1/pQ5p/1qp2R2/2k1p3/4P3/2PP4/P1P3PP/6K1 w - - bm Rxc6+; id ""WAC.126""; d5+4",
"2k4r/1pr1n3/p1p1q2p/5pp1/3P1P2/P1P1P3/1R2Q1PP/1RB3K1 w - - bm Rxb7; id ""WAC.127""; d5+3",
"6rk/1pp2Qrp/3p1B2/1pb1p2R/3n1q2/3P4/PPP3PP/R6K w - - bm Qg6; id ""WAC.128""; d5+2",
"3r1r1k/1b2b1p1/1p5p/2p1Pp2/q1B2P2/4P2P/1BR1Q2K/6R1 b - - bm Bf3; id ""WAC.129""; d5+1.3",
"6k1/1pp3q1/5r2/1PPp4/3P1pP1/3Qn2P/3B4/4R1K1 b - - bm Qh6 Qh8; id ""WAC.130""; Qh6 d5+1",
"2rq1bk1/p4p1p/1p4p1/3b4/3B1Q2/8/P4PpP/3RR1K1 w - - bm Re8; id ""WAC.131""; d6+2",
"4r1k1/5bpp/2p5/3pr3/8/1B3pPq/PPR2P2/2R2QK1 b - - bm Re1; id ""WAC.132""; d5+m3",
"r1b1k2r/1pp1q2p/p1n3p1/3QPp2/8/1BP3B1/P5PP/3R1RK1 w kq - bm Bh4; id ""WAC.133""; d5+3",
"3r2k1/p6p/2Q3p1/4q3/2P1p3/P3Pb2/1P3P1P/2K2BR1 b - - bm Rd1+; id ""WAC.134""; d5+m4",
"3r1r1k/N2qn1pp/1p2np2/2p5/2Q1P2N/3P4/PP4PP/3R1RK1 b - - bm Nd4; id ""WAC.135""; d5+1.3",
"6kr/1q2r1p1/1p2N1Q1/5p2/1P1p4/6R1/7P/2R3K1 w - - bm Rc8+; id ""WAC.136""; Qf6 d6+m6 Rc8+ d8+m3",
"3b1rk1/1bq3pp/5pn1/1p2rN2/2p1p3/2P1B2Q/1PB2PPP/R2R2K1 w - - bm Rd7; id ""WAC.137""; d5+2",
"r1bq3r/ppppR1p1/5n1k/3P4/6pP/3Q4/PP1N1PP1/5K1R w - - bm h5; id ""WAC.138""; d5+m7 d8+m5",
"rnb3kr/ppp2ppp/1b6/3q4/3pN3/Q4N2/PPP2KPP/R1B1R3 w - - bm Nf6+; id ""WAC.139""; d5+m4",
"r2b1rk1/pq4p1/4ppQP/3pB1p1/3P4/2R5/PP3PP1/5RK1 w - - bm Bc7 Rc7; id ""WAC.140""; Rc7 d5+9",
"4r1k1/p1qr1p2/2pb1Bp1/1p5p/3P1n1R/1B3P2/PP3PK1/2Q4R w - - bm Qxf4; id ""WAC.141""; d10+19 d12+m6",
"r2q3n/ppp2pk1/3p4/5Pr1/2NP1Qp1/2P2pP1/PP3K2/4R2R w - - bm Re8 f6+; id ""WAC.142""; f6+ d5+3",
"5b2/pp2r1pk/2pp1pRp/4rP1N/2P1P3/1P4QP/P3q1P1/5R1K w - - bm Rxh6+; id ""WAC.143""; d5+m3",
"r2q1rk1/pp3ppp/2p2b2/8/B2pPPb1/7P/PPP1N1P1/R2Q1RK1 b - - bm d3; id ""WAC.144""; d5+1.6",
"r1bq4/1p4kp/3p1n2/p4pB1/2pQ4/8/1P4PP/4RRK1 w - - bm Re8; id ""WAC.145""; d8+15",
"8/8/2Kp4/3P1B2/2P2k2/5p2/8/8 w - - bm Bc8 Bd3 Bh3; id ""WAC.146""; Bc8 d5+2",
"r2r2k1/ppqbppbp/2n2np1/2pp4/6P1/1P1PPNNP/PBP2PB1/R2QK2R b KQ - bm Nxg4; id ""WAC.147""; d5+1.7",
"2r1k3/6pr/p1nBP3/1p3p1p/2q5/2P5/P1R4P/K2Q2R1 w - - bm Rxg7; id ""WAC.148""; d5+9",
"6k1/6p1/2p4p/4Pp2/4b1qP/2Br4/1P2RQPK/8 b - - bm Bxg2; id ""WAC.149""; d5+2",
"r3r1k1/5p2/pQ1b2pB/1p6/4p3/6P1/Pq2BP1P/2R3K1 b - - bm Ba3 Be5 Bf8 e3; c0 ""All win but e3 is best.""; id ""WAC.150""; e3 d3+3.5",
"8/3b2kp/4p1p1/pr1n4/N1N4P/1P4P1/1K3P2/3R4 w - - bm Nc3; id ""WAC.151""; d5+1",
"1br2rk1/1pqb1ppp/p3pn2/8/1P6/P1N1PN1P/1B3PP1/1QRR2K1 w - - bm Ne4; id ""WAC.152""; d5+2",
"2r3k1/q4ppp/p3p3/pnNp4/2rP4/2P2P2/4R1PP/2R1Q1K1 b - - bm Nxd4; id ""WAC.153""; d5+2",
"r1b2rk1/2p2ppp/p7/1p6/3P3q/1BP3bP/PP3QP1/RNB1R1K1 w - - bm Qxf7+; id ""WAC.154""; d5+m2",
"5bk1/1rQ4p/5pp1/2pP4/3n1PP1/7P/1q3BB1/4R1K1 w - - bm d6; id ""WAC.155""; d5+2.5",
"r1b1qN1k/1pp3p1/p2p3n/4p1B1/8/1BP4Q/PP3KPP/8 w - - bm Qxh6+; id ""WAC.156""; d5+m2",
"5rk1/p4ppp/2p1b3/3Nq3/4P1n1/1p1B2QP/1PPr2P1/1K2R2R w - - bm Ne7+; id ""WAC.157""; d7+1.5",
"5rk1/n1p1R1bp/p2p4/1qpP1QB1/7P/2P3P1/PP3P2/6K1 w - - bm Rxg7+; id ""WAC.158""; d5+m3",
"r1b2r2/5P1p/ppn3pk/2p1p1Nq/1bP1PQ2/3P4/PB4BP/1R3RK1 w - - bm Ne6+; id ""WAC.159""; d5+6",
"qn1kr2r/1pRbb3/pP5p/P2pP1pP/3N1pQ1/3B4/3B1PP1/R5K1 w - - bm Qxd7+; id ""WAC.160""; d5+m2",
"3r3k/3r1P1p/pp1Nn3/2pp4/7Q/6R1/Pq4PP/5RK1 w - - bm Qxd8+; id ""WAC.161""; d5+m4",
"r3kbnr/p4ppp/2p1p3/8/Q1B3b1/2N1B3/PP3PqP/R3K2R w KQkq - bm Bd5; id ""WAC.162""; d5+1.2",
"5rk1/2p4p/2p4r/3P4/4p1b1/1Q2NqPp/PP3P1K/R4R2 b - - bm Qg2+; id ""WAC.163""; d12+m11",
"8/6pp/4p3/1p1n4/1NbkN1P1/P4P1P/1PR3K1/r7 w - - bm Rxc4+; id ""WAC.164""; d5+3.5",
"1r5k/p1p3pp/8/8/4p3/P1P1R3/1P1Q1qr1/2KR4 w - - bm Re2; id ""WAC.165""; d5+3.5",
"r3r1k1/5pp1/p1p4p/2Pp4/8/q1NQP1BP/5PP1/4K2R b K - bm d4; id ""WAC.166""; d5+2",
"7Q/ppp2q2/3p2k1/P2Ppr1N/1PP5/7R/5rP1/6K1 b - - bm Rxg2+; id ""WAC.167""; d5+m5",
"r3k2r/pb1q1p2/8/2p1pP2/4p1p1/B1P1Q1P1/P1P3K1/R4R2 b kq - bm Qd2+; id ""WAC.168""; d5+17 d9+m11 d11+m8",
"5rk1/1pp3bp/3p2p1/2PPp3/1P2P3/2Q1B3/4q1PP/R5K1 b - - bm Bh6; id ""WAC.169""; d7+4",
"5r1k/6Rp/1p2p3/p2pBp2/1qnP4/4P3/Q4PPP/6K1 w - - bm Qxc4; id ""WAC.170""; d5+4",
"2rq4/1b2b1kp/p3p1p1/1p1nNp2/7P/1B2B1Q1/PP3PP1/3R2K1 w - - bm Bh6+; id ""WAC.171""; d5+1.8",
"5r1k/p5pp/8/1P1pq3/P1p2nR1/Q7/5BPP/6K1 b - - bm Qe1+; id ""WAC.172""; d5+m3",
"2r1b3/1pp1qrk1/p1n1P1p1/7R/2B1p3/4Q1P1/PP3PP1/3R2K1 w - - bm Qh6+; id ""WAC.173""; d5+m3",
"2r2rk1/6p1/p3pq1p/1p1b1p2/3P1n2/PP3N2/3N1PPP/1Q2RR1K b - - bm Nxg2; id ""WAC.174""; d5+2",
"r5k1/pppb3p/2np1n2/8/3PqNpP/3Q2P1/PPP5/R4RK1 w - - bm Nh5; id ""WAC.175""; d5+1.7",
"r1bq3r/ppp2pk1/3p1pp1/8/2BbPQ2/2NP2P1/PPP4P/R4R1K b - - bm Rxh2+; id ""WAC.176""; d5+2",
"r1b3r1/4qk2/1nn1p1p1/3pPp1P/p4P2/1p3BQN/PKPBN3/3R3R b - - bm Qa3+; id ""WAC.177""; d5+m3",
"3r2k1/p1rn1p1p/1p2pp2/6q1/3PQNP1/5P2/P1P4R/R5K1 w - - bm Nxe6; id ""WAC.178""; d6+1",
"r1b2r1k/pp4pp/3p4/3B4/8/1QN3Pn/PP3q1P/R3R2K b - - bm Qg1+; id ""WAC.179""; d5+m3",
"r1q2rk1/p3bppb/3p1n1p/2nPp3/1p2P1P1/6NP/PP2QPB1/R1BNK2R b KQ - bm Nxd5; id ""WAC.180""; d5+1",
"r3k2r/2p2p2/p2p1n2/1p2p3/4P2p/1PPPPp1q/1P5P/R1N2QRK b kq - bm Ng4; id ""WAC.181""; d5+5.5",
"r1b2rk1/ppqn1p1p/2n1p1p1/2b3N1/2N5/PP1BP3/1B3PPP/R2QK2R w KQ - bm Qh5; id ""WAC.182""; d5+m7 d11+m6",
"1r2k1r1/5p2/b3p3/1p2b1B1/3p3P/3B4/PP2KP2/2R3R1 w - - bm Bf6; id ""WAC.183""; d5+1.7",
"4kn2/r4p1r/p3bQ2/q1nNP1Np/1p5P/8/PPP3P1/2KR3R w - - bm Qe7+; id ""WAC.184""; d5+m2",
"1r1rb1k1/2p3pp/p2q1p2/3PpP1Q/Pp1bP2N/1B5R/1P4PP/2B4K w - - bm Qxh7+; id ""WAC.185""; d6+4",
"r5r1/p1q2p1k/1p1R2pB/3pP3/6bQ/2p5/P1P1NPPP/6K1 w - - bm Bf8+; id ""WAC.186""; d5+m3",
"6k1/5p2/p3p3/1p3qp1/2p1Qn2/2P1R3/PP1r1PPP/4R1K1 b - - bm Nh3+; id ""WAC.187""; d5+6",
"3RNbk1/pp3p2/4rQpp/8/1qr5/7P/P4P2/3R2K1 w - - bm Qg7+; id ""WAC.188""; d5+m2",
"3r1k2/1ppPR1n1/p2p1rP1/3P3p/4Rp1N/5K2/P1P2P2/8 w - - bm Re8+; id ""WAC.189""; d6+7",
"8/p2b2kp/1q1p2p1/1P1Pp3/4P3/3B2P1/P2Q3P/2Nn3K b - - bm Bh3; id ""WAC.190""; d5+6",
"2r1Rn1k/1p1q2pp/p7/5p2/3P4/1B4P1/P1P1QP1P/6K1 w - - bm Qc4; id ""WAC.191""; d5+m3",
"r3k3/ppp2Npp/4Bn2/2b5/1n1pp3/N4P2/PPP3qP/R2QKR2 b Qq - bm Nd3+; id ""WAC.192""; d5+6",
"5bk1/p4ppp/Qp6/4B3/1P6/Pq2P1P1/2rr1P1P/R4RK1 b - - bm Qxe3; id ""WAC.193""; d6+8 d11+m8",
"5rk1/ppq2ppp/2p5/4bN2/4P3/6Q1/PPP2PPP/3R2K1 w - - bm Nh6+; id ""WAC.194""; d6+5",
"3r1rk1/1p3p2/p3pnnp/2p3p1/2P2q2/1P5P/PB2QPPN/3RR1K1 w - - bm g3; id ""WAC.195""; d5+2",
"rr4k1/p1pq2pp/Q1n1pn2/2bpp3/4P3/2PP1NN1/PP3PPP/R1B1K2R b KQ - bm Nb4; id ""WAC.196""; d8+1",
"7k/1p4p1/7p/3P1n2/4Q3/2P2P2/PP3qRP/7K b - - bm Qf1+; id ""WAC.197""; d5+m3",
"2br2k1/ppp2p1p/4p1p1/4P2q/2P1Bn2/2Q5/PP3P1P/4R1RK b - - bm Rd3; id ""WAC.198""; d5+3.5",
"r1br2k1/pp2nppp/2n5/1B1q4/Q7/4BN2/PP3PPP/2R2RK1 w - - bm Bxc6 Rcd1 Rfd1; id ""WAC.199""; Rfd1 d5+3",
"2rqrn1k/pb4pp/1p2pp2/n2P4/2P3N1/P2B2Q1/1B3PPP/2R1R1K1 w - - bm Bxf6; id ""WAC.200""; d5+2.5",
"2b2r1k/4q2p/3p2pQ/2pBp3/8/6P1/1PP2P1P/R5K1 w - - bm Ra7; id ""WAC.201""; d5+m6",
"QR2rq1k/2p3p1/3p1pPp/8/4P3/8/P1r3PP/1R4K1 b - - bm Rxa2; id ""WAC.202""; d5+0.3",
"r4rk1/5ppp/p3q1n1/2p2NQ1/4n3/P3P3/1B3PPP/1R3RK1 w - - bm Qh6; id ""WAC.203""; d5+m3",
"r1b1qrk1/1p3ppp/p1p5/3Nb3/5N2/P7/1P4PQ/K1R1R3 w - - bm Rxe5; id ""WAC.204""; d5+0.4",
"r3rnk1/1pq2bb1/p4p2/3p1Pp1/3B2P1/1NP4R/P1PQB3/2K4R w - - bm Qxg5; id ""WAC.205""; d6+3",
"1Qq5/2P1p1kp/3r1pp1/8/8/7P/p4PP1/2R3K1 b - - bm Rc6; id ""WAC.206""; d5+1.5",
"r1bq2kr/p1pp1ppp/1pn1p3/4P3/2Pb2Q1/BR6/P4PPP/3K1BNR w - - bm Qxg7+; id ""WAC.207""; d5+2",
"3r1bk1/ppq3pp/2p5/2P2Q1B/8/1P4P1/P6P/5RK1 w - - bm Bf7+; id ""WAC.208""; d5+10",
"4kb1r/2q2p2/r2p4/pppBn1B1/P6P/6Q1/1PP5/2KRR3 w k - bm Rxe5+; id ""WAC.209""; d6+8",
"3r1rk1/pp1q1ppp/3pn3/2pN4/5PP1/P5PQ/1PP1B3/1K1R4 w - - bm Rh1; id ""WAC.210""; d7+3",
"r1bqrk2/pp1n1n1p/3p1p2/P1pP1P1Q/2PpP1NP/6R1/2PB4/4RBK1 w - - bm Qxf7+; id ""WAC.211""; d5+m5",
"rn1qr2Q/pbppk1p1/1p2pb2/4N3/3P4/2N5/PPP3PP/R4RK1 w - - bm Qxg7+; id ""WAC.212""; d5+m5",
"3r1r1k/1b4pp/ppn1p3/4Pp1R/Pn5P/3P4/4QP2/1qB1NKR1 w - - bm Rxh7+ Rxg7+; id ""WAC.213""; d7+0",
"r2r2k1/1p2qpp1/1np1p1p1/p3N3/2PPN3/bP5R/4QPPP/4R1K1 w - - bm Ng5; id ""WAC.214""; d5+3",
"3r2k1/pb1q1pp1/1p2pb1p/8/3N4/P2QB3/1P3PPP/1Br1R1K1 w - - bm Qh7+; id ""WAC.215""; d5+m4",
"r2qr1k1/1b1nbppp/p3pn2/1p1pN3/3P1B2/2PB1N2/PP2QPPP/R4RK1 w - - bm Nxf7 a4; id ""WAC.216""; d5+1.5",
"r3kb1r/1pp3p1/p3bp1p/5q2/3QN3/1P6/PBP3P1/3RR1K1 w kq - bm Qd7+; id ""WAC.217""; d7+m5",
"6k1/pp5p/2p3q1/6BP/2nPr1Q1/8/PP3R1K/8 w - - bm Bh6; id ""WAC.218""; d7+m6",
"7k/p4q1p/1pb5/2p5/4B2Q/2P1B3/P6P/7K b - - bm Qf1+; id ""WAC.219""; d5+m3",
"3rr1k1/ppp2ppp/8/5Q2/4n3/1B5R/PPP1qPP1/5RK1 b - - bm Qxf1+; id ""WAC.220""; d6+6",
"r3k3/P5bp/2N1bp2/4p3/2p5/6NP/1PP2PP1/3R2K1 w q - bm Rd8+; id ""WAC.221""; d5+5",
"2r1r2k/1q3ppp/p2Rp3/2p1P3/6QB/p3P3/bP3PPP/3R2K1 w - - bm Bf6; id ""WAC.222""; d9+m6",
"r1bqk2r/pp3ppp/5n2/8/1b1npB2/2N5/PP1Q2PP/1K2RBNR w kq - bm Nxe4; id ""WAC.223""; d7+0.7",
"5rk1/p1q3pp/1p1r4/2p1pp1Q/1PPn1P2/3B3P/P2R2P1/3R2K1 b - - bm Rh6 e4; id ""WAC.224""; Rh6 d6+3 e4 d9+4",
"4R3/4q1kp/6p1/1Q3b2/1P1b1P2/6KP/8/8 b - - bm Qh4+; id ""WAC.225""; d5+m3",
"2b2rk1/p1p4p/2p1p1p1/br2N1Q1/1p2q3/8/PB3PPP/3R1RK1 w - - bm Nf7; id ""WAC.226""; d7+3.5 ",
"2k1rb1r/ppp3pp/2np1q2/5b2/2B2P2/2P1BQ2/PP1N1P1P/2KR3R b - - bm d5; id ""WAC.227""; d5+2.5",
"r4rk1/1bq1bp1p/4p1p1/p2p4/3BnP2/1N1B3R/PPP3PP/R2Q2K1 w - - bm Bxe4; id ""WAC.228""; d6+1.5",
"8/8/8/1p5r/p1p1k1pN/P2pBpP1/1P1K1P2/8 b - - bm Rxh4 b4; id ""WAC.229""; b4 d14+4.5",
"2b5/1r6/2kBp1p1/p2pP1P1/2pP4/1pP3K1/1R3P2/8 b - - bm Rb4; id ""WAC.230""; ??? Rh7d5+2.5",
"r4rk1/1b1nqp1p/p5p1/1p2PQ2/2p5/5N2/PP3PPP/R1BR2K1 w - - bm Bg5; id ""WAC.231""; d5+0.5",
"1R2rq1k/2p3p1/Q2p1pPp/8/4P3/8/P1r3PP/1R4K1 w - - bm Qb5 Rxe8; id ""WAC.232""; Rxe8 d8+3 Qb5 d14+7",
"5rk1/p1p2r1p/2pp2p1/4p3/PPPnP3/3Pq1P1/1Q1R1R1P/4NK2 b - - bm Nb3; id ""WAC.233""; d7+6",
"2kr1r2/p6p/5Pp1/2p5/1qp2Q1P/7R/PP6/1KR5 w - - bm Rb3; id ""WAC.234""; d5+5",
"5r2/1p1RRrk1/4Qq1p/1PP3p1/8/4B3/1b3P1P/6K1 w - - bm Qe4 Qxf7+ Rxf7+; id ""WAC.235""; Qe4 d5+3 Rxf7 13+5",
"1R6/p5pk/4p2p/4P3/8/2r3qP/P3R1b1/4Q1K1 b - - bm Rc1; id ""WAC.236""; d5+0.8",
"r5k1/pQp2qpp/8/4pbN1/3P4/6P1/PPr4P/1K1R3R b - - bm Rc1+; id ""WAC.237""; d12+15",
"1k1r4/pp1r1pp1/4n1p1/2R5/2Pp1qP1/3P2QP/P4PB1/1R4K1 w - - bm Bxb7; id ""WAC.238""; d5+4",
"8/6k1/5pp1/Q6p/5P2/6PK/P4q1P/8 b - - bm Qf1+; id ""WAC.239""; d10+13",
"2b4k/p1b2p2/2p2q2/3p1PNp/3P2R1/3B4/P1Q2PKP/4r3 w - - bm Qxc6; id ""WAC.240""; d6+5",
"2rq1rk1/pp3ppp/2n2b2/4NR2/3P4/PB5Q/1P4PP/3R2K1 w - - bm Qxh7+; id ""WAC.241""; d11+m6",
"r1b1r1k1/pp1nqp2/2p1p1pp/8/4N3/P1Q1P3/1P3PPP/1BRR2K1 w - - bm Rxd7; id ""WAC.242""; d5+3.5",
"1r3r1k/3p4/1p1Nn1R1/4Pp1q/pP3P1p/P7/5Q1P/6RK w - - bm Qe2; id ""WAC.243""; d7+1.5",
"r6r/pp3ppp/3k1b2/2pb4/B4Pq1/2P1Q3/P5PP/1RBR2K1 w - - bm Qxc5+; id ""WAC.244""; d5+m4",
"4rrn1/ppq3bk/3pPnpp/2p5/2PB4/2NQ1RPB/PP5P/5R1K w - - bm Qxg6+; id ""WAC.245""; d7+4",
"6R1/4qp1p/ppr1n1pk/8/1P2P1QP/6N1/P4PP1/6K1 w - - bm Qh5+; id ""WAC.246""; d5+m2",
"2k1r3/1p2Bq2/p2Qp3/Pb1p1p1P/2pP1P2/2P5/2P2KP1/1R6 w - - bm Rxb5; id ""WAC.247""; d8+7",
"5r1k/1p4pp/3q4/3Pp1R1/8/8/PP4PP/4Q1K1 b - - bm Qc5+; id ""WAC.248""; d6+0.5",
"r4rk1/pbq2pp1/1ppbpn1p/8/2PP4/1P1Q1N2/PBB2PPP/R3R1K1 w - - bm c5 d5; id ""WAC.249""; d5 d5+9 c5 d10+1.7", 
"1b5k/7P/p1p2np1/2P2p2/PP3P2/4RQ1R/q2r3P/6K1 w - - bm Re8+; id ""WAC.250""; d5+m8",
"k7/p4p2/P1q1b1p1/3p3p/3Q4/7P/5PP1/1R4K1 w - - bm Qe5 Qf4; id ""WAC.251""; d8+m6",
"1rb1r1k1/p1p2ppp/5n2/2pP4/5P2/2QB4/qNP3PP/2KRB2R b - - bm Bg4 Re2; c0 ""Bg4 wins, but Re2 is far better.""; id ""WAC.252""; Bg4 +3 Re2 d7+m7 d8+m5",
"k5r1/p4b2/2P5/5p2/3P1P2/4QBrq/P5P1/4R1K1 w - - bm Qe8+; id ""WAC.253""; d5+m4",
"r6k/pp3p1p/2p1bp1q/b3p3/4Pnr1/2PP2NP/PP1Q1PPN/R2B2RK b - - bm Nxh3; id ""WAC.254""; d5+2.5",
"3r3r/p4pk1/5Rp1/3q4/1p1P2RQ/5N2/P1P4P/2b4K w - - bm Rfxg6+; id ""WAC.255""; d5+4",
"3r1rk1/1pb1qp1p/2p3p1/p7/P2Np2R/1P5P/1BP2PP1/3Q1BK1 w - - bm Nf5; id ""WAC.256""; d9+3",
"4r1k1/pq3p1p/2p1r1p1/2Q1p3/3nN1P1/1P6/P1P2P1P/3RR1K1 w - - bm Rxd4; id ""WAC.257""; d5+3",
"r3brkn/1p5p/2p2Ppq/2Pp3B/3Pp2Q/4P1R1/6PP/5R1K w - - bm Bxg6; id ""WAC.258""; d5+0.2",
"r1bq1rk1/ppp2ppp/2np4/2bN1PN1/2B1P3/3p4/PPP2nPP/R1BQ1K1R w - - bm Qh5; id ""WAC.259""; d5-1.5",
"2r2b1r/p1Nk2pp/3p1p2/N2Qn3/4P3/q6P/P4PP1/1R3K1R w - - bm Qe6+; id ""WAC.260""; d5+m5",
"r5k1/1bp3pp/p2p4/1p6/5p2/1PBP1nqP/1PP3Q1/R4R1K b - - bm Nd4; id ""WAC.261""; d6+9",
"6k1/p1B1b2p/2b3r1/2p5/4p3/1PP1N1Pq/P2R1P2/3Q2K1 b - - bm Rh6; id ""WAC.262""; d5+1.2",
"rnbqr2k/pppp1Qpp/8/b2NN3/2B1n3/8/PPPP1PPP/R1B1K2R w KQ - bm Qg8+; id ""WAC.263""; d6+m4",
"r2r2k1/1R2qp2/p5pp/2P5/b1PN1b2/P7/1Q3PPP/1B1R2K1 b - - bm Qe5 Rab8; id ""WAC.264""; Rab8 d5+2",
"2r1k2r/2pn1pp1/1p3n1p/p3PP2/4q2B/P1P5/2Q1N1PP/R4RK1 w k - bm exf6; id ""WAC.265""; d10+2",
"r3q2r/2p1k1p1/p5p1/1p2Nb2/1P2nB2/P7/2PNQbPP/R2R3K b - - bm Rxh2+; id ""WAC.266""; d5+m6",
"2r1kb1r/pp3ppp/2n1b3/1q1N2B1/1P2Q3/8/P4PPP/3RK1NR w Kk - bm Nc7+; id ""WAC.267""; d5+m5",
"2r3kr/ppp2n1p/7B/5q1N/1bp5/2Pp4/PP2RPPP/R2Q2K1 w - - bm Re8+; id ""WAC.268""; d5+11",
"2kr2nr/pp1n1ppp/2p1p3/q7/1b1P1B2/P1N2Q1P/1PP1BPP1/R3K2R w KQ - bm axb4; id ""WAC.269""; d8+3",
"2r1r1k1/pp1q1ppp/3p1b2/3P4/3Q4/5N2/PP2RPPP/4R1K1 w - - bm Qg4; id ""WAC.270""; d8+0.7",
"2kr4/ppp3Pp/4RP1B/2r5/5P2/1P6/P2p4/3K4 w - - bm Rd6; id ""WAC.271""; d6+0",
"nrq4r/2k1p3/1p1pPnp1/pRpP1p2/P1P2P2/2P1BB2/1R2Q1P1/6K1 w - - bm Bxc5; id ""WAC.272""; d5+2",
"2k4B/bpp1qp2/p1b5/7p/1PN1n1p1/2Pr4/P5PP/R3QR1K b - - bm Ng3+ g3; id ""WAC.273""; Ng3+ d6+6",
"8/1p6/p5R1/k7/Prpp4/K7/1NP5/8 w - - am Rd6; bm Rb6 Rg5+; id ""WAC.274""; Rb6 d6+2.5",
"r1b2rk1/1p1n1ppp/p1p2q2/4p3/P1B1Pn2/1QN2N2/1P3PPP/3R1RK1 b - - bm Nc5 Nxg2 b5; id ""WAC.275""; b5 d5+1.3",
"r5k1/pp1RR1pp/1b6/6r1/2p5/B6P/P4qPK/3Q4 w - - bm Qd5+; id ""WAC.276""; d5+9",
"1r4r1/p2kb2p/bq2p3/3p1p2/5P2/2BB3Q/PP4PP/3RKR2 b - - bm Rg3 Rxg2; id ""WAC.277""; Rg3 d5+1.3",
"r2qkb1r/pppb2pp/2np1n2/5pN1/2BQP3/2N5/PPP2PPP/R1B1K2R w KQkq - bm Bf7+; id ""WAC.278""; d5+m5",
"r7/4b3/2p1r1k1/1p1pPp1q/1P1P1P1p/PR2NRpP/2Q3K1/8 w - - bm Nxf5 Rc3; id ""WAC.279""; d5+1.6",
"r1r2bk1/5p1p/pn4p1/N2b4/3Pp3/B3P3/2q1BPPP/RQ3RK1 b - - bm Bxa3; id ""WAC.280""; d6+2",
"2R5/2R4p/5p1k/6n1/8/1P2QPPq/r7/6K1 w - - bm Rxh7+; id ""WAC.281""; d5+6",
"6k1/2p3p1/1p1p1nN1/1B1P4/4PK2/8/2r3b1/7R w - - bm Rh8+; id ""WAC.282""; d5+m4",
"3q1rk1/4bp1p/1n2P2Q/3p1p2/6r1/Pp2R2N/1B4PP/7K w - - bm Ng5; id ""WAC.283""; d5+m4",
"3r3k/pp4pp/8/1P6/3N4/Pn2P1qb/1B1Q2B1/2R3K1 w - - bm Nf5; id ""WAC.284""; d6+3.5",
"2rr3k/1b2bppP/p2p1n2/R7/3P4/1qB2P2/1P4Q1/1K5R w - - bm Qxg7+; id ""WAC.285""; d5+m5",
"3r1k2/1p6/p4P2/2pP2Qb/8/1P1KB3/P6r/8 b - - bm Rxd5+; id ""WAC.286""; d5+3",
"rn3k1r/pp2bBpp/2p2n2/q5N1/3P4/1P6/P1P3PP/R1BQ1RK1 w - - bm Qg4 Qh5; id ""WAC.287""; Qh5 d5+4",
"r1b2rk1/p4ppp/1p1Qp3/4P2N/1P6/8/P3qPPP/3R1RK1 w - - bm Nf6+; id ""WAC.288""; d5+3.5",
"2r3k1/5p1p/p3q1p1/2n3P1/1p1QP2P/1P4N1/PK6/2R5 b - - bm Qe5; id ""WAC.289""; d5+3.5",
"2k2r2/2p5/1pq5/p1p1n3/P1P2n1B/1R4Pp/2QR4/6K1 b - - bm Ne2+; id ""WAC.290""; d5+m4",
"5r1k/3b2p1/p6p/1pRpR3/1P1P2q1/P4pP1/5QnP/1B4K1 w - - bm h3; id ""WAC.291""; d7+3",
"4r3/1Q1qk2p/p4pp1/3Pb3/P7/6PP/5P2/4R1K1 w - - bm d6+; id ""WAC.292""; d5+2.5",
"1nbq1r1k/3rbp1p/p1p1pp1Q/1p6/P1pPN3/5NP1/1P2PPBP/R4RK1 w - - bm Nfg5; id ""WAC.293""; d9+m4",
"3r3k/1r3p1p/p1pB1p2/8/p1qNP1Q1/P6P/1P4P1/3R3K w - - bm Bf8 Nf5 Qf4; id ""WAC.294""; Qf4 d5+4 Bf8 d6+7",
"4r3/p4r1p/R1p2pp1/1p1bk3/4pNPP/2P1K3/2P2P2/3R4 w - - bm Rxd5+; id ""WAC.295""; d5+m3",
"3r4/1p2k2p/p1b1p1p1/4Q1Pn/2B3KP/4pP2/PP2R1N1/6q1 b - - bm Rd4+ Rf8; id ""WAC.296""; Rf8 d5+5 Rd4+ d9+9",
"3r1rk1/p3qp1p/2bb2p1/2p5/3P4/1P6/PBQN1PPP/2R2RK1 b - - bm Bxg2 Bxh2+; id ""WAC.297""; Bxg2 d8+1.6",
"3Q4/p3b1k1/2p2rPp/2q5/4B3/P2P4/7P/6RK w - - bm Qh8+; id ""WAC.298""; d6+m4",
"1n2rr2/1pk3pp/pNn2p2/2N1p3/8/6P1/PP2PPKP/2RR4 w - - bm Nca4; id ""WAC.299""; d5+5",
"b2b1r1k/3R1ppp/4qP2/4p1PQ/4P3/5B2/4N1K1/8 w - - bm g6; id ""WAC.300""; d5+2"
  };

  
  //  s="r3r1k1/1pq2pp1/2p2n2/1PNn4/2QN2b1/6P1/3RPP2/2R3KB b - - "; // bm Re3; id "WMT 01"; c0 "...Te8-e3 (K)"; -6Fto
  //  s="r1q2rk1/p2bb2p/1p1p2p1/2pPp2n/2P1PpP1/3B1P2/PP2QR1P/R1B2NK1 b - -"; //  bm Bxg4; id "WMT 02"; c0 "...Ld7xg4 (K)"; -7F
  //  s="3r1r2/pp1q2bk/2n1nppp/2p5/3pP1P1/P2P1NNQ/1PPB3P/1R3R1K w - -"; //  bm Nf5; id "WMT 03"; c0 "...Sg3-f5 (K)"; -6F
  //  s="2rr2k1/pp2b1pp/1nb1p3/5p2/1P1B4/P2BPP2/3NK1PP/R6R b - -"; //  bm e5; id "WMT 04"; c0 "...e6-e5 (K)"; -6F
  //  s="rnbqnrk1/5ppp/pp1b4/3NpP2/P2N4/8/1PP1B1PP/R1BQ1R1K w - -"; //  bm f6; id "WMT 05"; c0 "f5-f6 (K)"; -6F
  //  s="r3k1nr/pp1b1ppp/2n1p3/1B4B1/3N4/2q5/P1P2PPP/R2Q1RK1 w kq -"; //  bm Nf5; id "WMT 06"; c0 "Sd4-f5 (K)"; -6F
  //  s="r4k1r/pp2pp1p/8/2PPb3/Q7/4p3/B2q1PPP/2R2RK1 w - -"; //  bm Rcd1; id "WMT 07"; c0 "Tc1-d1 (K)"; -6F
  //  s="r2qrbk1/5ppp/pn1p4/np2P1P1/3p4/5N2/PPB2PP1/R1BQR1K1 w - -"; //  bm Bxh7+; id "WMT 08"; c0 "Lc2-h7+ (K)";
  //  s="rnbr2k1/pp3p2/1qp3pp/4Q1N1/4P3/2P3P1/PP3PBP/R3R1K1 w - -"; //  bm Nxf7; id "WMT 09"; c0 "Sg5xf7 (K)";
  //  s="r1bqr1k1/5p2/p2p1npb/1p1Pp2p/4P1P1/1PP1NPP1/2BBQ1K1/5R1R w - -"; //  bm Nf5; id "WMT 10"; c0 "Se3-f5 (K)";
  //  s="r1b2rk1/p3bp1p/4pp2/n3p3/1p1qNP2/1B4N1/PPP3PP/R2Q1R1K w - -"; //  bm Qh5; id "WMT 11"; c0 "Dd1-h5 (K)";
  //  s="3q3r/4b1k1/4P1p1/rpp2p1p/1nn2Q2/5N1P/P4PP1/RBB1R1K1 w - -"; //  bm Bxf5; id "WMT 12"; c0 "Lb1xf5 (K)";
  //  s="1rb1r1k1/p1p1qppp/2pb4/8/2P3n1/4P1P1/PB2BP1P/R1QN1RK1 b - -"; //  bm Nxh2; id "WMT 13"; c0 "...Sg4xh2 (K)";
  //  s="q4bk1/pp1b1r1p/3p2n1/P2Pp3/1PB1Pp2/5Pn1/5RPP/2RQN1K1 b - -"; //  bm a6; id "WMT 14"; c0 "...a7-a6 (K)";
  //  s="b2r3r/k4p1p/p2q1np1/NppP4/3p1Q2/P4PPB/1PP4P/1K1RR3 w - -"; //  bm Rxd4; id "WMT 15"; c0 "Td1xd4 (K)";
  //  s="r1k4r/1b1q1pp1/p6p/3n4/1B3Pn1/1P1B4/P1PQ3P/2KRR3 w - -"; //  bm Ba5; id "WMT 16"; c0 "Lb4-a5 (K)";
  //  s="6k1/p4p1p/1p3np1/8/3pr3/4P2q/PP3PRP/2RQ2K1 w - -"; //  bm Rc4; id "WMT 17"; c0 "Tc1-c4 (P) +";
  //  s="r4rk1/2qbbppp/1pp1pn2/p7/1PNP4/P1N1P1P1/2Q2PBP/R4RK1 w - -"; //  bm Ne5; id "WMT 18"; c0 "Sc4-e5 (P)";
  //  s="r3k2r/pp3ppp/2n2n2/q2p4/1b4b1/P1N2N2/1P2BPPP/R1BQ1RK1 w kq -"; //  bm axb4; id "WMT 19"; c0 "a3xb4 (P)";
  //  s="Q3b3/2q1k1p1/5p1p/1pP1p3/nPnp4/3N2P1/4PPBP/B5K1 w - -"; //  bm f4; id "WMT 20"; c0 "f2-f4 (P)";
  //  s="r4rk1/1p1nbppp/p2p1n2/Pq1Pp3/8/1N2BB2/1PP2PPP/R2Q1RK1 w - -"; //  bm Qd3; id "WMT 21"; c0 "Dd1-d3 (P) +++";
  //  s="3q1rk1/1r3ppp/pp2Pb2/2p2Q2/3P4/6P1/PP3P1P/R1BR2K1 b - -"; //  bm Re7; id "WMT 22"; c0 "...Tb7-e7 (P)";
  //  s="r3r1k1/1bnq1pbn/p2p2p1/1p1P3p/2p1PP1B/P1N2B1P/1PQN2P1/3RR1K1 w - -"; //  bm e5; id "WMT 23"; c0 "e4-e5 (P)";
  //  s="2b2r1k/5qb1/2Np3p/2pNpn2/2P5/3PBQPp/P4P1P/5RK1 b - -"; //  bm Qe8; id "WMT 24"; c0 "...Df7-e8 (P)";
  //  s="r2q1rk1/pp3p1p/3p2p1/4P3/2PQP1b1/2N5/PP4PP/R3K2R b KQ -"; //  bm Qh4+; id "WMT 25"; c0 "...Dd8-h4+ (P)";
  //  s="4k2r/1Q1nq1pp/5n2/5R2/4P3/8/PPr3PP/R1B3K1 w k -"; //  bm Bg5; id "WMT 26"; c0 "Lc1-g5 (P)";
  //  s="3r2k1/p3r2p/1pbpnbp1/2p1pp2/1PP4q/PBB1PP2/2QP2PP/3RNRK1 b - -"; //  bm Nd4; id "WMT 27"; c0 "...Se6-d4 (P)";
  //  s="2q2r1k/4pp2/3p1np1/3P1P2/pP2P3/2p1N3/5PQP/2R3K1 b - -"; //  bm a3; id "WMT 28"; c0 "...a4-a3";
  //  s="5rk1/1p1bbr1p/1qp1p1p1/p1pnP3/4NP1P/6P1/PPQB2BK/4RR2 w - -"; //  bm Be3; id "WMT 29"; c0 "Ld2-e3 (P)";
  //s="1q2k2r/1b1n2pp/3bNn2/8/1p2P3/5N2/1P2QPPP/2B2RK1 b k -"; //  bm h6; id "WMT 30"; c0 "...h7-h6 (P)";
  //s="3q1rk1/r3bppp/p4n2/np1p2B1/7Q/5N2/PPB2PPP/3R1RK1 b - -"; //  bm h5; id "WMT 31"; c0 "...h7-h5 (P)";
  //s="2r1r1k1/p2q2pn/2bn1p1p/P2p4/2pP2N1/2Q1PP1P/2B2BP1/R3R1K1 w - -"; //  bm e4; id "WMT 32"; c0 "e3-e4 (P)";
  //s="8/8/4b3/2Bp2Pp/7P/1pK1Pk2/8/8 b - -"; //  bm d4+; id "WMT 33"; c0 "...d5-d4+ (E)";
  //s="8/pp4pp/4k3/3rPp2/1Pr4P/2B1KPP1/1P6/4R3 b - -"; //  bm Rxc3+; id "WMT 34"; c0 "...Tc4xc3+ (E)";
  //s="1r4k1/5p2/1p3npp/pR6/P1Pr3P/2R2B2/1P4P1/7K w - -"; //  bm c5; id "WMT 35"; c0 "c4-c5 (E)";
  //s="2r2k2/4pp1p/P2p2p1/1prn4/3R1PPP/PK2R3/2P5/5B2 w - -"; //  bm Rxd5; id "WMT 36"; c0 "Td4xd5 (E)";
  //s="4kb2/8/2P4p/1p2B1pP/8/6p1/3K2P1/8 w - -"; //  bm Bf6; id "WMT 37"; c0 "Le5-f6 (E)";
  //s="8/1k4n1/1p1p3p/2pPp1pP/1PP1P1P1/1K2N3/8/8 w - -"; //  bm Ka4; id "WMT 38"; c0 "Kb3-a4 (E)";
  //s="8/1k4n1/3p3p/K1pPp1pP/2P1P1P1/4N3/8/8 w - -"; //  bm Kb5; id "WMT 39"; c0 "Ka5-b5 (E)";
  //s="3Rnk1r/5ppp/4p3/8/PN6/7P/1P2bPP1/6K1 w - -"; //  bm f3; id "WMT 40"; c0 "f2-f3 (E)";
  //s="r3k2r/pp1qn1pp/2p2p2/8/3P4/5N2/PP2QPPP/2R1R1K1 w kq -"; //  bm d5; id "WMT 41"; c0 "d4-d5 (K)";
  //s="r1b1kb1r/1p4pp/p2ppn2/8/2qNP3/2N1B3/PPP3PP/R2Q1RK1 w kq -"; //  bm Rxf6; id "WMT 42"; c0 "Tf1xf6 (K)";
  //s="5k2/p1p4R/1pr5/3p1pP1/P2P1P2/2P2K2/8/8 w - -"; //  bm Kg3; id "WMT 43"; c0 "Kf3-g3 (E)";
  //s="r1bk1n1r/pp1n1q1p/2p2p1R/3p4/3PpN2/2NB2Q1/PPP2PP1/2K1R3 w - -"; //  bm Bxe4; id "WMT 44"; c0 "Ld3xe4 (K)";
  //s="r1b1n2r/1p2q3/1Qp1npk1/4p1p1/P1B1P3/2P1BNP1/1P3PK1/R3R3 b - -"; //  bm Nf4+; id "WMT 45"; c0 "...Se6-f4+ (K)";
  //s="1nr3k1/p4pbp/1n1Rp1p1/2p5/4bB2/P4NPB/1P3P1P/R5K1 w - -"; //  bm Ng5; id "WMT 46"; c0 "Sf3-g5 (P)";
  //s="2rq1rk1/pb3p1p/1p2pbp1/3p3R/2PP4/PP5R/1B3PPP/3Q1BK1 b - -"; //  bm dxc4; id "WMT 47"; c0 "...d5xc4 (P)";
  //s="6k1/rpr1ppb1/1Q1p2p1/P2P1q1p/8/2P2nPP/1P3P2/R2R1B1K b - -"; //  bm Rc4; id "WMT 48"; c0 "...Tc7-c4 (K)";
  //s="2r1nrk1/pbqn1p1p/1p6/2p1pP2/2Pp3P/1P1Pb1P1/PB2Q1BK/RN2NR2 b - -"; //  bm e4; id "WMT 49"; c0 "...e5-e4 (P)";
  //s="1qr2rk1/1p3pp1/7p/3p4/1P1N2Q1/1P6/PK1n2PP/2R4R w - -"; //  bm Qg3; id "WMT 50"; c0 "Dg4-g3 (P)";
  //s="2r5/R6p/5p2/1k2p1p1/1b1pP1P1/3K1P2/4N2P/8 b - -"; //  bm Ba5; id "WMT 51"; c0 "...Lb4-a5 (E)";
  //s="8/4kpbn/p1p3p1/Pp2p2p/1P2Pn2/N1P1BP2/5P1P/5BK1 w - -"; //  bm Nxb5; id "WMT 52"; c0 "Sa3xb5 (E)";
  //s="r3rnk1/3qbppp/1p2p1n1/pPppP2N/3P3P/2P2NP1/1P3PK1/R1BQR3 w - -"; //  bm Bh6; id "WMT 53"; c0 "Lc1-h6 (K)";
  //s="r1b2rk1/p2nbqpp/p3p3/2ppPpB1/N2P1N1P/8/PPP2PP1/R2Q1RK1 w - -"; //  bm c4; id "WMT 54"; c0 "c2-c4 (P)";
  //s="5b2/p2k1p2/P3pP1p/n2pP1p1/1p1P2P1/1P1KBN2/7P/8 w - -"; //  bm Nxg5; id "WMT 55"; c0 "Sf3xg5 (E)";
  //s="3r2k1/1r2bpp1/p2pbn1p/8/qp1BPQ2/1N1B4/PPP3PP/1KR1R3 b - -"; //  bm Rb5; id "WMT 56"; c0 "...Tb7-b5 (K)";
  //s="r1b1r1k1/ppp2ppp/2nb1q2/8/2B5/1P1Q1N2/P1PP1PPP/R1B2RK1 w - -"; //  bm Bb2; id "WMT 57"; c0 "Lc1-b2 (K)";
  //s="2b1rb1k/2r2ppp/n2p4/3P2PN/3NPQ2/2p3RP/1q3PK1/1B1R4 w - -"; //  bm Nf6; id "WMT 58"; c0 "Sh5-f6 (K)";
  //s="1r1qk2r/pp1n2pp/3bp2n/3p1p1b/3P4/1Q3NPP/PP1NPPB1/R1B1R1K1 w k -"; //  bm e4; id "WMT 59"; c0 "e2-e4 (K)";
  //s="r3r1k1/pp1bp2p/1n2q1P1/6b1/1B2B3/5Q2/5PPP/1R3RK1 w - -"; //  bm Bd2; id "WMT 60"; c0 "Lb4-d2 (P)";
  //s="rn3rk1/pbq2ppp/1p2pn2/8/2BP4/2P2N2/PB2QPPP/R2R2K1 b - -"; //  bm Bxf3; id "WMT 61"; c0 "...Lb7xf3 (P)";
  //s="2k5/p1p5/1pbppR2/7p/2PP3P/2P1K1r1/P3B1P1/8 w - -"; //  bm Kf4; id "WMT 62"; c0 "Ke3-f4 (E)";
  //s="2k5/2p3Rp/p1p5/1p2p3/1b2P3/1N1P1P2/PP2KP1r/8 w - -"; //  bm a3; id "WMT 63"; c0 "a2-a3 (E)";
  //s="2r3k1/2r1ppb1/3p2p1/pq4Pn/1p1BP3/1B3P2/PPPQ4/1K1R3R w - -"; //  bm Rxh5; id "WMT 64"; c0 "Th1xh5 (K)";
  //s="1rb2rk1/p3qp2/2p2n1b/1p2p3/n3P1pP/3B2B1/PPP2QPN/1KNR3R w - -"; //  bm h5; id "WMT 65"; c0 "h4-h5 (K)";
  //s="r3kb1r/1b1q1pp1/p3p2p/3n4/Np1N1Pn1/1P1BB3/P1PQ3P/2KRR3 w kq -"; //  bm Nb6; id "WMT 66"; c0 "Sa4-b6 (K)";
  //s="r1b2b1r/p1q2k1p/4ppB1/3p3Q/3p1P2/2P5/PP4PP/R1B1K2R b KQ -"; //  bm hxg6; id "WMT 67"; c0 "...h7xg6 (P)";
  //s="3r2k1/4b1pp/p3p1r1/1ppqB3/4R3/6P1/PPP1QP1P/4R1K1 w - -"; //  bm Bf4; id "WMT 68"; c0 "Le5-f4 (P)";
  //s="4k3/7p/1pr1np2/p1p1pNpP/P1P1K1P1/1P2P3/3R1P2/8 w - -"; //  bm Rd6; id "WMT 69"; c0 "Td2-d6 (E)";
  //s="r1r5/1Rp2k1p/2p1b1pB/p1b1Pp2/P1P5/2P5/4B1PP/1R5K b - -"; //  bm Ra6; id "WMT 70"; c0 "...Ta8-a6 (E)";
  //s="r4rk1/pp3ppp/1bbp1n2/q3pP2/4P3/1BN5/PPP1QBPP/3R1RK1 w - -"; //  bm g4; id "WMT 71"; c0 "g2-g4 (K)";
  //s="r1b1rk2/2qp1pp1/2p2n1p/p3pP2/1bB1P3/2N3R1/PPPBQ1PP/R6K w - -"; //  bm Qe3; id "WMT 72"; c0 "De2-e3 (K)";
  //s="r3k2r/p1qnbpp1/1pb1p2p/2p5/3PP3/P1PB1N2/2Q2PPP/R1BR2K1 w kq -"; //  bm d5; id "WMT 73"; c0 "d4-d5 (K)";
  //s="1nbqkbr1/r4p1p/p2p4/5B2/Np6/8/PPP3PP/R1BQ1RK1 b - -"; //  bm Re7; id "WMT 74"; c0 "...Ta7-e7 (P)";
  //s="r2q1rk1/1p1n2np/p2p2pB/2pPp3/P1P1Pp1P/5QP1/1PB2P2/R3R1K1 w - -"; //  bm g4; id "WMT 75"; c0 "g3-g4 (P)";
  //s="3r4/p1k3rp/b4p2/2Bpn3/2p5/2P2PP1/P2R1K1P/3R1B2 b - -"; //  bm Nd3+; id "WMT 76"; c0 "...Se5-d3+ (P)";
  //s="2k5/2b5/pp6/4p3/1P4P1/1KP2p2/1P1R4/8 w - -"; //  bm Rd5; id "WMT 77"; c0 "Td2-d5 (E)";
  //s="2n5/5k1p/1n1p2pP/2pPp3/2P1P1B1/1K3P2/3B4/8 w - -"; //  bm Bxc8; id "WMT 78"; c0 "Lg4xc8 (E)";
  //s="3r4/p1k3pp/1p3p2/2p1p2P/P1B3r1/1K6/1P4P1/2R4R w - -"; //  bm h6; id "WMT 79"; c0 "h5-h6 (E)";
  //s="8/8/p2k1p2/1p1p3p/1P1P3p/P3NPP1/5K2/1b6 w - -"; //  bm Ng2; id "WMT 80"; c0 "Se3-g2 (E)";
  //s="r3q3/1ppr4/p2b1k1p/5P1Q/4P3/8/PP4PP/R4RK1 w - -"; //  bm e5+; id "WMT 81"; c0 "e4-e5+ (K)";
  //s="r1bq1rk1/pp3pbp/n3p3/3n4/6p1/PNNQB3/1PP3BP/2KR3R w - -"; //   bm h3; id "WMT 82"; c0 "h2-h3 (K)";
  //s="r3qrk1/2p1bppp/p3n3/1p2P3/4N3/4BQ2/PP3PPP/R2R2K1 w - -"; //  bm Nf6+; id "WMT 83"; c0 "Se4-f6+ (K)";
  //s="3q2k1/2r2pp1/BQ3bb1/p7/PP1N4/3P2PP/1B2P2K/8 b - -"; //  bm Rd7; id "WMT 84"; c0 "...Tc7-d7 (P)";
  //s="3rk2r/5ppp/3qpn2/p1p3Q1/1pPP4/3B2PP/PP3P2/1K1R3R b k -"; //  bm h6; id "WMT 85"; c0 "...h7-h6 (P)";
  //s="r1b1r1k1/1p4bp/6p1/8/1p1qp3/P5P1/1P2PPBP/RQ1NK2R b KQ -"; //  bm Bg4; id "WMT 86"; c0 "...Lc8-g4 (P)";
  //s="r2q2k1/4p1b1/pnn2rp1/1p1b2N1/2pPN1P1/4BP2/PP6/R2QKB1R b KQ -"; //  bm e5; id "WMT 87"; c0 "...e7-e5 (P)";
  //s="3rn3/1pp2k2/pn4p1/3P1pP1/3B1P2/1P3B2/P7/2R2K2 w - -"; //  bm d6; id "WMT 88"; c0 "d5-d6 (E)";
  //s="8/8/1p2k1n1/p1p2pBp/P1P5/1P4P1/3K4/8 b - -"; //  bm f4; id "WMT 89"; c0 "...f5-f4 (E)";
  //s="2B1k3/1b2r2p/1P3pp1/2R1p3/8/4P1P1/7P/6K1 w - -"; //  bm g4; id "WMT 90"; c0 "g3-g4 (E)";
  //s="1r3bk1/5p2/b5pB/2nP4/2p3N1/N2n2RP/1q3PP1/1B1Q2K1 w - -"; //  bm Qf3; id "WMT 91"; c0 "Dd1-f3 (K)";
  //s="r3qb1k/1b4p1/p2pr2p/3n4/Pnp1N1N1/6RP/1B3PP1/1B1QR1K1 w - -"; //  bm Nxh6; id "WMT 92"; c0 "Sg4xh6 (K)";
  //s="brq2rk1/3p2pp/1pnPp2n/5pNQ/2P2P2/3B3P/2R3PK/2B2R2 w - -"; //  bm Bb2; id "WMT 93"; c0 "Lc1-b2 (K)";
  //s="r3r3/2p2p1k/1p3Pp1/p2b4/Pn1R2N1/1P4NP/5RP1/6K1 w - -"; //  bm Nh6; id "WMT 94"; c0 "Sg4-h6 (P)";
  //s="r4rk1/3q2bp/n1bp1pp1/p3pPP1/PpPpP2P/1P1P2N1/6B1/R1BQ1R1K w - -"; //  bm Ra2; id "WMT 95"; c0 "Ta1-a2 (P)";
  //s="1r3rk1/p1pn1ppp/2p1pq2/8/3P3P/8/PPPQ1PP1/2KR1B1R w - -"; //  bm Rh3; id "WMT 96"; c0 "Th1-h3 (P)";
  //s="5r1k/1p2r1b1/p2p1q1p/P2B1P1R/2P1p2R/4Q2K/7P/8 w - -"; //  bm Rg4; id "WMT 97"; c0 "Th4-g4 (P)";
  //s="8/1b6/pp3kp1/7p/2P4P/1P2rP2/P5B1/3R1K2 b - -"; //  bm Bxf3; id "WMT 98"; c0 "...Lb7xf3 (E)";
  //s="8/2k1n3/1p4p1/pKp2p1p/P4P1P/2P2BP1/1P6/8 w - -"; //  bm Ka6; id "WMT 99"; c0 "Kb5-a6 (E)";
  //s="3r1k2/R4pp1/7p/8/8/7P/P3RPPK/3r4 b - -"; //  bm R1d2; id "WMT 100"; c0 "...Td1-d2 (E)";


  //s="1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - -"; //   bm Qd1+; id "BK.01"; +4F 3
  //  s="3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5 w - -"; //   bm d5; id "BK.02"; -7F
  //s="2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R b - -"; //   bm f5; id "BK.03"; +4F
  //s="rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R w KQkq -"; //   bm e6; id "BK.04";
  //s="r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1 w - -"; //   bm Nd5 a4; id "BK.05";
  //s="2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8 w - -"; //   bm g6; id "BK.06";
  //s="1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1 w - -"; //   bm Nf6; id "BK.07";
  //s="4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8 w - -"; //   bm f5; id "BK.08";
  //s="2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R w - -"; //   bm f5; id "BK.09";
  //s="3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1 b - -"; //   bm Ne5; id "BK.10";
  //s="2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1 w - -"; //   bm f4; id "BK.11";
  //s="r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1 b - -"; //   bm Bf5; id "BK.12";
  //s="r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1 w - -"; //   bm b4; id "BK.13";
  //s="rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1 w - -"; //   bm Qd2 Qe1; id "BK.14";
  //s="2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1 w - -"; //   bm Qxg7+; id "BK.15";
  //s="r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1 w kq -"; //   bm Ne4; id "BK.16";
  //s="r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1 b - -"; //   bm h5; id "BK.17";
  //s="r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1 b - -"; //   bm Nb3; id "BK.18";
  //s="3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1 b - -"; //   bm Rxe4; id "BK.19";
  //s="r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3 w - -"; //   bm g4; id "BK.20";
  //s="3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1 w - -"; //   bm Nh6; id "BK.21";
  //s="2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1 b - -"; //   bm Bxe4; id "BK.22";
  //s="r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1 b kq -"; //   bm f6; id "BK.23";
  //s="r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1 w - -"; //   bm f4; id "BK.24";
  

  //s="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";  // 
  //s="rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq -";  //e2-e4    
  //s="r1bqk1nr/pppp1ppp/2n5/2b1p3/2B1P3/5Q2/PPPP1PPP/RNB1K1NR w KQkq -"; //   
  //s="r1b1k1nr/pppp1ppp/2n2q2/2b1p3/2B1P3/2NP4/PPP2PPP/R1BQK1NR b KQkq -"; //   
  
  halt=0;

  if (numwac!=0) {
   String s;
   s=wacs[numwac-1];   
   fen(s);     
   show_position();    
   solvefen(s);
   return;     
  }

  int fenshard[22]={2,86,87,100,46,71,92,163,195,196,210,216,222,230,234,237,241,243,248,269,270,297}; 
  unsigned long sta=millis();
  int solvedpos=0; int allpos=0;      
  for (int i=0;i<300;i++) {        
   String s;
   s=wacs[i];   
   //s="r2q1rk1/pbp1bppp/1p2pn2/6B1/3P4/3B1N2/PPP1QPPP/R4RK1 w  -"; //23
   //s="rr4k1/1q3ppp/1bQRp3/6P1/4P3/P4N2/1P6/K3R3 w  -"; //24
   //s="8/2P5/3K4/5b2/1p6/6k1/8/8 w -"; //30
   //s=wacs[fenshard[i]-1];   
   if (solvefen(s)) solvedpos++;
   allpos++;   
   Serial.println(String(solvedpos)+"/"+String(allpos)+"  "+get_time((millis()-sta)/1000));     
   Serial.println();    
   delay(100);
   if (halt) break;
   //break;
  }       
  Serial.println("Solved= "+String(solvedpos)+"/"+String(allpos)+"  Time= "+get_time((millis()-sta)/1000));       
  
  //delay(6000000);   
  
}
//****************************
int solvefen(String s) {  
  fen(s);     
  Serial.println(s);     
  epd();   
  int ret=solve_step();
  if (ret) Serial.print("  +++++  "); else Serial.print("  -----  ");  
  Serial.println();   
  return ret;
}
//****************************
void game() {  
String s="";   
boolean gameover=0;  
  fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
  show_position();       
  game_ply=0;
  game_pos=pos[0];
  for (int i=0;i<64;i++) game_pole[i]=pole[i];  
  game_w=1;
 // timelimith=5000; //!!!!!!!!!!!! 
  while (!gameover) {   
   while (s=="") { s=Serial.readString(); delay(1); }
   s.trim();
   if (s=="exit") {gameover=1; continue; }
   if (s=="back"&&game_ply>1) {    
    pos[0]=game_pos;
    for (int i=0;i<64;i++) pole[i]=game_pole[i];        
    game_ply-=2;
    for (int i=0;i<game_ply;i++) {
     movestep(0,game_steps[i]);  
     movepos(0,game_steps[i]);       
     generate_steps(1);
     pos[1].w=!pos[0].w;
     pos[0]=pos[1];       
    }    
    Serial.println("move back! ");
    show_position(); 
    s="";
    continue;          
   }
   if (game_ply%2==0&&game_w) { // 
    generate_steps(0);
    bestmove[0].c1=-1;
    getbm(0,s);    
    if (bestmove[0].c1!=-1) {// 
     for (int i=0;i<pos[0].n_steps;i++) 
      if (pos[0].steps[i].c1==bestmove[0].c1&&pos[0].steps[i].c2==bestmove[0].c2
       &&pos[0].steps[i].type==bestmove[0].type) pos[0].cur_step=i;
     movestep(0,pos[0].steps[pos[0].cur_step]);  
     movepos(0,pos[0].steps[pos[0].cur_step]);  
     Serial.println("make move: "+str_step(pos[0].steps[pos[0].cur_step]));
     game_steps[game_ply]=pos[0].steps[pos[0].cur_step];
     pos[1].w=!pos[0].w;
     pos[0]=pos[1];          
     game_ply++;     
     show_position();           
    } else {
      Serial.println("move illegal, repeat:");
      s=""; 
      continue;
    }    
   } else { // 
    for (int i=0;i<MAXEPD;i++) bestmove[i].c1=-1;       
    pos[0].best.c1=-1;
    solve_step(); 
    if (pos[0].best.c1!=-1) {// 
     for (int i=0;i<pos[0].n_steps;i++) 
      if (pos[0].steps[i].c1==pos[0].best.c1&&pos[0].steps[i].c2==pos[0].best.c2
       &&pos[0].steps[i].type==pos[0].best.type) pos[0].cur_step=i;
     movestep(0,pos[0].steps[pos[0].cur_step]);  
     movepos(0,pos[0].steps[pos[0].cur_step]);  
     Serial.println("make move: "+str_step(pos[0].steps[pos[0].cur_step]));     
     game_steps[game_ply]=pos[0].steps[pos[0].cur_step];
     pos[0]=pos[1];     
     game_ply++;     
     show_position();                
    } else {
      Serial.println("ERROR!");
    }
    s=""; 
   }
   delay(100);
  } 
  Serial.println("Game Over");  
  
}
//****************************
void loop() {
String s;   
  s=load_usb();  
  String s1=s;
  s1.toUpperCase();  
  int numwac=s1.toInt();  
  if (s.indexOf("/")==-1&&numwac>0&&numwac<301) WAC(numwac);      
   else if (s1=="WAC") { 
    WAC();    
  } else if (s1.indexOf("WAC")==0) {
    halt=0;
    int numwac=s.substring(3).toInt();
    if (numwac>0&&numwac<301) WAC(numwac);      
    delay(1000);
  } else if (s.indexOf("TIME")==0||s.indexOf("time")==0) {
    halt=0;
    int tim=s.substring(4).toInt();
    if (tim!=0) timelimith=tim*1000;
    if (timelimith/60000>0) {
     Serial.print("timelimith = "+String(timelimith/60000)+" min ");
     int sec=(timelimith%60000)/1000;
     if (sec>0) Serial.println(String(sec)+" sec"); else Serial.println();  
    } else  Serial.println("timelimith = "+String(timelimith/1000)+" sec");    
    delay(1000);
   } else if (s.indexOf("nullmove")==0||s.indexOf("NULLMOVE")==0) {    
    if (s.length()>8) {
      int n=s.substring(8).toInt(); 
      if (!n) nullmove=0; else nullmove=1;
    }
    Serial.println("nullmove = "+String(nullmove));            
   } else if (s.indexOf("futility")==0||s.indexOf("FUTILITY")==0) {    
    if (s.length()>9) {
      int n=s.substring(9).toInt(); 
      if (!n) futility=0; else futility=1;
    }
    Serial.println("futility = "+String(futility));            
   } else if (s.indexOf("game")==0||s.indexOf("GAME")==0) {     
    game();   
   } else {   
   //timelimith=180*60*1000; //180 .  
   halt=0;
   fen(s);     
   show_position();       
   solvefen(s); 
   Serial.println("move="+str_step(pos[0].best));
   movestep(0,pos[0].best);
   movepos(0,pos[0].best);   
   Serial.println(fenout(1));    
   delay(1000);
  } 
  delay(100);

}
