# background
a live star desktop background
(if you expect to see pictures of neil degrasse tyson i'm sorry,
this was not my intention).
see AUTHORS for &copy; included data.

![Background](background.png "background")


Is it really worth to do something that is already available
with e.g. stellarium?

It depends what you expect.
if you want the full package with much more details and
i guess the best precision that is available,
go for a astronomy application.

On the other hand, if you want some infos on a regular basis,
this offers you some option with a app that does
not take a whole bunch of resources
(but i must admit there are more needed than i expected).
And as a side effect, some insight might be easier to gain
at a somewhat smaller scale...

Depends on genericImg see there for build instructions.

## Infos

The infos available for non linux systems are limited ...

### Python

By default for rendering the info blocks python will be used
(this requires the Development install of python (3), python-gobject and python-cairo
depending on your Linux flavor names may vary).
If you don't like this option use
```
./configure --prefix=/usr --without-python
```
the C++ functions for rendering will be used in this case.

With python you have the option to easily
modify the displayed infos e.g. use a weather or news service ...
Some alternative functions are already added to the sources
e.g. modify the clock drawAnalog function to:
```
        for i in range(60):
           self.drawRadial(ctx, i, 60, (i % 5) == 0, radius)
        self.drawHand(ctx, hours, 60, radius, True, now.hour >= 12)
        self.drawHand(ctx, min, 60, radius, False, now.hour >= 12)

```
to get a "classic" clock shape.

The .py files are localed by default in /usr/share/background-program
a user version can be created
by using the -> Script -> Edit option from the parameter context menu
(expects your desktop-enviroment has some editor for python files registered).
As long as the dialog is shown,
changes will be made visible when the file is saved.
The modified version will be placed in $USER_HOME/.local/share/background
(if you want to know where your files are).
If you modify the display function of modules remember to also
adapt the getHeight function, this allows
the correct layout when multiple modules will share
one position for example.
The python function will be precompiled
and place alongside the user version of the sources.

#### Windows/Msys2

The simple init does not work with Msys2/Windows as it seems (at least for me).
If you want to look into the issue there is a #ifdef section in PyWrapper
that outputs the used path. This should allows to idententify where things go wrong
(look for directory components with a weired location,
the included .zip entry seems to be optional).
By using a mingw shell and setting:
```
 export PYTHONPATH="C:/msys64/mingw64/lib/python312.zip;C:/msys64/mingw64/lib/python3.12;C:/msys64/mingw64/lib/python3.12/lib-dynload;C:/msys64/mingw64/lib/python3.12/site-packages"
```
things started to look better.
