#pragma once

namespace WindowTest
{

static void Run()
{
    InitWin32Info();
    WindowInfo window_info =
    {
        .x        = 0,
        .y        = 70,
        .width    = 1080,
        .height   = 720,
        .title    = "Window Test",
        .callback = DefaultWindowCallback,
    };
    OpenWindow(&window_info);
    while (WindowIsOpen())
    {
        ProcessWindowEvents();
        if (KeyReleased(Key::ESCAPE))
        {
            CloseWindow();
        }
        else
        {
            Sleep(1);
        }
    }
}

}
