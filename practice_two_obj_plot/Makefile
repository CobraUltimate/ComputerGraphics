TARGET=obj_processor_test
SRCDIR=src
OBJDIR=obj
INCDIR=inc

OBJS= $(subst src,obj,$(subst .c,.o,$(wildcard $(SRCDIR)/*.c))) obj/main.o

CC=gcc
CFLAGS=-g -Wall -I./$(INCDIR)
LDFLAGS=-lm -lpthread

all: $(TARGET)

$(OBJDIR)/%.o:$(SRCDIR)/%.c test/main.c
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJDIR)/main.o: test/main.c
	$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): $(OBJS)
	@echo $(OBJS)
	$(CC) $^ -o $(TARGET) $(LDFLAGS)

.PHONY: clean

clean:
	rm -fv obj/*.o $(TARGET)
	rm -fv *.exe
	rm -fv *.ppm
	rm -fv *.log
