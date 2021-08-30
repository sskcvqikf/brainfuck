# BRAINFUCK COMPILER
[![Build](https://github.com/sskcvqikf/brainfuck/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/sskcvqikf/brainfuck/actions/workflows/build.yml)
[![Test](https://github.com/sskcvqikf/brainfuck/actions/workflows/test.yml/badge.svg?branch=main)](https://github.com/sskcvqikf/brainfuck/actions/workflows/test.yml)
[![Valgrind](https://github.com/sskcvqikf/brainfuck/actions/workflows/valgrind.yml/badge.svg?branch=main)](https://github.com/sskcvqikf/brainfuck/actions/workflows/valgrind.yml)

Yet another implementation of brainfuck compiler (without `,` support).
#### Description:
A small piece of software that allows you to run brainfuck programs. Developed as compiler-like architecture.
#### Build:
Just run `make`.
#### Run
Main executable is `brainfuck`. In order to run your program you must pass the actual program as argument to an executable. For example, `./brainfuck "+++++++++++."`.
There are a few fancy and tricky programs located in `tests/func` with expected results, you can discover them.
