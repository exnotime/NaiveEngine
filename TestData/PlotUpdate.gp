
set xtic auto
set ytic auto
set title "UpdateTimes"
set ylabel "ms"
set yrange [0:1]
set term png
set output "UpdateTimes.png"

plot "UpdateTestData.txt" title 'Culling' with lines,\
 "UpdateTestDataNoCulling.txt" title 'NoCulling' with lines
