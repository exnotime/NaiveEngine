
set xtic auto
set ytic auto
set title "CombinedTimes"
set ylabel "ms"
set yrange [0:5]
set term png
set output "CombinedTimes.png"

plot "CombinedTestData.txt" title 'Culling' with lines,\
 "CombinedTestDataNoCulling.txt" title 'NoCulling' with lines
