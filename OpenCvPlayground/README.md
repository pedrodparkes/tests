# sceneSegmentation

Project contains openCV built in samples.

To run some specific sample go to main.cpp:

1. Include desired sample header
2. Define its name as sampleName

Example for Watershed sample:
```
1. #include "cppSamples\watershed.h"
2. #define sampleName watershed
```

Most samples require input parameters.
To set them up:

1. Open OpenCvPlaygound project properties (right click -> Properties in the end of context menu)
2. Go to Configuration Properties -> Debugging -> set Command Arguments property
3. Apply/OK
