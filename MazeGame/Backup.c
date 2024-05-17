#include <stdio.h>
#include <stdlib.h>

#define MAX_DIM 100
#define MIN_DIM 5

#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

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

int get_width(FILE *file)
{
    fseek(file, 0, SEEK_SET);
    int width = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF && ch != '\n')
    {
        width++;
    }
    fseek(file, 0, SEEK_SET);
    return width;
}

int get_height(FILE *file)
{
    fseek(file, 0, SEEK_SET);
    int height = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            height++;
        }
    }
    fseek(file, 0, SEEK_SET);
    return height;
}

int read_maze(maze *this, FILE *file)
{
    this->width = get_width(file);
    this->height = get_height(file);
    if (this->width == 0 || this->height == 0)
    {
        return 1;
    }
    if (create_maze(this, this->height, this->width))
    {
        return 1;
    }
    char ch;
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            ch = fgetc(file);
            if (ch == EOF)
            {
                return 1;
            }
            if (ch == 'S')
            {
                this->start.x = j;
                this->start.y = i;
            }
            else if (ch == 'E')
            {
                this->end.x = j;
                this->end.y = i;
            }
            this->map[i][j] = ch;
        }
        ch = fgetc(file); // consume newline character
        if (ch != '\n' && ch != EOF)
        {
            return 1;
        }
    }
    return 0;
}


void print_maze(maze *this, coord *player)
{
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else if (this->map[i][j] == 'E')
            {
                printf("E");
            }
            else if (this->map[i][j] == 'S')
            {
                printf(" ");
            }
            else if (this->map[i][j] == '#')
            {
                printf("#");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        printf("\n");
    }
}

void make_move(maze *this, coord *player, char direction)
{
    int new_x = player->x;
    int new_y = player->y;
    switch (direction)
    {
    case 'w':
    case 'W':
        new_y--;
        break;
    case 'a':
    case 'A':
        new_x--;
        break;
    case 's':
    case 'S':
        new_y++;
        break;
    case 'd':
    case 'D':
        new_x++;
        break;
    default:
        return;
    }
    if (new_x >= 0 && new_x < this->width && new_y >= 0 && new_y < this->height &&
        this->map[new_y][new_x] != '#')
    {
        player->x = new_x;
        player->y = new_y;
    }
    else
    {
        printf("Invalid move!\n");
    }
}

int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <mazefile path>\n", argv[0]);
        return EXIT_ARG_ERROR;
    }

    maze this_maze;
    coord player;
    FILE *f;

    f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return EXIT_FILE_ERROR;
    }

    if (read_maze(&this_maze, f))
    {
        printf("Invalid maze file!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    fclose(f);

    player.x = this_maze.start.x;
    player.y = this_maze.start.y;

    char move;
    while (!has_won(&this_maze, &player))
    {
        print_maze(&this_maze, &player);
        printf("Enter your move (WASD or M for map): ");
        scanf(" %c", &move);
        if (move == 'm' || move == 'M')
        {
            print_maze(&this_maze, &player);
            continue;
        }
        make_move(&this_maze, &player, move);
    }

    print_maze(&this_maze, &player);
    printf("Congratulations! You have won!\n");

    free_maze(&this_maze);

    return EXIT_SUCCESS;
}
