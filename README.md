Leave The Labyrinth (ltl)
=========================

This is only a very simple maze game. It runs in the GNU/Linux console, using
`ncurses.h`. The purpose of this project is to learn and make a maze generation
algorithm.

To compile this program, go in its directory and enter the command, `make`. But
before you do this, you may want to change the default key bindings. Change them
in the beginning of `text_ui.h`, and then compile and execute the game.

How to play
-----------

You start in the top-left corner, and win when you arrive in the green cell. Use
`t`, `s`, `r`, `n` to move for LEFT, DOWN, UP, RIGHT, or the arrow keys.

A trace is left every hundred steps.

