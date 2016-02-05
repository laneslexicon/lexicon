#!/bin/sh
test -f laneslexicon.dmg && rm laneslexicon.dmg
~/create-dmg/create-dmg \
--volname "LanesLexicon" \
--background background.png \
--window-pos 200 120 \
--window-size 800 400 \
--volicon laneslexicon.png \
--icon-size 100 \
--icon laneslexicon.app 200 190 \
--hide-extension laneslexicon.app \
--app-drop-link 600 185 \
laneslexicon.dmg \
./laneslexicon.app
