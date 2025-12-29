#ifndef GENERAL_FILE_INCLUDE_H
#define GENERAL_FILE_INCLUDE_H

/*

    File module.

    To include the implementation as cpp file use:

    #define FILE_IMPLEMENTATION
    #include "file.h"

*/

#include "general.h"

struct File_OS;
struct String_Builder;

typedef struct File {
    File_OS *os_specific;
} File;


bool file_open(File *file, const char *name, 
               bool for_writing = false,
               bool keep_existing_content = false);
void file_close(File *file);

inline File file_open(const char *name, 
                      bool for_writing = false,
                      bool keep_existing_content = false) {
    File result = {};
    file_open(&result, name, for_writing, keep_existing_content);
    return result;
}

inline bool is_valid(File file) {
    return file.os_specific != null;
}

bool file_read(File *file, void *data, s64 count, s64 *bytes_read = null);
bool file_length(File *file, s64 *length_return);
bool file_get_position(File *file, s64 *pos_return);
bool file_set_position(File *file, s64 pos);

bool file_write(File *file, void *data, s64 count);

inline bool file_write(File *file, String s) {
    return file_write(file, s.data, s.count);
}

bool read_entire_file(const char *name, String *result);
bool read_entire_file(File file, String *result);
bool write_entire_file(const char *name, void *data, s64 count);

inline bool write_entire_file(const char *name, String s) {
    return write_entire_file(name, s.data, s.count);
}

#endif  // GENERAL_FILE_INCLUDE_H



#ifdef FILE_IMPLEMENTATION

#if OS_WINDOWS

#ifdef INCLUDE_WINDEFS
#include "windefs.h"
#else
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#endif

typedef struct File_OS {
    HANDLE handle;
} File_OS;

bool file_open(File *file, const char *name, bool for_writing, bool keep_existing_content) {
    WCHAR *w32_utf8_to_wide(const char *s, Allocator allocator);

    HANDLE handle;
    if (for_writing) {
        u32 mode;
        if (keep_existing_content) {
            mode = OPEN_ALWAYS;
        } else {
            mode = CREATE_ALWAYS;
        }

        handle = CreateFileW(w32_utf8_to_wide(name, temporary_allocator), 
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ,
                             null,
                             mode,
                             0, null);
    } else {
        handle = CreateFileW(w32_utf8_to_wide(name, temporary_allocator),
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             null,
                             OPEN_EXISTING,
                             0, null);
    }

    if (handle == INVALID_HANDLE_VALUE) {
        error_logger(LOG_MINIMAL, "File", "Failed to open \"%s\"", name);
        return false;
    }

    file->os_specific = (File_OS *)heap_alloc(size_of(File_OS));
    file->os_specific->handle = handle;
    return true;
}

void file_close(File *file) {
    if (file->os_specific) {
        CloseHandle(file->os_specific->handle);
        heap_free(file->os_specific);
        file->os_specific = null;
    }
}

bool file_read(File *file, void *data_, s64 bytes_to_read, s64 *bytes_read) {
    if (!data_) return false;
    if (bytes_to_read <= 0) return false;

    HANDLE handle = file->os_specific->handle;
    s64 total_read = 0;
    u8 *data = (u8 *)data_;

    while (total_read < bytes_to_read) {
        s64 remaining = bytes_to_read - total_read;
        DWORD to_read;
        if (remaining <= MAX_S32) {
            to_read = (DWORD)remaining;
        } else {
            to_read = MAX_S32;
        }

        DWORD single_read_length = 0;
        BOOL success = ReadFile(handle, data + total_read, to_read, &single_read_length, null);
        total_read += single_read_length;
        if (!success) {
            return false;
        }

        if (single_read_length == 0) {
            // End of file.
            if (bytes_read) *bytes_read = total_read;
            return true;
        }
    }

    if (bytes_read) *bytes_read = total_read;
    return true;
}

bool file_length(File *file, s64 *length_return) {
    LARGE_INTEGER size;
    BOOL ok = GetFileSizeEx(file->os_specific->handle, &size);
    *length_return = size.QuadPart;
    return ok != 0;
}

bool file_get_position(File *file, s64 *pos_return) {
    LARGE_INTEGER start = {};
    LARGE_INTEGER offset;
    BOOL ok = SetFilePointerEx(file->os_specific->handle, start, &offset, FILE_CURRENT);
    *pos_return = offset.QuadPart;
    return ok != 0;
}

bool file_set_position(File *file, s64 pos) {
    LARGE_INTEGER pos_large;
    pos_large.QuadPart = pos;
    BOOL ok = SetFilePointerEx(file->os_specific->handle, pos_large, null, FILE_BEGIN);
    return ok != 0;
}

bool file_write(File *file, void *data_, s64 count) {
    if (!data_) return false;
    if (count <= 0) return false;

    HANDLE handle = file->os_specific->handle;
    s64 total_write = 0;
    u8 *data = (u8 *)data_;

    while (total_write < count) {
        s64 remaining = count - total_write;
        DWORD to_write;
        if (remaining <= MAX_S32) {
            to_write = (DWORD)remaining;
        } else {
            to_write = MAX_S32;
        }

        DWORD single_write_length = 0;
        BOOL success = WriteFile(handle, data + total_write, to_write, &single_write_length, null);
        total_write += single_write_length;
        if (!success) {
            return false;
        }
    }

    return true;
}

