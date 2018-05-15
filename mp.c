#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>

#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4B
#define RIGHT 0x4D
#define BACKLASH "\\"
#define FORWARDSLASH "/"
#define UNDERSCORE "_"
#define BAR "|"
#define EXCLAMATION "!"
#define V "v"


#define SWARM_DELAY 120000
#define SHIP_DELAY 150000
#define SWARM_CHAR "*"
#define SWARM_ROWS 75
#define SWARM_COLS 13
#define SHIP_ROWS 4
#define SHIP_COLS 8

int swarm[SWARM_ROWS][SWARM_COLS] = { {0,0,0,95,95,95,95,95,95,95,0,0,0}, {0,95,95,92,0,0,0,0,0,47,95,95,0}, {124,95,95,47,95,0,0,0,95,92,95,95,124}, {0,33,33,0,0,124,95,124,0,0,33,33,0},{0,0,0,0,0,0,118,0,0,0,0,0,0}};

int max_x = 0, max_y = 0;
int swarm_direction = 1;

int ship[SHIP_ROWS][SHIP_COLS] = { {0,0,0,47,92,0,0,0}, {0,0,47,92,47,92,0,0}, {0,47,124,47,92,124,92}, {47,95,124,124,124,124,95,92}};
int ship_direction_x = 0;

void print_swarm(int topleft_y, int topleft_x, void *win)
{
    int x = topleft_x;
    int y = topleft_y;
    int i, j;


    for (i = 0; i < SWARM_ROWS; i++) {
        for (j = 0; j < SWARM_COLS; j++) {
            //mvwprintw(win, topleft_y+i, topleft_x+j, swarm[i]);
            /*if (swarm[i][j])
            {
                mvwprintw(win, topleft_y+i, topleft_x+j, SWARM_CHAR);
            }*/
            if(swarm[i][j] == 47) mvwprintw(win, topleft_y+i, topleft_x+j, FORWARDSLASH);
            else if (swarm[i][j] == 92) mvwprintw(win, topleft_y+i, topleft_x+j, BACKLASH);
            else if (swarm[i][j] == 95) mvwprintw(win, topleft_y+i, topleft_x+j, UNDERSCORE);
            else if (swarm[i][j] == 124) mvwprintw(win, topleft_y+i, topleft_x+j, BAR);
            else if (swarm[i][j] == 33) mvwprintw(win, topleft_y+i, topleft_x+j, EXCLAMATION);
            else if (swarm[i][j] == 118) mvwprintw(win, topleft_y+i, topleft_x+j, V);
        }
       // printf("\n");
    }
}

void print_ship(int topleft_x, int topleft_y, void *win)
{
    int x = topleft_x;
    int y = topleft_y;
    int i, j;

    for (i = 0; i < SHIP_ROWS; i++) {
        for (j = 0; j < SHIP_COLS; j++) {
            //if (ship[i][j] > 0)
            //{
                //mvwprintw(win, topleft_y+i, topleft_x+j, FORWARDSLASH);
                if(ship[i][j] == 47) mvwprintw(win, topleft_y+i, topleft_x+j, FORWARDSLASH);
                else if (ship[i][j] == 92) mvwprintw(win, topleft_y+i, topleft_x+j, BACKLASH);
                else if (ship[i][j] == 95) mvwprintw(win, topleft_y+i, topleft_x+j, UNDERSCORE);
                else if (ship[i][j] == 124) mvwprintw(win, topleft_y+i, topleft_x+j, BAR);
                //else mvwprintw(win, topleft_y+i, topleft_x+j, BAR);
            //}
        }
        printf("\n");
    }
}

void *swarm_dance(void *win)
{
    int swarm_x = 0, swarm_y = 0;
    int swarm_next_x = 0;
    int swarm_swipe_count = 0;

    while(1) {
        wclear(win);
        
        //wmove(win, y, x);
        //wprintw(win, "X");
        print_swarm(swarm_y, swarm_x, win);
        wrefresh(win);

        usleep(SWARM_DELAY);

        //clear();
        //getmaxyx(stdscr, swarm_max_y, swarm_max_x);

        
        //refresh();

        //usleep(DELAY);

        swarm_next_x = swarm_x + swarm_direction;

        if (swarm_next_x + SWARM_COLS >= max_x || swarm_next_x < 0) {
            swarm_direction*= -1;
            swarm_swipe_count++;
        
            if (swarm_swipe_count % 3 == 0) swarm_y++;
        } 

        else {
            swarm_x+= swarm_direction;
        }

    }
    
}

void *ship_moving(void *win)
{
    int ship_x = 0, ship_y = 0;
    int ship_next_x = 0;
    
    while(1) {
        wclear(win);
        
        //wmove(win, y, x);
        //wprintw(win, "X");
        print_ship(ship_x,ship_y, win);
        wrefresh(win);

        usleep(SHIP_DELAY);

        ship_next_x = ship_x + ship_direction_x;
        int input_char;
        input_char = getc(stdin);
        //input_char = getc(stdin);
        //if(input_char == 0)
        {
            
        }

        ship_x = ship_x + ship_direction_x;

        //swarm_next_x = swarm_x + swarm_direction;
        if (ship_next_x >= max_x || ship_next_x < 0) {
            //direction_y*= -1;
        } else {
            if(input_char == 'A' || input_char == 'a') ship_direction_x = -1;
            else if (input_char == 'D' || input_char == 'd') ship_direction_x = 1;
            else ship_direction_x = 0;
        }

        
    }

    pthread_exit(NULL);

    endwin();
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) 
{
    pthread_t thread_swarm, thread_shipcontrol;//, thread_shipmissile;
    void *status_swarm, *status_shipcontrol;//, *status_shipmissile;

    WINDOW *win_swarm, *win_ship;//, *win_missile;   

    initscr();
    noecho();
    curs_set(FALSE);

    getmaxyx(stdscr, max_y, max_x);
    win_swarm = newwin(max_y, max_x, 0, 0);
    //win_ship = newwin(max_y, max_x, 20, 2);

    pthread_create(&thread_swarm, NULL, swarm_dance, (void *) win_swarm);
    //pthread_create(&thread_shipcontrol, NULL, ship_moving, (void *) win_ship); NOT WORKINGGGG
    //pthread_create(&thread_shipmissile, NULL, direction_changer, NULL);

    //pthread_join(thread_swarm, &status_swarm);
    //pthread_join(thread_shipcontrol, &status_shipcontrol); NOT WORKINGGGG
    //pthread_join(thread_shipmissile, &status_shipmissile);
    endwin();
    pthread_exit(NULL);

    

    endwin();
}