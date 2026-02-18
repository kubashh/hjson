#pragma once
#include "stdlib.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#define HJson_Version "0.4.0"

// json types
#define HJson_Invalid       0b00000000
#define HJson_False         0b00000001
#define HJson_True          0b00000010
#define HJson_Number        0b00000100
#define HJson_String        0b00001000
#define HJson_Array         0b00010000
#define HJson_Object        0b00100000
#define HJson_Null          0b01000000 // to raw?
// TODO #define HJson_Refrence      0b10000000 // for static strings like "false"
// masks for t1 & mask
#define HJson_BoolMask      0b00000011
#define HJson_RefrenceMask  0b00110000


#define HJson_new_alloc() (HJson*)calloc(1, sizeof(HJson))
#define HJson_move_input_buf_end(input_buf) ((*input_buf) += strlen(*input_buf))


#define INDENT_LEVEL 4
#define LINE_WIDTH 112


// HJson error
char* err = NULL;


// HJson structure
typedef struct HJson {
    // next for walking on array/object linked list
    struct HJson *next;

    // Item name string for object childs
    char* name;

    int type;

    union {
        double number;
        char* string;
        // An array or object item will have a child pointer pointing to a chain of the items in the array/object
        struct HJson *child;
    };
} HJson;

// typedef struct {
//     void* allocated;
// } SaveAllocation;

typedef char* ParseBuffer;


// Lib headers Api
bool HJson_is_false(const HJson* json);
bool HJson_is_true(const HJson* json);
bool HJson_is_bool(const HJson* json);
bool HJson_is_number(const HJson* json);
bool HJson_is_string(const HJson* json);
bool HJson_is_array(const HJson* json);
bool HJson_is_object(const HJson* json);
bool HJson_has_childs(const HJson* json);

HJson* HJson_create_null();
HJson* HJson_create_bool(bool value);
HJson* HJson_create_number(double number);
HJson* HJson_create_string(char* string);
HJson* HJson_create_array();
HJson* HJson_create_object();

HJson* HJson_parse(char* src);
void HJson_free(HJson* json);
char* HJson_stringify(const HJson* json);
int HJson_array_len(const HJson* array);
HJson* HJson_array_at(HJson* array, int index);
HJson* HJson_object_get(const HJson* object, const char* name);


// Api

bool HJson_is_false(const HJson* json) {
    return json && json->type == HJson_False;
}
bool HJson_is_true(const HJson* json) {
    return json && json->type == HJson_True;
}
bool HJson_is_bool(const HJson* json) {
    return json && (json->type & HJson_BoolMask);
}
bool HJson_is_number(const HJson* json) {
    return json && json->type == HJson_Number;
}
bool HJson_is_string(const HJson* json) {
    return json && json->type == HJson_String;
}
bool HJson_is_array(const HJson* json) {
    return json && json->type == HJson_Array;
}
bool HJson_is_object(const HJson* json) {
    return json && json->type == HJson_Object;
}
bool HJson_has_childs(const HJson* json) {
    return json && (json->type & (HJson_Array | HJson_Object)) && json->child;
}

HJson* HJson_create_null() {
    HJson* json = HJson_new_alloc();
    json->type = HJson_Null;
    return json;
}
HJson* HJson_create_bool(bool value) {
    HJson* json = HJson_new_alloc();
    json->type = value ? HJson_True : HJson_False;
    return json;
}
HJson* HJson_create_number(double number) {
    HJson* json = HJson_new_alloc();
    json->type = HJson_Number;
    json->number = number;
    return json;
}
HJson* HJson_create_string(char* string) {
    HJson* json = HJson_new_alloc();
    json->type = HJson_String;
    json->string = string;
    return json;
}
HJson* HJson_create_array() {
    HJson* json = HJson_new_alloc();
    json->type = HJson_Array;
    return json;
}
HJson* HJson_create_object() {
    HJson* json = HJson_new_alloc();
    json->type = HJson_Object;
    return json;
}


// for array/object
int HJson_array_len(const HJson* array) {
    HJson *child = NULL;
    int size = 0;

    if(array == NULL) {
        return 0;
    }

    child = array->child;

    for(HJson *child = array->child; child != NULL; child = child->next) {
        size++;
    }

    return size;
}


