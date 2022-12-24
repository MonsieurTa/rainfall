# Binary behaviour
By playing with the binary, we observe that it prompt the user twice
```
bonus0@RainFall:~$ ./bonus0
 -

 -

bonus0@RainFall:~$
```

# Disassembly
main
```
(gdb) disas main
Dump of assembler code for function main:
   0x080485a4 <+0>:     push   ebp
   0x080485a5 <+1>:     mov    ebp,esp
   0x080485a7 <+3>:     and    esp,0xfffffff0        ; 16 bytes memory alignment
   0x080485aa <+6>:     sub    esp,0x40              ; make 64 bytes for local variables
   0x080485ad <+9>:     lea    eax,[esp+0x16]        ; load effective address at esp+0x16
   0x080485b1 <+13>:    mov    DWORD PTR [esp],eax   ; load as parameter for pp
   0x080485b4 <+16>:    call   0x804851e <pp>
   0x080485b9 <+21>:    lea    eax,[esp+0x16]        ; load effective address at esp+0x16
   0x080485bd <+25>:    mov    DWORD PTR [esp],eax   ; load as parameter for puts
   0x080485c0 <+28>:    call   0x80483b0 <puts@plt>  ; print string
   0x080485c5 <+33>:    mov    eax,0x0
   0x080485ca <+38>:    leave
   0x080485cb <+39>:    ret
End of assembler dump.
```

pp
```
(gdb) disas pp
Dump of assembler code for function pp:
   0x0804851e <+0>:     push   ebp                                ; prepare stack frame
   0x0804851f <+1>:     mov    ebp,esp
   0x08048521 <+3>:     push   edi
   0x08048522 <+4>:     push   ebx
   0x08048523 <+5>:     sub    esp,0x50                           ; 80 bytes
   0x08048526 <+8>:     mov    DWORD PTR [esp+0x4],0x80486a0      ; p's second parameter is a string - 0x80486a0 = " - "
   0x0804852e <+16>:    lea    eax,[ebp-0x30]                     ; load effective address at ebp-0x30 (48) bytes
   0x08048531 <+19>:    mov    DWORD PTR [esp],eax                ; load this address as p parameter
   0x08048534 <+22>:    call   0x80484b4 <p>
   0x08048539 <+27>:    mov    DWORD PTR [esp+0x4],0x80486a0      ; p's second parameter is a string - 0x80486a0 = " - "
   0x08048541 <+35>:    lea    eax,[ebp-0x1c]                     ; load effective address at ebp-0x1C (28) bytes
   0x08048544 <+38>:    mov    DWORD PTR [esp],eax                ; load this address as p parameter
   0x08048547 <+41>:    call   0x80484b4 <p>
   0x0804854c <+46>:    lea    eax,[ebp-0x30]                     ; load first buffer
   0x0804854f <+49>:    mov    DWORD PTR [esp+0x4],eax            ; set second parameter to this buffer for strcpy
   0x08048553 <+53>:    mov    eax,DWORD PTR [ebp+0x8]            ; get pp's first parameters
   0x08048556 <+56>:    mov    DWORD PTR [esp],eax                ; load it for strcpy
   0x08048559 <+59>:    call   0x80483a0 <strcpy@plt>
   0x0804855e <+64>:    mov    ebx,0x80486a4                      ; load string " " to ebx
   0x08048563 <+69>:    mov    eax,DWORD PTR [ebp+0x8]            ; get pp's first parameter
   0x08048566 <+72>:    mov    DWORD PTR [ebp-0x3c],0xffffffff    ; store 0xffffffff to ebp-0x3c (60)
   0x0804856d <+79>:    mov    edx,eax                            ; set edx to pp's first parameter
   0x0804856f <+81>:    mov    eax,0x0
   0x08048574 <+86>:    mov    ecx,DWORD PTR [ebp-0x3c]           ; set ecx (counter register) to 0xffffffff
   0x08048577 <+89>:    mov    edi,edx
   0x08048579 <+91>:    repnz scas al,BYTE PTR es:[edi]           ; basicaly, do strlen on pp's first parameter
   0x0804857b <+93>:    mov    eax,ecx
   0x0804857d <+95>:    not    eax                                ; get strlen+1 result
   0x0804857f <+97>:    sub    eax,0x1                            ; get strlen
   0x08048582 <+100>:   add    eax,DWORD PTR [ebp+0x8]            ; pp first parameter (char*) pointer + offset strlen
   0x08048585 <+103>:   movzx  edx,WORD PTR [ebx]                 ; set value to ' '
   0x08048588 <+106>:   mov    WORD PTR [eax],dx
   0x0804858b <+109>:   lea    eax,[ebp-0x1c]                     ; get second read buffer
   0x0804858e <+112>:   mov    DWORD PTR [esp+0x4],eax            ; load it as src for strcat
   0x08048592 <+116>:   mov    eax,DWORD PTR [ebp+0x8]            ; get pp first parameter
   0x08048595 <+119>:   mov    DWORD PTR [esp],eax                ; load it as dst for strcat
   0x08048598 <+122>:   call   0x8048390 <strcat@plt>
   0x0804859d <+127>:   add    esp,0x50
   0x080485a0 <+130>:   pop    ebx
   0x080485a1 <+131>:   pop    edi
   0x080485a2 <+132>:   pop    ebp
   0x080485a3 <+133>:   ret
End of assembler dump.
```

