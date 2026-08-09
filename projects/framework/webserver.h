#ifndef _framework_webserver_h
#define _framework_webserver_h

#include <circle/net/httpdaemon.h>

class CTimer;
class CScreenDevice;

// Serves read-only HTML documents from a directory on the mounted SD card.
class CSDWebServer : public CHTTPDaemon
{
public:
	CSDWebServer (CNetSubSystem *pNetSubSystem, const char *pDocumentRoot,
			  CTimer *pTimer, CScreenDevice *pScreen, CSocket *pSocket = 0);

	CHTTPDaemon *CreateWorker (CNetSubSystem *pNetSubSystem, CSocket *pSocket);
	THTTPStatus GetContent (const char *pPath, const char *pParams,
				const char *pFormData, u8 *pBuffer, unsigned *pLength,
				const char **ppContentType);

private:
	boolean IsSafePath (const char *pPath) const;
	boolean IsJSONPayload (const char *pPayload) const;
	THTTPStatus GetAction (const char *pPayload, u8 *pBuffer, unsigned *pLength,
				const char **ppContentType);
	THTTPStatus GetStatus (u8 *pBuffer, unsigned *pLength, const char **ppContentType);

private:
	const char *m_pDocumentRoot;
	CNetSubSystem *m_pNetSubSystem;
	CTimer *m_pTimer;
	CScreenDevice *m_pScreen;
};

#endif