bool read_entire_file(File file, String *s) {
    HANDLE handle = file.os_specific->handle;

    LARGE_INTEGER large_size;
    BOOL ok = GetFileSizeEx(handle, &large_size);
    if (!ok) return false;

    s->count = large_size.QuadPart;
    s->data = (u8 *)heap_alloc(s->count + 1);

    s64 previous_pos = 0;
    if (!file_get_position(&file, &previous_pos)) {
        return false;
    }

    if (!file_set_position(&file, 0)) {
        return false;
    }

    s64 total_read = 0;
    while (total_read < s->count) {
        s64 remaining = s->count - total_read;
        DWORD to_read;
        if (remaining <= MAX_S32) {
            to_read = (DWORD)remaining;
        } else {
            to_read = MAX_S32;
        }

        DWORD single_read_length = 0;
        BOOL success = ReadFile(handle, s->data + total_read, to_read, &single_read_length, null);
        if (!success) {
            heap_free(s->data);
            return false;
        }

        total_read += single_read_length;
    }

    s->data[s->count] = 0;

    if (!file_set_position(&file, previous_pos)) {
        return false;
    }

    return true;
}

#endif  // OS_WINDOWS



#if OS_LINUX

typedef struct File_OS {
    FILE *handle;
} File_OS;

bool file_open(File *file, const char *name, bool for_writing, bool keep_existing_content) {
    FILE *handle;
    if (for_writing && keep_existing_content) {
        handle = fopen(name, "a+");
    } else if (for_writing) {
        handle = fopen(name, "wb+");
    } else {
        handle = fopen(name, "rb");
    }

    if (!handle) {
        error_logger(LOG_MINIMAL, "File", "Failed to open \"%s\"", name);
        return false;
    }

    file->os_specific = (File_OS *)heap_alloc(size_of(File_OS));
    file->os_specific->handle = handle;
    return true;
}

void file_close(File *file) {
    assert(file->os_specific != null);
    assert(file->os_specific->handle != null);
    if (file->os_specific && file->os_specific->handle) {
        fclose(file->os_specific->handle);
        heap_free(file->os_specific);
        file->os_specific = null;
    }
}

bool file_read(File *file, void *data, s64 bytes_to_read, s64 *bytes_read) {
    if (!data) return false;
    if (bytes_to_read <= 0) {
        if (bytes_read) *bytes_read = 0;
        return false;
    }

    s64 single_read_length = fread(data, 1, bytes_to_read, file->os_specific->handle);
    if (single_read_length < bytes_to_read) {
        if (feof(file->os_specific->handle)) {
            // End of file.
            if (bytes_read) *bytes_read = single_read_length;
            return true;
        } else {
            if (bytes_read) *bytes_read = single_read_length;
            return false;
        }
    }

    if (bytes_read) *bytes_read = single_read_length;
    return true;
}

bool file_length(File *file, s64 *length_return) {
    s64 pos = ftello64(file->os_specific->handle);
    if ((pos == -1) || (pos == 0x7fffffffffffffff)) {
        *length_return = 0;
        return false;
    }

    fseek(file->os_specific->handle, 0, SEEK_END);
    s64 size = ftello64(file->os_specific->handle);
    if ((size == -1) || (size == 0x7fffffffffffffff)) {
        *length_return = 0;
        return false;
    }

    fseek(file->os_specific->handle, pos, SEEK_SET);
    *length_return = size;
    return true;
}

bool file_get_position(File *file, s64 *pos_return) {
    s64 result = ftello64(file->os_specific->handle);
    *pos_return = result;
    return (result != -1) && (result != 0x7fffffffffffffff);
}

bool file_set_position(File *file, s64 pos) {
    int result = fseek(file->os_specific->handle, pos, SEEK_SET);
    return (result == 0);
}

bool file_write(File *file, void *data, s64 count) {
    if (!data) return false;
    if (count <= 0) return false;

    size_t written = fwrite(data, 1, (size_t)count, file->os_specific->handle);
    return written == (size_t)count;
}

bool read_entire_file(File file, String *s) {
    s64 size;
    if (!file_length(&file, &size)) return false;


    s->count = size;
    s->data  = (u8 *)heap_alloc(s->count + 1);
    if (!s->data) return false;

    s64 single_read_length = fread(s->data, 1, (size_t)size, file.os_specific->handle);
    if ((single_read_length != size) && (!feof(file.os_specific->handle))) {
        heap_free(s->data);
        *s = {};
        return false;
    }

    s->count = single_read_length;
    s->data[s->count] = 0;

    return true;
}

#endif  // OS_LINUX



bool read_entire_file(const char *name, String *result) {
    File file;
    if (file_open(&file, name)) {
        bool success = read_entire_file(file, result);

        file_close(&file);
        return success;
    }

    return false;
}

bool write_entire_file(const char *name, void *data, s64 count) {
    File file;
    if (file_open(&file, name, /*for_writing=*/true, /*keep_existing_content=*/false)) {
        bool success = file_write(&file, data, count);

        file_close(&file);
        return success;
    }

    return false;
}

#endif  // FILE_IMPLEMENTATION
