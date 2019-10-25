#ifndef SYSTEMWATCH_WIN_H
#define SYSTEMWATCH_WIN_H

#include "systemwatch.h"

#include <qt_windows.h>

class WinSystemWatch : public SystemWatch {
public:
    WinSystemWatch();
    ~WinSystemWatch();

private:
    class EventFilter;
    EventFilter *d;
    bool         processWinEvent(MSG *m, long *result);
};

#endif // SYSTEMWATCH_WIN_H
