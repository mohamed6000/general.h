#ifndef INCLUDE_GENERAL_H
#define INCLUDE_GENERAL_H

/*

    A general layer for C++ projects.

    Use:    #define GENERAL_IMPLEMENTATION
    before: #include "general.h"
    to include the implementation code.

*/

#define ENABLE_ASSERTS
#define ENABLE_DEFERS

/******** Compiler detection ********/

#if defined(__clang__)
    // Clang compiler.
    #define COMPILER_CLANG 1

    // Platform detection for clang.
    #if defined(_WIN32)
        #define OS_WINDOWS 1
    #elif defined(__gnu_linux__)
        #define OS_LINUX 1
    #elif defined(__APPLE__) && defined(__MACH__)
        #define OS_MAC 1
    #else
        #error Missing OS platform
    #endif

    // Architecture detection for clang.
    #if defined(__amd64__)
        #define ARCH_X64 1
    #elif defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__arm__)
        #define ARCH_ARM 1
    #elif defined(__aarch64__)
        #define ARCH_ARM64 1
    #else 
        #error Unknown architecture
    #endif

#elif defined(_MSC_VER)
    // Microsoft compiler.
    #define COMPILER_CL 1

    // Platform detection for msvc.
    #if defined(_WIN32)
        #define OS_WINDOWS 1
    #else
        #error Missing OS platform
    #endif

    // Architecture detection for msvc.
    #if defined(_M_AMD64)
        #define ARCH_X64 1
    #elif defined(_M_IX86)
        #define ARCH_X86 1
    #elif defined(_M_ARM)
        #define ARCH_ARM 1
    #else
        #error Unknown architecture
    #endif

#elif defined(__GNUC__)
    // GNU C Compiler.
    #define COMPILER_GCC 1

    // Platform detection for gcc.
    #if defined(_WIN32)
        #define OS_WINDOWS 1
    #elif defined(__gnu_linux__)
        #define OS_LINUX 1
    #elif defined(__APPLE__) && defined(__MACH__)
        #define OS_MAC 1
    #else
        #error Missing OS platform
    #endif

    // Architecture detection for gcc.
    #if defined(__amd64__)
        #define ARCH_X64 1
    #elif defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__arm__)
        #define ARCH_ARM 1
    #elif defined(__aarch64__)
        #define ARCH_ARM64 1
    #else
        #error Unknown architecture
    #endif
#endif


// Language detection.
#ifdef __cplusplus
    #define LANGUAGE_CPP 1
#else
    #define LANGUAGE_C 1
#endif


#if !defined(COMPILER_CL)
    #define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
    #define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
    #define COMPILER_GCC 0
#endif

#if !defined(OS_WINDOWS)
    #define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
    #define OS_LINUX 0
#endif
#if !defined(OS_MAC)
    #define OS_MAC 0
#endif

#if !defined(ARCH_X64)
    #define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
    #define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
    #define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
    #define ARCH_ARM64 0
#endif

#if !defined(LANGUAGE_CPP)
    #define LANGUAGE_CPP 0
#endif
#if !defined(LANGUAGE_C)
    #define LANGUAGE_C 0
#endif


#if COMPILER_CL
#include <intrin.h>
#endif


/******** Primitives ********/
#include <stdint.h>
#include <stddef.h>

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef float  float32;
typedef double float64;

typedef uintptr_t umm;
typedef intptr_t  smm;

typedef u64 b64;  // For consistency.
typedef u32 b32;
typedef u16 b16;  // For consistency.
typedef u8  b8;   // For consistency.


#if LANGUAGE_CPP
#define null 0
#define TINYRT_EXTERN extern "C"
#else
#define null ((void *)0)
#define TINYRT_EXTERN
#endif

#if COMPILER_CL
#define thread_var __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
#define thread_var __thread
#else
#error Undefined thread_var for this compiler
#endif

#if OS_WINDOWS
    #if COMPILER_CL
    #define SHARED_EXPORT __declspec(dllexport)
    #elif COMPILER_CLANG
    #define SHARED_EXPORT __declspec(dllexport)
    #elif COMPILER_GCC
    #define SHARED_EXPORT __declspec(dllexport)
    #else
    #error Undefined SHARED_EXPORT for this compiler
    #endif
