CC=clang
CFLAGS=--std=c11
EXE=virt
CFILES:=$(wildcard src/*.c)
OFILES:=$(addprefix obj/,$(notdir $(CFILES:.c=.o)))


all: $(EXE)
	python asm.py test.asm test.bin

run: all
	./$(EXE)

$(EXE): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm obj/*.o
