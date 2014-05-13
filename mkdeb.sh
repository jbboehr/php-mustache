#!/bin/sh
phpize
./configure CFLAGS="-O3"
make clean all
DIR=`php-config --extension-dir | cut -c 2-`

rm -rf debian

mkdir -p debian
mkdir -p debian/DEBIAN
mkdir -p debian/$DIR

cp debian.control debian/DEBIAN/control

UBUNTU=`uname -v | grep -ci ubuntu`
mkdir -p debian/etc/php5/conf.d/
echo "extension=mustache.so" >> debian/etc/php5/conf.d/mustache.ini

cp modules/mustache.so debian/$DIR
dpkg -b debian php5-mustache-`git describe --abbrev=0 --tags`_`uname -m`.deb
rm -rf debian/