// for array/object
HJson* HJson_array_at(HJson* array, int index) {
    if(array == NULL) return NULL;

    HJson* current_child = array->child;
    while((current_child != NULL) && (index > 0)) {
        index--;
        current_child = current_child->next;
    }

    return current_child;
}


HJson* HJson_object_get(const HJson* object, const char* name) {
    if((object == NULL) || (name == NULL)) return NULL;

    HJson *current_element = object->child;
    while(current_element != NULL && current_element->name != NULL && strcmp(name, current_element->name) != 0) {
        current_element = current_element->next;
    }

    return current_element;
}

// object/array
void HJson_array_add(HJson* array, HJson* node) {
    HJson_array_at(array, HJson_array_len(array) - 1)->next = node;
}

void HJson_object_add_false(HJson* object, const char* name) {
    // TODO ???
}




// Utils
static HJson* parse_value(ParseBuffer* const input_buf);



// Helpers
static inline void skip_whitespace(ParseBuffer* const input_buf) {
    while(**input_buf != '\0' && (**input_buf == ' ' || **input_buf == '\n')) {
        (*input_buf)++;
    }


    if(**input_buf == '/') {
        (*input_buf)++;
        switch(**input_buf) {
            case '/': {
                while(*(*input_buf)++ != '\n');
                skip_whitespace(input_buf);
                break;
            }

            case '*': {
                while(*(*input_buf)++ != '*' || **input_buf != '/');
                (*input_buf)++;
                skip_whitespace(input_buf);
                break;
            }
        }
    }
}

// Duplicate string and allocate
char* duplicate_string(const char* str) {
    if (str == NULL) return NULL;
    char* dup_str = malloc(strlen(str) + 1);
    if (dup_str) {
        strcpy(dup_str, str);
    }
    return dup_str;
}


// HJson

static inline HJson* parse_number(ParseBuffer* const input_buf) {
    int len = 1;
    while(true) {
        switch((*input_buf)[len]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '+':
            case '-':
            case '.':
            case 'e':
                len++;
                break;
            default:
                goto loop_end;
        }
    }

loop_end:

    char buf[len + 1];

    memcpy(buf, *input_buf, len);
    buf[len] = '\0';

    *input_buf += len;

    HJson* node = HJson_new_alloc();
    node->type = HJson_Number;
    node->number = atof(buf);
    return node;
}


// Need validated string
// TODO utf chars like \u3243
static inline char* parse_string(ParseBuffer* const input_buf) {
    (*input_buf)++; // skip "
    // allocate bigger? and realocate?
    int skipped_bytes = 0;
    int src_len = 0;
    while((*input_buf)[src_len] != '"') {
        if((*input_buf)[src_len] == '\\') {
            skipped_bytes++;
            src_len++;
        }
        src_len++;
    }

    char* out = malloc(src_len - skipped_bytes + 1);

    char* out_ptr = out;
    char* src_end = (*input_buf) + src_len;
    while(*input_buf < src_end) {
        if(**input_buf != '\\') {
            *(out_ptr++) = *(*input_buf)++;
        } else { // escape sequence
            switch ((*input_buf)[1]) {
                case 'n':
                    *(out_ptr++) = '\n';
                    break;
                case 't':
                    *(out_ptr++) = '\t';
                    break;
                case '"':
                    *(out_ptr++) = '"';
                    break;
                case '\\':
                    *(out_ptr++) = '\\';
                    break;
                case 'u':
                    err = duplicate_string("Error at parse_string (HJson): unicode not supported yet!\n");
                    HJson_move_input_buf_end(input_buf);
                    return NULL;

                default:
                    char buf[64];
                    sprintf(buf, "Error at parse_string (HJson): invalid escape char: %c\n", (*input_buf)[1]);
                    err = duplicate_string(buf);
                    HJson_move_input_buf_end(input_buf);
                    return NULL;
            }
            (*input_buf) += 2;
        }
    }

    (*input_buf)++;
    (*out_ptr) = '\0';

    return out;
}


