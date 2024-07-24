/// Data
////////////////////////////////////////////////////////////
struct Win32Info
{
    SYSTEM_INFO system_info;
    uint32      cache_line_size;
    bool        initialized;
};

constexpr uint32 WIN32_ERROR_MESSAGE_SIZE = 1024;
struct Win32Error
{
    char   message[WIN32_ERROR_MESSAGE_SIZE];
    uint32 message_length;
    DWORD  code;
};

Win32Info g_win32_info;

/// Forward Declarations
////////////////////////////////////////////////////////////
void GetWin32Error(Win32Error* e);

/// Interface
////////////////////////////////////////////////////////////
void GetWin32Error(Win32Error* e)
{
    e->code = GetLastError();
    e->message_length = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                      NULL,                       // Format String
                                      e->code,
                                      0,                          // Language ID
                                      e->message,
                                      WIN32_ERROR_MESSAGE_SIZE,
                                      NULL);                      // Args
    if (e->message_length == 0)
    {
        CTK_FATAL("FormatMessage() failed with 0x%x", GetLastError());
    }
}

void InitWin32Info()
{
    if (g_win32_info.initialized)
    {
        CTK_FATAL("win32 module already initialized");
    }

    GetSystemInfo(&g_win32_info.system_info);

    // Get L1 cache line size.
    Win32Error e = {};
    DWORD buffer_size = 0;

    // Get buffer size, should generate a ERROR_INSUFFICIENT_BUFFER error.
    GetLogicalProcessorInformation(NULL, &buffer_size);
    GetWin32Error(&e);
    if (e.code != ERROR_INSUFFICIENT_BUFFER)
    {
        CTK_FATAL("GetLogicalProcessorInformation() didn't generate ERROR_INSUFFICIENT_BUFFER; buffer_size is "
                  "undefined");
    }

    auto buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)Allocate(&g_std_allocator, buffer_size, 16);
    if (!GetLogicalProcessorInformation(buffer, &buffer_size))
    {
        GetWin32Error(&e);
        CTK_FATAL("GetLogicalProcessorInformation() failed: %.*s", e.message_length, e.message);
    }

    for (uint32 i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i)
    {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1)
        {
            g_win32_info.cache_line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    Deallocate(&g_std_allocator, buffer);

    g_win32_info.initialized = true;
}

Win32Info* GetWin32Info()
{
    if (!g_win32_info.initialized)
    {
        CTK_FATAL("can't get win32 info; win32 module has not been initialized with InitWin32Info()");
    }

    return &g_win32_info;
}

sint16 GetConsoleScreenBufferWidth()
{
    CONSOLE_SCREEN_BUFFER_INFO info = {};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    return info.srWindow.Right - info.srWindow.Left + 1;
}

uint8* AllocatePages(uint32 page_count)
{
    if (page_count == 0)
    {
        CTK_FATAL("can't call Allocate() with page_count == 0");
    }

    uint32 total_allocation_size = page_count * GetWin32Info()->system_info.dwPageSize;
    auto mem = (uint8*)VirtualAlloc(NULL,                     // Base Address (NULL means the system determines where to allocate)
                                    total_allocation_size,    // Size (in page-size multiples)
                                    MEM_RESERVE | MEM_COMMIT, // Allocation Type
                                    PAGE_READWRITE);          // Protection Flags
    if (mem == NULL)
    {
        Win32Error e = {};
        GetWin32Error(&e);
        CTK_FATAL("VirtualAlloc() returned NULL: %.*s", e.message_length, e.message);
    }
    return mem;
}

void DeallocatePages(void* mem)
{
    if (mem == NULL)
    {
        CTK_FATAL("can't deallocate NULL mem pages");
    }

    VirtualFree(mem, 0, MEM_RELEASE);
}

