# OMM TAS Tools
Fork of [PeachyPeachSM64/sm64ex-omm](https://github.com/PeachyPeachSM64/sm64ex-omm) with TAS tools built in. 

Feel free to report bugs and contribute, but remember, there must be **no upload of any copyrighted asset**. 
Run `./extract_assets.py --clean && make clean` or `make distclean` to remove ROM-originated content.

## About
You can build this with the [usual build method](https://github.com/sm64pc/sm64ex/wiki).
Unfortunately save states are not are not implemented currently.

## How to use

### Frame Advancing
Frame advancing is available. Press Enter on your keyboard to pause rendering frames, and tap Tab on your keyboard to advance 1 frame.

### Recording a TAS
You can record your TAS, but be aware that you cannot just open the game using your default method, you need to open your game using the terminal. This [guide](https://www.groovypost.com/howto/open-command-window-terminal-window-specific-folder-windows-mac-linux/) may be helpful. For an example, type `sm64.us.f3dex2e --tas 1 1` to open the game with TAS recording keyboard inputs starting as soon as the game opens. The first number is which controller to use. Set it to 1 for keyboard, 2 for controller, 0 for none (don't record anything). The second number is for whether or not you want the TAS to start when the game starts (useful for full game runs), or when you press Backspace on your keyboard (useful for single star runs). When you have finished recording your TAS, simply closing the game will save your TAS to `write.m64`, in the same folder as your game executable.

### Viewing a TAS
In order to view a TAS, you must change the file name of the TAS from `write.m64` to depending on what type of TAS it is, one of these names. If it is a single star TAS (or a run that starts at a certain point in the game), change the name to `singleStar.m64`. If it is a full game run (or a run that starts when the game starts), change the name to `cont.m64`.
For runs that start when the game starts, no command line parameters are needed to start the play back. To start a run that begun from a certain point (for example on star select), use the command `sm64.us.f3dex2e --tas 0 0`, manually get to the point in the game where the TAS started when recording and press Backspace on your keyboard.
