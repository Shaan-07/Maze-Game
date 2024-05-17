#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DIM 100
#define MIN_DIM 5

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

int create_maze(maze *this, int height, int width)
{
    this->map = (char **)malloc(height * sizeof(char *));
    if (this->map == NULL)
    {
        return 1;
    }
    for (int i = 0; i < height; i++)
    {
        this->map[i] = (char *)malloc(width * sizeof(char));
        if (this->map[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(this->map[j]);
            }
            free(this->map);
            return 1;
        }
    }
    this->height = height;
    this->width = width;
    return 0;
}

void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        free(this->map[i]);
    }
    free(this->map);
}

void print_maze(maze *this)
{
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            printf("%c", this->map[i][j]);
        }
        printf("\n");
    }
}

void generate_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (i == 0 || i == this->height - 1 || j == 0 || j == this->width - 1)
            {
                this->map[i][j] = '#';
            }
            else
            {
                if (rand() % 3 == 0)
                {
                    this->map[i][j] = '#';
                }
                else
                {
                    this->map[i][j] = ' ';
                }
            }
        }
    }
}

void generate_path(maze *this, int x, int y)
{
    this->map[y][x] = ' ';
    while (x != this->end.x || y != this->end.y)
    {
        int dir = rand() % 4;
        switch (dir)
        {
        case 0: // Up
            if (y > this->end.y || (y > 0 && this->map[y - 1][x] == '#'))
            {
                this->map[--y][x] = ' ';
            }
            break;
        case 1: // Down
            if (y < this->end.y || (y < this->height - 1 && this->map[y + 1][x] == '#'))
            {
                this->map[++y][x] = ' ';
            }
            break;
        case 2: // Left
            if (x > this->end.x || (x > 0 && this->map[y][x - 1] == '#'))
            {
                this->map[y][--x] = ' ';
            }
            break;
        case 3: // Right
            if (x < this->end.x || (x < this->width - 1 && this->map[y][x + 1] == '#'))
            {
                this->map[y][++x] = ' ';
            }
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <mazefile path> <height> <width>\n", argv[0]);
        return 1;
    }

    maze this_maze;
    FILE *f;

    int height = atoi(argv[2]);
    int width = atoi(argv[3]);

    if (height < MIN_DIM || height > MAX_DIM || width < MIN_DIM || width > MAX_DIM)
    {
        printf("Height and width must be between %d and %d.\n", MIN_DIM, MAX_DIM);
        return 1;
    }

    if (create_maze(&this_maze, height, width))
    {
        printf("Error creating maze!\n");
        return 1;
    }

    this_maze.start.x = 0;
    this_maze.start.y = 0;
    this_maze.end.x = width - 1;
    this_maze.end.y = height - 1;

    srand(time(NULL));

    generate_maze(&this_maze);

    generate_path(&this_maze, 0, 0);

    this_maze.map[0][0] = 'S';
    this_maze.map[height - 1][width - 1] = 'E';

    f = fopen(argv[1], "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        free_maze(&this_maze);
        return 1;
    }

    print_maze(&this_maze);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fprintf(f, "%c", this_maze.map[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    free_maze(&this_maze);

    printf("Maze generated and saved to %s\n", argv[1]);

    return 0;
}