#elif OS_LINUX || OS_MAC
    #if COMPILER_GCC || COMPILER_CLANG
    #define SHARED_EXPORT __attribute__((visibility("default")))
    #else
    #error Undefined SHARED_EXPORT for this compiler
    #endif
#else
#error Undefined SHARED_EXPORT for this compiler
#endif

#define size_of(x) sizeof(x)
#define offset_of(Type, member) (umm)&(((Type *)0)->member)
#define array_count(a) (size_of(a) / size_of((a)[0]))

#define align_forward_offset(s, a) (((s) & ((a)-1)) ? ((a) - ((s) & ((a)-1))) : 0)
#define align_forward(s, a) (((s) + ((a)-1)) & ~((a)-1))

#define align_forward_pointer(p, a) ((u8 *)(((umm)(p) + ((a)-1)) & ~((a)-1)))


// Assuming the input is non-zero.
#define is_power_of_2(x) (((x) & ((x)-1)) == 0)

#if COMPILER_CL
#define UNUSED(x) (void)(x)
#else
#define UNUSED(x) (void)size_of(x)
#endif

#define CONCAT_INTERNAL(x, y) x##y
#define CONCAT(x, y) CONCAT_INTERNAL(x, y)

#define STRINGIFY2(s) #s
#define STRINGIFY(s) STRINGIFY2(s)

#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Max(a, b) (((a) > (b)) ? (a) : (b))

#define pointer_to_int(ptr) (u64)((uintptr_t)(ptr))
#define pointer_to_type(Type, ptr) (Type)((uintptr_t)(ptr))

#define u32_to_pointer(Type, n) (Type *)((uintptr_t)(n))
#define int_to_pointer(Type, n) (Type *)((uintptr_t)(n))

#define M_PI 3.14159265358979323846
#define TAU 6.283185307179586476925

#define absolute_value(x) (((x) < 0) ? -(x) : (x))

#define for_range(it, start, end) for (s64 it = start; it <= end; ++it)


#if COMPILER_GCC
    #define debug_break() __builtin_trap()
#elif COMPILER_CL
    #define debug_break() __debugbreak()
#elif COMPILER_CLANG
    #define debug_break() __builtin_trap()
#else
    #if ARCH_X64
        #define debug_break() *((int *)null) = 0
    #else
        #error Unknown architecture for debug_break
    #endif
#endif


#ifndef NO_ASSERT
#ifdef ENABLE_ASSERTS
// https://nullprogram.com/blog/2022/06/26/
#undef assert
#define assert(expression) \
do { \
    if (!(expression)) { \
        write_string("Assertion Failure: " STRINGIFY(expression) " at " __FILE__ ":" STRINGIFY(__LINE__) "\n"); \
        if (tinyrt_abort_error_message("Assertion Failed", "Assert Failed\n" STRINGIFY(expression) "\nAt: " __FILE__ ":" STRINGIFY(__LINE__) "\n")) { \
            debug_break(); \
        } \
    } \
} while (0)

#else
#undef assert
#define assert(expression)
#endif  // ENABLE_ASSERTS
#endif  // NO_ASSERT


#ifdef ENABLE_DEFERS
#if LANGUAGE_CPP
template<typename T>
struct ExitScope {
    T lambda;
    ExitScope(T lambda):lambda(lambda){}
    ~ExitScope(void){lambda();}
    ExitScope(const ExitScope &);
private:
    ExitScope &operator =(const ExitScope &);
};

struct ExitScopeHelp {
    template<typename T>
    ExitScope<T> operator+(T t){ return t; }
};

#define defer const auto& CONCAT(defer__, __LINE__) = ExitScopeHelp() + [&]()
#endif  // LANGUAGE_CPP
#endif  // ENABLE_DEFERS


#define INVALID_DEFAULT_CASE() default: {assert(!"Invalid Default Case");} break


#if LANGUAGE_C
#include <stdbool.h>

#define static_assert(c, m) typedef char CONCAT(static_assertion,__LINE__)[(c)?1:-1]
#endif

static_assert(size_of(s8)  == 1, "size_of(s8)  == 1");
static_assert(size_of(s16) == 2, "size_of(s16) == 2");
static_assert(size_of(s32) == 4, "size_of(s32) == 4");
static_assert(size_of(s64) == 8, "size_of(s64) == 8");

