CC=c99
CFLAGS=-O3
OMPFLAGS=-fopenmp

tvl1flow: main.c tvl1flow_lib.c bicubic_interpolation.c mask.c zoom.c iio.o writePng.o hsvToRgb.o
	$(CC) $(CFLAGS) $(OMPFLAGS) -o tvl1flow main.c iio.o writePng.o hsvToRgb.o -ltiff -lpng -ljpeg -lm

iio.o: iio.c
	$(CC) $(CFLAGS) -DNDEBUG -D_GNU_SOURCE -c iio.c


writePng.o: ../pngWritingTest/writePng.c hsvToRgb.o
	gcc -o writePng.o -c ../pngWritingTest/writePng.c -lm -lpng

hsvToRgb.o: ../pngWritingTest/hsvToRgb.c
	gcc -c ../pngWritingTest/hsvToRgb.c

clean:
	rm -f iio.o main.o tvl1flow writePng.o hsvToRgb.o
