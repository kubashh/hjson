# HJson

Fast json parser for C lang

## Usage

Compile `hjson.c` or copy soure code to project

## Linux for dev's

```sh
sh run.sh           # test performance comparing with cJSON
sh run.sh -m        # write performance test to md file comparing with cJSON
sh run.sh -f        # test HJson formatting
sh run.sh -c        # test HJson corectness
```

## Benchmarks

See https://github.com/kubashh/hjson/tree/main/bench

## Project Features

- Fast JSON parsing (faster than cJSON)
- JSON stringifying and formatting
- Performance, formatting and correctness testing
- Useful utility functions
- File (I/O) support (HJson_parse_file, HJson_stringify_write, HJson_format_file)
- Built with C libraries and types

## Headers

```c
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
```
