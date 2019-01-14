# 2019-FBSD

Fast Blurred Segment Detector
-----------------------------

Requires at least Qt5.5
Compilation by qmake and make

Execution : `FBSD <imageName>`

Test on synthetized images : `FBSD -random` (requires some patience)

Controls :
--------
Right mouse button to define an input stroke

Ctrl-B : Toggles the window background (in cycle).
Ctrl-D : Sets density test after initial detection on or off (off by default).
Ctrl-E : Sets line or edge detection mode (line detection mode by default).
Ctrl-K : Sets fragmentation test on or off (off by default).
Ctrl-L : Sets final density test on or off (off by default).
Ctrl-M : Sets the multi-detection on or off (off by default).
Ctrl-N : Sets extension test after initial detection on or off (off by default).
Ctrl-P : Sets the initial detection duplication on or off.
Ctrl-Q : Sets the adaptive directional scan on or off (on by default).
Ctrl-U : Sets the enclosing segments display on or off (off by default).
Ctrl-Y : Sets the blurred segment display on or off (on by default).
Ctrl-Z : Sets the assigned width control on or off (on by default).

a   : Registers the last blurred segment.
c   : Withdraws the registered segments.
e   : Sets main or opposite edge detection mode (main by default).
g/G : Tunes the sensitivity to the image contrast.
k/K : Tunes the fragments (connected components) minimal size
l/L : Tunes the minimal size of the detected blurred segments.
m   : Detects all the segments in the image
n   : Hilghlights the next detected segment in a multi-detection.
p   : Saves detection window in capture.png
q   : Displays all the registered blurred segments.
r/R : Tunes the sweeping resolution of automatic detections.
s/S : Tunes the tolerance to detection lacks.
u   : Runs the last detection again.
x/X : Tunes the initial assigned width.
z/Z : Tunes the assigned width control delay.
6   : Sets the previous detector on or off (off by default).
8   : Performs comparative tests between previous and new detectors.
