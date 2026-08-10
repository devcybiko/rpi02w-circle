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
core number, provides cross-core event sending through the router, and
implements the standard event-consumer loop used by the secondary cores.

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

`CKernel` owns one `CEventRouter` and constructs it before the four core
objects. Each core registers its queue pointer and wakeup policy with that
router from the `CCore` base constructor. The router is therefore shared by
reference but is not global. It maps a destination core number to a queue
endpoint and maintains an `EventMask` subscription bitmap for each core.

All producer-side operations belong to `CEventRouter::QueueEvent()`: it finds
the destination endpoint, pushes a copy into that queue, and sends an IPI when
the endpoint requires one. A sender never calls a method on the destination
`CCore`. The destination core only performs consumer-side operations by
popping its own queue and dispatching `HandleEvent()`.

The `CCore` constructor also receives a `bNeedsWakeUp` policy flag. Queueing an
event sends an IPI only when that destination has enabled the flag. Core 0 sets
it to `FALSE` because its scheduler loop polls the queue; cores 1 through 3 set
it to `TRUE` because their default loops sleep in `wfi`. A future core-specific
loop that does not sleep can disable wakeups without adding special handling
for its core number to the router.

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

Code running in a `CCore` subclass sends directly to another core through the
protected base method `SendEventToCore()`. It automatically replaces the
event's `sourceCore` with the sending core number:

```cpp
void CCore1::HandleEvent (const Event &)
{
	Event Reply {};
	Reply.type = EventType::Status;
	Reply.status.code = 2;
	Reply.status.value = 100;

	if (!SendEventToCore (3, Reply))
	{
		// Core 3's queue is full or the destination is invalid.
	}
}
```

Both routing APIs return `FALSE` for an invalid destination or a full queue.
`CKernel::QueueEvent()` preserves the supplied `sourceCore`; only the
core-to-core helper stamps it automatically.

### Publish and subscribe

Direct routing coexists with event-type subscriptions. `EventType::Count`
marks the end of the event enumeration, and `EventBit()` maps each event type
to one bit in the 64-bit `EventMask`. `None` and `Count` cannot be subscribed
to or published. Because `None` reserves bit 0, the mask supports up to 63
subscribable event types.

A core subscribes and publishes through protected `CCore` methods:

```cpp
CCore2::CCore2 (CEventRouter *pEventRouter)
: CCore (pEventRouter, 2, TRUE)
{
	Subscribe (EventType::GPIO);
	Subscribe (EventType::Status);
}

void CCore2::HandleEvent (const Event &EventToHandle)
{
	if (EventToHandle.type == EventType::GPIO)
	{
		Event Status {};
		Status.type = EventType::Status;
		Status.status.code = EventToHandle.gpio.pin;
		Status.status.value = EventToHandle.gpio.value;

		u32 DeliveredTo = PublishEvent (Status);
		(void) DeliveredTo;
	}
}
```

`PublishEvent()` stamps the publisher's core number into `sourceCore`, skips
the publishing core, and copies the event into every other queue subscribed to
that type. Its return value is a core bitmap: bit 0 represents core 0, bit 1
represents core 1, and so on. A bit is set only when that destination accepted
the event, so a full queue appears as a missing bit.

Subscriptions can be removed with `Unsubscribe(EventType)`. Kernel-level code
can use `CKernel::Subscribe(core, type)`, `CKernel::Unsubscribe(core, type)`,
and `CKernel::PublishEvent(event)`. The kernel-level publish operation uses the
event's existing `sourceCore` field.

Subscription changes are protected by the router's IRQ-safe spinlock. Publish
takes a snapshot of matching recipients under that lock, releases it, and only
then writes to their queues. This avoids holding the router lock while queue
locks and wakeup IPIs are used.

All four cores have queues and can receive direct or published events. After a
successful enqueue to a core whose `bNeedsWakeUp` policy is enabled, that core
receives a user inter-processor interrupt to wake its `wfi` wait loop. A
sleeping consumer checks its queue with interrupts temporarily masked so an
event cannot be lost between the empty check and entering the wait state. Core
0 disables the policy because it checks its queue as part of its existing
scheduler loop.

The virtual `CCore::HandleEvent()` method is the extension point each subclass
implements to add core-specific behavior for each event type.

