#include "kernel.h"
#include <circle/timer.h>

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ())
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	return m_Screen.Initialize ();
}

TShutdownMode CKernel::Run (void)
{
	static const TScreenColor Colors[] =
	{
		RED_COLOR,
		GREEN_COLOR,
		BLUE_COLOR,
		WHITE_COLOR,
		BLACK_COLOR
	};

	const unsigned width = m_Screen.GetWidth ();
	const unsigned height = m_Screen.GetHeight ();

	while (1)
	{
		for (unsigned i = 0; i < sizeof Colors / sizeof Colors[0]; ++i)
		{
			for (unsigned y = 0; y < height; ++y)
			{
				for (unsigned x = 0; x < width; ++x)
				{
					m_Screen.SetPixel (x, y, Colors[i]);
				}
			}

			m_Screen.Update ();

			m_ActLED.On ();
			CTimer::SimpleMsDelay (100);

			m_ActLED.Off ();
			CTimer::SimpleMsDelay (900);
		}
	}

	return ShutdownHalt;
}