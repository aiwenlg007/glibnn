Glibnn
======

An implemetation of a multicouche neural network using the glib library

Description
===========

GLibnn is a very simple example of how to program a multicouche neural network using GLib and Gobect.
The user can choose the number of layer and the number of neurons per layer. Then he can use data to train the neural network in order ot proceed online data at the end.

Compilation
============

    gcc *.c -o glibnn `pkg-config --libs --cflags glib-2.0 gobject-2.0` -lm -O2 -g3
