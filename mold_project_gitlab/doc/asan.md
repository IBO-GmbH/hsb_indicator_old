# asan

## install
apt install llvm-3.7

## enable
set `USE_ASAN="TRUE"` in `build/make_build`
build as usual

## run
set environment variables
```
export ASAN_OPTIONS=symbolize=1
export ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-3.7/bin/llvm-symbolizer
```
`ASAN_SYMBOLIZER_PATH` already gets set by environment.\
run as usual. `./bin/tests_gtest`
