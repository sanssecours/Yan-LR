GRAMMAR := YAML
export ASAN_OPTIONS := detect_leaks=1,detect_container_overflow=0
export CC := /usr/local/opt/llvm/bin/clang
export CXX := /usr/local/opt/llvm/bin/clang++

.PHONY: compile clean configure

run: compile
	@sed -E 's~([^=]+)=(.*)~s/<\2>/<\1>/~' Build/$(GRAMMAR).tokens > \
	     Build/$(GRAMMAR).sed
	@set -o pipefail; Build/badger | sed -f Build/$(GRAMMAR).sed

clean:
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	ninja -C Build
