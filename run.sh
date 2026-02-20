# clear
clear

# Build
if [ "$1" = "-f" ]; then
    gcc test/fmt_test.c -ohjson
elif [ "$1" = "-c" ]; then
    gcc test/corectness_test.c -ohjson
else
    gcc test/performance_test.c -ohjson
fi

# Run
./hjson $1

# Cleanup
rm ./hjson
