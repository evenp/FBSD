# FBSD: Fast Blurred Segment Detector

Requires at least Qt5.5

Compilation by qmake and make

Execution : `FBSD <imageName>`

Detects ans edits segments in naivelines.txt : `FBSD -out <imageName>`

Test on synthetized images : `FBSD -random` (requires some patience)

<a href="http://ipol-geometry.loria.fr/~kerautre/ipol_demo/FBSD_IPOLDemo">Online demo</a> also available.

# Evaluation of ADS and ATC concepts
First tests compare the performance of the detector with and without adaptive directional scans (ADS) and assigned thickness control (ATC).
On the detector without ADS, the fine tracking step must be performed twice
to get less risk of growing blurred segment escape from the scan strip.

## 1. Experimentations on synthesized images
These tests compare both versions on a set of 1000 synthesized images containing
10 randomly placed input segments with random width between 2 and 5 pixels.
The absolute value of the difference of each found segment to its matched input segment is measured.
On these groundtruth image, the numerical error on the gradient extraction biases the line width measures.
This bias was first estimated using 1000 images containing only one input segment (no possible interaction) and the found value (1.4 pixel) was taken into account in the test.
Results are given in the following table.
If we call S the count of pixels of all input segments in an image, D the count of pixels of all output blurred segments, and I the count of successfully detected pixels (intersection of D ans S), the given measures are 1) the count of output blurred segments, 2) the count of output long (> 40 pixels) blurred segments, 3) the count of undetected input segments, 4) the precision P = I/D, 5) the recall R = I/S, 6) the F-measure F = 2.P.R / (P+R), 7) the absolute value of the width difference between matched output segments with input segments, and 8) the absolute value of the angle difference between matched output segments with input segments.

![Figure](Images/randimage1.png?raw=true) &nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;
![Figure](Images/randold1.png?raw=true) &nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;
![Figure](Images/randnew1.png?raw=true)

![Figure](Images/randimage2.png?raw=true) &nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;
![Figure](Images/randold2.png?raw=true) &nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;
![Figure](Images/randnew2.png?raw=true)

**Figure 1: Evaluation on synthesized images: two of the randomly generated images(left), bounding lines of output blurred segments without (middle) and with (right) ADS and ATC concepts.**

| ADS and ATC Concepts | Without | With |Without | With | Without | With |
| :---          |     :---:     |     :---:     |     :---:     |     :---:     |     :---:     |     :---:     |
| Data | Fig.1 up | Fig.1 up | Fig.1 bottom | Fig.1 bottom | Whole set | Whole set |
| Detected blurred segments | 16 | 17 | 14 | 18 | 17.06 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.22 | 16.83 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.11  |
| Detected long segments | 11 | 8 | 10 | 10 | 11.24 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 1.94 | 11.36 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 1.97 |
| Undetected input segments | 0 | 0 | 1 | 0 | 0.152 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.43 | 0.003 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.05 |
| Precision (%) | 76.30 | 85.47 | 75.38 | 83.41 | 80.46 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 7.22 | 83.87 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 6.04 |
| Recall (%) | 89.81 | 93.51 | 90.88 | 91.47 | 90.23 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.30 | 91.15 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.52 |
| F-measure (%) | 82.51 | 89.31 | 82.40 | 87.26 | 84.87 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 4.42 | 87.23 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.59 |
| Thickness difference (pixels) | 0.95 | 0.68 | 1.15 | 0.65 | 0.70 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.24 | 0.59 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.19 |
| Angle difference (degrees) | 1.11 | 0.71 | 1.99 | 1.03 | 0.61 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.66 | 0.57 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.62 |

**Table 1: Measured performance on both Figure 1 image examples and on a whole 1000 synthesized images set, without and with adaptive directional scans and assigned width control.**

## 2. Experimentations on real images
Next tests compare both versions on real images: first the set of 102 images of York Urban data base [1] augmented with manually extracted groundtruth lines, then selected images for more detailed visual analysis. Reported measures are execution time T, groundtruth covering ratio C (only for the York Urban data base), number of output line segments N,
mean length of output line segments L/N, and mean thickness of output line
segments W.

![Figure](Images/expe0.png?raw=true)

**Figure 2: Automatic detection on real images: P1020928 image from York Urban data base [1] (a), the associated groundtruth lines (d), the naive lines found without (b) and with (e) ADS and ATC concepts, the thick lines found without (c) and with (f) ADS and ATC concepts.**

