#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

const int grid_width = 30;
const int grid_height = 30;
static pthread_t *threads = NULL;
bool initialized = false;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}


void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void ignore_handler(int signo, siginfo_t* info, void* context) {}

typedef struct {
    char* src;
    char* dst;
    int row;
    int col;
    int idx;
} grid_arguments;

void* start_routine(void* received_arg) {
    grid_arguments * arg = (grid_arguments *) received_arg;

    while (1) {
        arg->dst[arg->idx] = is_alive(arg->row, arg->col, arg->src);
        pause();
        char *tmp = arg->src;
        arg->src = arg->dst;
        arg->dst = tmp;
    }
}


void inits(char *foreground, char *background) {
    if (initialized){
        return;
    }
    init_grid(foreground);
    threads = malloc(sizeof(pthread_t) * grid_width * grid_height);
    int counter = 0;
    for (int i = 0; i < grid_width; i++) {
        for (int j = 0; j < grid_height; j++) {
            grid_arguments *arg = malloc(sizeof(grid_arguments));
            arg->src = foreground;
            arg->dst = background;
            arg->row = i;
            arg->col = j;
            arg->idx = counter;
            pthread_create(threads+arg->idx, NULL, start_routine, (void *)arg);
            counter++;
        }
    }
    initialized = true;
}

void destroy_all(char *foreground, char *background){
    free(foreground);
    free(background);
    if (!initialized){
        return;
    }
    free(threads);
}

void update_grid(char *src, char *dst){
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = ignore_handler;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return;
    }


    for (int i = 0; i < grid_width * grid_height; i++)
        if (pthread_kill(threads[i], SIGUSR1) != 0) {
            perror("pthread_kill");
            return;   
        }
}