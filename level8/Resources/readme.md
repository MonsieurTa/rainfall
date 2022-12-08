## Binary behavior
By executing the binary we can observe a first output
```
(nil), (nil)
```

and the program waits for the user's input.
By entering a random input, it prompts once again
```
(nil), (nil)
```

and still waits for the user's input.

## Decompilation

We can see two data segment addresses loaded
```
   0x08048575 <+17>:    mov    ecx,DWORD PTR ds:0x8049ab0 <- Here
   0x0804857b <+23>:    mov    edx,DWORD PTR ds:0x8049aac <- Here
   0x08048581 <+29>:    mov    eax,0x8048810
   0x08048586 <+34>:    mov    DWORD PTR [esp+0x8],ecx
   0x0804858a <+38>:    mov    DWORD PTR [esp+0x4],edx
   0x0804858e <+42>:    mov    DWORD PTR [esp],eax
   0x08048591 <+45>:    call   0x8048410 <printf@plt>
```

Those point to `service` and `auth` symbols respectively.
```
(gdb) x/x 0x8049ab0
0x8049ab0 <service>:    0x00000000
(gdb) x/x 0x8049aac
0x8049aac <auth>:       0x00000000
```

Those addresses are then passed to printf as parameter
```
   0x08048581 <+29>:    mov    eax,0x8048810
   0x08048586 <+34>:    mov    DWORD PTR [esp+0x8],ecx
   0x0804858a <+38>:    mov    DWORD PTR [esp+0x4],edx
   0x0804858e <+42>:    mov    DWORD PTR [esp],eax
   0x08048591 <+45>:    call   0x8048410 <printf@plt>
```

along with this address
```
   0x08048581 <+29>:    mov    eax,0x8048810
```

that represents the string format
```
 "%p, %p \n"
```

Then we get the user's prompt
```
   0x08048596 <+50>:    mov    eax,ds:0x8049a80 <--- stdin
   0x0804859b <+55>:    mov    DWORD PTR [esp+0x8],eax
   0x0804859f <+59>:    mov    DWORD PTR [esp+0x4],0x80
   0x080485a7 <+67>:    lea    eax,[esp+0x20]
   0x080485ab <+71>:    mov    DWORD PTR [esp],eax
   0x080485ae <+74>:    call   0x8048440 <fgets@plt>
```
We can then translate the following ASM into a much more comprehensible C
```
   0x080485bb <+87>:    lea    eax,[esp+0x20]
   0x080485bf <+91>:    mov    edx,eax
   0x080485c1 <+93>:    mov    eax,0x8048819 <--- "auth "
   0x080485c6 <+98>:    mov    ecx,0x5
   0x080485cb <+103>:   mov    esi,edx
   0x080485cd <+105>:   mov    edi,eax
   0x080485cf <+107>:   repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080485d1 <+109>:   seta   dl
   0x080485d4 <+112>:   setb   al
   0x080485d7 <+115>:   mov    ecx,edx
   0x080485d9 <+117>:   sub    cl,al
   0x080485db <+119>:   mov    eax,ecx
   0x080485dd <+121>:   movsx  eax,al
   0x080485e0 <+124>:   test   eax,eax
   0x080485e2 <+126>:   jne    0x8048642 <main+222>
   0x080485e4 <+128>:   mov    DWORD PTR [esp],0x4
   0x080485eb <+135>:   call   0x8048470 <malloc@plt>
   0x080485f0 <+140>:   mov    ds:0x8049aac,eax
   0x080485f5 <+145>:   mov    eax,ds:0x8049aac
   0x080485fa <+150>:   mov    DWORD PTR [eax],0x0
```

C translation
```
if (strcmp(buffer, "auth ") == 0) {
  auth = (void (*)())malloc(4);
  *(void**)auth = 0;
}
```

An ASM version of strlen is then used
```
   0x08048600 <+156>:   lea    eax,[esp+0x20]
   0x08048604 <+160>:   add    eax,0x5
   0x08048607 <+163>:   mov    DWORD PTR [esp+0x1c],0xffffffff
   0x0804860f <+171>:   mov    edx,eax
   0x08048611 <+173>:   mov    eax,0x0
   0x08048616 <+178>:   mov    ecx,DWORD PTR [esp+0x1c]
   0x0804861a <+182>:   mov    edi,edx
   0x0804861c <+184>:   repnz scas al,BYTE PTR es:[edi]
   0x0804861e <+186>:   mov    eax,ecx
   0x08048620 <+188>:   not    eax
   0x08048622 <+190>:   sub    eax,0x1
   0x08048625 <+193>:   cmp    eax,0x1e
   0x08048628 <+196>:   ja     0x8048642 <main+222>
   0x0804862a <+198>:   lea    eax,[esp+0x20]
   0x0804862e <+202>:   lea    edx,[eax+0x5]
   0x08048631 <+205>:   mov    eax,ds:0x8049aac
   0x08048636 <+210>:   mov    DWORD PTR [esp+0x4],edx
   0x0804863a <+214>:   mov    DWORD PTR [esp],eax
   0x0804863d <+217>:   call   0x8048460 <strcpy@plt>
```
is the same as
```
if (!strlen(&buffer[5]) < 0x1e) {
  strcpy((char*)auth, &buffer[5]);
}
```

