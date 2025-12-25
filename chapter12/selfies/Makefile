all: selfie

selfie: selfie.0
	./selfie.0 -c selfie.c -o selfie

selfie.0: selfie.c
	$(CC) -D"uint64_t = unsigned long" selfie.c -o selfie.0

clean:
	rm -f selfie selfie.0
