
\\intialize
add $r2, $imm, $zero, 16;
add $r13, $zero, $imm,08	\\devide the matrix into 4 blocks 
add $r14, $zero, $imm,16	
								\\devide the matrix into 4 blocks 
							\\ block end in 8-th 
								\\ and 16-th row,cluom respectivly
							\\and start in the 8th and  8th raw clom resp.
add $r3, $zero, $imm,0;			\\r=0
forloop1:
add $r4, $zero, $imm,8;		\\c=0    folloop1=4
forloop2:
xor $r5, $r5, $r5, 0;		\\i=0   \\\foorlop2=5
xor $r6, $r6, $r6, 0;    \\sum=0
forloop3:
//clculate address
mull $r7, $r3, $imm, 16;	\\forloop3=7
add $r7, $r7, $r5, 0;		
add $r7, $r7, $imm, 0x0;    \\base of the first input matrix
//clculate address
mull $r8, $r5, $imm, 16;
add $r8, $r8, $r4, 0;
add $r8, $r8, $imm, 0x100;   \\base of the second input matrix

lw $r9, $r7,$zero, 0;
lw $r10, $r8, $zero, 0;
mul $r11, $r9,$r10,0;
add $r6, $r6, $r11, 0;     \\sum+=a[r][]*b[][]


add $r5, $r5, $imm, 1;
blt $imm, $r5, $r2, forloop3;
add $zero, $zero, $zero,0 ;
//culclate store address
mull $r12, $r3, $imm, 16;
add $r12, $r12, $r4, 0;
add $r12, $r12, $imm, 0x200;\\base of the output matrix
sw $r6, $r12, $zero, 0;   \\store result

add $r12, $r12,$imm, 0x400;   \\cache conflict the cache flushes the data in cache
lw $r13,$r12,$zero,0;

add $r4, $r4, $imm, 1;
blt $imm, $r4, $r2, forloop2;
add $zero, $zero, $zero,0 ;

add $r3, $r3, $imm, 1;
blt $imm, $r3, $r2, forloop1;
add $zero, $zero, $zero,0 ;
halt $zero, $zero, $zero,0 ;