static_assert(size_of(u8)  == 1, "size_of(u8)  == 1");
static_assert(size_of(u16) == 2, "size_of(u16) == 2");
static_assert(size_of(u32) == 4, "size_of(u32) == 4");
static_assert(size_of(u64) == 8, "size_of(u64) == 8");

static_assert(size_of(float32) == 4, "size_of(float32) == 4");
static_assert(size_of(float64) == 8, "size_of(float64) == 8");

static_assert(size_of(umm) == size_of(void *), "size_of(umm) == size_of(void *)");
static_assert(size_of(smm) == size_of(void *), "size_of(smm) == size_of(void *)");

static_assert(size_of(b8)  == 1, "size_of(b8)  == 1");
static_assert(size_of(b16) == 2, "size_of(b16) == 2");
static_assert(size_of(b32) == 4, "size_of(b32) == 4");
static_assert(size_of(b64) == 8, "size_of(b64) == 8");

/******** Min/Max type size ********/

#define MIN_S8  0x80 // -128
#define MAX_S8  0x7f // 127

#define MIN_S16  0x8000 // -32768
#define MAX_S16  0x7fff // 32767

#define MIN_S32  0x80000000 // -2147483648
#define MAX_S32  0x7fffffff // 2147483647

#define MIN_S64  ((s64)0x8000000000000000ull) // -2^63
#define MAX_S64  ((s64)0x7fffffffffffffffull) // 2^63 - 1

#define MAX_U8   0xff               // 255
#define MAX_U16  0xffff             // 65535
#define MAX_U32  0xffffffff         // 4294967295
#define MAX_U64  0xffffffffffffffff // 2^64 - 1

#define F32_MIN  1.17549435e-38f
#define F32_MAX  3.40282347e+38f

#define F64_MIN   2.2250738585072014e-308
#define F64_MAX   1.7976931348623157e+308


#define BIT(x) (1ull << (x))
#define KB(x) ((u64)(x) << 10ull)
#define MB(x) ((u64)(x) << 20ull)
#define GB(x) ((u64)(x) << 30ull)


// Memory.
#include <string.h>
#define memory_zero(p, s)     memset((p), 0, (s))
#define memory_zero_struct(p) memory_zero((p), size_of(*(p)))
#define memory_zero_array(p)  memory_zero((p), size_of(p))


typedef enum Operating_System {
    OPERATING_SYSTEM_NONE,
    OPERATING_SYSTEM_WINDOWS,
    OPERATING_SYSTEM_LINUX,
    OPERATING_SYSTEM_MAC,

    OPERATING_SYSTEM_COUNT,
} Operating_System;

typedef enum Architecture {
    ARCHITECTURE_NONE,
    ARCHITECTURE_X64,
    ARCHITECTURE_X86,
    ARCHITECTURE_ARM,
    ARCHITECTURE_ARM64,
    
    ARCHITECTURE_COUNT,
} Architecture;

// Custom Allocators.

typedef enum Allocator_Mode {
    ALLOCATOR_ALLOCATE,
    ALLOCATOR_RESIZE,
    ALLOCATOR_FREE,
    ALLOCATOR_FREE_ALL,
} Allocator_Mode;

#define ALLOCATOR_PROC(name) void *name(Allocator_Mode mode, s64 size, s64 old_size, void *old_memory, void *allocator_data)

typedef ALLOCATOR_PROC(Allocator_Proc);

typedef struct Allocator {
    Allocator_Proc *proc;
    void *data;
} Allocator;

extern thread_var Allocator current_allocator;

#define SET_ALLOCATOR(a) do { current_allocator = a; } while (0)
#define GET_ALLOCATOR() (current_allocator)

// Heap allocator.
TINYRT_EXTERN void *heap_allocator(Allocator_Mode mode, s64 size, s64 old_size, void *old_memory, void *allocator_data);

#define heap_alloc(s) heap_allocator(ALLOCATOR_ALLOCATE, (s), 0, null, null)
#define heap_realloc(mem, size, old_size) heap_allocator(ALLOCATOR_RESIZE, (size), (old_size), (mem), null)
#define heap_free(mem) heap_allocator(ALLOCATOR_FREE, 0, 0, (mem), null)

