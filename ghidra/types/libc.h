/*
 * C library types the standard prototypes in ghidra/symbols/ use. size_t is
 * unsigned int for sh-elf (GCC's __SIZE_TYPE__). va_list is here only so a
 * prototype can name it: GCC's for SH-4 is a 20-byte record
 * (__builtin_va_list) passed by value, which Ghidra need not lay out. The
 * exported headers get both from the compiler's <stddef.h> and <stdarg.h>
 * instead (ExportSource drops these copies).
 */
typedef unsigned int size_t;
typedef char *va_list;

/* newlib's malloc statistics, returned by value (through a hidden pointer
 * in r2): main_heap_mallinfo fills one from the allocator's state. */
struct mallinfo {
    int arena;
    int ordblks;
    int smblks;
    int hblks;
    int hblkhd;
    int usmblks;
    int fsmblks;
    int uordblks;
    int fordblks;
    int keepcost;
};
