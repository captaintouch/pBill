#!/bin/bash

outputFile="resources/graphicResources.rcp"
outputConstantsFile="sauce/resources.h"
assetsPath="resources/assets/"
fileSuffix=".bmp"
fileHiResSuffix="-144.bmp"
fileGraySuffix="-gray.bmp"
hires=0
if [ "$1" == "--hires" ]; then
    hires=1
fi

if [ -f $outputFile ]; then
    rm $outputFile
fi

if [ -f $outputConstantsFile ]; then
    rm $outputConstantsFile
fi

function writeResource() {
    local identifier=$1
    local name=$2
    
    echo "BITMAPFAMILY ID $identifier" >> $outputFile
    echo "BEGIN" >> $outputFile
    hiresFile=$assetsPath$name$fileHiResSuffix
    grayFile=$assetsPath$name$fileGraySuffix
    if [ $hires == 1 ] && [ -f $hiresFile ]; then
        echo "	BITMAP \"$assetsPath$name$fileHiResSuffix\" BPP 8 TRANSPARENTINDEX 4 DENSITY 144 COMPRESS"  >> $outputFile
    else
        echo "	BITMAP \"$assetsPath$name$fileSuffix\" BPP 8 TRANSPARENTINDEX 4 COMPRESS"  >> $outputFile
        if [ -f $grayFile ]; then
            echo "	BITMAP \"$assetsPath$name$fileGraySuffix\" BPP 4 TRANSPARENTINDEX 4 COMPRESS"  >> $outputFile
        else
            echo "	BITMAP \"$assetsPath$name$fileSuffix\" BPP 4 TRANSPARENTINDEX 4 COMPRESS"  >> $outputFile
        fi
    fi
    echo "END"  >> $outputFile
}

function writeConstants() {
    local identifier=$1
    local name=$2
    local numParts=$3

    
    echo "#define GFX_RES_${name^^} $identifier" >> $outputConstantsFile
    echo "#define GFX_RES_${name^^}_PARTS $numParts" >> $outputConstantsFile

    nameSuffix=""
    if ((numParts > 1)); then
        nameSuffix="0"
    fi

    local fileName="$assetsPath$name$nameSuffix$fileSuffix"
    width=$(identify -format "%w" "$fileName")
    height=$(identify -format "%h" "$fileName")
    echo "#define GFX_RES_${name^^}_WIDTH $width"  >> $outputConstantsFile
    echo "#define GFX_RES_${name^^}_HEIGHT $height"  >> $outputConstantsFile
}

while IFS=';' read -r identifier name numParts prefWidth
do
    writeConstants $identifier $name $numParts
    if (( $numParts == 1 )); then
        writeResource $identifier $name
    else
        for ((i=0; i<numParts; i++)); do
            writeResource $identifier "${name}${i}"
            ((identifier+=1))
        done
    fi
    
    
done < "resources/graphicResources.map"
