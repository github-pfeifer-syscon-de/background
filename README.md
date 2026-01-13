# StarDesk
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

## Running

There are two separate modi how to run this program.
Without any parameter the program runs in interactive mode.
A window is opened that displays the described infos,
changes can be made by right clicking on the content.

Alternativly this can be run in daemon mode with
either the option -d or --daemon
(since desktops may be stubborn about starting a program with
a parameter, this mode is also available when adding daemon=true
to main config section).
In this case a placeholder window is opened
that gives access to a application menu.
The image is updated by the desktop enviroment.
As there is no common convention for this setting,
it might help to search some suggestions on the web
what command changes the background.
> [!WARNING]
> But be warned the image stays if the program is terminated,
> i suggest playing "i will always be in a club in 1973 ... "
> so you check a instance of the program is running before
> trusting the displayed time.

### Find config-name for Xfce4

On the first start in daemaon-mode
the setting desktopBackground in background.conf
is created. Most likely this will not work, so you have
to find the correct setting by running
```
xfconf-query -c xfce4-desktop -m
```
in a terminal and changing the background
by the config dialog of the desktop environment,
this will print the settings name used for this.
So the correct settings name can be modified for
the desktopBackground setting (after -p).
The placeholder $img and will be replaced with the image file.

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
(expects your desktop-environment has some editor for python files registered).
As long as the dialog is shown,
changes will be made visible when the file is saved.
The modified version will be placed in `$USER_HOME/.local/share/background`
(if you want to know where your files are).
If you modify the display function of modules remember to also
adapt the getHeight function, this allows
the correct layout when multiple modules will share
one position for example.
The python function will be precompiled
and place alongside the user version of the sources.

One error may arise from the precompiled .pyc files in case of 
a python version change, this was tried to be avoided by a workaround.
But still it might help to remove the .pyc files from 
`$USER_HOME/.local/share/background`
in case of a stacktrace that ends somewhere into python. 

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
