#include "renderer.h"
#include "video.h"
#include <stdarg.h>
#include "../kernel.h"

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

uint32_t MouseCursorBuffer[16 * 16];
uint32_t MouseCursorBufferAfter[16 * 16];
bool MouseDrawn;
static void limine_term_callback(struct limine_terminal *term, uint64_t t, uint64_t a, uint64_t b, uint64_t c);

volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0,
    .callback = limine_term_callback
};

static void limine_term_callback(struct limine_terminal *term, uint64_t t, uint64_t a, uint64_t b, uint64_t c) {
    if (t == 50)
    {
        Point termPos;
        termPos.X = a;
        termPos.Y = b;
        comout("termPos.X: ");
        comout(to_string(termPos.X));
        comout("\ntermPos.Y: ");
        comout(to_string(termPos.Y));
        comout("\n\n");
    }
}

void printf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    const char *ptr;
    const char *strPtr;

    int i = 0;
    for (ptr = format; *ptr != '\0'; ptr++)
    {
        if (*ptr == '%')
        {
            ptr++;
            switch (*ptr)
            {
                case 's':
                    strPtr = va_arg(ap, const char*);
                    terminal_request.response->write(terminal, strPtr, mystrlen(strPtr));
                    break;
                case '%':
                    putChar('%');
                    break;
                case 'i':
                    strPtr = to_string(va_arg(ap, uint64_t));
                    terminal_request.response->write(terminal, strPtr, mystrlen(strPtr));
                    break;
                case 'x':
                    strPtr = to_hstring(va_arg(ap, uint64_t));
                    terminal_request.response->write(terminal, "0x", 2);
                    terminal_request.response->write(terminal, strPtr, mystrlen(strPtr));
                    break;
            }
        }
        else
        {
            putChar(*ptr);
        }
        i++;
    }
    va_end(ap);
}

void gPrint(const char chr[], unsigned int xOff, unsigned int yOff, uint32_t Color)
{
    unsigned int *pixPtr = (unsigned int *)buffer->address;
    int offset = 0;
    for (unsigned int i = 0; i < mystrlen(chr); i++)
    {
        char *fontPtr = (char *)font->address + (chr[i] * 16) + 4;
        for (unsigned long y = yOff; y < yOff + 16; y++)
        {
            for (unsigned long x = xOff; x < xOff + 8; x++)
            {
                if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
                {
                    *(unsigned int *)(pixPtr + x + offset + (y * (buffer->pitch / 4))) = Color;
                }
            }
            fontPtr++;
        }    
        offset += 8;
    }
}

void gPutChar(char letter, int startx, int starty)
{

}

void putChar(char c)
{
    char yes[2] = {c, '\0'};
    terminal_request.response->write(terminal, yes, 2);
}

void ClearMouseCursor(uint8_t *mouseCursor, Point position)
{
    if (!MouseDrawn)
        return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = buffer->width - position.X;
    int differenceY = buffer->height - position.Y;

    if (differenceX < 16)
        xMax = differenceX;
    if (differenceY < 16)
        yMax = differenceY;

    for (int y = 0; y < yMax; y++)
    {
        for (int x = 0; x < xMax; x++)
        {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                if (getPixel(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y * 16])
                {
                    putPixel(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}

void DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, uint32_t color)
{
    int xMax = 16;
    int yMax = 16;
    int differenceX = buffer->width - position.X;
    int differenceY = buffer->height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                MouseCursorBuffer[x + y * 16] = getPixel(position.X + x, position.Y + y);
                putPixel(position.X + x, position.Y + y, color);
                MouseCursorBufferAfter[x + y * 16] = getPixel(position.X + x, position.Y + y);

            }
        }
    }
    MouseDrawn = true;
}