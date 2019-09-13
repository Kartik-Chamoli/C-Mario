#define _POSIX_C_SOURCE 199309L //For nanosleep
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

WINDOW *pad;
int b=20;
enum Mariocolors{
mario_cap=1,marioSkin,marioShoe,marioHair,marioPants,marioEyes,marioButton
};
enum MapColors{
landMap=8,skyMap,brickMap
};
enum EnemyColors{
goomba=11,TEXT
};
enum marioPair{
capPair=1,facePair,shoePair,hairPair,pantsPair,eyePair,buttonPair
};
enum MapPair{
landPair=8,skyPair,brickPair
};
enum EnemyPair{
goombaPair=11,TEXTPAIR
};
typedef struct{
int posY;
int posX;
}marioSprite;
marioSprite Mario;

typedef struct{
int posY;
int posX;
char state;
char direc;
}goombaSprite;
goombaSprite Goomba[10];

void initScreen();
void initColor();
void pair();

void marioCap();
void mario_hair();
void mario_eye();
void mario_face();

void mario_body();
void mario_button();
void mario_pants();
void mario_hands();
void mario_boots();

void mario_stand_body();
void mario_stand_buttons();
void mario_stand_hands();
void mario_stand_pants();
void mario_stand_shoes();

void mario_full(bool action);
void mario_clean();
void mario_jumpclean();

double timeMilli(){
struct timeval  tv;
gettimeofday(&tv,NULL);
double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
return time_in_mill;
}

void controls(int a)
{  static double prev=0;
   int downch;
    switch (a){
    case KEY_UP:
    if(timeMilli()-prev<200)
    {   prev=timeMilli();
    }
    else{
    mario_clean();
    Mario.posY-=30;
    halfdelay(5);
    mario_full(true);
    prefresh(pad,0,b,0,0,getmaxy(stdscr),getmaxx(stdscr)-1); //-1 if the pad is greater than screen
   if((a=getch())==KEY_RIGHT){
    mario_jumpclean();
    timedelay(90000000);
   if(Mario.posX<getmaxx(stdscr)/2+b-30){ //Mario can go back and move right and stop at the exact same spot now
     Mario.posX+=74;
   }
    else{  
    Mario.posX+=74; //Implement pad scrolling with jump
    //b+=37;
    }
   }
    timedelay(90000000);
    mario_jumpclean();

//     mvwhline(pad,Mario.posY+16,Mario.posX-12,' ',37); mvwhline(pad,Mario.posY+16,Mario.posX-12,' ',37);
   for(int i=0;i<30;i++){
        Mario.posY+=1;
     if(i==14)
        {
          for(int j=-12;j<=19;j++){ //CHANGE THIS LINE .Remove the range if it doesn't work
            downch=mvwinch(pad,Mario.posY+32,Mario.posX+j); //Here 31 represents the 1 more than shoe from posY
             if((downch & A_CHARTEXT)>='0'&& (downch & A_CHARTEXT)<='9') //No need it works just remove these comments. Use the code for object detection
              {
                Goomba[downch&A_CHARTEXT-48].state='d';
                goombasClean(downch & A_CHARTEXT-48,'o');
                Mario.posY+=6;
                mario_full(false);
                prefresh(pad,0,b,0,0,getmaxy(stdscr),getmaxx(stdscr)-1);
                timedelay(99999999);
                i+=6;
                goombasClean(downch & A_CHARTEXT-48,'l');
                attron(COLOR_PAIR(shoePair));
                for(int i=0;i<31;i++){                            //Function for clearing the mario
                mvwhline(pad,Mario.posY+i,Mario.posX-11,' ',39);
                }
                attroff(COLOR_PAIR(shoePair));
                }
          }
        }
    }

    mario_full(false);
    a=' ';
    cbreak();
    }
    prev=timeMilli();
    break;

     case KEY_RIGHT:

   mario_clean();
   if(Mario.posX<getmaxx(stdscr)/2+b-10){ //Mario can go back and move right and stop at the exact same spot now
     Mario.posX+=2;
   }
   else{
     Mario.posX+=1;
     b++;
   }
     mario_full(false);
    break;

//    case KEY_DOWN:
//    mario_clean();
//    Mario.posY+=2;
//    mario_full(false);
//    break;



    case KEY_LEFT:


    if(Mario.posX>=20+b){ //Added b so that mario doesn't go all the way back after scrolling is done
    mario_clean();
    Mario.posX-=2;
     mario_full(false);
    }
    break;

    default:
    break;
    }

}


