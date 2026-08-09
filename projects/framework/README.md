# RPi Zero W Circle framework

This is a 32-bit Circle base application for an RPi Zero W (and the Circle
targets supported by the selected `circle/Config.mk`). It initializes:

- HDMI framebuffer display and serial logging;
- the boot SD card as the `SD:` FAT filesystem;
- onboard WLAN with DHCP and WPA supplicant;
- USB host plug-and-play, including keyboard input and a framebuffer mouse cursor.
- an HTTP server on port 80 serving SD-card HTML files from `/www`.

## SD-card files

Copy `wpa_supplicant.conf.example` to the root of the boot SD card as
`wpa_supplicant.conf` and replace its country, SSID, and password. Copy the
matching Circle WLAN firmware files into `/firmware` on that same card. The
firmware preparation details and supported filenames are in
`circle/addon/wlan/sample/hello_wlan/README`.

Copy the contents of this project's `web/` directory to `/www` on the SD card.
`/` maps to `/www/index.html`; nested HTML, CSS, JavaScript, image, and font
files are served with appropriate content types. Files larger than 1 MiB return
HTTP 500; this is a fixed per-connection buffer, so keep the number of
simultaneous clients low on the Zero W.

`GET /status` is a dynamic JSON endpoint. It reports uptime, WLAN connection
state and IP address, hostname, display dimensions, and currently free heap.

## Build

Build Circle's WLAN dependencies once (including its `hostap` submodule):

```sh
cd circle
git submodule update --init addon/wlan/hostap
cd addon/wlan
./makeall
cd ../../../projects/framework
make
```

The output image name is selected by `circle/Config.mk`; with the supplied
32-bit configuration for an RPi Zero W it is normally `kernel8-32.img`.
