#ifndef MP_OUTPUT_H_INCLUDED
#define MP_OUTPUT_H_INCLUDED

template<typename ... Args>
char* Format(char *value, Args ... args)
{
    size_t size = snprintf(nullptr, 0, value, args ... ) + 1;
    char* buffer = new char[size];
    snprintf(buffer, size, value, args ...);
    return buffer;
}

void Print(char *message);

float Lerp(float a, float b, float t);

#endif