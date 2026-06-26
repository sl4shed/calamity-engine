# Tips & Tricks {#tips_and_tricks}

## Tracy

[Tracy](https://github.com/wolfpld/tracy) is a real-time profiler. It's pretty useful if you're trying to find performance issues in your code.

To use it, simply configure your app with the `-DTRACY_ENABLE=ON` option, then open the tracy client and connect to your app.

For more information regarding tracy usage, please take a look at it's [documentation](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf).

## Emscripten games

You may have noticed that the template project and the examples all use a separate `loop()` function, which you don't need at all if you are only targeting native platforms! (In fact, it's a lot easier to not use a separate loop function).

However, if you want your game to compile to Emscripten, you need a separate loop function and you need SOME way to pass the Engine service and the Graphics service to it (you also need to register it with Emscripten). This is the way it's done in the template project and in every example:

```cpp
// at the top of the file
static Engine engine = Engine("game name goes here");
static Graphics* graphics = nullptr;

void loop() {
    engine.update();
    engine.render(*graphics);
}

// in the main function
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!input.shouldQuit) {
        loop();
    }
    engine.exit();
    delete graphics;
#endif
```
