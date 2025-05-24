# background
a live star desktop background (some of the info functions are liunx specific)
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

## Python

By default for rendering the info blocks python will be used
(this requires the Development install of python3, python-gobject, py3cairo ).
If you don't like this option use
```
./configure --prefix=/usr --without-python
```
the C++ functions for rendering will be used in this case.

With python you have the option to easily
modify the displayed infos e.g. use a weather or news service ...

The .py files are localed by default in /usr/share/background-program
a user version can be created
by using the -> Script -> Edit option from preferences
(expects your desktop-enviroment has some editor for python files registered).
As long as the dialog is shown changes will be made visible when the file
is saved.
The modified version will be placed in $USER_HOME/.local/share/background
if you want to know where your files are.
If you modify the display function of modules remember to also
adapt the getHeight function, this allows
the correct layout when multiple modules will share
one position for example.
The python function will be precompiled
and place alongside the user version of the sources.