#if COMPILER_CL
#define New(Type, ...) (Type *)core_new_alloc(size_of(Type), __VA_ARGS__)
#else
#define New(Type, ...) (Type *)core_new_alloc(size_of(Type), ##__VA_ARGS__)
#endif

#if COMPILER_CL
#define NewArray(Type, count, ...) (Type *)core_new_alloc((count) * size_of(Type), __VA_ARGS__)
#else
#define NewArray(Type, count, ...) (Type *)core_new_alloc((count) * size_of(Type), ##__VA_ARGS__)
#endif

#if COMPILER_CL
#define MemRealloc(m, new_size, old_size, ...) core_mem_realloc((m), (new_size), (old_size), __VA_ARGS__)
#else
#define MemRealloc(m, new_size, old_size, ...) core_mem_realloc((m), (new_size), (old_size), ##__VA_ARGS__)
#endif

#if COMPILER_CL
#define MemFree(m, ...) core_memfree((m), __VA_ARGS__)
#else
#define MemFree(m, ...) core_memfree((m), ##__VA_ARGS__)
#endif


/******** String ********/

typedef struct String {
    s64 count;
    u8 *data;
} String;

#if LANGUAGE_CPP
#define S(s) String{size_of(s)-1, (u8 *)(s)}
#else
#define S(s) (String){size_of(s)-1, (u8 *)(s)}
#endif

inline String make_string(u8 *data, s64 count) {
    String result;
    result.count = count;
    result.data  = data;
    return result;
}



/******** Custom Loggers ********/

typedef enum Log_Mode {
    LOG_NONE,
    LOG_MINIMAL,
    LOG_EVERYDAY,
    LOG_VERBOSE,
} Log_Mode;

#define LOGGER_PROC(name) void name(String ident, String message, Log_Mode mode, void *data)
typedef LOGGER_PROC(Logger_Proc);

extern thread_var Logger_Proc *current_logger;

#define Log(ident, message, mode) current_logger((ident), (message), (mode), null)
#define SET_LOGGER(l) do { current_logger = l; } while (0)
#define GET_LOGGER() (current_logger)

// Cross-platform write string functions.
void write_string(const char *s, bool to_standard_error = false);
void write_string(const char *s, u32 count, bool to_standard_error = false);

void write_string(String s, bool to_standard_error = false);

inline void default_logger(String ident, String message, Log_Mode mode, void *data) {
    UNUSED(mode);
    UNUSED(data);

    if (ident.count) {
        write_string("[");
        write_string(ident);
        write_string("]: ");
    }

    write_string(message);
    write_string("\n");
}

inline void error_logger(String ident, String message, Log_Mode mode, void *data) {
    UNUSED(mode);
    UNUSED(data);

    if (ident.count) {
        write_string("[", true);
        write_string(ident, true);
        write_string("]: ", true);
    }

    write_string(message, true);
    write_string("\n", true);
}

TINYRT_EXTERN bool tinyrt_abort_error_message(const char *title, const char *message);



inline u32 safe_truncate_u64(u64 value) {
    assert(value <= MAX_U32);
    u32 result = (u32)value;
    return result;
}



inline void *core_new_alloc(s64 size, Allocator a = GET_ALLOCATOR()) {
    assert(a.proc != null);
    return a.proc(ALLOCATOR_ALLOCATE, size, 0, null, a.data);
}

inline void *core_mem_realloc(void *mem, s64 new_size, s64 old_size, Allocator a = GET_ALLOCATOR()) {
    assert(a.proc != null);
    return a.proc(ALLOCATOR_RESIZE, new_size, old_size, mem, a.data);
}

inline void core_memfree(void *mem, Allocator a = GET_ALLOCATOR()) {
    assert(a.proc != null);
    a.proc(ALLOCATOR_FREE, 0, 0, mem, a.data);
}




/******** Utility functions ********/

inline u16 swap2(u16 mem) {
    u16 a = mem >> 8;
    u16 b = (mem & 0xFF) << 8;
    
    u16 result = a | b;
    return result;
}

inline u32 swap4(u32 mem) {
    u32 a = (mem >> 24);
    u32 b = (mem >> 16) & 0xFF;
    u32 c = (mem >>  8) & 0xFF;
    u32 d = (mem) & 0xFF;
    
    u32 result = (d << 24) | (c << 16) | (b << 8) | a;
    return result;
}

