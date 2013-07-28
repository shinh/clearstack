clearstack

This is an experimental clang plugin. If you compiles your program
with this plugin, pointer values and integer values whose size is
equal to pointers will be zeroed before a function exits. If this
plugin is used to compile a program which uses conservative GC,
you might be able to reduce the chance to have a false reference which
prevents objects from being freed.
