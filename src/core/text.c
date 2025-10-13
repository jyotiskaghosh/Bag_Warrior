#include "core/text.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static TextPrinter sTextPrinters[MAX_PRINTERS];

static int FindFreePrinter(void) {
    for (int i = 0; i < MAX_PRINTERS; ++i) if (!sTextPrinters[i].active) return i;
    return -1;
}

// Start a text printer
int AddTextPrinter(const TextPrinterTemplate *template, int framesPerChar) {
    int id = FindFreePrinter();
    if (id < 0) return -1;
    TextPrinter *printer = &sTextPrinters[id];
    printer->active = true;
    printer->template = *template;
    printer->cursor = 0;
    printer->framesPerChar = framesPerChar;
    printer->frameCounter = 0;

    return id;
}

static void UpdateTextPrinter(TextPrinter *printer) {
    if (printer->done || !printer->template.text || printer->framesPerChar == 0) return;

    printer->frameCounter++;
    if (printer->frameCounter >= printer->framesPerChar) {
        printer->frameCounter = 0;
        printer->cursor++;
    }
}

// Draw a single printer (draws its own window + the currently revealed text)
static void DrawPrinter(const TextPrinter *printer) {
    if (!printer->active) return;

    // Draw window background and frame
    DrawRectangleRec(printer->template.box, printer->template.bgColor);
    DrawRectangleLinesEx(printer->template.box, 1, printer->template.frameColor);

    // Use DrawTextRec to handle wrapping and newlines.
    // Note: DrawTextRec expects a char*; output buffer is stored as printer->output.
    DrawText(
        printer->framesPerChar == 0 ? printer->template.text : TextSubtext(printer->template.text, 0, printer->cursor), 
        printer->template.box.x + printer->template.padding, 
        printer->template.box.y + printer->template.padding, 
        printer->template.fontSize, 
        printer->template.textColor
    );
}

// Public API to draw all printers (each draws its own window)
void RunTextPrinters(void) {
    for (int i = 0; i < MAX_PRINTERS; ++i) {
        if (sTextPrinters[i].active) {
            UpdateTextPrinter(&sTextPrinters[i]);
            DrawPrinter(&sTextPrinters[i]);
        }
    }
}

// Stop (free) a printer
void StopTextPrinter(int id) {
    if (id < 0 || id >= MAX_PRINTERS) return;
    sTextPrinters[id].active = false;
}

void StopAllTextPrinters(void) {
    for (int i = 0; i < MAX_PRINTERS; ++i) 
        sTextPrinters[i].active = false;
}

void AddTextPrinterDefault(const char * str, Rectangle box, int framesPerChar) {
    const TextPrinterTemplate template = {
        .box = box,
        .padding = 4,
        .font = FONT_DEFAULT,
        .fontSize = 8,
        .bgColor = BLACK,
        .frameColor = WHITE,
        .textColor = WHITE
    };

    strcpy(template.text, str);

    AddTextPrinter(&template, framesPerChar); 
}