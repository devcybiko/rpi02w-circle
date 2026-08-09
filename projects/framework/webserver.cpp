#include "webserver.h"

#include <fatfs/ff.h>
#include <circle/memory.h>
#include <circle/screen.h>
#include <circle/string.h>
#include <circle/timer.h>
#include <circle/util.h>

#define MAX_CONTENT_SIZE	(1024 * 1024)
#define TIMEOUT_SECONDS		10

static boolean HasSuffix (const char *pString, unsigned nLength, const char *pSuffix)
{
	const unsigned nSuffixLength = strlen (pSuffix);
	return nLength >= nSuffixLength
		&& strcmp (pString + nLength - nSuffixLength, pSuffix) == 0;
}

static const char *GetContentType (const char *pPath)
{
	const unsigned nLength = strlen (pPath);
	if (HasSuffix (pPath, nLength, ".html") || HasSuffix (pPath, nLength, ".htm"))
	{
		return "text/html; charset=utf-8";
	}
	if (HasSuffix (pPath, nLength, ".css"))
	{
		return "text/css; charset=utf-8";
	}
	if (HasSuffix (pPath, nLength, ".js"))
	{
		return "application/javascript; charset=utf-8";
	}
	if (HasSuffix (pPath, nLength, ".svg"))
	{
		return "image/svg+xml";
	}
	if (HasSuffix (pPath, nLength, ".png"))
	{
		return "image/png";
	}
	if (HasSuffix (pPath, nLength, ".jpg") || HasSuffix (pPath, nLength, ".jpeg"))
	{
		return "image/jpeg";
	}
	if (HasSuffix (pPath, nLength, ".ico"))
	{
		return "image/x-icon";
	}
	if (HasSuffix (pPath, nLength, ".woff2"))
	{
		return "font/woff2";
	}

	return 0;
}

CSDWebServer::CSDWebServer (CNetSubSystem *pNetSubSystem, const char *pDocumentRoot,
				    CTimer *pTimer, CScreenDevice *pScreen, CSocket *pSocket)
: CHTTPDaemon (pNetSubSystem, pSocket, MAX_CONTENT_SIZE, HTTP_PORT, 0, TIMEOUT_SECONDS),
  m_pDocumentRoot (pDocumentRoot),
  m_pNetSubSystem (pNetSubSystem),
  m_pTimer (pTimer),
  m_pScreen (pScreen)
{
}

CHTTPDaemon *CSDWebServer::CreateWorker (CNetSubSystem *pNetSubSystem, CSocket *pSocket)
{
	return new CSDWebServer (pNetSubSystem, m_pDocumentRoot, m_pTimer, m_pScreen, pSocket);
}

THTTPStatus CSDWebServer::GetContent (const char *pPath, const char *, const char *,
					       u8 *pBuffer, unsigned *pLength,
					       const char **ppContentType)
{
	if (pPath != 0 && strcmp (pPath, "/status") == 0)
	{
		return GetStatus (pBuffer, pLength, ppContentType);
	}

	if (pPath == 0 || pBuffer == 0 || pLength == 0 || ppContentType == 0
		|| !IsSafePath (pPath))
	{
		return HTTPNotFound;
	}

	char FileName[HTTP_MAX_PATH + 16];
	if (strcmp (pPath, "/") == 0)
	{
		CString Path;
		Path.Format ("%s/index.html", m_pDocumentRoot);
		if (Path.GetLength () >= sizeof FileName)
		{
			return HTTPInternalServerError;
		}
		memcpy (FileName, (const char *) Path, Path.GetLength () + 1);
	}
	else
	{
		CString Path;
		Path.Format ("%s%s", m_pDocumentRoot, pPath);
		if (Path.GetLength () >= sizeof FileName)
		{
			return HTTPNotFound;
		}
		memcpy (FileName, (const char *) Path, Path.GetLength () + 1);
	}

	FIL File;
	FRESULT Result = f_open (&File, FileName, FA_READ | FA_OPEN_EXISTING);
	if (Result == FR_NO_FILE || Result == FR_NO_PATH)
	{
		return HTTPNotFound;
	}
	if (Result != FR_OK)
	{
		return HTTPInternalServerError;
	}

	const unsigned nFileSize = (unsigned) f_size (&File);
	if (nFileSize > *pLength)
	{
		f_close (&File);
		return HTTPInternalServerError;
	}

	UINT nBytesRead = 0;
	Result = f_read (&File, pBuffer, nFileSize, &nBytesRead);
	f_close (&File);
	if (Result != FR_OK || nBytesRead != nFileSize)
	{
		return HTTPInternalServerError;
	}

	*pLength = nFileSize;
	*ppContentType = strcmp (pPath, "/") == 0 ? "text/html; charset=utf-8" : GetContentType (pPath);
	return HTTPOK;
}

THTTPStatus CSDWebServer::GetStatus (u8 *pBuffer, unsigned *pLength,
					      const char **ppContentType)
{
	if (pBuffer == 0 || pLength == 0 || ppContentType == 0
		|| m_pTimer == 0 || m_pScreen == 0)
	{
		return HTTPInternalServerError;
	}

	CString IPAddress;
	m_pNetSubSystem->GetConfig ()->GetIPAddress ()->Format (&IPAddress);

	CString Status;
	Status.Format ("{\n"
		       "  \"uptime_seconds\": %u,\n"
		       "  \"wlan_connected\": %s,\n"
		       "  \"ip_address\": \"%s\",\n"
		       "  \"hostname\": \"%s\",\n"
		       "  \"display_width\": %u,\n"
		       "  \"display_height\": %u,\n"
		       "  \"heap_free_bytes\": %u\n"
		       "}\n",
		       m_pTimer->GetUptime (),
		       m_pNetSubSystem->IsRunning () ? "true" : "false",
		       (const char *) IPAddress, m_pNetSubSystem->GetHostname (),
		       m_pScreen->GetWidth (), m_pScreen->GetHeight (),
		       (unsigned) CMemorySystem::Get ()->GetHeapFreeSpace (HEAP_ANY));

	if (Status.GetLength () > *pLength)
	{
		return HTTPInternalServerError;
	}

	memcpy (pBuffer, (const char *) Status, Status.GetLength ());
	*pLength = Status.GetLength ();
	*ppContentType = "application/json; charset=utf-8";
	return HTTPOK;
}

boolean CSDWebServer::IsSafePath (const char *pPath) const
{
	if (strcmp (pPath, "/") == 0)
	{
		return TRUE;
	}

	const unsigned nLength = strlen (pPath);
	if (nLength < 6 || pPath[0] != '/')
	{
		return FALSE;
	}

	for (unsigned i = 0; i < nLength; i++)
	{
	if (pPath[i] == '\\' || (pPath[i] == '.' && pPath[i+1] == '.'))
		{
			return FALSE;
		}
	}

	return GetContentType (pPath) != 0;
}
