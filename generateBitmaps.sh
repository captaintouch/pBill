#!/bin/bash
if ! command -v convert &> /dev/null; then
    echo "Could not find ImageMagick, aborting..."
    exit 1
fi

echo "Generating bitmaps..."

if [ ! -d ./resources/assets ]; then
    mkdir ./resources/assets
fi

function resizeImage() {
    local input_image="$1"
    local output_image="$2"
    local prefWidth="$3"
    
    convert "$input_image" -filter Point -scale ${prefWidth}x -channel alpha -threshold 10% +channel -background '#ff33ff' -alpha remove -compress None "BMP3:$output_image"
}

function resizeImageBW() {
    local input_image="$1"
    local output_image="$2"
    local prefWidth="$3"
    
    convert "$input_image" -fill black -colorize 15% -filter Point -scale ${prefWidth}x -channel alpha +channel -background '#ff33ff' -alpha remove -compress None "BMP3:$output_image"
}

while IFS=';' read -r identifier name numParts prefWidth
do
    prefWidthLoRes=$(( prefWidth / 2 ))
    outputPrefix="resources/assets/$name"
    if (($numParts <= 1)); then
        echo "Generating $name"
        resizeImage "resources/original/${name}.xpm" "${outputPrefix}-144.bmp" "$prefWidth"
        resizeImage "resources/original/${name}.xpm" "${outputPrefix}.bmp" "$prefWidthLoRes"
    else
        echo "Generating $name with $numParts frames"
        for ((i=0; i<numParts; i++)); do
            resizeImage "resources/original/${name}${i}.xpm" "${outputPrefix}${i}-144.bmp" "$prefWidth"
            resizeImage "resources/original/${name}${i}.xpm" "${outputPrefix}${i}.bmp" "$prefWidthLoRes"
        done
    fi

done < "resources/graphicResources.map"

resizeImageBW "resources/original/systems1.xpm" "resources/assets/systems1-gray.bmp" 28

resizeImage "resources/original/icon.xpm" "resources/assets/icon-144.bmp" 44
resizeImage "resources/original/icon.xpm" "resources/assets/icon.bmp" 22

convert "resources/original/icon-1.xpm" -scale 22x22 -gravity center -background transparent -extent 22x22 -negate  "BMP3:resources/assets/icon-1.bmp"
convert "resources/original/icon-1.xpm" -scale 15x9 -gravity center -background transparent -extent 15x9 -negate "BMP3:resources/assets/iconsmall-1.bmp"
convert "resources/original/icon-1.xpm" -scale 15x9 -gravity center -background transparent -extent 15x9 "BMP3:resources/assets/iconsmall.bmp"