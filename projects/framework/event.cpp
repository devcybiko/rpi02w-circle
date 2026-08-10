#include "event.h"

#include <circle/string.h>
#include <circle/util.h>
#include <stdarg.h>

Event Event::Key (u16 nKey, boolean bPressed)
{
	Event KeyEvent = {};
	KeyEvent.type = EventType::Key;
	KeyEvent.key.key = nKey;
	KeyEvent.key.pressed = bPressed ? 1 : 0;

	return KeyEvent;
}

Event Event::Log (unsigned nLevel, const char *pFormat, ...)
{
	va_list Args;
	va_start (Args, pFormat);
	CString Message;
	Message.FormatV (pFormat, Args);
	va_end (Args);

	Event LogEvent = {};
	LogEvent.type = EventType::Logger;
	LogEvent.logger.level = static_cast<u8> (nLevel);
	strncpy (LogEvent.logger.message, Message.c_str (),
		 sizeof LogEvent.logger.message);
	LogEvent.logger.message[sizeof LogEvent.logger.message - 1] = '\0';

	return LogEvent;
}

Event Event::Mouse (unsigned nEvent, unsigned nButtons,
		    int nX, int nY, int nWheel)
{
	Event MouseEvent = {};
	MouseEvent.type = EventType::Mouse;
	MouseEvent.mouse.event = static_cast<u8> (nEvent);
	MouseEvent.mouse.buttons = static_cast<u8> (nButtons);
	MouseEvent.mouse.x = static_cast<s16> (nX);
	MouseEvent.mouse.y = static_cast<s16> (nY);
	MouseEvent.mouse.wheel = static_cast<s16> (nWheel);

	return MouseEvent;
}
