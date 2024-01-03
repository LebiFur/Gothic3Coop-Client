#include <stdio.h>

#include "Script.h"

void Print(char *message)
{
    bCUnicodeString msg = bCUnicodeString(message);

    gui2.PrintGameMessage(msg, gEGameMessageType::gEGameMessageType_White_3);
    printf(message);
}

float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}