## Service dispatcher

`CService` and `CDispatcher` provide the core-local service runtime. Every
`CCore` owns a dispatcher connected to that core's event queue. Core 0 registers
Screen, Serial, Logger, HID, Storage, and Web services in that dependency order
and supplies its Circle scheduler to the inherited dispatcher.

A service implements:

- `InitService()` for startup work after hardware dependencies are ready;
- `Update()` for polling and housekeeping;
- `OnEvent(const Event&)` for event handling.

The base `InitService()` returns `TRUE`, so services without startup work do not
have to override it. Constructors should only store dependencies and initialize
local state; hardware access belongs in `InitService()` or later methods.

Each service contains `m_timeout`, an absolute deadline in Circle's continuous
1 MHz clock ticks, and `m_periodMS`, its update period in milliseconds. The
period is passed to the `CService` constructor. After calling `Update()`, the
dispatcher automatically calls `ResetTimer()` to schedule the next deadline.
Pass `CService::PeriodPolling` to call `Update()` on every dispatcher pass, or
`CService::PeriodDisabled` for an event-only service. These sentinels have the
values `0` and `UINT32_MAX`, respectively; other values are periods in
milliseconds.

After all services are registered, `CDispatcher::InitServices()` iterates over
them in registration order, calls each service's `InitService()`, and starts
its timer. Registration is rejected after successful service initialization,
and `Run()` requires initialization first. If any service fails,
`InitServices()` returns `FALSE` and does not start the run loop.

The dispatcher is constructed with an event queue and accepts up to 64 service
registrations. Its hard `Run()` loop first calls `Update()` on every service
whose deadline has expired. It then pops at most one event and passes that
event to every registered service's `OnEvent()` method. Services are
responsible for ignoring event types that do not interest them. Processing one
event per pass ensures timeout checks occur between queued events. Core 0 gives
its dispatcher a scheduler pointer, so each pass yields to cooperative tasks such
as the HTTP server.

`CHIDService` owns `CUSBHCIDevice` and all USB keyboard and mouse behavior. It
initializes the USB host controller, performs task-level plug-and-play polling,
discovers attached input devices, handles removal and input callbacks, updates
keyboard LEDs and the mouse cursor, configures the framebuffer cursor, and
logs attachment changes. Core 0 registers it after the logger, and the dispatcher
calls its polling `Update()`. Its `OnEvent()` currently ignores core events.

`CScreenService` owns and initializes `CScreenDevice`. It is event-only and
requires no periodic update. Core 0 uses its screen accessor for logger output
fallback and injects the same screen device into the HID and web-server
services.

`CSerialService` owns the Raspberry Pi hardware UART and initializes it at the
configured baud rate, currently 115200. It is event-only and is initialized
after the screen but before the logger resolves its configured output device,
preserving serial logging during the rest of startup.

`CLoggerService` owns `CLogger`, resolves the configured logging device through
Circle's device-name registry, and falls back to the screen when that device is
unavailable. It is event-only and is initialized after screen and serial.
Circle's global `CLogger::Get()` access remains available to all other services.

`CStorageService` owns `CEMMCDevice` and the FATFS mount object. It initializes
the SD/eMMC controller, mounts the configured `SD:` drive, and reports mount
failures through the logger. It is event-only and is initialized before WLAN
so firmware and configuration files are available from the SD card.

`CWebServerService` owns the complete WLAN-backed web stack: `CBcm4343Device`,
`CNetSubSystem`, `CWPASupplicant`, and the process-lifetime `CSDWebServer`
scheduler task. Its `InitService()` preserves the required
`WLAN → Network → WPA` order after storage has mounted the firmware and
configuration files. Its `Update()` waits for DHCP/network readiness, logs the
assigned address, creates the web-server task once, and logs the listening port
and document root. It is registered last with core 0's dispatcher so storage and
all device dependencies are ready before its initialization begins.

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

## Shared LVGL UI

`projects/guiapp/shared/` contains the LVGL-only application code. Its
platform adapters are deliberately separate: `projects/guiapp/rpi/` provides
the Circle HDMI framebuffer integration, and `projects/guiapp/macos/` provides
the SDL simulator. See the macOS README for its build command.

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
