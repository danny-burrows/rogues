#include "drawing.h"

void Draw_Buffer_Fill(Draw_Buffer *buffer, char *source_string, int source_start_x, int source_start_y) 
{
    char *xstart, *xend;

    int wid = 0;
    char * curLine = source_string;
    int i = 0;
    int line_no = 0;
    while(curLine && i < buffer->h)
    {
        char * nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
        
        if (line_no > source_start_y){
            xstart = curLine + (source_start_x * 12);
            xend = curLine + (source_start_x * 12) + (buffer->w * 12) - 1;

            char r = *xend;
            *xend = '\0';
            
            strcpy((char *)buffer->data[i], xstart);

            *xend = r;
            i++;
        }
        
        if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy    
        curLine = nextLine ? (nextLine+1) : NULL;
        line_no++;
    }
}

int Draw_Buffer_AddString(Draw_Buffer *buffer, const char *string, int string_len, int x, int y)
{
    if (x < 0 || y < 0 || string_len < 1) return -1;

    int tx = x;
    int ty = y;
    while (*string) {

        // If we see a \n char we add a newline.
        if (*string == '\n') {
            if (ty >= buffer->h) return -1;
            tx = x;
            ty++; string++;
            continue;
        }

        if (tx >= buffer->w * 12) return -1;

        buffer->data[ty][tx] = *string;

        tx++; string++;
    }

    return 0;
}

void Draw_Buffer_Display(Draw_Buffer *buffer, int draw_start_x, int draw_start_y) 
{
    for (int i = 0; i < buffer->h; i++) {
        printf("\033[%d;%dH%s", draw_start_y + i, draw_start_x, buffer->data[i]);
    }

    printf("\033[0m");
    fflush(stdout);
}
