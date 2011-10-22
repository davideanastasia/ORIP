#!/usr/bin/bash

./orip_trans_std_i_v1_roi.exe -i in/UH_ext304_silent_30Hz_300fs.cif -o direct.yuv -t 1 -f 10
./orip_trans_std_i_v1_mod.exe -i direct.yuv -o invers.yuv -t 5 -f 10
./int32_to_yuv.exe -i invers.yuv -o silent_roi_char.yuv -s 400 -f 10

rm direct.yuv
rm invers.yuv