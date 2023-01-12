# espy

A wheelchair motor-propelled, battery-powered, ESP32-driven remote control snow plow.

Using the build script from a xonsh terminal:

```bash
. build.xsh bdf # [b]uilds with [d]ebug mode before [f]lashing
```

Debug mode just changes the application log verbosity for now. Assuming you're
not using xonsh as your terminal, `. build.xsh bf` is equivalent to `idf.py build flash`.

Dev branch: 2023 firmware rewrite

![Parked snow plow](https://joshstock.in/static/images/rc-plow.jpg)
