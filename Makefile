.PHONY: init release debug clean

release: init
	-mkdir release
	cd release; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd release; make

debug: init
	-mkdir debug
	cd debug; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd debug; make

init:
	git submodule init
	git submodule update

clean:
	-rm -rf debug release
