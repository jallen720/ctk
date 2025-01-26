/// Utils
////////////////////////////////////////////////////////////
bool PathExists(DWORD attribs) {
    return attribs != INVALID_FILE_ATTRIBUTES;
}

bool IsFile(DWORD attribs) {
    return !(attribs & FILE_ATTRIBUTE_DIRECTORY);
}

uint32 GetFileSize(HANDLE file) {
    return ::GetFileSize(file, NULL);
}

/// Interface
////////////////////////////////////////////////////////////
uint32 GetFileSize(const char* path) {
    OFSTRUCT file_info = {};
    auto file = (HANDLE)OpenFile(path, &file_info, OF_READ);
    if (file_info.nErrCode != ERROR_SUCCESS) {
        Win32Error e = {};
        GetWin32Error(&e);
        CTK_FATAL("OpenFile() failed for \"%s\": %.*s", path, e.message_length, e.message);
    }
    uint32 file_size = GetFileSize(file);
    CloseHandle(file);
    return file_size;
}

uint32 GetPathDirSize(const char* path) {
    uint32 path_size = StringSize(path);
    uint32 dir_size = 0;
    for (uint32 path_index = 0; path_index < path_size; path_index += 1) {
        if (path[path_index] == '/' || path[path_index] == '\\') {
            dir_size = path_index;
        }
    }
    return dir_size + 1; // Include last slash.
}

template<typename Type>
void ReadFile(Type** array, uint32* size, Allocator* allocator, const char* path) {
    // Open file.
    OFSTRUCT file_info = {};
    auto file = (HANDLE)OpenFile(path, &file_info, OF_READ);
    if (file_info.nErrCode != ERROR_SUCCESS) {
        Win32Error e = {};
        GetWin32Error(&e);
        CTK_FATAL("OpenFile() failed for \"%s\": %.*s", path, e.message_length, e.message);
    }

    // Get file size.
    uint32 file_byte_size = GetFileSize(file);
    if (file_byte_size == INVALID_FILE_SIZE) {
        CTK_FATAL("failed to get size of file '%s'", path);
    }

    if (file_byte_size > 0) {
        // Allocate array to store file data.
        uint32 type_size = sizeof(Type);
        *size  = Align(file_byte_size, type_size) / type_size;
        *array = Allocate<Type>(allocator, *size);

        // Read file into array.
        DWORD bytes_read = 0;
        if (!::ReadFile(file, *array, file_byte_size, &bytes_read, NULL)) {
            CTK_FATAL("failed to read file '%s'", path);
        }

        if (bytes_read != file_byte_size) {
            CTK_FATAL("not all bytes read while reading file '%s'", path);
        }
    }

    // Cleanup
    CloseHandle(file);
}

template<typename Type>
Array<Type> ReadFile(Allocator* allocator, const char* path) {
    Array<Type> array = CreateArray<Type>(allocator);
    ReadFile(&array.data, &array.size, allocator, path);
    array.count = array.size;
    return array;
}

template<typename Type, uint32 size>
void ReadFile(FArray<Type, size>* array, const char* path) {
    array->count = 0;

    // Open file.
    OFSTRUCT file_info = {};
    auto file = (HANDLE)OpenFile(path, &file_info, OF_READ);
    if (file_info.nErrCode != ERROR_SUCCESS) {
        Win32Error e = {};
        GetWin32Error(&e);
        CTK_FATAL("OpenFile() failed for \"%s\": %.*s", path, e.message_length, e.message);
    }

    // Get file size.
    uint32 file_byte_size = GetFileSize(file);
    if (file_byte_size == INVALID_FILE_SIZE) {
        CTK_FATAL("failed to get size of file '%s'", path);
    }

    if (file_byte_size > 0) {
        uint32 type_size = sizeof(Type);
        array->count = Min(Align(file_byte_size, type_size) / type_size, size);

        // Read file into array.
        DWORD bytes_read = 0;
        if (!::ReadFile(file, array->data, ByteSize(array), &bytes_read, NULL)) {
            CTK_FATAL("failed to read file '%s'", path);
        }

        if (bytes_read != file_byte_size) {
            PrintWarning("Only %u / %u bytes read from file '%s'", bytes_read, file_byte_size, path);
        }
    }

    // Cleanup
    CloseHandle(file);
}

template<typename Type>
void WriteFile(const char* path, const Type* array, uint32 array_size) {
    Win32Error err = {};

    // CreateFile
    HANDLE file = ::CreateFile(path,
                               GENERIC_READ | GENERIC_WRITE, // Access
                               0,                            // Share Mode
                               NULL,                         // Security Attributes
                               CREATE_ALWAYS,                // Create Mode
                               FILE_ATTRIBUTE_NORMAL,        // File Attributes
                               NULL);                        // Template File
    if (file == INVALID_HANDLE_VALUE) {
        GetWin32Error(&err);
        CTK_FATAL("CreateFile() returned INVALID_HANDLE_VALUE: %.*s", err.message_length, err.message);
    }

    // Write bytes in array to file.
    uint32 array_byte_size = sizeof(Type) * array_size;
    DWORD bytes_written = 0;
    if (!::WriteFile(file, array, array_byte_size, &bytes_written, NULL)) {
        GetWin32Error(&err);
        CTK_FATAL("WriteFile() failed: %.*s", err.message_length, err.message);
    }
    if (bytes_written != array_byte_size) {
        CTK_FATAL("failed to write %u bytes to '%s': only wrote %u bytes", array_byte_size, path, bytes_written);
    }

    // Cleanup
    CloseHandle(file);
}

void WriteFile(const char* path, String* string) {
    WriteFile(path, string->data, string->count);
}

template<typename Type>
void WriteFile(const char* path, Array<Type>* array) {
    WriteFile(path, array->data, array->count);
}

bool PathExists(const char* path) {
    return PathExists(GetFileAttributes(path));
}

bool IsFile(const char* path) {
    return IsFile(GetFileAttributes(path));
}

