CC = cc
CFLAGS = -O2 -g -Wall -Werror -lSDL2

all: fractal

fractal:
	@echo "[+] Building project..."
	$(CC) $(CFLAGS) -o fractal ./fractal.c

run: fractal
	./fractal

clean:
	@echo "[+] Cleaning..."
	rm -f fractal