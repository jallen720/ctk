#pragma once

#include <cstdio>
#include "ctk/ctk.h"
#include "ctk/containers.h"

template<typename Type>
static CTK_Array<Type> ctk_read_file(cstr path) {
    CTK_ASSERT(path != NULL);
    CTK_Array<Type> elems = {};

    FILE *f = fopen(path, "rb");
    if (f == NULL)
        CTK_FATAL("failed to open \"%s\"", path);
    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);

    if (fsize > 0) {
        rewind(f);
        elems = ctk_create_array_full<Type>(fsize);
        fread(elems.data, fsize, 1, f);
    }

    fclose(f);
    return elems;
}
