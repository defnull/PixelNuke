.PHONY: release debug clean

release:
	-mkdir release
	cd release; cmake -DCMAKE_BUILD_TYPE=Release ..
	cd release; make

debug:
	-mkdir debug
	cd debug; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd debug; make

clean:
	-rm -rf debug release
