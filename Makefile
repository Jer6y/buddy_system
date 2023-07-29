SRC:= ./src
BUILD:= ./build
INCLUDE_PATH:=\
$(SRC)/include

CC:= gcc
CCFLAGS:= -g -O0 -lpthread\
$(foreach it,$(INCLUDE_PATH),-I$(it))

LD:= ld
LDFLAGS:= 

SRC_FILE:= \
$(SRC)/main.c			\
$(SRC)/buddy/buddy.c	\
$(SRC)/util/assert.c

OBJECTS:= $(patsubst $(SRC)/buddy/%.c,$(BUILD)/buddy/%.o, $(SRC_FILE))
OBJECTS:= $(patsubst $(SRC)/%.c,$(BUILD)/%.o, $(OBJECTS))
OBJECTS:= $(patsubst $(SRC)/util/%.c,$(BUILD)/util/%.o, $(OBJECTS))


compile:$(BUILD)/main
	
$(BUILD)/main: $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o $@

$(BUILD)/buddy/%.o:$(SRC)/buddy/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/%.o:$(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

$(BUILD)/util/%.o:$(SRC)/util/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

debug:
	@echo $(CCFLAGS)
	@echo $(OBJECTS)

clean:
	rm -rf $(BUILD)

.PHONY: compile clean debug