# Bomber

This program finds routes  through an ASCII map. The map is fairly  standard, with
flat ground (`.`), walls (`#`), and water (`~`).  But  there are also some bombs
(`*`) laying around!  If these are found, they are collected and used to blow up walls
to reach places.


## The Program

The program takes in one command line argument.  This will be the path to a
map file,  as described below.  The program reads routing requests
from its standard input. Each request consists of four `int`s:

- The latitude of the starting point.
- The longitude of the starting point.
- The latitude of the destination.
- The longitude of the destination.

The program attempts to find a route from the starting point to the
destination, and prints a single line with the result:

- If either point is invalid, prints a line starting with `Invalid`.
- If there is no possible route, prints a line starting with `No route`.
- Otherwise, prints the route as described below.

It repeats reading routing requests and printing routes until the end of the
input is reached.  Then the program exits.

```
[ariadne@naxos]$ cat data/map.txt
...#..
.*.###
......
###.##
.....#
[ariadne@naxos]$ ./bomber -i data/map.txt
src> 0 0
dst> 4 0
eessesswww
src> 4 0
dst> 0 4
eeennwwnneee
src> 0 4
dst> 4 0
No route from (0, 4) to (4, 0).
```

`Map` functions:

- The constructor should take an input stream and read in a map file.
- Add a destructor if your `Map` needs to clean up any external resources.
- The `route()` function should try to find a route between two points:
  - If either input point is invalid, throw a `PointError`.
  - If there is no possible route, throw a `RouteError`.
  - Otherwise, return a valid route as a string.


### Map Files

A map is a text file.  All the lines in the file are the same length,
and contains only the following characters (and newline characters).

- `.`  This is regular ground.  You can walk on it.
- `*`  This is a bomb sitting on regular ground.  If walked on, the bomb can be picked 
  up and the space becomes regular ground (`.`).
- `#`  This is a boulder.  If you  aren't carrying any bombs,  you can't walk on
  it. If you're carrying at least one bomb, you _can_ walk onto it: this uses up
  one bomb to destroy the boulder and the square becomes regular ground (`.`).
- `~`  This is water.  You can't walk on it.

### Routes

A route is  a string that describes where to go.  To follow a route, loop
over the characters in the string and move accordingly:

- `n`  Move one square north.
- `e`  Move one square east.
- `s`  Move one square south.
- `w`  Move one square west.

All other characters are invalid.

### Coordinates

The first line of a map file is the north edge; the last line is the south edge.
The first character of each line  is the westernmost,  and the last character of
each line is the easternmost.

The northernmost edge of the map is at latitude zero;  latitude increases as you
move  south.  The westernmost  edge of the map  is at longitude zero;  longitude
increases as you move east.

When entering or displaying points, latitude comes before longitude.

### Extra Details

- A starting point is invalid if its coordinates are out of bounds, or if you
  can't walk on the terrain at that point.
- A destination point is only invalid if its coordinates are out of bounds.
- You always start without any bombs.
- You can carry any number of bombs.
- If you start on a square with a bomb, you immediately pick it up.
- All  routes are for the  original map.  Any changes  you make to the map while
  calculating one route need to be reset before calculating the next route.
