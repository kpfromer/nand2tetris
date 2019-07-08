# Jack Compiler

## Usage

To use the jack compiler you must build the c++ binary first.

### Building the Binary

To do so you need to have [cmake](https://cmake.org/), a [c++ compiler (like clang, or gcc)](http://www.stroustrup.com/compilers.html), and the [c++ boost libraries](https://www.boost.org/users/download/).

Then create a folder called `build` in the root directory and run:
```
cmake ..
make
```

This will create the `JackCompiler` binary!

### Running the Binary

On unix based systems you can run the binary like so `./JackCompiler File.jack`

