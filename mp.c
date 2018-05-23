#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

int WINDOW_WIDTH = 52;
int WINDOW_HEIGHT = 30;

WINDOW *swarm;
WINDOW *ship;

pthread_t swarm_thread;
pthread_t ship_thread;
pthread_t ship_missile_thread[100];

char SWARM_CH = 42;
char SHIP_CH = 94;
char SHIP_MS = 124;
char BLANK = 32;

#define SWARM_ROWS 5
#define SWARM_COLS 9
#define SHIP_ROWS 2
#define SHIP_COLS 3
#define SWARM_DELAY 150000

int swarm_rows = 5;
int swarm_cols = 9;
int swarm_position = 0;
int swarm_direction = 0;

int ship_rows = 2;
int ship_cols = 3;
int ship_position = 0;

int ch;
int counter = 0;

int swarm_data[SWARM_ROWS][SWARM_COLS] = {{0,0,0,1,1,1,0,0,0},
                                          {0,0,1,1,1,1,1,0,0},
                                          {1,1,1,1,1,1,1,1,1},
                                          {0,0,0,1,1,1,0,0,0},
                                          {0,0,0,0,1,0,0,0,0}};

int ship_data[SHIP_ROWS][SHIP_COLS] = {{0,1,0},
                                                {1,1,1}};


int get_leftmost(){
    int row, col;
    int swarm_leftmost = -1;
    for(col=0;col<SWARM_COLS;col++){
        for(row=0;row<SWARM_ROWS;row++){
            if(swarm_data[row][col] == 1){
                swarm_leftmost = col;
                break;
            }
        }
        if(swarm_leftmost != -1){
            break;
        }
    }
    return swarm_leftmost;
}

int get_rightmost(){
    int row, col;
    int swarm_rightmost = -1;
    for(col=SWARM_COLS-1;col>-1;col--){
        for(row=SWARM_ROWS-1;row>-1;row--){
            if(swarm_data[row][col]==1){
                swarm_rightmost = col;
                break;
            }
        }
        if(swarm_rightmost != -1){
            break;
        }
    }
    return swarm_rightmost;
}

int get_cols(){
    int width = get_rightmost() - get_leftmost() + 1;
    return width;
}

void swarm_create(){
    int row, col;

    swarm_cols = get_cols();
    swarm = newwin(swarm_rows,swarm_cols,WINDOW_HEIGHT-(WINDOW_HEIGHT-1),swarm_position);

    int temp;
    for(row=0;row<SWARM_ROWS;row++){
        for(col=get_leftmost(),temp=0;col<get_rightmost()+1;col++,temp++){
            move(temp,row);
            if(swarm_data[row][col]==1){
                waddch(swarm,SWARM_CH);
            }
            else{
                waddch(swarm,BLANK);
            }
        }
    }

    wrefresh(swarm);
}

void ship_create(){
    int row, col;

    ship = newwin(ship_rows,ship_cols,WINDOW_HEIGHT-(ship_rows+1),ship_position);
    
    for(row=0;row<ship_rows;row++){
        for(col=0;col<ship_cols;col++){
            move(col,row);
            if(ship_data[row][col]==1){
                waddch(ship,SHIP_CH);
            }
            else{
                waddch(ship,BLANK);
            }
        }
    }

    wrefresh(ship);
}

void swarm_initialize(){
    int swarm_cols = get_cols();
    swarm_position = (WINDOW_WIDTH-swarm_cols)/2+1;
    swarm_create();
    
    time_t t;
    srand((unsigned) time(&t));
    swarm_direction = rand() % 2;
    if(swarm_direction == 0){
        swarm_direction = -1;
    }
    else{
        swarm_direction = 1;
    }
}

void ship_initialize(){
    ship_position = (WINDOW_WIDTH-SHIP_COLS)/2+1;
    ship_create();
}

void swarm_delete(){
    wclear(swarm);
    wrefresh(swarm);
}

void ship_delete(){
    wclear(ship);
    wrefresh(ship);
}

void *swarm_engine(void *args){
    while(1){
        if(ch==KEY_F(1)){
            break;
        }
        if(swarm_position==1){
            swarm_direction *= -1;
        }
        if((swarm_position+get_cols()+1)==WINDOW_WIDTH){
            swarm_direction *= -1;
        }
        usleep(SWARM_DELAY);
        swarm_delete();
        swarm_position += swarm_direction;
        swarm_create();
    }
    pthread_exit(NULL);
}

void *ship_engine(void *args){
    int x,y;
    while((ch = getch())!= KEY_F(1)){
        switch(ch){
            case KEY_LEFT:
                if(ship_position>1){
                    ship_delete();
                    ship_position -= 1;
                    ship_create();
                }
                break;
            case KEY_RIGHT:
                if(ship_position<WINDOW_WIDTH-4){
                    ship_delete();
                    ship_position += 1;
                    ship_create();
                }
                break;
        }
    }
    pthread_exit(NULL);    
}

int main(void){
    system("resize -s 30 52");

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr,TRUE);

    refresh();

    swarm_initialize();
    ship_initialize();

    pthread_create(&swarm_thread,NULL,swarm_engine,NULL);
    pthread_create(&ship_thread,NULL,ship_engine,NULL);

    pthread_join(swarm_thread,NULL);
    pthread_join(ship_thread,NULL);

    endwin();

    return 0;
}