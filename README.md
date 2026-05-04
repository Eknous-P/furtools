# furtools

general collection of one-off tools for [furnace](https://github.com/tildearrow/furnace)

## building

### linux

install the qt6 dev libraries, then the usual
```bash
mkdir build
cd build
cmake ..
make -jxx
```
(where xx is the numebr of jobs to run, usually the number of cpu threads you have)

### windows

???