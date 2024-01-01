
CROSS_COMPILE:=
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)ld
NM:=$(CROSS_COMPILE)nm
OBJDUMP:=$(CROSS_COMPILE)objdump 
OBJCOPY:=$(CROSS_COMPILE)objcopy

SRC:=./src
BUILD:=./build
INCLUDE_PATH:=$(SRC)/include
TARGET_NAME:=main

INCLUDE_PATH:=$(foreach it,$(INCLUDE_PAH),-I$(it))

CCFLAGS:= -g $(INCLUDE_PAH) 

OBJECTS:= 	$(BUILD)/m_blk.o 	\
		$(BUILD)/mln_list.o 	\
		$(BUILD)/m_pool.o		\
		$(BUILD)/m_data.o		\
		$(BUILD)/m_slab.o		\
		$(BUILD)/main.o			

compile: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $^ -o $(BUILD)/$(TARGET_NAME)

$(BUILD)/%.o:$(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c $< -o $@

test:
	@echo $(OBJECTS)
clean:
	rm -rf $(BUILD)

.PHONY: compile clean test
