#pragma once

#include <raylib.h>

#define MAX_PRINTERS 16

typedef struct {
    char text[64];

    // Position & visual
    Font font;                      // x,y,width,height (window)
    Color textColor;
    Color frameColor;
    Color bgColor;
    
    Rectangle box;

    int padding;                    // interior padding for text
    int fontSize;
} TextPrinterTemplate;

typedef struct  {
    TextPrinterTemplate template;

    int cursor;                     // text cursor

    // Typewriter timing
    int framesPerChar;              // frames per character (0 => instant)
    int frameCounter;
    
    bool active;
    bool done;
} TextPrinter;

int AddTextPrinter(const TextPrinterTemplate *template, int framesPerChar);
void RunTextPrinters(void);
void StopTextPrinter(int id);
void StopAllTextPrinters(void);

void AddTextPrinterDefault(const char * str, Rectangle box, int framesPerChar);
