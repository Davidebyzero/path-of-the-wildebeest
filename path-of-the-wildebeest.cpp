#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>

typedef   signed long long int  int64;
typedef unsigned long long int Uint64;
typedef unsigned char Uchar;
typedef unsigned int  Uint;

#define countof(a) sizeof(a) / sizeof(a[0])

const Uint64 maxPositions = 12951523532uLL; // make this bigger if you want to change the problem parameters and see what happens
Uchar *visited;
int64 maxVisitedPos = 0;
int64 maxExaminedPos = 0;

#if 0
// This is the function I came up with
int64 coordToPos(int64 x, int64 y)
{
    int64 m = max(abs(x), abs(y));
    int64 a = 2*m-1;
    return a*a + (-y == m && x <= -y ? 3*m + 2*a + 1 + x
                            : x == m ? m + y - 1
                                     : 4*m - 1 - x - y);
}
#else
// This is a version adapted from https://math.stackexchange.com/a/2639611/293996; it is equivalent in function to mine
int64 coordToPos(int64 x, int64 y)
{
    int64 A = abs(abs(y) - abs(x)) + abs(y) + abs(x);
    int64 B = A + x - y;
    if (x + y > 0)
        B = -B;
    return A*A + B;
}
#endif

int main(int argc, char *argv[])
{
    fputs("Initializing...", stdout); fflush(stdout);
    visited = (Uchar*)malloc(((maxPositions - 1) >> 3) + 1);
    memset(visited, 0, maxPositions / 8);
    putchar('\n');

    int64 pos=0, x=0, y=0;

    struct {int x; int y;} moves[] =
    {
        {-3,-1},
        {-3, 1},
        {-2,-1},
        {-2, 1},
        {-1,-3},
        {-1,-2},
        {-1, 2},
        {-1, 3},
        { 1,-3},
        { 1,-2},
        { 1, 2},
        { 1, 3},
        { 2,-1},
        { 2, 1},
        { 3,-1},
        { 3, 1},
    };

    for (Uint64 numSteps=1;; numSteps++)
    {
        if ((numSteps & 0xFFFFF) == 0)
            printf("Step %llu: Position %lld (max: %lld)\n", numSteps, pos+1, maxVisitedPos+1);
        visited[pos >> 3] |= 1 << (pos & (8-1));
        if (maxVisitedPos < pos)
            maxVisitedPos = pos;
        if (maxExaminedPos < pos)
            maxExaminedPos = pos;
        int64 bestMove = LLONG_MAX;
        int64 bestMove_x;
        int64 bestMove_y;
        for (Uint i=0; i<countof(moves); i++)
        {
            int64 pos1;
            int64 x1 = x + moves[i].x;
            int64 y1 = y + moves[i].y;
            pos1 = coordToPos(x1, y1);
            if (pos1 >= maxPositions)
            {
                printf("Reached position %lld (max visited: %lld); considering going to %lld but this exceeds allocated array\n", pos+1, maxVisitedPos+1, pos1+1);
                goto out_of_memory;
            }
            if (maxExaminedPos < pos1)
                maxExaminedPos = pos1;
            if (bestMove > pos1 && (visited[pos1 >> 3] & (1 << (pos1 & (8-1)))) == 0)
            {
                bestMove = pos1;
                bestMove_x = x1;
                bestMove_y = y1;
            }
        }
        if (bestMove == LLONG_MAX)
        {
            printf("Trapped at step %lld at position %lld (max visited: %lld; max examined: %lld)\n", numSteps, pos+1, maxVisitedPos+1, maxExaminedPos+1);
            break;
        }
        pos = bestMove;
        x = bestMove_x;
        y = bestMove_y;
    }

out_of_memory:
    FILE *f = fopen("path-of-the-wildebeest.bin", "wb");
    fwrite(visited, ((maxVisitedPos+1 - 1) >> 3) + 1, 1, f);
    fclose(f);

    free(visited);
    return 0;
}
