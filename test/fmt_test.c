#include "../hjson.c"
#include "cjson/cJSON.c"
#include "cross_util.c"

#define TEST_DIR "test/json/"

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

int main() {
    test_fmt();

    return 0;
}
