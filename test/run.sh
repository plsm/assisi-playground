cat > config-1-CASU-nolight.txt <<EOF
0.1  60000
0.5 0.5
1
0.225 0.225   0   0.1 .1
1
0.01 0.04
100 190
EOF

cat > config-2-CASU-nolight.txt <<EOF
0.1  40000
0.5 0.5
2
0.225 0.225   0   0.1 .1
0.325 0.425   0   0.1 .1
1
0.01 0.04
100 190
EOF


cat > config-4-CASU-nolight-c.txt <<EOF
5  100000
0.5 0.5
4
0.225 0.225   0   0.1 .1
0.425 0.425   0   0.1 .1
0.225 0.425   0   0.1 .1
0.425 0.225   0   0.1 .1
1
0.01 0.04
100 190
EOF



cat > ./config-1-CASU-novibe.txt <<EOF
0.1  6000
0.5 0.5
2
0.225 0.225   100   0 0
0.425 0.425   100   0 0
1
0.01 0.04
100 450
EOF


cat > ./config-4-CASU-vibe-light.txt <<EOF
5  200000
0.5 0.5
4
0.225 0.225   100   0.1 .1
0.425 0.425   100   0.1 .1
0.225 0.425   100   0.1 .1
0.425 0.225   100   0.1 .1
1
0.01 0.04
100 450
EOF

cat > ./config-2-CASU-novibe.txt <<EOF
0.1  6000
0.5 0.5
2
0.225 0.225   600   0 0
0.425 0.425   550   0 0
1
0.01 0.04
100 450
EOF

cat > ./config-4-CASU-novibe-nolight-a.txt <<EOF
0.1  100000
0.5 0.5
4
0.225 0.225   0   0 0
0.425 0.425   0   0 0
0.225 0.225   0   0 0
0.425 0.425   0   0 0
1
0.01 0.04
100 450
EOF

cat > ./config-4-CASU-novibe-nolight-b.txt <<EOF
5  100000
0.5 0.5
4
0.225 0.225   0   0 0
0.425 0.425   0   0 0
0.225 0.225   0   0 0
0.425 0.425   0   0 0
1
0.01 0.04
100 450
EOF

for CONFIG in config-4-CASU-vibe-light  ; do
# config-4-CASU-vibe-light
# config-4-CASU-vibe-light
# config-2-CASU-novibe-nolight
# config-4-CASU-novibe-nolight-b
# config-2-CASU-nolight config-1-CASU-novibe
# config-2-CASU-novibe config-1-CASU-nolight
	./test_casu  ${CONFIG}.txt output.txt plotData.gpi lightIntensityMap.txt bees-${CONFIG}.png vibrationAmplitudeMap.txt
	gnuplot plotData.gpi
done

