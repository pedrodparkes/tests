all: 
	gcc -o testBin main.c writePng.c hsvToRgb.c -lm -lpng
	
#writePng: writePng.c hsvToRgb.o
#	gcc -o writePng writePng.c -lm -lpng

#hsvToRgb.o: hsvToRgb.c
#	gcc -c hsvToRgb.c

clean:
	rm -f writePng testBin hsvToRgb.o *.png

test:
	./testBin 162 1 1

