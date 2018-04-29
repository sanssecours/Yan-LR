.PHONY: compile clean configure

run: compile
	@sed -E 's~([^=]+)=(.*)~s/<\2>/<\1>/~' Build/Test.tokens > Build/Test.sed
	@Build/badger | sed -f Build/Test.sed

clean:
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	ninja -C Build
