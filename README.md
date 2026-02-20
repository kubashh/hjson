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
- Performance, formatting, and correctness testing
- Useful utility functions
- File input/output (I/O) support
- Built with C libraries and types
