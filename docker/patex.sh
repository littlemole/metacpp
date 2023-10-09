#!/bin/bash
set -e

cd /usr/local/src/
git clone https://github.com/littlemole/patex.git

/usr/local/bin/build.sh patex
