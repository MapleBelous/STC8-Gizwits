#include "GizwitsHandle.h"
#include "Init.h"
void main(void)
{
    Init();
    while (true)
    {
        GizwitsMainLoop();
    }
}
