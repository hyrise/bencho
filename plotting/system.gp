

LW = 6
set style line 1 lw LW lt 1
set style line 2 lw LW lt 5
set style line 3 lw LW lt 7
set style line 4 lw LW lt 4
set style line 5 lw LW
set style line 6 lw LW
set style line 7 lw LW
set style line 8 lw LW
set style line 9 lw LW

#set format x "%.0s%c"
set format y "%.0s%c"

set pointsize 1

XTIC_FACTOR = 1
#(rows): set xlabel "Number of Rows"
#(distinct): set xlabel "Number of Distinct Values"
#(value_disorder): set xlabel "Value Disorder"
#(value_length): set xlabel "Length of Values in Bytes"
#(value_skewness): set xlabel "Skewness of Values"
#(value_skewness): XTIC_FACTOR = 1000
#(value_skewness): set format x "%.1f"
#(value_skewness): set mxtics 5

TERMINAL_PDF: set terminal $(TERMINAL) font "Times,8pt" size 12.1716cm,8.1144cm dashed
TERMINAL_X11: set term x11 persist
TERMINAL_PS: set terminal postscript eps enhanced "Times" 24
TERMINAL_TEST: set terminal pdf enhanced font "Times,8pt" dashed

TITLE_UNCOMPR = "Uncompr."
TITLE_SORTED = "S.Dict."
TITLE_UNSORTED = "U.Dict."
TITLE_SORTED_REENCODE = "S.Dict. + Reencode"

set key out horiz
set key bot center
set grid
set key samplen 2
set key width 0
set key autotitle columnheader

set output "OUTPUT1.$(TERMINAL)"