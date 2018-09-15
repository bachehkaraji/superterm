#include "ui/consoletabfactory.h"
#include "ui/consoletab.h"
#include "serial/portenumerator.h"
#include "session/session.h"

CPortEnumerator* CConsoleTabFactory::sPortEnumerator = nullptr;

void CConsoleTabFactory::setPortEnumerator(CPortEnumerator* portEnumerator)
{
    sPortEnumerator = portEnumerator;
}

CConsoleTab* CConsoleTabFactory::createTab()
{
    return new CConsoleTab(sPortEnumerator, nullptr);
}

CConsoleTab* CConsoleTabFactory::createTabFromSession(CSession* session)
{
    CConsoleTab* tab = new CConsoleTab(sPortEnumerator, session);
    tab->setLabel(session->getTabLabel());

    return tab;
}

// EOF <stefan@scheler.com>
