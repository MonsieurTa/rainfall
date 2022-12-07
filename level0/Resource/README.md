Execute the *level1* binary:

```bash
./level0
```

```
Segmentation fault (core dumped)
```

> By default, the *level1* binary segfault.

```bash
./level0 123
```

```
No !
```

> The *level1* need an argument.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```
Dump of assembler code for function main:
...
   0x08048ed4 <+20>:	call   0x8049710 <atoi>
   0x08048ed9 <+25>:	cmp    eax,0x1a7
   0x08048ede <+30>:	jne    0x8048f58 <main+152>
   0x08048ee0 <+32>:	mov    DWORD PTR [esp],0x80c5348
...
   0x08048f51 <+145>:	call   0x8054640 <execv>
...
   0x08048f85 <+197>:	leave
   0x08048f86 <+198>:	ret
End of assembler dump.
```

The main compare the first argument given to the __0x1a7__ value (423 in decimal).

If the comparaison is equal, the function continue to the execve calling and start a new shell with its rights.

Use this value for the as argument:

```bash
./level0 423
cat /home/user/level1/.pass
exit
```

```
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
```