TESTS := $(patsubst %.test.c, %.test, $(wildcard *.test.c))

TEST_LIBRARIES = -lcheck -lrt -lpthread -lm -lsubunit

TEST_SOURCES = test_utils.c
TEST_OBJECTS := $(TEST_SOURCES:.c=.o)

test: $(TESTS)

# Run
%.test: %.test.tmp
	./$^

# Compile
%.test.tmp: %.test.c $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(SRC_OBJECTS) $^ -o $@ $(LIBRARIES) $(TEST_LIBRARIES)

clean:
	$(RM) $(patsubst %.test.c, %.test.tmp, $(wildcard *.test.c))

.PHONY: all test clean