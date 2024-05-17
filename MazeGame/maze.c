/**
 * @file maze.c
 * @author Shaan Shaikh
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

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
    // X & Y coordinates are defined here.
    int x; 
    int y;
} coord;

typedef struct __Maze
{
    // Here we are creatin a 2D array to store the maze.
    char **map;
    int height;
    int width;
    coord start; // Starting position
    coord end; // Ending position
} maze;

int create_maze(maze *this, int height, int width)
{
    // Allocating memory for the map of the maze.
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
            // Here we are freeing the allocated memory if the file fails.
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

// This frees the memory allocated for the structure of maze.
void free_maze(maze *this)
{
    for (int i = 0; i < this->height; i++)
    {
        free(this->map[i]);
    }
    free(this->map);
}

// Calculates the width of the maze from file.
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

// Calculates the height of the maze from file.
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

// This reads the maze from the file and initializes the maze structure.
int read_maze(maze *this, FILE *file)
{
    this->width = get_width(file);
    this->height = get_height(file);
    if (this->width < MIN_DIM || this->width > MAX_DIM || this->height < MIN_DIM || this->height > MAX_DIM)
    {
        return 1;
    }
    if (create_maze(this, this->height, this->width))
    {
        return 1;
    }
    char ch;
    int start_count = 0, end_count = 0;
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
                start_count++;
            }
            else if (ch == 'E')
            {
                this->end.x = j;
                this->end.y = i;
                end_count++;
            }
            else if (ch != '#' && ch != ' ')
            {
                return 1; // Invalid character found
            }
            this->map[i][j] = ch;
        }
        ch = fgetc(file); // consume newline character
        if (ch != '\n' && ch != EOF)
        {
            return 1;
        }
    }
    if (start_count != 1 || end_count != 1)
    {
        return 1;
    }
    return 0;
}

// This prints the maze
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

// Player movement in the maze.
void make_move(maze *this, coord *player, char direction)
{
    int new_x = player->x;
    int new_y = player->y;
    switch (direction)
    {
    case 'w':
    case 'W':
        new_y--; // UP 
        break;
    case 'a':
    case 'A':
        new_x--; // LEFT
        break;
    case 's':
    case 'S':
        new_y++; // DOWN
        break;
    case 'd':
    case 'D':
        new_x++;// RIGHT
        break;
    default:
        return; // INVALID Move
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

// Checks if the player has reached the exit.
int has_won(maze *this, coord *player)
{
    return (player->x == this->end.x && player->y == this->end.y);
}

// Main function for the maze game.
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
        printf("Please check your maze file again. Its Invalid!\n");
        fclose(f);
        return EXIT_MAZE_ERROR;
    }

    fclose(f);

    player.x = this_maze.start.x;
    player.y = this_maze.start.y;

    char move;
    int show_map = 0; // Flag to indicate whether to show the map

    while (!has_won(&this_maze, &player))
    {
        printf("Enter your move (Press M for map): ");
        scanf(" %c", &move);
        if (move == 'm' || move == 'M')
        {
            show_map = 1; // Set the flag to show the map
        }
        else
        {
            make_move(&this_maze, &player, move);
        }

        if (show_map)
        {
            print_maze(&this_maze, &player);
            show_map = 0; // Reset the flag after displaying the map
        }
    }

    print_maze(&this_maze, &player);
    printf("Congratulations! You reached the Exit!\n");

    free_maze(&this_maze);

    return EXIT_SUCCESS;
}
