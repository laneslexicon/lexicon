cd frontis.iconset
SZ=16x16
sips -z 16 16 frontis.png --out  "icon_$SZ.png"
sips -z 32 32 frontis.png --out  "icon_16x16@2x.png"

SZ=32x32
sips -z 32 32 frontis.png --out "icon_$SZ.png"
sips -z 64 64 frontis.png --out "icon_32x32@2x.png"


SZ=128x128
sips -z 128 128 frontis.png --out  "icon_$SZ.png"
sips -z 256 156 frontis.png --out  "icon_128x128@2x.png"

SZ=256x256
sips -z 256 256 frontis.png --out "icon_$SZ.png"
sips -z 512 512 frontis.png --out "icon_256x256@2x.png"


SZ=512x512
sips -z 512 512 frontis.png --out "icon_$SZ.png"
sips -z 1024 1024 frontis.png --out "icon_512x512@2x.png"

cd ..
iconutil -c icns frontis.iconset

