# FBSD: Fast Blurred Segment Detector

Requires at least Qt5.5
Compilation by qmake and make

Execution : `FBSD <imageName>`

Test on synthetized images : `FBSD -random` (requires some patience)

<a href="http://ipol-geometry.loria.fr/~kerautre/ipol_demo/FBSD_IPOLDemo">Online demo</a> also available.

# Experimentals results

## 1. Experimentations on synthesized images
These tests compare the performance of both detectors on a set of 1000
synthesized images containing 10 randomly placed input segments with random
width between 2 and 5 pixels. The absolute value of the difference of each
found segment to its matched input segment is measured. On these groundtruth
image, the numerical error on the gradient extraction biases the line width
measures. This bias was first estimated using 1000 images containing only one
input segment (no possible interaction) and the found value (1.4 pixel) was taken
into account in the test. The results are given in the following table.

| Detector  | Old | New |
| :---          |     :---:     |     :---:     | 
| Detected blurred segments per image  | 25.35 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 7.17 | 24.35 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 5.03  |
| Detected long (> 40 pixels) blurred segments per image  | 10.41 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 1.84 | 11.14 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 1.92 |
| Undetected input segments per image  | 2.53 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.54 | 0.64 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.91 |
| Precision (%) : P = #(D ![eq](https://latex.codecogs.com/gif.latex?%5Ccap) S)/#D  | 72.76 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 9.69 | 79.19 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 6.03 |
| Recall (ratio of true detection) (%) : R = #(D ![eq](https://latex.codecogs.com/gif.latex?%5Ccap) S)/#S  | 89.20 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.94 | 90.08 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.77 |
| F-measure (harmonic mean) (%) : F = 2 x P x R /(P + R)  | 79.85 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 6.78 | 84.17 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 4.17 |
| Width difference (in pixels) to matched input segment  | 0.92 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.31 | 0.76 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.23 |
| Angle difference (in degrees) to matched input segment  | 1.48 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 1.42 | 1.05 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 0.80 |

## 2. Experimentations on real images

![Figure](Images/expe.png?raw=true)
**Figure: Automatic detection on real images: an input image (a), the segments found
by the old detector (b) and those found by the new detector (c), and a detail of the
image (d) and the enclosing digital segments for both old (e) and new (f) detectors.**

| Measure M  | T | N | L | W | 
| :---          |     :---:     |     :---:     |     :---:     |    :---:     | 
| Old method (M_old) on the image  | 29.51 ms | 306 segments | 38.58 pixels | 2.47 pixels |
| New method (M_new) on the image  | 25.85 ms | 352 segments | 33.25 pixels | 2.17 pixels |
| **M_old/M_new (%)** |
|   &nbsp;&nbsp;&nbsp; on the image  | 87.60 | 115.03 | 86.18 | 87.85 |
|   &nbsp;&nbsp;&nbsp; on the set of test images  | 85.47 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.26 | 108.19 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 4.86| 91.23 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.63 | 86.18 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 3.88 |
|   &nbsp;&nbsp;&nbsp; on CannyLines images  | 86.02 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.44 | 110.15 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 6.51 | 89.23 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 5.11 | 84.70 ![eq](https://latex.codecogs.com/gif.latex?%5Cpm) 2.98 |

# Application control

Use right mouse button to define an input stroke.

On Mac systems, Cmd keys should be used instead of Ctrl keys.

Keyboard controls are :
---------------------

| Key | Description |
|-----|-------------|
| Ctrl-B | Toggles the window background (in cycle).
| Ctrl-D | Sets density test after initial detection on or off (off by default).
| Ctrl-E | Sets line or edge detection mode (line detection mode by default).
| Ctrl-K | Sets fragmentation test on or off (off by default).
| Ctrl-L | Sets final density test on or off (off by default).
| Ctrl-M | Sets the multi-detection on or off (off by default).
| Ctrl-N | Sets extension test after initial detection on or off (off by default).
| Ctrl-P | Sets the initial detection duplication on or off.
| Ctrl-Q | Sets the adaptive directional scan on or off (on by default).
| Ctrl-U | Sets the enclosing segments display on or off (off by default).
| Ctrl-Y | Sets the blurred segment display on or off (on by default).
| Ctrl-Z | Sets the assigned width control on or off (on by default).
|-----|-------------|
| a   | Registers the last blurred segment.
| c   | Withdraws the registered segments.
| e   | Sets main or opposite edge detection mode (main by default).
| g/G | Tunes the sensitivity to the image contrast.
| k/K | Tunes the fragments (connected components) minimal size
| l/L | Tunes the minimal size of the detected blurred segments.
| m   | Detects all the segments in the image
| n   | Highlights the next detected segment in a multi-detection.
| p   | Saves detection window in capture.png
| q   | Displays all the registered blurred segments.
| r/R | Tunes the sweeping resolution of automatic detections.
| s/S | Tunes the tolerance to detection lacks.
| u   | Runs the last detection again.
| x/X | Tunes the initial assigned width.
| z/Z | Tunes the assigned width control delay.
| 6   | Sets the previous detector on or off (off by default).
| 8   | Performs comparative tests between previous and new detectors.
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
