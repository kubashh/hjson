#pragma once

// TODO move ALL includes on file top


//
// cross include
//

// just for IDE
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 0
#endif

// All platforms
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "time.h"
#include "dirent.h"


#ifdef _WIN32 // Windows

#include "windows.h" // Sleep

#else // Posix (Linux / Mac)

#include "sys/time.h"

#endif



//
// int, uint, float
//

#define u8 unsigned char
#define u32 uint32_t

#define usize size_t
#define isize ssize_t

#define f32 float
#define f64 double


//
// string
//

u8* duplicate_string(const u8* str);

// Duplicate string and allocate
u8* duplicate_string(const u8* str) {
    if (str == NULL) return NULL;
    u8* dup_str = malloc(strlen(str) + 1);
    if (dup_str) {
        strcpy(dup_str, str);
    }
    return dup_str;
}

//
// getchar
//

#ifdef _WIN32
    #include "conio.h"  // _getch
#else
    #include "termios.h" // termios
    #include "unistd.h" // tcgetattr, tcsetattr, read, tcsetattr
#endif

// Cross-platform function to get a single character
u8 cross_getchar() {
#ifdef _WIN32
    return _getch();  // Reads a single key without waiting for Enter
#else
    struct termios oldt, newt;
    u8 ch;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    read(STDIN_FILENO, &ch, 1);  // Read 1 character

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
#endif
}


//
// thread
//

#ifdef _WIN32
    #include "windows.h" // HANDLE, DWORD, CreateThread, WaitForSingleObject, CloseHandle
    typedef HANDLE cross_thread_t;
    typedef DWORD thread_return_t;
#else
    #include "pthread.h" // pthread_t, pthread_create, pthread_join
    typedef pthread_t cross_thread_t;
    typedef void* thread_return_t;
#endif

typedef thread_return_t (*thread_func_t)(void *);

u32 cross_thread_create_basic(cross_thread_t *t, thread_func_t func);
cross_thread_t cross_thread_create(thread_func_t func);
u32 cross_thread_join(cross_thread_t t);


#ifdef _WIN32

static DWORD WINAPI win_thread_func(LPVOID arg) {
    thread_func_t func = ((thread_func_t*)arg)[0];
    void *farg = ((void**)arg)[1];
    free(arg);
    return func(farg);
}

u32 cross_thread_create(cross_thread_t *t, thread_func_t func, void *arg) {
    void **pack = malloc(2 * sizeof(void*));
    pack[0] = func;
    pack[1] = arg;
    *t = CreateThread(NULL, 0, win_thread_func, pack, 0, NULL);
    return (*t == NULL);
}

u32 cross_thread_join(cross_thread_t t) {
    WaitForSingleObject(t, INFINITE);
    CloseHandle(t);
    return 0;
}

#else // POSIX

u32 cross_thread_create_basic(cross_thread_t *t, thread_func_t func) {
    return pthread_create(t, NULL, func, NULL);
}

u32 cross_thread_join(cross_thread_t t) {
    return pthread_join(t, NULL);
}

#endif

cross_thread_t cross_thread_create(thread_func_t func) {
    cross_thread_t t;
    cross_thread_create_basic(&t, func);
    return t;
}

//
// clear
//

void cross_clear();

void cross_clear() {
#if _WIN32
    system("cls");
#else
    system("clear");
#endif
}


//
// performance_now()
//

time_t performance_now();




#ifdef _WIN32

// Get current time in nanoseconds
u64 performance_now() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / frequency.QuadPart; // Convert to nanoseconds
}

#else

// Get current time in nanoseconds
time_t performance_now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // CLOCK_MONOTONIC for a steady clock
    return (time_t)(ts.tv_sec) * 1000000000LL + ts.tv_nsec; // Convert to nanoseconds
}

#endif


//
// measure_end
//

inline time_t measure_end(const time_t start);
inline void measure_end_print(const u8* const label, const time_t start);
inline void measure_fmt(u8* buf, const time_t micro);
inline void measure_end_buf(u8* buf, const time_t start);


time_t measure_end(const time_t start_nano) {
    return performance_now() - start_nano;
}

void measure_end_print(const u8* const label, time_t nano) {
    nano = performance_now() - nano;
    if(nano < 1000) {
        printf("%s: %3.2fns\n", label, (f64)nano);
    } else if(nano < 1000000) {
        printf("%s: %3.2fμs\n", label, (f64)nano / 1000.0);
    } else if(nano < 1000000000) {
        printf("%s: %3.2fms\n", label, (f64)nano / 1000000.0);
    } else if(nano < 1000000000000LL) {
        printf("%s: %3.2fs\n", label, (f64)nano / 1000000000.0);
    } else if(nano < 60LL * 1000000000000LL) {
        printf("%s: %3.2fs\n", label, (f64)nano / (60.0 * 1000000000.0));
    } else {
        printf("%s: %3.2fs\n", label, (f64)nano / (360.0 * 1000000000.0));
    }
}

void measure_fmt(u8* buf, const time_t nano) {
    if(nano < 1000) {
        sprintf(buf, "%3.2fns", (f64)nano);
    } else if(nano < 1000000) {
        sprintf(buf, "%3.2fμs", (f64)nano / 1000.0);
    } else if(nano < 1000000000) {
        sprintf(buf, "%3.2fms", (f64)nano / 1000000.0);
    } else if(nano < 1000000000000LL) {
        sprintf(buf, "%3.2fs", (f64)nano / 1000000000.0);
    } else if(nano < 60LL * 1000000000000LL) {
        sprintf(buf, "%3.2fs", (f64)nano / (60.0 * 1000000000.0));
    } else {
        sprintf(buf, "%3.2fs", (f64)nano / (360.0 * 1000000000.0));
    }
    // if(micro < 1000) {
    //     sprintf(buf, "%ldμs", micro);
    // } else if(micro < 1000000) {
    //     sprintf(buf, "%.3fms", micro / 1000.0);
    // } else if(micro < 1000000000) {
    //     sprintf(buf, "%.3fs", micro / 1000000.0);
    // } else if(micro < 60LL * 1000000000LL) {
    //     sprintf(buf, "%.3fm", micro / (60.0 * 1000000.0));
    // } else {
    //     sprintf(buf, "%.3fh", micro / (360.0 * 1000000.0));
    // }
}

void measure_end_buf(u8* buf, const time_t start) {
    measure_fmt(buf, performance_now() - start);
}


//
// Files
//

u8* read_file_alloc(const u8* const filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;

    // Move the file pointer to the end of the file to get the size
    fseek(file, 0, SEEK_END);
    u32 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);  // Move back to the beginning of the file

    // Allocate memory for the file content, including space for the null terminator
    u8 *content = (u8 *)malloc(fileSize + 1);
    if (content == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // Read the file contents into the allocated memory
    u32 bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';  // Null-terminate the string

    // Close the file
    fclose(file);

    return content;
}

bool write_file(const u8* const path, const u8* const src) {
    FILE* file = fopen(path, "w");

    if (file == NULL) return true;

    fputs(src, file);

    // Close the file when done
    fclose(file);
}
