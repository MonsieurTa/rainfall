```
(gdb) i functions
All defined functions:

Non-debugging symbols:
0x08048464  _init
0x080484b0  __cxa_atexit
0x080484b0  __cxa_atexit@plt
0x080484c0  __gmon_start__
0x080484c0  __gmon_start__@plt
0x080484d0  std::ios_base::Init::Init()
0x080484d0  _ZNSt8ios_base4InitC1Ev@plt
0x080484e0  __libc_start_main
0x080484e0  __libc_start_main@plt
0x080484f0  _exit
0x080484f0  _exit@plt
0x08048500  _ZNSt8ios_base4InitD1Ev
0x08048500  _ZNSt8ios_base4InitD1Ev@plt
0x08048510  memcpy
0x08048510  memcpy@plt
0x08048520  strlen
0x08048520  strlen@plt
0x08048530  operator new(unsigned int)
0x08048530  _Znwj@plt
0x08048540  _start
0x08048570  __do_global_dtors_aux
0x080485d0  frame_dummy
0x080485f4  main
0x0804869a  __static_initialization_and_destruction_0(int, int)
0x080486da  _GLOBAL__sub_I_main
0x080486f6  N::N(int)                                             <- interesting
0x080486f6  N::N(int)
0x0804870e  N::setAnnotation(char*)                               <- interesting
0x0804873a  N::operator+(N&)                                      <- interesting
0x0804874e  N::operator-(N&)                                      <- interesting
0x08048770  __libc_csu_init
0x080487e0  __libc_csu_fini
0x080487e2  __i686.get_pc_thunk.bx
0x080487f0  __do_global_ctors_aux
0x0804881c  _fini
```

```
(gdb) disas main
Dump of assembler code for function main:
   0x080485f4 <+0>:     push   ebp                                  ; prepare stack frame
   0x080485f5 <+1>:     mov    ebp,esp
   0x080485f7 <+3>:     push   ebx
   0x080485f8 <+4>:     and    esp,0xfffffff0
   0x080485fb <+7>:     sub    esp,0x20
   0x080485fe <+10>:    cmp    DWORD PTR [ebp+0x8],0x1              ; compare argc to 1
   0x08048602 <+14>:    jg     0x8048610 <main+28>                  ; if greater than 2 continue
   0x08048604 <+16>:    mov    DWORD PTR [esp],0x1
   0x0804860b <+23>:    call   0x80484f0 <_exit@plt>                ; else exit with exit(1)
   0x08048610 <+28>:    mov    DWORD PTR [esp],0x6c
   0x08048617 <+35>:    call   0x8048530 <_Znwj@plt>                ; operator new - 108 bytes (0x6c)
   0x0804861c <+40>:    mov    ebx,eax
   0x0804861e <+42>:    mov    DWORD PTR [esp+0x4],0x5
   0x08048626 <+50>:    mov    DWORD PTR [esp],ebx
   0x08048629 <+53>:    call   0x80486f6 <_ZN1NC2Ei>                ; call N contructor with the newly allocated memory pointer
                                                                    ; (same as this) and 0x5 as main parameter
   0x0804862e <+58>:    mov    DWORD PTR [esp+0x1c],ebx
   0x08048632 <+62>:    mov    DWORD PTR [esp],0x6c
   0x08048639 <+69>:    call   0x8048530 <_Znwj@plt>                ; operator new - 108 bytes (0x6c)
   0x0804863e <+74>:    mov    ebx,eax
   0x08048640 <+76>:    mov    DWORD PTR [esp+0x4],0x6
   0x08048648 <+84>:    mov    DWORD PTR [esp],ebx
   0x0804864b <+87>:    call   0x80486f6 <_ZN1NC2Ei>                ; new N(0x6)
   0x08048650 <+92>:    mov    DWORD PTR [esp+0x18],ebx             ; stores y to esp+0x18
   0x08048654 <+96>:    mov    eax,DWORD PTR [esp+0x1c]
   0x08048658 <+100>:   mov    DWORD PTR [esp+0x14],eax             ; stores x to esp+0x14
   0x0804865c <+104>:   mov    eax,DWORD PTR [esp+0x18]
   0x08048660 <+108>:   mov    DWORD PTR [esp+0x10],eax             ; stores y to 0x10
   0x08048664 <+112>:   mov    eax,DWORD PTR [ebp+0xc]              ; get argv[0]
   0x08048667 <+115>:   add    eax,0x4                              ; get argv[1]
   0x0804866a <+118>:   mov    eax,DWORD PTR [eax]
   0x0804866c <+120>:   mov    DWORD PTR [esp+0x4],eax
   0x08048670 <+124>:   mov    eax,DWORD PTR [esp+0x14]
   0x08048674 <+128>:   mov    DWORD PTR [esp],eax
   0x08048677 <+131>:   call   0x804870e <_ZN1N13setAnnotationEPc>  ; (*x).setAnnotation(argv[1])
   0x0804867c <+136>:   mov    eax,DWORD PTR [esp+0x10]             ; load x address
   0x08048680 <+140>:   mov    eax,DWORD PTR [eax]                  ; dereference the value at x address
   0x08048682 <+142>:   mov    edx,DWORD PTR [eax]                  ; move the value pointed by x address to edx
   0x08048684 <+144>:   mov    eax,DWORD PTR [esp+0x14]             ; load y address
   0x08048688 <+148>:   mov    DWORD PTR [esp+0x4],eax              ; set x address as second parameter
   0x0804868c <+152>:   mov    eax,DWORD PTR [esp+0x10]
   0x08048690 <+156>:   mov    DWORD PTR [esp],eax                  ; set y address to first parameter
   0x08048693 <+159>:   call   edx                                  ; call the function pointed at dereferenced pointer by x (it is supposed to be operator+ overload)
   0x08048695 <+161>:   mov    ebx,DWORD PTR [ebp-0x4]
   0x08048698 <+164>:   leave
   0x08048699 <+165>:   ret
```