int main()
{    bool running=true;
     initScreen();

     Mario.posY=getmaxy(stdscr)-42;
     Mario.posX=30;



     initColor();
     pair();
    int a;
   initGoombas();
   while((a=getch())!=KEY_F(2)){
    map_land();
    bricks();
    mario_full(false);
    wattron(pad,COLOR_PAIR(brickPair));
    wattroff(pad,COLOR_PAIR(brickPair));
    collisions();
    controls(a); //Keep controls above Goomba so that mario doesnt clear goomba area
    goombas();
    prefresh(pad,0,b,0,0,getmaxy(stdscr),getmaxx(stdscr)-1); //-1 if the pad is greater than the screen
    }
    getch();
    getch();
    endwin();
}

void collisions()
{
    int rightch = mvwinch(pad,Mario.posY+21, Mario.posX+22);//22 is 4 more than end of mario which is 18
    int leftch =  mvwinch(pad,Mario.posY+21, Mario.posX-11);
    //Store it inside a struct for all the text
     char _gameOver[ 5 ][ 78 ] = {
  { " XXXX    XXXX   XX   XX  XXXXXX           XXXX   XX  XX  XXXXXX  XXXXX     XX" },
  { "XX      XX  XX  XXX XXX  XX              XX  XX  XX  XX  XX      XX  XX    XX" },
  { "XX XXX  XXXXXX  XX X XX  XXXX            XX  XX  XX  XX  XXXX    XXXXX     XX" },
  { "XX  XX  XX  XX  XX   XX  XX              XX  XX   XXXX   XX      XX  XX      " },
  { " XXXX   XX  XX  XX   XX  XXXXXX           XXXX     XX    XXXXXX  XX  XX    XX" } };


     //Make one function for both the left and right detection
    if((leftch & A_COLOR)==COLOR_PAIR(goombaPair)){
      wattron(pad,COLOR_PAIR(TEXTPAIR)|A_BOLD|A_BLINK);

     for(int i=0;i<5;i++)
     {
       wmove(pad,getmaxy(stdscr)/2+i,getmaxx(stdscr)/2-10+b);
       wprintw(pad,"%s",_gameOver[i]);
     }
     prefresh(pad,0,b,0,0,getmaxy(stdscr),getmaxx(stdscr)-1);
     wattroff(pad,COLOR_PAIR(TEXTPAIR)|A_BOLD|A_BLINK);
     sleep(5);
     delwin(pad);
     endwin();
    exit(0);
    }


    if((rightch & A_COLOR) == COLOR_PAIR(goombaPair)){
     wattron(pad,COLOR_PAIR(TEXTPAIR)|A_BOLD|A_BLINK);
     for(int i=0;i<5;i++)
     {
       wmove(pad,getmaxy(stdscr)/2+i,getmaxx(stdscr)/2-10+b);
       wprintw(pad,"%s",_gameOver[i]);
     }

     prefresh(pad,0,b,0,0,getmaxy(stdscr),getmaxx(stdscr)-1);
     wattroff(pad,COLOR_PAIR(TEXTPAIR)|A_BOLD|A_BLINK);
     sleep(5);
     delwin(pad);
     endwin();
      exit(0);
    }

}
void goombasClean(int GoomVar,char B)
{

    wattron(pad,COLOR_PAIR(skyPair));
    for(int i=0;i<15;i++)
    {
        mvwhline(pad,Goomba[GoomVar].posY-12+i,Goomba[GoomVar].posX-11,' ',40);
        if(i==5 && B=='o')
        {
          break;
        }
    }
    wattroff(pad,COLOR_PAIR(skyPair));
}
void initGoombas()
{
     Goomba[0].posY=getmaxy(stdscr)-14;
     Goomba[0].posX=80;
     Goomba[0].state='a';

     Goomba[1].posY=getmaxy(stdscr)-14;
     Goomba[1].posX=160;
     Goomba[1].state='a';

     Goomba[2].posY=getmaxy(stdscr)-14;
     Goomba[2].posX=240;
     Goomba[2].state='a';

     Goomba[3].posY=getmaxy(stdscr)-14;
     Goomba[3].posX=320;
     Goomba[3].state='a';

     Goomba[4].posY=getmaxy(stdscr)-14;
     Goomba[4].posX=400;
     Goomba[4].state='a';

     Goomba[5].posY=getmaxy(stdscr)-14;
     Goomba[5].posX=480;
     Goomba[5].state='a';

     Goomba[6].posY=getmaxy(stdscr)-14;
     Goomba[6].posX=560;
     Goomba[6].state='a';

     Goomba[7].posY=getmaxy(stdscr)-14;
     Goomba[7].posX=640;
     Goomba[7].state='a';

     Goomba[8].posY=getmaxy(stdscr)-14;
     Goomba[8].posX=720;
     Goomba[8].state='a';

     Goomba[9].posY=getmaxy(stdscr)-14;
     Goomba[9].posX=800;
     Goomba[9].state='a';

}

