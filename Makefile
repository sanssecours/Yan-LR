.PHONY: compile clean configure

run: compile
	@Build/badger
	@echo ""

clean:
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	ninja -C Build
