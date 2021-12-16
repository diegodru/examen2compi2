.data

.globl main
.text
li.s $f1, 5.6
s.s $f1, 0($sp)
li.s $f2, 6.7
s.s $f2, 4($sp)
l.s $f4, 0($sp)
l.s $f5, 4($sp)
sub.s $f3, $f4, $f5
l.s $f7, 0($sp)
l.s $f8, 4($sp)
div.s $f6, $f7, $f8
l.s $f9, 0($sp)
l.s $f10, 4($sp)
c.le.s $f9, $f10
bc1f true0
bc1t false1
j end2
true0: 
li.s $f11, 1.0
j end
false1:
li.s $f11, 0.0
j end
l.s $f12, 0($sp)
l.s $f13, 4($sp)
c.lt.s $f12, $f13
bc1t true4
bc1f false5
j end6
true4: 
li.s $f14, 1
j end
false5:
li.s $f14, 0
j end
l.s $f15, 0($sp)
l.s $f16, 4($sp)
c.eq.s $f15, $f16
bc1t true8
bc1f false9
j end10
true8: 
li.s $f17, 1
j end
false9:
li.s $f17, 0
j end

