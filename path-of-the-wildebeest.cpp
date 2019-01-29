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
#define max(a,b) (((a) > (b)) ? (a) : (b))

const Uint64 maxPositions = 1000000000uLL; // you may need to make this bigger if you change the problem parameters
Uchar *visited;
int64 maxVisitedPos = 0;
int64 visitedOffset = 0;

#define USE_MOVE_LOOKUP_TABLE

// Adapted from the explanation on https://codegolf.stackexchange.com/a/179158
// see also https://math.stackexchange.com/a/2639611/293996
int64 coordToPos(int64 x, int64 y)
{
    int64 L = 2 * max(abs(x), abs(y));
    int64 P = L + x + y;
    return L*L + (x > y ? -P : P);
}

void writeVisitedToFile()
{
    char filename[500];
    sprintf(filename, "path-of-the-wildebeest_offset_%d_bytes.bin", visitedOffset);
    FILE *f = fopen(filename, "wb");
    size_t size = ((maxVisitedPos >> 3) + 1) - visitedOffset;
    Uchar *p = visited;
    for (;;)
    {
        if (size > (1uLL << 31))
        {
            fwrite(p, 1uLL << 31, 1, f);
            size   -= 1uLL << 31;
            p      += 1uLL << 31;
        }
        else
        {
            fwrite(visited, size, 1, f);
            break;
        }
    }
    fclose(f);
}

void pruneVisitedArray()
{
    int64 prePruneVisitedSize = ((maxVisitedPos >> 3) + 1) - visitedOffset;
    int64 pruneOffset=0;
    for (; pruneOffset < prePruneVisitedSize; pruneOffset++)
        if (visited[pruneOffset] != (Uchar)~0)
            break;
    memmove(visited, visited + pruneOffset, prePruneVisitedSize - pruneOffset);
    visitedOffset += pruneOffset;
    memset(visited + (prePruneVisitedSize - pruneOffset), 0, pruneOffset);
}

int main(int argc, char *argv[])
{
    fputs("Initializing...", stdout); fflush(stdout);
    int64 visitedArraySize = ((maxPositions - 1) >> 3) + 1;
    visited = (Uchar*)malloc(visitedArraySize);
    memset(visited, 0, visitedArraySize);
    putchar('\n');

    int64 pos=0, x=0, y=0;

#ifdef USE_MOVE_LOOKUP_TABLE
    struct {char x; char y;} moves[] =
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
#endif

    for (Uint64 numSteps=1;; numSteps++)
    {
        int64 newVisitOffset = (pos >> 3) - visitedOffset;
        if (newVisitOffset >= visitedArraySize)
        {
            pruneVisitedArray();

            newVisitOffset = (pos >> 3) - visitedOffset;
            if (newVisitOffset >= visitedArraySize)
            {
                printf("Reached position %lld (pruned to %lld; max previously visited: %lld); this exceeds allocated array\n", pos+1, visitedOffset << 3, maxVisitedPos+1);
                goto out_of_memory;
            }
        }
        visited[newVisitOffset] |= 1 << (pos & (8-1));
        if (maxVisitedPos < pos)
            maxVisitedPos = pos;
        if ((numSteps & 0xFFFFF) == 0)
            printf("Step %llu: Position %lld (max: %lld); pruned to %lld\n", numSteps, pos+1, maxVisitedPos+1, visitedOffset << 3);
        int64 bestMove = LLONG_MAX;
        int64 bestMove_x;
        int64 bestMove_y;
#ifdef USE_MOVE_LOOKUP_TABLE
        for (Uint i=0; i<countof(moves); i++)
#else
        for (int i=0; i<16; i++)
#endif
        {
            int64 pos1;
#ifdef USE_MOVE_LOOKUP_TABLE
            int64 x1 = x + moves[i].x;
            int64 y1 = y + moves[i].y;
#else
            char move[2];
            move[    i / 8] = (i & 2) - 1;
            move[1 - i / 8] = (i & 4) - 2 + (((i & 4) / 2 - 1) & -(i & 1));
            int64 x1 = x + move[0];
            int64 y1 = y + move[1];
#endif
            pos1 = coordToPos(x1, y1);
            int64 arrayPos = (pos1 >> 3) - visitedOffset;
            if (bestMove > pos1 && arrayPos >= 0 && (arrayPos >= visitedArraySize || (visited[arrayPos] & (1 << (pos1 & (8-1)))) == 0))
            {
                bestMove = pos1;
                bestMove_x = x1;
                bestMove_y = y1;
            }
        }
        if (bestMove == LLONG_MAX)
        {
            printf("Trapped at step %lld at position %lld (max visited: %lld)\n", numSteps, pos+1, maxVisitedPos+1);
            pruneVisitedArray();
            break;
        }
        pos = bestMove;
        x = bestMove_x;
        y = bestMove_y;
    }

out_of_memory:
    writeVisitedToFile();

    free(visited);
    return 0;
}
