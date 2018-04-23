.PHONY: compile clean configure

run: compile
	@Build/badger | sed -e 's/<1>/<NEWLINE>/' \
	                    -e 's/<2>/<ID>/'

clean:
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	ninja -C Build
