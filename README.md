# Compiler From Ya! -> MIPS

*Ya!* is a programming language designed by the teacher of compilators at Universidade of Évora.

This project compiles code from *ya!* to [*MIPS instructions*](https://en.wikipedia.org/wiki/MIPS_architecture). In the process a *abstract parse tree* ([APT](https://en.wikipedia.org/wiki/Parse_tree)) is also created using [*Latex*](https://en.wikipedia.org/wiki/LaTeX).
The compiler is done using [*Flex*](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)), [*Bison*](https://en.wikipedia.org/wiki/GNU_Bison) and [C99](https://en.wikipedia.org/wiki/C99).

## Dependencies
[Flex](https://github.com/westes/flex), [Bison](https://www.gnu.org/software/bison/), [Texlive](https://tug.org/texlive/) (used to compile tex to pdf).

## Compiling
```bash
make
```
## Run
```bash
./ya!c <program.ya>
```

## APT
```bash
pdftex <program.tex>
```

