.data

str0: .asciiz "a+b="

str1: .asciiz "a>b="

str2: .asciiz "a<b="

str3: .asciiz "a==b="

.globl main
.text
li $v0, 6
syscall
s.s $f0, 0($sp)
li $v0, 6
syscall
s.s $f0, 4($sp)
la $a0, str0
li $v0, 4
syscall
l.s $f2, 0($sp)
l.s $f3, 4($sp)
sub.s $f1, $f2, $f3
mov.s $f12, $f1
li $v0, 2
syscall
la $a0, str1
li $v0, 4
syscall
l.s $f4, 0($sp)
l.s $f5, 4($sp)
c.le.s $f4, $f5
bc1f true0
bc1t false1
j end2
true0: 
li.s $f6, 1
j end
false1:
li.s $f6, 0
j end
mov.s $f12, $f6
li $v0, 2
syscall
la $a0, str2
li $v0, 4
syscall
l.s $f7, 0($sp)
l.s $f8, 4($sp)
c.lt.s $f7, $f8
bc1t true4
bc1f false5
j end6
true4: 
li.s $f9, 1
j end
false5:
li.s $f9, 0
j end
mov.s $f12, $f9
li $v0, 2
syscall
la $a0, str3
li $v0, 4
syscall
l.s $f10, 0($sp)
l.s $f11, 4($sp)
c.eq.s $f10, $f11
bc1t true8
bc1f false9
j end10
true8: 
li.s $f12, 1
j end
false9:
li.s $f12, 0
j end
mov.s $f12, $f12
li $v0, 2
syscall

