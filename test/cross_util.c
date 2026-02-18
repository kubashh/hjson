#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "time.h"

// just for IDE
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif
#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW 4
#endif

// Headers
char* duplicate_string(const char* str);
uint64_t performance_now();
uint64_t measure_end(const uint64_t start);
void measure_fmt(char* buf, const uint64_t micro);
void measure_end_buf(char* buf, const uint64_t start);
char* read_file_alloc(const char* const filename);
bool write_file(const char* const path, const char* const src);



//
// performance_now()
//

// Get current time in nanoseconds
uint64_t performance_now() {
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / frequency.QuadPart; // Convert to nanoseconds

#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts); // CLOCK_MONOTONIC for a steady clock
    return (uint64_t)(ts.tv_sec) * 1000000000LL + ts.tv_nsec; // Convert to nanoseconds

#endif
}


//
// measure_end
//

uint64_t measure_end(const uint64_t start_nano) {
    return performance_now() - start_nano;
}

void measure_fmt(char* buf, const uint64_t nano) {
    if(nano < 1000) {
        sprintf(buf, "%3.2fns", (double)nano);
    } else if(nano < 1000000) {
        sprintf(buf, "%3.2fμs", (double)nano / 1000.0);
    } else if(nano < 1000000000) {
        sprintf(buf, "%3.2fms", (double)nano / 1000000.0);
    } else if(nano < 1000000000000LL) {
        sprintf(buf, "%3.2fs", (double)nano / 1000000000.0);
    } else if(nano < 60LL * 1000000000000LL) {
        sprintf(buf, "%3.2fs", (double)nano / (60.0 * 1000000000.0));
    } else {
        sprintf(buf, "%3.2fs", (double)nano / (360.0 * 1000000000.0));
    }
}

void measure_end_buf(char* buf, const uint64_t start) {
    measure_fmt(buf, performance_now() - start);
}


//
// Files
//

char* read_file_alloc(const char* const filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;

    // Move the file pointer to the end of the file to get the size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // Move back to the beginning of the file

    // Allocate memory for the file content, including space for the null terminator
    char *content = (char *)malloc(file_size + 1);
    if (content == NULL) {
        fclose(file);
        return NULL;
    }

    // Read the file contents into the allocated memory
    size_t bytes_reed = fread(content, 1, file_size, file);
    content[bytes_reed] = '\0';  // Null-terminate the string

    // Close the file
    fclose(file);

    return content;
}

bool write_file(const char* const path, const char* const src) {
    FILE* file = fopen(path, "w");

    if (file == NULL) return true;

    fputs(src, file);

    // Close the file when done
    fclose(file);
}

//
// error
//

void printerr(char* str) {
    printf("\x1b[31mError: %s\x1b[0m\n", str);
}
