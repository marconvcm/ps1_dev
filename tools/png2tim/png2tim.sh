#!/bin/sh
set -e

# Convert ball sprite with specific VRAM coordinates (offscreen at 960, 256 with CLUT at 960, 272)
if [ -f "ball16c.png" ]; then
    png2tim -p 960 256 -c 960 272 ball16c.png
    echo "Converted ball16c.png to TIM (VRAM: 960,256 CLUT: 960,272)"
fi

# Convert other PNG images with safe off-screen VRAM coordinates (X=320, Y=0, CLUT at X=320, Y=480)
for img in *.png; do
    [ -f "$img" ] || continue
    [ "$img" = "ball16c.png" ] && continue
    png2tim -p 320 0 -c 320 480 "$img"
    echo "Converted $img to TIM format."
done
