# Raspberry Pi Zero 2 W Circle framework

This is a 32-bit, four-core Circle base application for a Raspberry Pi Zero 2 W.
The project builds with `RASPPI=3`, `AARCH=32`, and
`ARM_ALLOW_MULTI_CORE`. It initializes:

- HDMI framebuffer display and serial logging;
- the boot SD card as the `SD:` FAT filesystem;
- onboard WLAN with DHCP and WPA supplicant;
- USB host plug-and-play, including keyboard input and a framebuffer mouse cursor;
- an HTTP server on port 80 serving SD-card HTML files from `/www`.

## Multicore architecture

`CKernel` derives from Circle's `CMultiCoreSupport`. Hardware and shared
services are initialized before the secondary cores are started. Circle then
calls `CKernel::Run(unsigned nCore)` on cores 1 through 3, while the main kernel
path dispatches core 0 through that same function.

All CPU-specific classes derive from the common `CCore` base in `core.h` and
`core.cpp`. The base owns the per-core event queue, records the destination
core number, sends wakeup IPIs after enqueueing, and provides the standard
event-consumer loop used by the secondary cores.

Each CPU core then has its own subclass:

- `CCore0` overrides the standard loop and contains the original framework
  functionality: display, logging, timer, scheduler, USB, SD card, WLAN, input
  devices, and the web server. It drains its inherited queue from that loop.
- `CCore1`, `CCore2`, and `CCore3` use the inherited consumer loop and currently
  supply empty `HandleEvent()` implementations for future core-specific work.

The relevant files are `kernel.cpp`, `core0.cpp`, `core1.cpp`, `core2.cpp`, and
`core3.cpp`, with matching headers.

## Per-core event queues

Every core owns a bounded multiple-producer, single-consumer event queue. Any
core or interrupt handler can produce an event, but only the destination core
removes events from its queue.

The queues have these properties:

- fixed capacity of 64 events per core;
- events are copied into the ring buffer;
- no dynamic allocation or event pointers are used;
- producers are serialized with Circle's IRQ-safe `CSpinLock`;
- FIFO ordering is preserved;
- enqueueing returns `FALSE` if the destination queue is full.

`event.h` defines the `EventType` tag and the single `Event` representation.
The event contains its type, its source core, and an anonymous union containing
one of these payloads:

- `KeyEvent` (`key`, `pressed`);
- `MouseEvent` (`x`, `y`, `buttons`);
- `GPIOEvent` (`pin`, `value`);
- `NetworkEvent` (`address`);
- `StatusEvent` (`code`, `value`).

Events can be routed through the kernel with `QueueEvent()`:

```cpp
Event EventToSend {};
EventToSend.type = EventType::Status;
EventToSend.sourceCore = 0;
EventToSend.status.code = 1;
EventToSend.status.value = 42;

if (!Kernel.QueueEvent (2, EventToSend))
{
	// Core 2's event queue is full.
}
```

After a successful enqueue to cores 1 through 3, the destination core receives
a user inter-processor interrupt. This wakes its `wfi` wait loop. The consumer
checks its queue with interrupts temporarily masked so an event cannot be lost
between the empty check and entering the wait state. Core 0 checks its queue as
part of its existing scheduler loop.

The virtual `CCore::HandleEvent()` method is the extension point each subclass
implements to add core-specific behavior for each event type.

## SD-card files

Copy `wpa_supplicant.conf.example` to the root of the boot SD card as
`wpa_supplicant.conf` and replace its country, SSID, and password. Copy the
matching Circle WLAN firmware files into `/firmware` on that same card. The
firmware preparation details and supported filenames are in
`circle/addon/wlan/sample/hello_wlan/README`.

## Web server

The server starts after WLAN has received an IP address and listens on port 80.
It serves static files from `SD:/www`, which is the `/www` directory at the
root of the boot SD card. The bundled site lives in this project's `www/`
directory; its contents must be copied to the card as follows:

```text
projects/framework/www/index.html  ->  SD:/www/index.html
projects/framework/www/css/...     ->  SD:/www/css/...
projects/framework/www/js/...      ->  SD:/www/js/...
```

The Docker deployment script copies this directory automatically.

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
the Zero 2 W.

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

### Action API

`POST /action` accepts a JSON object or array with `Content-Type:
application/json`. It currently validates and acknowledges the payload but has
no side effects. This endpoint is reserved for future runtime actions.

```sh
curl -X POST http://<pi-address>/action \
  -H 'Content-Type: application/json' \
  -d '{"action":"example"}'
```

The current response is:

```json
{"accepted":true}
```

## Build

Initialize Circle and its nested WLAN dependency:

```sh
git submodule update --init circle
git -C circle submodule update --init addon/wlan/hostap
```

Build the Docker image and start the persistent build container:

```sh
scripts/docker-build.sh
scripts/docker-run.sh
```

Leave that container running. From another terminal, build Circle's required
libraries and the framework incrementally:

```sh
scripts/docker-make.sh
```

The build produces `projects/framework/kernel8-32.img`. When `/Volumes/CIRCLE`
is mounted, `docker-make.sh` also copies the kernel and web assets to the SD
card, synchronizes it, and ejects the volume. If the volume is not mounted, the
deployment steps are skipped.

Normal builds retain all generated objects so Make can rebuild only what
changed. To clean the framework and every Circle library used by it before
building again, run:

```sh
scripts/docker-make.sh clean-all
```