inline u32 find_least_significant_set_bit(u32 value) {
#if COMPILER_CL
    unsigned long result = 0;
    _BitScanForward(&result, value);
    return (u32)result;
#elif COMPILER_GCC
    return (u32)__builtin_ctz(value);
#else
    for (u32 test = 0; test < 32; ++test) {
        if (value & (1 << test)) {
            return test;
        }
    }

    return 0;
#endif
}

#if LANGUAGE_CPP
inline void Clamp(int *pointer, int low, int high) {
    if (*pointer < low) { *pointer = low; }
    if (*pointer > high) { *pointer = high; }
}

inline void Clamp(float *pointer, float low, float high) {
    if (*pointer < low) { *pointer = low; }
    if (*pointer > high) { *pointer = high; }
}
#else
#define Clamp(v, l, h) (((v) < (l)) ? (l) : (((v) > (h)) ? (h) : (v)))
#endif

inline Operating_System operating_system_get_current(void) {
    Operating_System result = OPERATING_SYSTEM_NONE;
#if OS_WINDOWS
    result = OPERATING_SYSTEM_WINDOWS;
#elif OS_LINUX
    result = OPERATING_SYSTEM_LINUX;
#elif OS_MAC
    result = OPERATING_SYSTEM_MAC;
#endif

    return result;
}

inline Architecture architecture_get_current(void) {
    Architecture result = ARCHITECTURE_NONE;
#if ARCH_X64
    result = ARCHITECTURE_X64;
#elif ARCH_X86
    result = ARCHITECTURE_X86;
#elif ARCH_ARM
    result = ARCHITECTURE_ARM;
#elif ARCH_ARM64
    result = ARCHITECTURE_ARM64;
#endif

    return result;
}

inline const char *operating_system_to_string(Operating_System os) {
    const char *result = null;
    switch (os) {
        case OPERATING_SYSTEM_WINDOWS:
            result = "Windows";
            break;

        case OPERATING_SYSTEM_LINUX:
            result = "Linux";
            break;

        case OPERATING_SYSTEM_MAC:
            result = "Mac";
            break;

        default: break;
    }

    return result;
}

inline const char *architecture_to_string(Architecture arch) {
    const char *result = null;
    switch (arch) {
        case ARCHITECTURE_X64:
            result = "X64";
            break;

        case ARCHITECTURE_X86:
            result = "X86";
            break;

        case ARCHITECTURE_ARM:
            result = "ARM";
            break;

        case ARCHITECTURE_ARM64:
            result = "ARM64";
            break;

        default: break;
    }

    return result;
}

inline void my_panic(void) {
    write_string(S("Panic.\n"));
    debug_break();
}

// Ratio helpers.

inline float safe_ratio_0(float n, float d) {
    float result = 0;

    if (d != 0) {
        result = n/d;
    }

    return result;
}

inline float safe_ratio_1(float n, float d) {
    float result = 1;

    if (d != 0) {
        result = n/d;
    }

    return result;
}

inline float safe_ratio_n(float a, float b, float n) {
    float result = n;

    if (b != 0) {
        result = a/b;
    }

    return result;
}

// String helpers.

inline bool strings_are_equal(String a, String b) {
    if (a.count != b.count) return false;

    for (s64 index = 0; index < a.count; ++index) {
        if (a.data[index] != b.data[index]) {
            return false;
        }
    }

    return true;
}

inline bool strings_are_equal(char *a, char *b) {
    bool result = (a == b);

    if (a && b) {
        while (*a && *b && (*a == *b)) {
            ++a;
            ++b;
        }

        result = ((*a == 0) && (*b == 0));
    }

    return result;
}

inline bool strings_are_equal(s64 length_a, char *a, s64 length_b, char *b) {
    bool result = (length_a == length_b);

    if (result) {
        result = true;
        for (s64 index = 0; index < length_a; ++index) {
            if (a[index] != b[index]) {
                result = false;
                break;
            }
        }
    }

    return result;
}

