TESTS := $(patsubst %.test.c, %.test, $(wildcard *.test.c))

LIBRARIES = -lcheck -lrt -lpthread -lm -lsubunit

test: $(TESTS)

# Run
%.test: %.test.tmp
	./$^

# Compile
%.test.tmp: %.test.c
	$(CC) $(CFLAGS) $(SRC_OBJECTS) $^ -o $@ $(LIBRARIES)

clean:
	$(RM) $(patsubst %.test.c, %.test.tmp, $(wildcard *.test.c))

.PHONY: all test clean