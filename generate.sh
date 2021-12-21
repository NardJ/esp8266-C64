#!/bin/bash
# shellcheck disable=SC2046
DIR=$(cd $(dirname "$0") && pwd)

rm $DIR/webpages.h
rm -r $DIR/out

mkdir -p $DIR/web
mkdir -p $DIR/out/b64


#for filepath in $DIR/web/images/*; do
#	filename=$(basename $filepath)
#	base64 -w 0 $filepath > $DIR/out/b64/$filename.b64
#done

for filepath in $DIR/web/*.html; do
	filename=$(basename $filepath)
	sless=${filename%}
	cp $filepath $DIR/out/$sless
#	sed -i "s^{{FAVICON_DATA}}^$(cat $DIR/out/b64/favicon.ico.b64)^" $DIR/out/$sless
#	sed -i "s^{{FFH_LOGO}}^$(cat $DIR/out/b64/logo-ff.png.b64)^" $DIR/out/$sless
#	sed -i "s^{{POWER_SYMBOL}}^$(cat $DIR/out/b64/power.svg.b64)^" $DIR/out/$sless
done

for filepath in $DIR/web/*.js; do
	filename=$(basename $filepath)
	sless=${filename%}
	cp $filepath $DIR/out/$sless
#	sed -i "s^{{FAVICON_DATA}}^$(cat $DIR/out/b64/favicon.ico.b64)^" $DIR/out/$sless
#	sed -i "s^{{FFH_LOGO}}^$(cat $DIR/out/b64/logo-ff.png.b64)^" $DIR/out/$sless
#	sed -i "s^{{POWER_SYMBOL}}^$(cat $DIR/out/b64/power.svg.b64)^" $DIR/out/$sless
done


cp $DIR/webpages.h.j2 $DIR/webpages.h

sed -i $'/{{INDEX}}/{r out/index.html\nd}' $DIR/webpages.h
sed -i $'/{{EXTENDED}}/{r out/VICII.js\nd}' $DIR/webpages.h
