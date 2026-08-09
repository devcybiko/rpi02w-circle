# RPi Zero W Circle framework

This is a 32-bit Circle base application for an RPi Zero W (and the Circle
targets supported by the selected `circle/Config.mk`). It initializes:

- HDMI framebuffer display and serial logging;
- the boot SD card as the `SD:` FAT filesystem;
- onboard WLAN with DHCP and WPA supplicant;
- USB host plug-and-play, including keyboard input and a framebuffer mouse cursor;
- an HTTP server on port 80 serving SD-card HTML files from `/www`.

## SD-card files

Copy `wpa_supplicant.conf.example` to the root of the boot SD card as
`wpa_supplicant.conf` and replace its country, SSID, and password. Copy the
matching Circle WLAN firmware files into `/firmware` on that same card. The
firmware preparation details and supported filenames are in
`circle/addon/wlan/sample/hello_wlan/README`.

## Web server

The server starts after WLAN has received an IP address and listens on port 80.
It serves static files from `SD:/www`, which is the `/www` directory at the
root of the boot SD card. The bundled site lives in this project's `web/`
directory; its contents must be copied to the card as follows:

```text
projects/framework/web/index.html  ->  SD:/www/index.html
projects/framework/web/css/...     ->  SD:/www/css/...
projects/framework/web/js/...      ->  SD:/www/js/...
```

The Docker deployment script copies this directory automatically. For a manual
SD-card deployment, copy the *contents* of `web/` into `/www/`, not the
directory itself into `/www/web/`.

### Static routes

`/` maps to `/www/index.html`. Nested static files are supported and the
following types are served with their appropriate HTTP content type:

- HTML: `.html`, `.htm`
- styles and scripts: `.css`, `.js`
- images: `.svg`, `.png`, `.jpg`, `.jpeg`, `.ico`
- web fonts: `.woff2`

The server rejects paths containing `..` or backslashes. Responses are read
into a fixed 1 MiB buffer; files larger than 1 MiB return HTTP 500. Because
each simultaneous request has its own buffer, keep concurrent clients low on
the Zero W.

### Status API

`GET /status` returns JSON computed at request time. It does not require a
file on the SD card. For example:

```json
{
  "uptime_seconds": 123,
  "wlan_connected": true,
  "ip_address": "192.168.1.50",
  "hostname": "raspberrypi",
  "display_width": 1920,
  "display_height": 1080,
  "heap_free_bytes": 123456
}
```

After boot, use the WLAN address displayed in the serial log or on HDMI:

```text
http://<pi-address>/
http://<pi-address>/status
```

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