| Measure  | T (ms) | C (\%) | N | L/N (pixels) | W (pixels) | 
| :---          |     :---:     |     :---:     |     :---:     |     :---:     |    :---:     | 
| Without ADS and ATC | 75.19 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 16.60 | 70.2 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 10.1 | 421 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 98 | 46.22 pixels ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 8.60 | 2.20 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.16 |
| With ADS and ATC | 66.62 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 15.47 | 67.9 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 9.6 | 478 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 111 | 41.67 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 7.53 | 1.89 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.13 |

**Table 2: Measure with and without ADS and ATC concepts on the York Urban Database [1].**

![Figure](Images/expe1.png?raw=true)

**Figure 3: Automatic detection on real images: 800x533 office image (a), the segments found without (b) and with (c) ADS and ATC concepts, a detail of the image (d,g), the points of detected blurred segments without (e) and with (f) ADS and ATC concepts and the bounding lines of detected blurred segments without (h) and with (i) ADS and ATC concepts.**

| Measure  | T (ms) | N | L/N (pixels) | W (pixels) | 
| :---          |     :---:     |     :---:     |     :---:     |    :---:     | 
| Without ADS and ATC | 48.16 | 254 | 53.92 | 1.99 |
| With ADS and ATC  | 42.17 | 285 | 49.69 | 1.69 |

**Table 3: Measure with and without ADS and ATC concepts on office image.**

![Figure](Images/expe2.png?raw=true)

**Figure 4: Automatic detection on real images: 768x512 castle image (a), the segments found without (b) and with (c) ADS and ATC concepts, a detail of the image (d,g), the points of detected blurred segments without (e) and with (f) ADS and ATC concepts and the bounding lines of detected blurred segments without (h) and with (i) ADS and ATC concepts.**

| Measure  | T (ms) | N | L/N (pixels) | W (pixels) |
| :---          |     :---:     |     :---:     |     :---:     |    :---:     | 
| Without ADS and ATC | 104.30 | 361 | 36.58 | 2.23 |
| With ADS and ATC  | 94.21 | 424 | 32.18 | 1.98 |

**Table 4: Measure with and without ADS and ATC concepts on castle image.**

Shorter execution time is achieved with the new concepts. Detected blurred segments are shorter but thinner. Obviously the constant assigned thickness augments the probability to extend the segments with outlier edge points as can be noticed in the detail of office and castle images. Moreover, brick joints are better detected in castle image.

[1] Denis, P., Elder, J.H. and Estrada, F.J.: Efficient edge-based methods for estimating Manhattan frames in urban imagery. In: European Conference on Computer Vision. Springer LNCS 5303, pp. 197-210 (2008).

# Application control

Use right mouse button to define an input stroke.

On Mac systems, Cmd keys should be used instead of Ctrl keys.

Keyboard controls are :
---------------------

| Key | Description |
|-----|-------------|
| Ctrl-B | Toggles the window background (in cycle).
| Ctrl-K | Sets fragmentation test on or off (off by default).
| Ctrl-M | Sets the multi-detection on or off (off by default).
| Ctrl-O | Sets the detection measurement display or off (off by default).
| Ctrl-S | Sets final size test on or off (on by default).
| Ctrl-X | Sets ADS and ATC tools (on by default).
| Ctrl-Y | Sets final sparsity test on or off (off by default).
|-----|-------------|
| a   | Registers the last blurred segment.
| b/B | Sets image gray level.
| c/C | Toggles blurred segment color set.
| d/D | Toggles blurred segment display style.
| k/K | Tunes the fragments (connected components) minimal size.
| m   | Detects all the segments in the image.
| o   | Saves detected segment(s) in seg(s).txt
| p   | Saves detection window in capture.png
| q   | Displays all the registered blurred segments.
| r/R | Tunes the stroke sweeping step of automatic detections.
| s/S | Tunes the minimal size of detected blurred segments.
| u   | Runs the last detection again.
| w/W | Tunes the initial assigned width.
| x   | Withdraws the registered segments.
| <>  | Horizontal shift (on large images, example: general.png).
| ^v  | Vertical shift (on large images, example: general.png).
| +-  | Zoom (on large images, example: general.png).

Example :
-------

Command : `FBSD Images/couloir.gif`

<img width="256" src="https://github.com/evenp/FBSD/blob/master/Images/couloir.gif" alt="Initial image" />

Right mouse click and drag :

<img width="256" src="https://github.com/evenp/FBSD/blob/master/Snapshots/snapSingle.png" alt="Single blurred segment detection" />

Keys "Ctrl-m" and "u" :

<img width="256" src="https://github.com/evenp/FBSD/blob/master/Snapshots/snapMulti.png" alt="Multiple blurred segments detection" />

Key "m" :

<img width="256" src="https://github.com/evenp/FBSD/blob/master/Snapshots/snapAll.png" alt="Automatic blurred segment detection" />
