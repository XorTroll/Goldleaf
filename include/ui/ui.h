#pragma once

#include <switch.h>

typedef enum
{
    ViewEntryType_None,
    ViewEntryType_Select,
    ViewEntryType_SelectInactive,
    ViewEntryType_Heading,
} ViewEntryType;

typedef struct
{
    ViewEntryType type;
    char text[256];
    void (*onSelected)(void);
} ViewEntry;

typedef struct _View
{
    ViewEntry viewEntries[44]; // 44 is the max number of lines we can print before going onto a new "page"
    unsigned char numEntries;
    struct _View *prevView;
    unsigned int cursorPos;
} View;

extern PrintConsole *g_console;
extern View *g_currentView;

void pushView(View *view);
void unwind(void);

void moveCursor(signed char off);
void onButtonPress(u64 kDown);

void displayCurrentView(void);
void displayCursor(void);
void clearCursor(void);