GRAMMAR := YAML
PARSER := Build/badger

export ASAN_OPTIONS := detect_leaks=1,detect_container_overflow=0
export CC := /usr/local/opt/llvm/bin/clang
export CXX := /usr/local/opt/llvm/bin/clang++

.PHONY: compile clean configure test

all: lint

lint: run
	@printf '\n✨ Lint\n'
	@oclint -p Build -no-analytics -enable-global-analysis \
	        -enable-clang-static-analyzer Source/*.cpp

run: test
	@printf '\n🏃🏼‍♂️ Run\n\n'
	@sed -nE "s~(^[^'][^=]+)=(.*)~s/<\2>/<\1>/~p" Build/$(GRAMMAR).tokens > \
	     Build/$(GRAMMAR).sed
	@set -o pipefail; $(PARSER) Input/Null.yaml | sed -f Build/$(GRAMMAR).sed

test: compile
	@printf '\n🐛 Test\n\n'
	@Test/test.fish

compile:
	@printf '👷🏽‍♀️ Build\n\n'
	@ninja -C Build | sed -e 's~\.\./~~'

configure:
	@mkdir -p Build
	@cd Build; cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

clean:
	@printf '🗑 Clean\n'
	@rm -rf Build
