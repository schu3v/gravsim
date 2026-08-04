# gravsim
Gravitation Simulation

## Building
To build gravsim you must have ``SDL2``.

```
git clone https://github.com/schu3v/gravsim.git 
cd gravsim
mkdir obj
make
```

## Configuration Files
The base for interaction with gravsim is configuration files, they enables write information about system and uses for saves.
What means "system"? System is necessary information for physics calculations and drawing:

- Iteration. Uses for countdowning iterations, saves and exiting from gravsim.
- Gravitational constant.
- Physics information of body: x, y, vx, vy, m.
- Rendering information of body: color (RGB) and radius. It's not required if flag ``-fnoreninfo`` specified.

To write a configuration file you may follow next template:

```
@iter 0
@G 1

# Bodies
%
x 0.0
y 0.0
vx 0.0
vy 0.0
m 30.0
r 7
rgb FFFF00
%
x 0.0
y -100.0
vx -0.4
vy 0.0
m 1.0
r 5
rgb	FF0000
%
x 0.0
y 100.0
vx 0.5
vy 0.0
m 1.0
r 5
rgb 0000FF

```

Every configuration file must to conform farther rules:
- Designations (keys): 
	- @iter - iteration.
	- @G - gravitational constant (may be decimal fraction).
	- r - radius (may be decimal fraction).
	- rgb - color in RRGGBB hexadecimal format.
	- And x, y, vx, vy, m - also may be decimal fractions.
- Line format: ``[KEY without spaces][TABS and/or SPACES][VALUE without spaces]``, or only comment, or only ``%``, or empty line
- Before body must be written ``%``.
- For single-line commentaries write ``#``. 
- If you specified ``-fnoreninfo`` flag, you may don't write ``r`` and ``rgb``.

## Flags and Options
When you built gravsim, you can start program from cli. The ``-sys`` option is **required**.

``./gravsim -sys path/to/your/system [Another flags and options]``

- ``-sys PATH`` - Path to system configuration file for drawing and calculating. This option is required.
- ``-sdir DIR`` - Save directory. (default: ``./``).
- ``-spref PREF`` - Prefix for configuration save files. (default: ``sys_``).
- ``-siter N`` - Program will automatically save current state in configuration file every N iterations.
- ``-eiter N`` - Exit iteration (save executes before exit). 
- ``-w N`` - The width of window. (default: full width).
- ``-h N`` - The height of window. (default: full height).
- ``-fps N`` - FPS. (default: 60).
- ``-scale N`` - Scale multiplier. (default: 2.0).
- ``-move N`` - Move step. (default: 10.0)
- ``-fixi N`` - The index (starts from 0) of fixed body. This body centered by window.
- ``-fpause`` - Stop simulation in the beginning.
- ``-fnodraw`` - Don't create window.
- ``-fnoreninfo`` - Don't read/write rendering information from/to configuration files. Use default radius (3) and color (white) for every body.
- ``-fnolog`` - Don't print all information in cli.
- ``-fnologi`` - Don't print iteration.
- ``-fnologb`` - Don't print bodies.

## Saves
Specially, I explain how saves works. Every saved file names in such format: ``[PREFIX][ITERATION].conf``, 
PREFIX defines via ``-spref``, ITERATION is iteration when save done, also you may define save directory via ``-sdir``.
Sometimes you need in automatically saves, then set number N via ``-siter``, and saves will be done automatically every N iterations.
When you or gravsim create save, file will be named in save file format, and this file may be used after.

## Keyboard and Mouse Control
- The keys w, a, s, d for movement to forward, right, backward and left respectively.
- The key l to save.
- The SPACE key to pause.
- The ARROW UP/DOWN for scaling.
- Click on body by left mouse button to fix on it.
- Click on right mouse button to reset movement and fixation.
