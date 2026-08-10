# macOS LVGL simulator

The macOS target uses SDL while compiling the same UI in `../shared/` as the
Raspberry Pi target. Install the host dependency once:

```sh
brew install sdl2 pkg-config
```

From the repository root, build and run it with:

```sh
./scripts/mac-run.sh
```