static inline HJson* parse_array(ParseBuffer* const input_buf) {
    (*input_buf)++; // skip '['
    HJson* array = HJson_new_alloc();
    array->type = HJson_Array;
    skip_whitespace(input_buf);
    HJson* prev = NULL;
    HJson* cur_item = NULL;

    while(**input_buf != ']' && **input_buf != '\0') {
        cur_item = parse_value(input_buf);
        if(prev == NULL) {
            array->child = cur_item;
        } else {
            prev->next = cur_item;
        }

        skip_whitespace(input_buf);
        if(**input_buf == ',') (*input_buf)++;
        skip_whitespace(input_buf);
        prev = cur_item;
    }

    (*input_buf)++; // skip ']'

    return array;
}


static inline HJson* parse_object(ParseBuffer* const input_buf) {
    (*input_buf)++; // skip '{'
    HJson* object = HJson_new_alloc();
    object->type = HJson_Object;
    skip_whitespace(input_buf);
    HJson* prev = NULL;
    HJson* cur_item = NULL;

    while(**input_buf != '}' && **input_buf != '\0') {
        char* name = parse_string(input_buf);
        (*input_buf)++;
        skip_whitespace(input_buf);
        cur_item = parse_value(input_buf);
        cur_item->name = name;
        // allocate string name
        if(prev == NULL) {
            object->child = cur_item;
        } else {
            prev->next = cur_item;
        }

        skip_whitespace(input_buf);
        if(**input_buf == ',') (*input_buf)++;
        skip_whitespace(input_buf);
        prev = cur_item;
    }

    (*input_buf)++; // skip '}'

    return object;
}


static HJson* parse_value(ParseBuffer* const input_buf) {
    skip_whitespace(input_buf);

    switch(**input_buf) {
        // string
        case '"': {
            HJson* node = HJson_new_alloc();
            node->type = HJson_String;
            node->string = parse_string(input_buf);
            return node;
        }

        // object
        case '{': {
            return parse_object(input_buf);
        }

        // array
        case '[': {
            return parse_array(input_buf);
        }

        // number
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-': {
            return parse_number(input_buf);
        }

        // true
        case 't': {
            if(strncmp(*input_buf + 1, "rue", 3) == 0) {
                (*input_buf) += 4;
                HJson* node = HJson_new_alloc();
                node->type = HJson_True;
                return node;
            }
            goto invalid;
        }

        // false
        case 'f': {
            if(strncmp(*input_buf + 1, "alse", 4) == 0) {
                (*input_buf) += 5;
                HJson* node = HJson_new_alloc();
                node->type = HJson_False;
                return node;
            }
            goto invalid;
        }

        // null
        case 'n': {
            if(strncmp(*input_buf + 1, "ull", 3) == 0) {
                (*input_buf) += 4;
                HJson* node = HJson_new_alloc();
                node->type = HJson_False;
                return node;
            }
            goto invalid;
        }
    }

invalid:
    // invalid
    HJson* node = HJson_new_alloc();
    node->type = HJson_Invalid;
    HJson_move_input_buf_end(input_buf);
    return node;
}


// HJson_stringify

