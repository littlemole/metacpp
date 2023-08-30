#!/bin/bash
set -e

cd /usr/local/src/
git clone https://github.com/littlemole/patex.git

if [ "$BRANCH" == "ng"  ]; then

	cd $1 && git checkout ng
fi
SKIPTESTS="true" /usr/local/bin/build.sh patex
