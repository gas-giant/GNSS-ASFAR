# GNSS-ASFAR

## build

```sh
# initialize
cmake -S . -B build

# build
cmake --build build

# test
ctest --test-dir build --output-on-failure
```