2nd part
```
   0x08048642 <+222>:   lea    eax,[esp+0x20]
   0x08048646 <+226>:   mov    edx,eax
   0x08048648 <+228>:   mov    eax,0x804881f ; "reset"
   0x0804864d <+233>:   mov    ecx,0x5
   0x08048652 <+238>:   mov    esi,edx
   0x08048654 <+240>:   mov    edi,eax
   0x08048656 <+242>:   repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x08048658 <+244>:   seta   dl
   0x0804865b <+247>:   setb   al
   0x0804865e <+250>:   mov    ecx,edx
   0x08048660 <+252>:   sub    cl,al
   0x08048662 <+254>:   mov    eax,ecx
   0x08048664 <+256>:   movsx  eax,al
   0x08048667 <+259>:   test   eax,eax
   0x08048669 <+261>:   jne    0x8048678 <main+276>
   0x0804866b <+263>:   mov    eax,ds:0x8049aac
   0x08048670 <+268>:   mov    DWORD PTR [esp],eax
   0x08048673 <+271>:   call   0x8048420 <free@plt>
```
equals
```
if (strcmp(buffer, "reset") == 0) {
  free(auth);
}
```
3rd part
```
   0x08048678 <+276>:   lea    eax,[esp+0x20]
   0x0804867c <+280>:   mov    edx,eax
   0x0804867e <+282>:   mov    eax,0x8048825 ; "service"
   0x08048683 <+287>:   mov    ecx,0x6
   0x08048688 <+292>:   mov    esi,edx
   0x0804868a <+294>:   mov    edi,eax
   0x0804868c <+296>:   repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x0804868e <+298>:   seta   dl
   0x08048691 <+301>:   setb   al
   0x08048694 <+304>:   mov    ecx,edx
   0x08048696 <+306>:   sub    cl,al
   0x08048698 <+308>:   mov    eax,ecx
   0x0804869a <+310>:   movsx  eax,al
   0x0804869d <+313>:   test   eax,eax
   0x0804869f <+315>:   jne    0x80486b5 <main+337>
   0x080486a1 <+317>:   lea    eax,[esp+0x20]
   0x080486a5 <+321>:   add    eax,0x7
   0x080486a8 <+324>:   mov    DWORD PTR [esp],eax
   0x080486ab <+327>:   call   0x8048430 <strdup@plt>
   0x080486b0 <+332>:   mov    ds:0x8049ab0,eax
```
equals
```
if (strcmp(buffer, "service") == 0) {
  service = (void (*)())strdup(&buffer[7]);
}
```
4rth part
```
   0x080486b5 <+337>:   lea    eax,[esp+0x20]
   0x080486b9 <+341>:   mov    edx,eax
   0x080486bb <+343>:   mov    eax,0x804882d ; "login"
   0x080486c0 <+348>:   mov    ecx,0x5
   0x080486c5 <+353>:   mov    esi,edx
   0x080486c7 <+355>:   mov    edi,eax
   0x080486c9 <+357>:   repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
   0x080486cb <+359>:   seta   dl
   0x080486ce <+362>:   setb   al
   0x080486d1 <+365>:   mov    ecx,edx
   0x080486d3 <+367>:   sub    cl,al
   0x080486d5 <+369>:   mov    eax,ecx
   0x080486d7 <+371>:   movsx  eax,al
   0x080486da <+374>:   test   eax,eax
   0x080486dc <+376>:   jne    0x8048574 <main+16>
   0x080486e2 <+382>:   mov    eax,ds:0x8049aac
   0x080486e7 <+387>:   mov    eax,DWORD PTR [eax+0x20]
   0x080486ea <+390>:   test   eax,eax
   0x080486ec <+392>:   je     0x80486ff <main+411> ; [eax+0x20] != 0
   0x080486ee <+394>:   mov    DWORD PTR [esp],0x8048833 ; "/bin/sh"
   0x080486f5 <+401>:   call   0x8048480 <system@plt>
```
equals
```
if (strcmp(buffer, "login") == 0) {
  if (*(uint8_t*)auth+32 != 0) {
    system("/bin/sh");
  }
}
```
5th part
```
   0x080486ff <+411>:   mov    eax,ds:0x8049aa0 ; stdout
   0x08048704 <+416>:   mov    edx,eax
   0x08048706 <+418>:   mov    eax,0x804883b ; "Password:\n"
   0x0804870b <+423>:   mov    DWORD PTR [esp+0xc],edx
   0x0804870f <+427>:   mov    DWORD PTR [esp+0x8],0xa
   0x08048717 <+435>:   mov    DWORD PTR [esp+0x4],0x1
   0x0804871f <+443>:   mov    DWORD PTR [esp],eax
   0x08048722 <+446>:   call   0x8048450 <fwrite@plt>
```
equals
```
fwrite("Password:\n", 1, 10, stdout);
```
