#include "../hjson.c"
#include "cjson/cJSON.c"
#include "cross_util.c"

#define TEST_DIR "test/json/"
#define OUT_BUF_MAX_LEN 1024 * 16
char* ALL_STR = "All";

typedef struct OutEntry {
    char* path;
    int file_size;
    int repeat_count;
    uint64_t hjson_time;
    uint64_t cjson_time;
} OutEntry;

typedef struct Benchmark {
    int repeat_count;
    OutEntry out_entries[4 * (11 + 1)];
    int out_entries_len;
    char* paths[];
} Benchmark;


Benchmark benchmark = (Benchmark){
    .out_entries = NULL,
    .out_entries_len = 0,
    .paths = {
        TEST_DIR "bool.json",
        TEST_DIR "number.json",
        TEST_DIR "string.json",
        TEST_DIR "array.json",
        TEST_DIR "object.json",
        TEST_DIR "realCase.json",
        TEST_DIR "realCase2.json",
        TEST_DIR "realCase3.json",
        TEST_DIR "realCase4-configs.json",
        TEST_DIR "small.json",
        TEST_DIR "big.json",
        NULL
    },
};

void out_entry_add(char* path, int file_size, int repeat_count, uint64_t hjson_time, uint64_t cjson_time) {
    OutEntry* out_entry = &benchmark.out_entries[benchmark.out_entries_len++];
    out_entry->path = path;
    out_entry->file_size = file_size;
    out_entry->repeat_count = repeat_count;
    out_entry->hjson_time = hjson_time;
    out_entry->cjson_time = cjson_time;
}


int print_results_buf(char* buf, OutEntry entry) {
    char hjson_buf[16], cjson_buf[16], dif_buf[16];
    measure_fmt(hjson_buf, entry.hjson_time);
    measure_fmt(cjson_buf, entry.cjson_time);
    double dif = 100.0 * ((double)entry.cjson_time / (double)entry.hjson_time - 1.0);
    return sprintf(buf, "%s x%d:\n  HJson: %s\n  cJSON: %s\n  Difference: %s%.2f%%\x1b[0m\n",
        entry.path, entry.repeat_count, hjson_buf, cjson_buf, entry.cjson_time < entry.hjson_time ? "\x1b[31m" : "", dif);
}

void print_results() {
    char out_buf[OUT_BUF_MAX_LEN];
    int out_buf_len = 0;
    for(int i = 0; i < benchmark.out_entries_len; i++) {
        OutEntry entry = benchmark.out_entries[i];
        // char buf[48];
        char hjson_buf[16], cjson_buf[16], dif_buf[16];
        measure_fmt(hjson_buf, entry.hjson_time);
        measure_fmt(cjson_buf, entry.cjson_time);
        double dif = 100.0 * ((double)entry.cjson_time / (double)entry.hjson_time - 1.0);
        // sprintf(buf, "%s (%.3fKB)", entry.path, (double)entry.file_size / 1000.0);
        out_buf_len += sprintf(&out_buf[out_buf_len], "%s (%dKB) x%d:\n  HJson: %s\n  cJSON: %s\n  Difference: %s%.2f%%\x1b[0m\n",
            entry.path, entry.file_size, entry.repeat_count, hjson_buf, cjson_buf,
            entry.cjson_time < entry.hjson_time ? "\x1b[31m" : "", dif);
    }
    puts(out_buf);
}

void make_md() {
    char out_buf[OUT_BUF_MAX_LEN];
    int out_buf_len = 0;

    // Get date
    time_t now;                 // stores current time
    struct tm *local_time;      // stores broken-down local time
    char date_buf[16];
    char md_out_path_buf[32];

    // Get current time
    time(&now);

    // Convert to local time
    local_time = localtime(&now);

    // Format time as string
    strftime(date_buf, sizeof(date_buf), "%d %b %Y", local_time);
    strftime(md_out_path_buf, sizeof(md_out_path_buf), "bench/bench-%d-%m-%Y.md", local_time);

    out_buf_len += sprintf(&out_buf[out_buf_len], "# Benchmark HJson (%s) vs cJSON (%d.%d.%d). %s\n\n",
        HJson_Version, CJSON_VERSION_MAJOR, CJSON_VERSION_MINOR, CJSON_VERSION_PATCH, date_buf);

    int entry_repeat = 0;
    for(int i = 0; i < benchmark.out_entries_len; i++) {
        OutEntry entry = benchmark.out_entries[i];
        if(entry_repeat != entry.repeat_count) {
            entry_repeat = entry.repeat_count;
            out_buf_len += sprintf(&out_buf[out_buf_len],
                "## x%d\n"
                "| path (size) | hjson | cjson | difference (%) |\n"
                "| ----------- | ----- | ----- | -------------- |\n", entry_repeat);
        }

        char hjson_buf[16], cjson_buf[16], dif_buf[16];
        measure_fmt(hjson_buf, entry.hjson_time);
        measure_fmt(cjson_buf, entry.cjson_time);
        double dif = 100.0 * ((double)entry.cjson_time / (double)entry.hjson_time - 1.0);
        out_buf_len += sprintf(&out_buf[out_buf_len], "| %s (%.2fKB) | %s | %s | %.2f%% |\n",
            entry.path, (double)(entry.file_size / 1000.0), hjson_buf, cjson_buf, dif);
    }

    write_file(md_out_path_buf, out_buf);
}


void test_json_parsers_performance(int repeat_count) {
    uint64_t hjson_all = 0, cjson_all = 0;
    int files_text_size = 0;

    char* filename;
    uint64_t start, hjson_time, cjson_time;
    for(int i = 0, j; (filename = benchmark.paths[i]) != NULL; i++) {
        char* file_text = read_file_alloc(filename);
        if(!file_text) {
            printerr("No such file!\n");
            continue;
        }

        // HJson parse/free
        start = performance_now();
        for(j = 0; j < repeat_count; j++) {
            HJson* json = HJson_parse(file_text);

            HJson_free(json);
        }
        hjson_time = measure_end(start);
        hjson_all += hjson_time;

        // cJSON parse/free
        start = performance_now();
        for(j = 0; j < repeat_count; j++) {
            cJSON* json2 = cJSON_Parse(file_text);

            cJSON_Delete(json2);
        }
        cjson_time = measure_end(start);
        cjson_all += cjson_time;

        int file_len = strlen(file_text) + 1;
        out_entry_add(&filename[sizeof(TEST_DIR) - 1], file_len, repeat_count, hjson_time, cjson_time);
        files_text_size += file_len;

        // Print info
        char buf[48];
        sprintf(buf, "%s (%.3fKB)\n\n", filename, (double)strlen(file_text) / 1000.0);

        free(file_text);
    }



    // Print All
    if(benchmark.paths[1] != NULL)
        out_entry_add(ALL_STR, files_text_size, repeat_count, hjson_all, cjson_all);
}

int main(int args_len, char* args[]) {
    test_json_parsers_performance(1);
    test_json_parsers_performance(33);
    test_json_parsers_performance(1000);

    print_results();

    if(args_len > 1 && strcmp(args[1], "-m"))
        make_md();

    return 0;
}