```
(gdb) disas p
Dump of assembler code for function p:
   0x080484b4 <+0>:     push   ebp
   0x080484b5 <+1>:     mov    ebp,esp
   0x080484b7 <+3>:     sub    esp,0x1018                   ; stack memory 4120
   0x080484bd <+9>:     mov    eax,DWORD PTR [ebp+0xc]      ; load second parameter " - " to eax
   0x080484c0 <+12>:    mov    DWORD PTR [esp],eax          ; load puts parameter
   0x080484c3 <+15>:    call   0x80483b0 <puts@plt>
   0x080484c8 <+20>:    mov    DWORD PTR [esp+0x8],0x1000   ; 4096 at 3rd parameter
   0x080484d0 <+28>:    lea    eax,[ebp-0x1008]             ; buffer[4096]
   0x080484d6 <+34>:    mov    DWORD PTR [esp+0x4],eax      ; buffer at 2nd parameter
   0x080484da <+38>:    mov    DWORD PTR [esp],0x0          ; stdin as first parameter
   0x080484e1 <+45>:    call   0x8048380 <read@plt>
   0x080484e6 <+50>:    mov    DWORD PTR [esp+0x4],0xa      ; 10 = \n
   0x080484ee <+58>:    lea    eax,[ebp-0x1008]             ; load buffer[4096] to eax
   0x080484f4 <+64>:    mov    DWORD PTR [esp],eax
   0x080484f7 <+67>:    call   0x80483d0 <strchr@plt>       ; search for \n in buffer[4096]
   0x080484fc <+72>:    mov    BYTE PTR [eax],0x0           ; replace occurence by 0x0
   0x080484ff <+75>:    lea    eax,[ebp-0x1008]             ; load buffer[4096]
   0x08048505 <+81>:    mov    DWORD PTR [esp+0x8],0x14     ; set 3rd parameter to 20
   0x0804850d <+89>:    mov    DWORD PTR [esp+0x4],eax      ; set 2nd parameter to buffer[4096]
   0x08048511 <+93>:    mov    eax,DWORD PTR [ebp+0x8]      ; load first parameter to eax
   0x08048514 <+96>:    mov    DWORD PTR [esp],eax
   0x08048517 <+99>:    call   0x80483f0 <strncpy@plt>      ; copy 20 bytes from buffer to parameter
   0x0804851c <+104>:   leave
   0x0804851d <+105>:   ret
End of assembler dump.
```

buffer address in main
0xbffff606

20 bytes payload
AAAAAAAAAAAAAAAAAAAA
BBBBBBBBBBBBBBBB
CCCCCCCCCCCCCCCC

first buffer - 0xbfffe580