void goombas()
{
  static double now=0;
  if(timeMilli()-now<200)
    {
    }
  else{

  //change 50 to a starting point and store in a variable for posX
  for(int GoomVar=0;GoomVar<10;GoomVar++){
        if(Goomba[GoomVar].state=='d')
        {
            continue;
        }
        else{
   goombasClean(GoomVar,'l');
  wattron(pad,COLOR_PAIR(goombaPair));
  for(int i=0;i<5;i++)
  {mvwhline(pad,Goomba[GoomVar].posY-12+i,Goomba[GoomVar].posX-2*i,'0'+GoomVar,20+4*i);
  }
  for(int j=5;j>0;j--)
  {
    mvwhline(pad,Goomba[GoomVar].posY-2-j,Goomba[GoomVar].posX-2*j,' ',20+4*j);
  }
  wattroff(pad,COLOR_PAIR(goombaPair));
  wattron(pad,COLOR_PAIR(facePair));
  for(int i=0;i<3;i++)
  {
    mvwhline(pad,Goomba[GoomVar].posY-i,Goomba[GoomVar].posX,' ',20);
  }
  wattroff(pad,COLOR_PAIR(facePair));
  wattron(pad,COLOR_PAIR(eyePair));
  for(int i=0;i<3;i++)
  {
    mvwhline(pad,Goomba[GoomVar].posY+i,Goomba[GoomVar].posX-2,' ',6);
    mvwhline(pad,Goomba[GoomVar].posY+i,Goomba[GoomVar].posX+14,' ',6);
    mvwhline(pad,Goomba[GoomVar].posY-10+i,Goomba[GoomVar].posX,' ',3);
    mvwhline(pad,Goomba[GoomVar].posY-10+i,Goomba[GoomVar].posX+14,' ',3);
  }
  wattroff(pad,COLOR_PAIR(eyePair));
  Goomba[GoomVar].posX++;
        }
  }
  now=timeMilli();
  }
}

void timedelay(long nsec)
{
    struct timespec tim;
    tim.tv_nsec=nsec;
    tim.tv_sec=0;
    nanosleep(&tim,NULL);

}
void bricks()
{ wattron(pad,COLOR_PAIR(brickPair));
  for(int j=15;j<getmaxx(stdscr);j+=90)
  {
  for(int i=1;i<25;i++)
  mvwvline(pad,getmaxy(stdscr)-82,10+i+j,' ',10);
  }
  wattroff(pad,COLOR_PAIR(brickPair));
}
void mario_clean()
{
    wattron(pad,COLOR_PAIR(skyPair));
    for(int i=0;i<31;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX-13,' ',40);
    }
    wattroff(pad,COLOR_PAIR(skyPair));
}
void mario_jumpclean()
{
    wattron(pad,COLOR_PAIR(skyPair));
    for(int i=0;i<31;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX-18,' ',54);
    }
    wattroff(pad,COLOR_PAIR(skyPair));
}

void initColor()
{
  init_color(mario_cap,996,8,126);
  init_color(marioSkin,1000,700,529);
  init_color(marioShoe,545,271,75);
  init_color(marioHair,545,271,75);
  init_color(marioPants,0,4,989);
  init_color(marioEyes,0,0,0);
  init_color(marioButton,985,816,0);
  init_color(landMap,840,530,68);
  init_color(skyMap,708,885,956);
  init_color(brickMap,933,687,212);
  init_color(goomba,757,510,250);
}

void pair()
{
    init_pair(capPair,mario_cap,mario_cap);
    init_pair(facePair,marioSkin,marioSkin);
    init_pair(shoePair,marioShoe,marioShoe);
    init_pair(hairPair,marioHair,marioHair);
    init_pair(pantsPair,marioPants,marioPants);
    init_pair(eyePair,marioEyes,marioEyes);
    init_pair(buttonPair,marioButton,marioButton);
    init_pair(landPair,landMap,landMap);
    init_pair(skyPair,skyMap,skyMap);
    init_pair(brickPair,brickMap,brickMap);
    init_pair(goombaPair,goomba,goomba);
    init_pair(TEXTPAIR,brickPair,eyePair);
}

