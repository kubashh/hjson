#include "../src/hjson.c"
#include "cJSON.c"

#define TEST_DIR "test/json/"

// #define TEST_HJSON
// #define TEST_FMT
#define TEST_PARSERS_PERFORMANCE

#define TEST_REPEAT_COUNT 1
const char* TEST_READ_PATHS[] = {
    TEST_DIR "bool.json",
    TEST_DIR "number.json",
    TEST_DIR "string.json",
    TEST_DIR "array.json",
    TEST_DIR "object.json",
    TEST_DIR "realCase.json",
    TEST_DIR "realCase2.json",
    TEST_DIR "realCase3.json",
    TEST_DIR "realCase4-configs.json",
    TEST_DIR "big.json",
    NULL
};

// TODO add hjson fast to benchmark
typedef struct {
    time_t hjson;
    time_t hjson_fast;
    time_t cjson;
} Benchmark;

static inline void printerr(char* str) {
    printf("\x1b[31mError: %s\x1b[0m\n", str);
}


void test_results_print(char* label, time_t hjson_time, time_t cjson_time) {
    char hjson_buf[14], cjson_buf[14], dif_buf[14];
    measure_fmt(hjson_buf, hjson_time);
    measure_fmt(cjson_buf, cjson_time);
    double dif = 100.0 * ((double)cjson_time / (double)hjson_time - 1);
    printf("%s x%d:\n  HJson: %s\n  cJSON: %s\n  Difference: %s%.2f%%\x1b[0m\n",
        label, TEST_REPEAT_COUNT, hjson_buf, cjson_buf, cjson_time < hjson_time ? "\x1b[31m" : "", dif);
}


void test_json_parsers_performance() {
    time_t hjson_all = 0, cjson_all = 0;

    char const* filename;
    time_t start, hjson_time, cjson_time;
    int i = 0, j;
    while((filename = TEST_READ_PATHS[i]) != NULL) {
        char* file_text = read_file_alloc(filename);
        if(!file_text) {
            printerr("No such file!\n");
            continue;
        }

        // HJson parse/free
        start = performance_now();
        for(j = 0; j < TEST_REPEAT_COUNT; j++) {
            HJson* json = HJson_parse(file_text);

            HJson_free(json);
        }
        hjson_time = measure_end(start);
        hjson_all += hjson_time;

        // cJSON parse/free
        start = performance_now();
        for(j = 0; j < TEST_REPEAT_COUNT; j++) {
            cJSON* json2 = cJSON_Parse(file_text);

            cJSON_Delete(json2);
        }
        cjson_time = measure_end(start);
        cjson_all += cjson_time;

        // Print info
        char buf[50];
        sprintf(buf, "%s (%.3fkb)", filename, (double)strlen(file_text) / 1000.0);
        test_results_print(buf, hjson_time, cjson_time);

        free(file_text);
        i++;
    }

    // Print All
    if(TEST_READ_PATHS[1] != NULL)
        test_results_print("\nAll", hjson_all, cjson_all);
}

void test_fmt() {
    HJson* json = HJson_parse_file(TEST_DIR "fmt.json");
    if(!json) {
        printerr("No such file!\n");
        return;
    }

    char* json_str = HJson_stringify(json);
    printf("%s\n", json_str);

    free(json_str);
    HJson_free(json);
}

void test_hjson() {
    // Parsing - no comments
    HJson* tmp;
    char* file_text = read_file_alloc(TEST_DIR "hjson.json");
    if(!file_text) {
        printerr("No such file!\n");
        return;
    }

    HJson* json = HJson_parse(file_text);

    if(!HJson_IsObject(json)) {
        printerr("no object");
    }

    if(!HJson_IsFalse(HJson_ObjectGet(json, "value1"))) {
        printerr("should be false");
    }

    tmp = HJson_ObjectGet(json, "value2");
    if(tmp->number != 3) {
        printf("%f %d\n", tmp->number, tmp->type);
        printerr("should be 3");
    }


    HJson_free(json);
    free(file_text);
    printf("Testing HJson parsing done.\n");


    // Parsing with comments

    char* file_text_com = read_file_alloc(TEST_DIR "comments.json");
    if(!file_text_com) {
        printerr("No such file!\n");
        return;
    }

    HJson* json_com = HJson_parse(file_text_com);

    if(!HJson_IsObject(json_com)) {
        printerr("no object");
    }

    if(!HJson_IsFalse(HJson_ObjectGet(json_com, "bool"))) {
        printerr("'bool' should be false");
    }

    tmp = HJson_ObjectGet(json_com, "num");
    if(!HJson_IsNumber(tmp)) {
        printerr("comments.num should be number!\n");
    }

    tmp = HJson_ObjectGet(json_com, "keys");
    if(HJson_IsObject(tmp)) {
        tmp = HJson_ObjectGet(tmp, "k1");
        if(!HJson_IsNumber(tmp) || tmp->number != 10) printerr("comments.keys.k1 should be k1 10 (number)");
    }

    HJson_free(json_com);
    free(file_text_com);
    printf("Testing HJson parsing with comments done.\n");
}

int main() {
#ifdef TEST_HJSON
    test_hjson();
#endif
#ifdef TEST_FMT
    test_fmt();
#endif
#ifdef TEST_PARSERS_PERFORMANCE
    test_json_parsers_performance();
#endif

    return 0;
}
