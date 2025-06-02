#include "shell.h"
#include "memory/mem.h"
#include "fs/ramfs.h"
#include "../tools/picsofbread.h"
#include "video/window.h"

char NataluzzVer[] = "7120";

Shell newShell;
bool xRunning = false;

void Shell::PrintPrompt()
{
    if (shouldPrint)
    {
        // Plain prompt without color escape sequences
        printf("root@Nataluzz / # ");
    }
}

char ver[]      = "ver";
char help[]     = "help";
char clear[]    = "clear";
char neofetch[] = "neofetch";
char rm[]       = "rm";
char panic[]    = "panic";
char exitx[]    = "exitx";
char startx[]   = "startx";
char pob[]      = "pob";
char tree[]     = "tree";
char cat[]      = "cat";
char lsStr[]    = "ls";
char shutdown[] = "shutdown";
char blank[]    = "";

void Shell::TestCMD(char* input)
{
    TwoStrings twoStrings;
    for (int i = 0; input[i] != '\0'; i++) {
        twoStrings.a[i] = '\0';
    }
    for (int i = 0; input[i] != '\0'; i++) {
        twoStrings.b[i] = '\0';
    }

    twoStrings = strsplit(input, ' ');

    if (mystrcmp(input, "ver"))
    {
        printf("Nataluzz version %s\n", NataluzzVer);
    }
    else if (mystrcmp(input, help))
    {
        printf("List of available commands:\n");
        printf("    ver:         shows the version of Nataluzz\n");
        printf("    help:        shows this menu\n");
        printf("    clear:       clears the screen\n");
        printf("    neofetch:    fancy system display\n");
    }
    else if (mystrcmp(input, "clear"))
    {
        Clear(0);
    }
    else if (mystrcmp(input, "neofetch"))
    {
        uint64_t totMem = 0;
        uint64_t useMem = 0;
        uint64_t freMem = 0;
        for (uint64_t i = 0; i < mmap.response->entry_count; i++)
        {
            uint64_t type = mmap.response->entries[i]->type;
            if (type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
            {
                freMem += mmap.response->entries[i]->length;
            }
            else
            {
                useMem += mmap.response->entries[i]->length;
            }
        }
        totMem = useMem + freMem;

        // ANSI color codes for neofetch
        const char* red     = "\033[31m";
        const char* green   = "\033[32m";
        const char* yellow  = "\033[33m";
        const char* blue    = "\033[34m";
        const char* magenta = "\033[35m";
        const char* cyan    = "\033[36m";
        const char* white   = "\033[37m";
        const char* reset   = "\033[0m";

        // Top ASCII art
        printf("\n");
        printf("%s              _        _                      %sroot@%sNataluzz%s\n", cyan, red, blue, reset);
        printf("%s  _ __   __ _| |_ __ _| |_   _ ________       --------------%s\n", cyan, reset);
        printf("%s | '_ \\ / _` | __/ _` | | | | |_  /_  /      OS: %sNataluzz %s%s\n", cyan, green, white, reset);
        printf("%s | | | | (_| | || (_| | | |_| |/ / / /       Kernel: %sNataluzz %s%s\n", cyan, green, white, reset);
        printf("%s |_| |_|\\__,_|\\__\\__,_|_|\\__,_/___/___|   Shell: %ssystemd (natalietextmeback)%s\n", cyan, magenta, reset);
        printf("                                              Memory: %i KB / %i KB\n", useMem / 1024, totMem / 1024);
        printf("                                              Resolution: %ix%i\n", buffer->width, buffer->height);
        printf("\n");

        // Bottom ASCII art (same as top) //AI formatted so leave it as is because it works
        printf("%s              _        _                      \n  _ __   __ _| |_ __ _| |_   _ ________       \n | '_ \\ / _` | __/ _` | | | | |_  /_  /        \n | | | | (_| | || (_| | | |_| |/ / / /         \n |_| |_|\\__,_|\\__\\__,_|_|\\__,_/___/___|       %s\n\n", cyan, reset);
        printf("%shi guys");
    }
    else if (mystrcmp(input, ""))
    {
        // Do nothing on empty input
    }
    else if (mystrcmp(input, "rm"))
    {
        printf("your 'rm' program is bad, sorry.\n");
    }
    else if (mystrcmp(input, "panic"))
    {
        Panic("User Caused Panic");
    }
    else if (mystrcmp(input, "exitx"))
    {
        xRunning = false;
        Clear(0);
        newShell.shouldPrint = true;
    }
    else if (mystrcmp(input, "startx"))
    {
        xRunning = true;
        drawRect(0, 0, buffer->width, buffer->height, 0xD97F1956);
        Window yes = Window(10, 10, 200, 200, "Hello World        ", 0x12345678);
        Window no = Window(100, 100, 200, 200, "1234567890!@#$%^&*()", 0x12345678);

        yes.DrawWindow();
        no.DrawWindow();

        drawRect(0, buffer->height - 40, buffer->width, 40, 0xff888888);

        newShell.shouldPrint = false;
    }
    else if (mystrcmp(input, "pob"))
    {
        drawImage(g_picsofbread_data, buffer->width / 2 - 36, buffer->height / 2 - 36);
    }
    else if (mystrcmp(input, "tree"))
    {
        for (int i = 0; headers[i]->filename != "\0"; i++)
        {
            printf("%s\n", headers[i]->filename);
        }
    }
    else if (mystrcmp(twoStrings.a, "cat"))
    {
        printf(readFile(twoStrings.b));
    }
    else if (mystrcmp(input, "ls"))
    {
        ls();
    }
    else if (mystrcmp(input, "shutdown"))
    {
        Clear(0);
        printf("SHUT DOWN SEQUENCE INITIATED");
        sleep(2);
        outw(0xB004, 0x2000);
        outw(0x604, 0x2000);
        outw(0x4004, 0x3400);
    }
    else
    {
        printf("Unknown command: \"%s\"\n", input);
    }
}