static char* HJson_stringify_value(const HJson* json, int indent) {
    char* buf;
    switch(json->type) {
        // false
        case HJson_False: {
            buf = malloc(6);
            strcpy(buf, "false");
            return buf;
        }

        // true
        case HJson_True: {
            buf = malloc(5);
            strcpy(buf, "true");
            return buf;
        }

        // null
        case HJson_Null: {
            buf = malloc(5);
            strcpy(buf, "null");
            return buf;
        }

        // number
        case HJson_Number: {
            buf = malloc(32);
            snprintf(buf, 32, "%f", json->number);
            char *p = buf + strlen(buf) - 1;
            while (*p == '0') *p-- = '\0';

            // remove trailing dot if needed
            if(*p == '.') *p = '\0';
            return buf;
        }

        // string
        case HJson_String: {
            int len = strlen(json->string);
            buf = malloc(len + 3);
            for(int i = 0; i < len; i++) {
                buf[i + 1] = json->string[i];
            }
            buf[0] = buf[len + 1] = '"';
            buf[len + 2] = '\0';
            return buf;
        }

        // array
        case HJson_Array: {
            if(json->child == NULL) {
                buf = malloc(3);
                strcpy(buf, "[]");
                return buf;
            }

            int len = 1;
            buf = malloc(len + 2); // + 2 for "]\0"
            buf[0] = '[';
            for(HJson* child = json->child; child != NULL; child = child->next) {
                char* value = HJson_stringify_value(child, indent);
                int len2 = strlen(value);
                buf = (char*)realloc(buf, len + len2 + 2); // + 2 for ", "

                // TODO if to long add \n and indent => realocate
                // if(indent + len + len2 <= INDENT_LEVEL) {
                //     buf = (char*)realloc(buf, len + len2 + 2); // + 2 for ", "
                // } else {
                //     buf = (char*)realloc(buf, len + (indent + 1) + len2 + 2); // + 2 for ", ",
                //     printf("deb: %c\n", value[1]);
                //     // buf[len++ - 1] = '\n';
                // }

                // Value
                for(int j = 0; j < len2; j++) {
                    buf[len + j] = value[j];
                }
                buf[len + len2] = ',';
                buf[len + len2 + 1] = ' ';
                len += len2 + 2;

                // Free
                free(value);
            }

            buf[len - 2] = ']';
            buf[len - 1] = '\0';
            return buf;
        }

        // object
        case HJson_Object: {
            if(json->child == NULL) {
                buf = malloc(3);
                strcpy(buf, "{}");
                return buf;
            }

            int len = 2;
            buf = malloc(len + indent);
            buf[0] = '{';
            buf[1] = '\n';
            for(HJson* child = json->child; child != NULL; child = child->next) {
                const char* name = child->name;
                const int name_len = strlen(name);
                char* value = HJson_stringify_value(child, indent + INDENT_LEVEL);
                const int value_len = strlen(value);
                buf = (char*)realloc(buf, len + indent + value_len + name_len + 2 + 2 + 2 + 1 + 10); // needs extra 10,
                    // I guess for formatting numbers

                // Indent
                memset(&buf[len], ' ', indent);
                len += indent;

                // Name
                buf[len] = '"';
                for(int j = 0; j < name_len; j++) {
                    buf[len + j + 1] = name[j];
                }
                buf[len + name_len + 1] = '"';
                buf[len + name_len + 2] = ':';
                buf[len + name_len + 3] = ' ';

                // Value
                for(int j = 0; j < value_len; j++) {
                    buf[len + name_len + 4 + j] = value[j];
                }
                buf[len + name_len + value_len + 4] = ',';
                buf[len + name_len + value_len + 5] = ' ';
                buf[len + name_len + value_len + 6] = '\n';
                len += name_len + value_len + 7;

                // Free
                free(value);
            }

            indent -= INDENT_LEVEL;
            buf[len - 3] = '\n';
            memset(&buf[len - 2], ' ', indent);
            buf[len + indent - 2] = '}';
            buf[len + indent - 1] = '\0';
            return buf;
        }
    }
}


// Init allocate in memory
HJson* HJson_parse(char* src) {
    return parse_value(&src);
}

// Init allocate in memory from file path. NULL == no file
HJson* HJson_parse_file(const char* path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) return NULL;

    // Move the file pointer to the end of the file to get the size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);  // Move back to the beginning of the file

    // Allocate memory for the file content, including space for the null terminator
    char* file_text = (char *)malloc(file_size + 1);
    if (file_text == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // Read the file contents into the allocated memory
    size_t bytes_reed = fread(file_text, 1, file_size, file);
    file_text[bytes_reed] = '\0';  // Null-terminate the string

    // Close the file
    fclose(file);

    if(!file_text) return NULL;
    HJson* json = HJson_parse(file_text);
    free(file_text);
    return json;
}


// Stringify allocate json string
char* HJson_stringify(const HJson* json) {
    return HJson_stringify_value(json, INDENT_LEVEL);
}


// Free memory
void HJson_free(HJson* json) {
    if(json->name) free(json->name);
    if(json->type & HJson_RefrenceMask && json->child) HJson_free(json->child);
    if(json->type == HJson_String) free(json->string);
    if(json->next) {
        HJson_free(json->next);
        free(json->next);
    }
}
