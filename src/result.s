.data

str0: .asciiz "a+b="

.globl main
.text
test:
swc1 $a0, 0($sp)
swc1 $a1, 0($sp)
la $a0, str0
li $v0, 4
syscall
l.s $f1, 0($sp)
l.s $f2, 0($sp)
c.lt.s $f1, $f2
bc1t true1
bc1f false2
j end3
true1: 
li.s $f3, 1.0
j end
false2:
li.s $f3, 0.0
j end
mov.s $f12, $f3
li $v0, 2
syscall
j $ra
main:
li $v0, 6
syscall
s.s $f0, 0($sp)
li $v0, 6
syscall
s.s $f0, 4($sp)
sw $ra, 8($sp)
addi $sp, $sp, 12
jal test
addi $sp, $sp, -12
lw $ra 8($sp)
mtc1 $v0, $f4
j $ra