void initScreen()
{
    initscr();
    pad=newpad(getmaxy(stdscr),getmaxx(stdscr)+1000);
    cbreak();
    if(has_colors()==FALSE)
    {
        fprintf(stderr,"%30s","your terminal doesn't support colors");
        endwin();
        exit(EXIT_FAILURE);
    }

    if(!(COLS>=44&&LINES>=22))
    {
        fprintf(stderr,"%s","ERROR");
        endwin();
        exit(EXIT_FAILURE);
    }

     noecho();
     timeout(300);
     wbkgd(pad, COLOR_PAIR(skyPair));
   //  wbkgd(stdscr, COLOR_PAIR(skyPair));
     keypad(stdscr,true);

    start_color();
}

void marioCap()
{
    wattron(pad,COLOR_PAIR(capPair));
    mvwhline(pad,Mario.posY,Mario.posX,'A',17);
    mvwhline(pad,Mario.posY+1,Mario.posX-3,' ',25);
    mvwhline(pad,Mario.posY+2,Mario.posX-3,' ',25);
    wattroff(pad,COLOR_PAIR(capPair));
}

void mario_hair(){

    wattron(pad,COLOR_PAIR(hairPair));
    for(int i=0;i<3;i++)
    mvwhline(pad,Mario.posY+3+i,Mario.posX-3,' ',9);
    for(int i=0;i<=1;i++)
    mvwvline(pad,Mario.posY+5,Mario.posX+i,' ',4);
    for(int i=4;i<=7;i++)
    mvwvline(pad,Mario.posY+5,Mario.posX-i,' ',4);
    for(int i=9;i<=10;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-7,' ',7);
    mvwhline(pad,Mario.posY+8,Mario.posX,' ',6);
    wattroff(pad,COLOR_PAIR(hairPair));
}

void mario_eye(){
    wattron(pad,COLOR_PAIR(eyePair));
    for(int i=1;i<=3;i++)
    mvwvline(pad,Mario.posY+3,Mario.posX+9+i,' ',4);
    for(int i=13;i<=15;i++)
    mvwvline(pad,Mario.posY+7,Mario.posX+i,' ',3);
    for(int i=9;i<=10;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+10,' ',12);
    wattroff(pad,COLOR_PAIR(eyePair));
}

void mario_face(){
    wattron(pad,COLOR_PAIR(facePair));
    for(int i=3;i<5;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-3,' ',18);
    for(int i=5;i<=6;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-3,' ',25);
    for(int i=7;i<=8;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-3,' ',30);
    for(int i=9;i<=10;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-3,' ',25);
    for(int i=11;i<=12;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX,' ',18);
    wattroff(pad,COLOR_PAIR(facePair));
}

void mario_body(){
    wattron(pad,COLOR_PAIR(capPair));
    for(int i=13;i<=14;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-13,' ',35);
    for(int i=15;i<=16;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-13,' ',31);
    for(int i=3;i<=4;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+20,' ',10);
    for(int i=4;i<=6;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+22,' ',8);
    for(int i=9;i<=12;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+22,' ',5);
    for(int i=7;i<=9;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+27,' ',3);
    for(int i=11;i<=12;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+18,' ',6);
    wattroff(pad,COLOR_PAIR(capPair));
}

void mario_button(){
    wattron(pad,COLOR_PAIR(buttonPair));
    for(int i=17;i<=18;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+14,' ',4);
    for(int i=19;i<=20;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+6,' ',4);
    wattroff(pad,COLOR_PAIR(buttonPair));
}

void mario_pants(){
    wattron(pad,COLOR_PAIR(pantsPair));
    for(int i=17;i<=18;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-9,' ',37);

    for(int i=19;i<=21;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-6,' ',34);

    for(int i=22;i<=23;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-6,' ',19);

    for(int i=15;i<=16;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX+14,' ',4);
    mvwhline(pad,Mario.posY+i,Mario.posX+3,' ',4);
    }

    for(int i=13;i<=14;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX+10,' ',4);
    mvwhline(pad,Mario.posY+i,Mario.posX-1,' ',4);
    }

    wattroff(pad,COLOR_PAIR(pantsPair));

    wattron(pad,COLOR_PAIR(capPair));
    for(int i=19;i<=20;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX,' ',4);
    for(int i=17;i<=18;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-13,' ',20);
    wattroff(pad,COLOR_PAIR(capPair));
}

void mario_hands(){
    wattron(pad,COLOR_PAIR(facePair));
    for(int i=15;i<=16;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-18,' ',5);
    for(int i=17;i<=18;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-18,' ',9);
    for(int i=19;i<=20;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX-15,' ',2);
    mvwhline(pad,Mario.posY,Mario.posX+20,' ',10);
    }
    for(int i=1;i<=2;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+22,' ',8);

    wattroff(pad,COLOR_PAIR(facePair));
}

