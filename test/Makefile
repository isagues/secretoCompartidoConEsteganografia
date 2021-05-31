TESTS := $(patsubst %.test.$(SRCEXT), %.test, $(wildcard *.test.$(SRCEXT)))

TEST_LIBRARIES = -lcheck -lrt -lpthread -lm -lsubunit

TEST_SOURCES = test_utils.c
TEST_OBJECTS := $(TEST_SOURCES:.c=.o)

test: $(TESTS)

# Run
%.test: %.test.tmp
	./$^

# Compile
%.test.tmp: %.test.c $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(INC) $(SRC_OBJECTS) $^ -o $@ $(LIBRARIES) $(TEST_LIBRARIES)

clean:
	$(RM) $(TESTS)

.PHONY: all test clean