
build/wdc: nob src/wdc.c
	-mkdir -p build
	./nob
nob:
	$(CC) nob.c -o nob
