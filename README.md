# Cell3D

Cell3D is a dynamic 3D cellular automata engine with lua scripting support

## Installation

**Dependencies**: 
* [Lua 5.3](https://www.lua.org/versions.html)
* [Raylib 3.5.0](https://github.com/raysan5/raylib/releases/tag/3.5.0)

**Simplest possible build command**
```
gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -llua
```
Your build command will vary depending on your system. Use `pkgconf --cflags --libs lua53` to find your lua installation.
## Usage

```
(path to executable) (path to lua file)
```
Example:
```
./main examples/random.lua
```
## Scripting
Cell3D is powered with lua scripts. If you want to create your own simulations, look at template.lua for a template and then examples/ for different cool examples on how Cell3D is used.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

New examples for cool cellular simulations are also welcome.

## License
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html)

# Screenshots
![clouds1](https://i.imgur.com/rps9yUr.png)
![gameoflife](https://i.imgur.com/hiGoWRa.png)
![crystalgrowth1](https://i.imgur.com/DY5R9qg.png)
![singlepointreplication](https://i.imgur.com/ju0PT5x.png)
