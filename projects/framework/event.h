#pragma once

#include <circle/types.h>

enum class EventType : u16
{
	None = 0,
	Key,
	Mouse,
	GPIO,
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

struct MouseEvent
{
	s16 x;
	s16 y;
	u8 buttons;
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
	EventType type;
	u8 sourceCore;

	union
	{
		KeyEvent key;
		MouseEvent mouse;
		GPIOEvent gpio;
		NetworkEvent network;
		StatusEvent status;
	};
};
