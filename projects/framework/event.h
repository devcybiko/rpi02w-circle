#pragma once

#include <circle/types.h>

enum class EventType : u16
{
	None = 0,
	GPIO,
	Key,
	Logger,
	Mouse,
	Network,
	Status,
	Count
};

using EventMask = u64;

constexpr EventMask EventBit (EventType Type)
{
	return static_cast<EventMask> (1ULL) << static_cast<unsigned> (Type);
}

static_assert (static_cast<unsigned> (EventType::Count) <= sizeof (EventMask) * 8,
	      "EventMask is too small for EventType");

struct KeyEvent
{
	u16 key;
	u8 pressed;
};

struct LoggerEvent
{
	u8 level;
	char message[64];
};

struct MouseEvent
{
	u8 event;
	u8 buttons;
	s16 x;
	s16 y;
	s16 wheel;
};

struct GPIOEvent
{
	u8 pin;
	u8 value;
};

struct NetworkEvent
{
	u32 address;
};

struct StatusEvent
{
	u32 code;
	u32 value;
};

struct Event
{
	static Event Key (u16 nKey, boolean bPressed);
	static Event Log (unsigned nLevel, const char *pFormat, ...);
	static Event Mouse (unsigned nEvent, unsigned nButtons,
			    int nX, int nY, int nWheel = 0);

	EventType type;
	u8 sourceCore;

	union
	{
		GPIOEvent gpio;
		KeyEvent key;
		LoggerEvent logger;
		MouseEvent mouse;
		NetworkEvent network;
		StatusEvent status;
	};
};
