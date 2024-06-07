#!/bin/bash

# this helper script allows you to integrate additional licenses and sources into the application binary
# just place the required licenses and sources into the directory licenses and sources respectively,
# run this script and then build the application.
# You need to place an overview.html into the licenses dir to integrate everything into the dialog.
# Be aware: the assets that you want to include depend on your build and your way of distribution.

TARGET=src/additional_legal.qrc

echo "<RCC>" > $TARGET

echo "  <qresource prefix=\"/additional_legal/\">" >> $TARGET
# Iterate over files in directory licenses
for file in `find licenses -type f` ; do
echo "    <file>"../$file"</file>" >> $TARGET    
done
# Iterate over files in directory sources
for file in `find sources -type f` ; do
echo  "   <file>"../$file"</file>"  >>  $TARGET
done
echo "  </qresource>" >> $TARGET

echo "</RCC>" >> $TARGET