void mario_boots(){
    wattron(pad,COLOR_PAIR(shoePair));
    for(int i=21;i<=22;i++) //1st Boot
    mvwhline(pad,Mario.posY+i,Mario.posX-12,' ',12);
    for(int i=23;i<=24;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-18,' ',12);
    for(int i=25;i<=26;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-18,' ',8);
    for(int i=16;i<=21;i++)//2nd Boot
    mvwhline(pad,Mario.posY+i,Mario.posX+28,' ',5);
    for(int i=14;i<=21;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX+33,' ',3);
}



void mario_stand_body()
{
    wattron(pad,COLOR_PAIR(capPair));
    for(int i=13;i<=14;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-4,' ',18);

    for(int i=15;i<=16;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-7,' ',25);

    for(int i=17;i<=18;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-10,' ',32);

    for(int i=19;i<=20;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-4,' ',20);

    wattroff(pad,COLOR_PAIR(capPair));
}

void mario_stand_pants()
{
    wattron(pad,COLOR_PAIR(pantsPair));
    for(int i=19;i<=22;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-2,' ',16);
    for(int i=23;i<=24;i++)
    mvwhline(pad,Mario.posY+i,Mario.posX-4,' ',20);
    for(int i=25;i<=26;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX-4,' ',7);
    mvwhline(pad,Mario.posY+i,Mario.posX+9,' ',7);
    }
    for(int i=13;i<=18;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX+1,' ',3);
    if(i>=15){
        mvwhline(pad,Mario.posY+i,Mario.posX+9,' ',3);
    }
    }
    mvwhline(pad,Mario.posY+18,Mario.posX+2,' ',8);
    wattroff(pad,COLOR_PAIR(pantsPair));
}

void mario_stand_buttons()
{
    wattron(pad,COLOR_PAIR(buttonPair));
    for(int i=19;i<=20;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX+1,' ',3);
    mvwhline(pad,Mario.posY+i,Mario.posX+9,' ',3);
    }
    wattroff(pad,COLOR_PAIR(buttonPair));
}

void mario_stand_hands(){
    wattron(pad,COLOR_PAIR(facePair));
    for(int i=19;i<=23;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX-10,' ',6);
    mvwhline(pad,Mario.posY+i,Mario.posX+16,' ',6);
    }

    for(int i=21;i<=22;i++){
    mvwhline(pad,Mario.posY+i,Mario.posX+14,' ',2);
    mvwhline(pad,Mario.posY+i,Mario.posX-4,' ',2);
    }
    wattroff(pad,COLOR_PAIR(facePair));
}

void mario_stand_shoes(){
    wattron(pad,COLOR_PAIR(shoePair));
    for(int i=27;i<=28;i++){
     mvwhline(pad,Mario.posY+i,Mario.posX-8,' ',11);
     mvwhline(pad,Mario.posY+i,Mario.posX+9,' ',11);
    }
    for(int i=29;i<=30;i++){
     mvwhline(pad,Mario.posY+i,Mario.posX-12,' ',15);
     mvwhline(pad,Mario.posY+i,Mario.posX+9,' ',15);
    }
    wattroff(pad,COLOR_PAIR(shoePair));
}

void map_land()
{

 wattron(pad,COLOR_PAIR(landPair));
    for(int i=0;i<20;i++){
    mvwhline(pad,getmaxy(pad)-11+i,0,' ',getmaxx(pad));
    }
    wattroff(pad,COLOR_PAIR(landPair));
    wattron(pad,COLOR_PAIR(eyePair));
    for(int j=getmaxy(pad)-11;j<=getmaxy(pad);j+=1)
    {
    for(int i=2;i<=8;i+=3){
    mvwhline(pad,getmaxy(pad)-11+i,0,' ',getmaxx(pad));
    }
    for(int i=3;i<=getmaxx(stdscr);i+=9)
    mvwvline(pad,j,i,' ',5);
    }
    wattroff(pad,COLOR_PAIR(eyePair));
}
void mario_stand()
{
    mario_stand_body();
    mario_stand_pants();
    mario_stand_buttons();
    mario_stand_hands();
    mario_stand_shoes();
}

void mario_jump()
{
    mario_body();
    mario_pants();
    mario_hands();
    mario_boots();
    mario_button();
}

void mario_full(bool action)
{
    marioCap();
    mario_face();
    mario_eye();
    mario_hair();
    if(action==true)
        mario_jump();
    else
        mario_stand();
}

