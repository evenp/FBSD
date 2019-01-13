#!/bin/sh
PATH=/usr/bin/:/usr/local/bin:/opt/local/bin
LD_LIBRARY_PATH=/home/kerautre/DGtal/build/src:/home/kerautre/libQGLViewer-2.6.4/QGLViewer
export LD_LIBRARY_PATH

BG_IMAGE=$1
LINE_FILE=$2
OUT_FILE_BASENAME=$3
convert +contrast +contrast +contrast +contrast +contrast -modulate 160,100,100 -type grayscale -depth 8 ${BG_IMAGE} inputBG.png
displayLineSegments -i ${LINE_FILE} -o ${OUT_FILE_BASENAME}.fig  --backgroundImage inputBG.png
fig2dev -L eps ${OUT_FILE_BASENAME}.fig ${OUT_FILE_BASENAME}.eps
convert -background \#FFFFFF -flatten ${OUT_FILE_BASENAME}.eps -geometry 512x ${OUT_FILE_BASENAME}.png
displaySet2dPts -i segmentsPoints.dat -o ${OUT_FILE_BASENAME}Pts.fig --backgroundImage inputBG.png
fig2dev -L eps ${OUT_FILE_BASENAME}Pts.fig ${OUT_FILE_BASENAME}Pts.eps
convert -background \#FFFFFF -flatten ${OUT_FILE_BASENAME}Pts.eps -geometry 512x ${OUT_FILE_BASENAME}Pts.png



