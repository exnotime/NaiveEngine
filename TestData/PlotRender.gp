
set xtic auto
set ytic auto
set title "RenderTime"
set ylabel "ms"
set yrange [0:10]
set term png
set output "RenderTimes.png"

plot "RenderTestData.txt" title 'Culling' with lines,\
 "RenderTestDataNoCulling.txt" title 'NoCulling' with lines