inline bool strings_are_equal(s64 length_a, char *a, char *b) {
    char *it = b;
    for (s64 index = 0; index < length_a; ++index, ++it) {
        if ((*it == 0) || (a[index] != *it)) {
            return false;
        }
    }

    bool result = (*it == 0);
    return result;
}

inline bool is_end_of_line(int c) {
    bool result = ((c == '\n') || (c == '\r'));
    return result;
}

inline bool is_white_space(int c) {
    bool result = ((c == ' ') || 
                   (c == '\t') ||
                   (c == '\v') || 
                   (c == '\f') || 
                   (c == '\n') ||
                   (c == '\r'));

    return result;
}

inline bool is_digit(int c) {
    bool result = (c >= '0') && (c <= '9');
    return result;
}

inline s64 string_length(const char *s) {
    if (!s) return 0;

    const char *it = s;
    while (*it++);

    return (s64)(it-1 - s);
}

inline char *eat_spaces(char *s) {
    char *it = s;
    while (*it == ' ') {
        it++;
    }

    return it;
}

inline char *get_extension(char *s) {
    char *result = null;

    char *it = s;
    while (*it) {
        if (*it == '.') result = it;

        it++;
    }

    return result;
}

inline char *find_character_from_right(char *s, u8 c) {
    s64 length = string_length(s);

    for (s64 i = length - 1; i >= 0; --i) {
        if (s[i] == c) {
            return s + i;
        }
    }

    return null;
}

inline char *path_cleanup(char *s) {
    char *it = s;
    while (*it) {
        if (*it == '\\') *it = '/';

        it++;
    }

    return s;
}

inline void advance(String *s, s64 amount) {
    s->data  += amount;
    s->count -= amount;
}



/******** Platform specific helper functions ********/


#endif  // INCLUDE_GENERAL_H













/******** General Implementation ********/

#ifdef GENERAL_IMPLEMENTATION

thread_var Logger_Proc *current_logger = default_logger;
thread_var Allocator current_allocator = {heap_allocator, null};


#if OS_WINDOWS

#ifdef INCLUDE_WINDEFS
#include "windefs.h"
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#if COMPILER_CL
#pragma comment(lib, "User32.lib")
#endif


void write_string(const char *s, bool to_standard_error) {
    DWORD written = 0;
    HANDLE handle = to_standard_error ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
    s32 status = WriteFile(handle, s, (DWORD)string_length(s), &written, null);
    UNUSED(status);
}

void write_string(const char *s, u32 count, bool to_standard_error) {
    DWORD written = 0;
    HANDLE handle = to_standard_error ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
    s32 status = WriteFile(handle, s, count, &written, null);
    UNUSED(status);
}

void write_string(String s, bool to_standard_error) {
    DWORD written = 0;
    HANDLE handle = to_standard_error ? GetStdHandle(STD_ERROR_HANDLE) : GetStdHandle(STD_OUTPUT_HANDLE);
    s32 status = WriteFile(handle, s.data, (DWORD)s.count, &written, null);
    UNUSED(status);
}

TINYRT_EXTERN bool tinyrt_abort_error_message(const char *title, const char *message) {
    int id = MessageBoxA(null, message, title, MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_SYSTEMMODAL | MB_DEFBUTTON3);
    if (id == IDABORT) {
        ExitProcess(3);
    }

    return (id == IDRETRY);
}




TINYRT_EXTERN void *heap_allocator(Allocator_Mode mode, s64 size, s64 old_size, void *old_memory, void *allocator_data) {
    UNUSED(allocator_data);

    switch (mode) {
        case ALLOCATOR_ALLOCATE:
            return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

        case ALLOCATOR_RESIZE: {
            // Allocate, copy, free.

            void *result = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
            if (result == null) return null;

            if (old_memory && (old_size > 0)) {
                memcpy(result, old_memory, old_size);
                HeapFree(GetProcessHeap(), 0, old_memory);
            }

            return result;
        } break;

        case ALLOCATOR_FREE: {
            HeapFree(GetProcessHeap(), 0, old_memory);
            return null;
        } break;

        case ALLOCATOR_FREE_ALL: {
            // Not supported.
            assert(!"Not supported");
            return null;
        } break;

        default: {
            assert(false);
            return null;
        } break;
    }
}

#endif  // OS_WINDOWS


#endif  // GENERAL_IMPLEMENTATION