N::N(int)
```
(gdb) disas 0x80486f6
Dump of assembler code for function _ZN1NC2Ei:
   0x080486f6 <+0>:     push   ebp                       ; prepare stack frame
   0x080486f7 <+1>:     mov    ebp,esp
   0x080486f9 <+3>:     mov    eax,DWORD PTR [ebp+0x8]   ; load this
   0x080486fc <+6>:     mov    DWORD PTR [eax],0x8048848
   0x08048702 <+12>:    mov    eax,DWORD PTR [ebp+0x8]
   0x08048705 <+15>:    mov    edx,DWORD PTR [ebp+0xc]   ; load first parameter
   0x08048708 <+18>:    mov    DWORD PTR [eax+0x68],edx  ; store first parameter at this+104
   0x0804870b <+21>:    pop    ebp
   0x0804870c <+22>:    ret
   End of assembler dump.
```

N::setAnnotation(char*)
```
(gdb) disas _ZN1N13setAnnotationEPc
Dump of assembler code for function _ZN1N13setAnnotationEPc:
   0x0804870e <+0>:     push   ebp                       ; prepare stack frame
   0x0804870f <+1>:     mov    ebp,esp
   0x08048711 <+3>:     sub    esp,0x18
   0x08048714 <+6>:     mov    eax,DWORD PTR [ebp+0xc]   ; load parameter (string) to eax
   0x08048717 <+9>:     mov    DWORD PTR [esp],eax       ; load eax for strlen
   0x0804871a <+12>:    call   0x8048520 <strlen@plt>
   0x0804871f <+17>:    mov    edx,DWORD PTR [ebp+0x8]   ; load this
   0x08048722 <+20>:    add    edx,0x4                   ; get to buffer
   0x08048725 <+23>:    mov    DWORD PTR [esp+0x8],eax   ; set strlen result to 3rd parameter
   0x08048729 <+27>:    mov    eax,DWORD PTR [ebp+0xc]   ; load parameter (string) to eax
   0x0804872c <+30>:    mov    DWORD PTR [esp+0x4],eax   ; set string as 2nd parameter
   0x08048730 <+34>:    mov    DWORD PTR [esp],edx       ; set buffer as 1st parameter
   0x08048733 <+37>:    call   0x8048510 <memcpy@plt>
   0x08048738 <+42>:    leave
   0x08048739 <+43>:    ret
End of assembler dump.
```

N::operator+(N&)
```
(gdb) disas 0x0804873a
Dump of assembler code for function _ZN1NplERS_:
   0x0804873a <+0>:     push   ebp
   0x0804873b <+1>:     mov    ebp,esp
   0x0804873d <+3>:     mov    eax,DWORD PTR [ebp+0x8]   ; load this
   0x08048740 <+6>:     mov    edx,DWORD PTR [eax+0x68]  ; load stored variable
   0x08048743 <+9>:     mov    eax,DWORD PTR [ebp+0xc]   ; load N&
   0x08048746 <+12>:    mov    eax,DWORD PTR [eax+0x68]  ; load N->variable
   0x08048749 <+15>:    add    eax,edx                   ; sum them
   0x0804874b <+17>:    pop    ebp
   0x0804874c <+18>:    ret
End of assembler dump.
```
