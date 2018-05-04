GRAMMAR := YAML

.PHONY: compile clean configure

run: compile
	@sed -E 's~([^=]+)=(.*)~s/<\2>/<\1>/~' Build/$(GRAMMAR).tokens > \
	     Build/$(GRAMMAR).sed
	@ASAN_OPTIONS=detect_container_overflow=0 Build/badger | \
	                                          sed -f Build/$(GRAMMAR).sed

clean:
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	ninja -C Build
