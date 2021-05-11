
all: $(TARGET)

$(TARGET): $(MAIN_OBJECT) $(SRC_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(MAIN_OBJECT) $(SRC_OBJECTS) $(TARGET)


.PHONY: all clean
