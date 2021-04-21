# !/bin/bash
mkdir 9091
mkdir 9091/Yushin
cd 9091/Yushin
date > Grigoriy.txt
date -dnext-Monday > filedate.txt
cat Grigoriy.txt filedate.txt > result.txt
cat result.txt
