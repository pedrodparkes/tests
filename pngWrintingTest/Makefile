all: 
	gcc -o writePng writePng.c hsvToRgb.c -lm -lpng
	
writePng: writePng.c hsvToRgb.o
	gcc -o writePng writePng.c -lm -lpng

hsvToRgb.o: hsvToRgb.c
	gcc -c hsvToRgb.c

clean:
	rm -f writePng hsvToRgb.o *.png

