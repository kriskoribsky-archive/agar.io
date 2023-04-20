# Links for resources used as a reference in this project
## Curses/Ncurses documentation
https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
https://pubs.opengroup.org/onlinepubs/7908799/xcurses/curses.h.html

## Menus
https://techlister.com/linux/creating-menu-with-ncurses-in-c/
https://www.viget.com/articles/c-games-in-ncurses-using-multiple-windows/

## Datatype sizes
https://www.tutorialspoint.com/c_standard_library/limits_h.htm

## Int vs Short
### I was wondering why not use short datatype instead of int to save up memory space
https://dev.to/joaofbantunes/til-int-vs-short-and-the-unexpected-performance-impact-1ij4
https://social.msdn.microsoft.com/Forums/en-US/839b9948-4fc7-471e-8ad6-f8473e2cc317/why-do-we-always-use-int-why-not-short?forum=csharplanguage

## String length from string pointer (without <string.h>)
https://stackoverflow.com/a/3213955

## Variadic functions
https://www.geeksforgeeks.org/variadic-functions-in-c/

## Macros in C
https://stackoverflow.com/a/5382326

## Valgrind test
valgrind --leak-check=full --show-leak-kinds=all --show-error-list=yes --log-file="tests/valgrind" ./agario