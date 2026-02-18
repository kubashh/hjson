#include "../hjson.c"
#include "cjson/cJSON.c"
#include "cross_util.c"

#define TEST_DIR "test/json/"

void test_hjson() {
    // Parsing - no comments
    HJson* tmp;
    char* file_text = read_file_alloc(TEST_DIR "hjson.json");
    if(!file_text) {
        printerr("No such file!\n");
        return;
    }

    HJson* json = HJson_parse(file_text);

    if(!HJson_is_object(json)) {
        printerr("no object");
    }

    if(!HJson_is_false(HJson_object_get(json, "value1"))) {
        printerr("should be false");
    }

    tmp = HJson_object_get(json, "value2");
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

    if(!HJson_is_object(json_com)) {
        printerr("no object");
    }

    if(!HJson_is_false(HJson_object_get(json_com, "bool"))) {
        printerr("'bool' should be false");
    }

    tmp = HJson_object_get(json_com, "num");
    if(!HJson_is_number(tmp)) {
        printerr("comments.num should be number!\n");
    }

    tmp = HJson_object_get(json_com, "keys");
    if(HJson_is_object(tmp)) {
        tmp = HJson_object_get(tmp, "k1");
        if(!HJson_is_number(tmp) || tmp->number != 10) printerr("comments.keys.k1 should be k1 10 (number)");
    }

    HJson_free(json_com);
    free(file_text_com);
    printf("Testing HJson parsing with comments done.\n");
}

int main() {
    test_hjson();

    return 0;
}
