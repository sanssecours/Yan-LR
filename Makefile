GRAMMAR := YAML
export ASAN_OPTIONS := detect_leaks=1,detect_container_overflow=0
export CC := /usr/local/opt/llvm/bin/clang
export CXX := /usr/local/opt/llvm/bin/clang++

.PHONY: compile clean configure test

run: compile test
	@printf '\n🏃🏼‍♂️ Run\n\n'
	@sed -nE "s~(^[^'][^=]+)=(.*)~s/<\2>/<\1>/~p" Build/$(GRAMMAR).tokens > \
	     Build/$(GRAMMAR).sed
	@set -o pipefail; Build/badger Input/Test.yaml | sed -f Build/$(GRAMMAR).sed

test:
	@printf '\n🐛 Test\n\n'
	@./test.fish

clean:
	@printf '✨ Clean\n'
	rm -rf Build

configure:
	mkdir -p Build
	cd Build; cmake -G Ninja ..

compile:
	@printf '👷🏽‍♀️ Build\n\n'
	ninja -C Build
