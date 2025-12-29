#ifndef GENERAL_STRING_BUILDER_INCLUDE_H
#define GENERAL_STRING_BUILDER_INCLUDE_H

/*

    String_Builder

    A mutable string of characters used to dynamically build a string.
    You can use custom allocators

    To include the implementation as cpp file use:

    #define STRING_BUILDER_IMPLEMENTATION
    #include "string_builder.h"

*/

#include "general.h"

const s64 SB_BLOCK_SIZE_DEFAULT = 4096;

typedef struct String_Buffer {
    s64 count;
    s64 occupied;
    u8 *data;
    String_Buffer *next;
} String_Buffer;

typedef struct String_Builder {
    String_Buffer *current_buffer;
    String_Buffer *start;

    s64 buffer_size;
    s64 count;

    Allocator allocator;
} String_Builder;

void sb_init(String_Builder *sb, s64 buffer_size = SB_BLOCK_SIZE_DEFAULT, Allocator allocator = {heap_allocator, null});
void sb_reset(String_Builder *sb);

void sb_append(String_Builder *sb, const char *s);
void sb_append(String_Builder *sb, const char *s, s64 count);
void sb_append(String_Builder *sb, u8 byte);

void sb_print(String_Builder *sb, const char *fmt, ...) IS_PRINTF_LIKE(2, 3);

char *to_string(String_Builder *sb, Allocator allocator = {heap_allocator, null});

void sb_free_buffers(String_Builder *sb);

#endif  // GENERAL_STRING_BUILDER_INCLUDE_H



#ifdef STRING_BUILDER_IMPLEMENTATION

void sb_init(String_Builder *sb, s64 buffer_size, Allocator allocator) {
    sb->current_buffer = null;
    sb->buffer_size = buffer_size;
    sb->count = 0;

    if (!allocator.proc) {
        allocator.proc = heap_allocator;
        allocator.data = null;
    }
    sb->allocator = allocator;

    sb->start = New(String_Buffer, sb->allocator);
    if (!sb->start) return;

    // Init string buffer.
    sb->start->count = buffer_size;
    sb->start->occupied = 0;
    sb->start->data = NewArray(u8, buffer_size, sb->allocator);
    sb->start->next = null;

    sb->current_buffer = sb->start;
}

void sb_reset(String_Builder *sb) {
    sb_free_buffers(sb);

    sb->current_buffer = null;
    sb->start = null;
    sb->count = 0;
}

inline void sb_grow_buffer(String_Builder *sb, s64 bytes) {
    String_Buffer *new_buffer = New(String_Buffer, sb->allocator);

    bytes = Max(bytes, sb->buffer_size);

    // Init string buffer.
    new_buffer->count = bytes;
    new_buffer->occupied = 0;
    new_buffer->data = NewArray(u8, bytes, sb->allocator);
    new_buffer->next = null;

    sb->current_buffer->next = new_buffer;
    sb->current_buffer = new_buffer;
    if (sb->start->next == null) {
        sb->start->next = sb->current_buffer;
    }
}

void sb_append(String_Builder *sb, const char *s) {
    if (!s) return;

    s64 count = string_length(s);

    if ((sb->current_buffer->count - sb->current_buffer->occupied) < count) {
        sb_grow_buffer(sb, count);
    }

    u8 *dest = sb->current_buffer->data + sb->current_buffer->occupied;
    memcpy(dest, s, (umm)count);

    sb->current_buffer->occupied += count;
    sb->count += count;
}

void sb_append(String_Builder *sb, const char *s, s64 count) {
    if (!count) return;

    if ((sb->current_buffer->count - sb->current_buffer->occupied) < count) {
        sb_grow_buffer(sb, count);
    }

    u8 *dest = sb->current_buffer->data + sb->current_buffer->occupied;
    // @Todo: Loop over for x86 when the count is larger than MAX_U32.
    memcpy(dest, s, (umm)count);

    sb->current_buffer->occupied += count;
    sb->count += count;
}

void sb_append(String_Builder *sb, u8 byte) {
    if ((sb->current_buffer->count - sb->current_buffer->occupied) < 1) {
        sb_grow_buffer(sb, 1);
    }

    sb->current_buffer->data[sb->current_buffer->occupied] = byte;
    sb->current_buffer->occupied += 1;
    sb->count += 1;
}

void sb_print(String_Builder *sb, const char *fmt, ...) {
    s64 mark = get_temporary_storage_mark();

    va_list args;
    va_start(args, fmt);
    char *s = tprint_valist(fmt, args);
    va_end(args);

    sb_append(sb, s);
    set_temporary_storage_mark(mark);
}

char *to_string(String_Builder *sb, Allocator allocator) {
    assert(sb->count != null);
    if (!sb->count) return null;

    char *s = NewArray(char, sb->count, allocator);

    char *dest = s;
    for (String_Buffer *it = sb->start; 
         it != null;
         it = it->next) {
        memcpy(dest, it->data, (umm)it->occupied);
        dest += it->occupied;
    }

    s[sb->count] = 0;

    return s;
}

void sb_free_buffers(String_Builder *sb) {
    assert(sb->allocator.proc != null);

    String_Buffer *it = sb->start;
    while (it) {
        String_Buffer *next = it->next;

        sb->allocator.proc(ALLOCATOR_FREE, 0, 0, it->data, sb->allocator.data);
        sb->allocator.proc(ALLOCATOR_FREE, 0, 0, it, sb->allocator.data);

        it = next;
    }
    sb->start = null;
}

#endif
