# CS585-Lab3
Solution for BU CS585 Lab3. <br>
Coding with Liang and Ning.

## **Requirements**

Design and implement algorithms that recognize hand shapes (such as making a fist, thumbs up, thumbs down, pointing with an index finger etc.) or gestures (such as waving with one or both hands, swinging, drawing something in the air etc.) and create a graphical display that responds to the recognition of the hand shapes or gestures. For your system, you could use some of the following computer vision techniques that were discussed in class:

1. template matching (e.g., create templates of a closed hand and an open hand)
2. background differencing: D(x,y,t) = |I(x,y,t)-I(x,y,0)|
3. frame-to-frame differencing: D’(x,y,t) = |I(x,y,t)-I(x,y,t-1)|
4. motion energy templates (union of binary difference images over a window of time)
5. skin-color detection (e.g., thresholding red and green pixel values)
6. horizontal and vertical projections to find bounding boxes of ”movement blobs” or ”skin-color blobs”
7. size, position, and orientation of ”movement blobs” or ”skin-color blobs”
8. circularity of ”movement blobs” or ”skin-color blobs”
9. tracking the position and orientation of moving objects

The code is written in C++ with OpenCV 3.3.0
