#!/bin/bash
set -e

if [[ ${1:-} == clean-all ]]; then
	docker exec -it circle-dev sh -c '
		export RASPPI=3 AARCH=32 DEFINE=-DARM_ALLOW_MULTI_CORE
		make -C projects/framework clean-all
	'
fi

docker exec -it circle-dev sh -c '
	export RASPPI=3 AARCH=32 DEFINE=-DARM_ALLOW_MULTI_CORE
	cd circle
	./makeall --nosample CHECK_DEPS=0
	cd addon/SDCard
	make CHECK_DEPS=0
	cd ../fatfs
	make CHECK_DEPS=0
	cd ../wlan
	./makeall --nosample CHECK_DEPS=0
	cd ../../..
	make -C projects/framework
'
if [[ -d /Volumes/CIRCLE ]]; then
	cp projects/framework/kernel8-32.img /Volumes/CIRCLE/kernel8-32.img
	mkdir -p /Volumes/CIRCLE/www
	cp -R projects/framework/www/. /Volumes/CIRCLE/www/
	sync
	diskutil eject /Volumes/CIRCLE
fi
