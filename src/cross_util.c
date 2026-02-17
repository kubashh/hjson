#pragma once
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"

// just for IDE
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif


//
// string
//

char* duplicate_string(const char* str);

// Duplicate string and allocate
char* duplicate_string(const char* str) {
    if (str == NULL) return NULL;
    char* dup_str = malloc(strlen(str) + 1);
    if (dup_str) {
        strcpy(dup_str, str);
    }
    return dup_str;
}


//
// performance_now()
//

time_t performance_now();



// Get current time in nanoseconds
time_t performance_now() {
#ifdef _WIN32
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / frequency.QuadPart; // Convert to nanoseconds

#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // CLOCK_MONOTONIC for a steady clock
    return (time_t)(ts.tv_sec) * 1000000000LL + ts.tv_nsec; // Convert to nanoseconds

#endif
}


//
// measure_end
//

time_t measure_end(const time_t start);
void measure_fmt(char* buf, const time_t micro);
void measure_end_buf(char* buf, const time_t start);


time_t measure_end(const time_t start_nano) {
    return performance_now() - start_nano;
}

void measure_fmt(char* buf, const time_t nano) {
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

void measure_end_buf(char* buf, const time_t start) {
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
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);  // Move back to the beginning of the file

    // Allocate memory for the file content, including space for the null terminator
    char *content = (char *)malloc(fileSize + 1);
    if (content == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // Read the file contents into the allocated memory
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';  // Null-terminate the string

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
