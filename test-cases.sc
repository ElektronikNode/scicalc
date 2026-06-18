% tests
% send output back

// Temporary display settings
getVersion()
				2.2.0-003
setDigits(6)
				6.00000
setTrailingZeros(0)
				0
setAccounting(0)
				0

// Numeric literals and SI prefixes
0
				0
1
				1
1.5
				1.5
1,5
				1.5
1e3
				1k
2.5e-3
				2.5m
3k
				3k
4M
				4M
5G
				5G
6m
				6m
7u
				7u
8n
				8n
9p
				9p
10f
				10f
11a
				11a
12T
				12T
13P
				13P
14E
				14E
15e-21
				15e-21
sin(30°)
				500m

// Basic operators and precedence
+5
				5
-5
				-5
-(-5)
				5
2+3
				5
7-5
				2
4*6
				24
8/2
				4
2^3
				8
2^3^2
				64
(2+3)*4
				20
2+3*5^2
				77
2^(-3)
				125m
10/4
				2.5
5-8
				-3

// Parallel operator
100||100
				50
100||200
				66.6667
10+100||100
				60
(10+100)||100
				52.381
100||(100+100i)
				60+20j

// Variables, constants and previous result
a=3
				3
b=2*a+1
				7
b
				7
$
				7
pi
				3.14159
_g
				9.81
_G
				66.7384p
_mu0
				1.25664u
_eps0
				8.85419p
_c0
				299.792M
_e
				160.218e-21
_kB
				13.8065e-24
_h
				662.607e-36

// Units and stored unit display for autocomplete
force=3 #N
				3N
resistance=470 #Ohm
				470Ω
hidden_unit=12; #V
resistance
				470

// Comments and suppressed output
// This line should have no output
% This line should also have no output
123;
456; #A

// Built-in functions
sqrt(9)
				3
exp(1)
				2.71828
log(exp(1))
				1
ln(exp(1))
				1
log10(1000)
				3
ceil(1.2)
				2
floor(1.8)
				1
round(1.5)
				2
sin(pi/2)
				1
cos(0)
				1
tan(pi/4)
				1
asin(1)
				1.5708
acos(1)
				0
atan(1)
				785.398m
atan2((1),(1))
				785.398m
abs(-5)
				5
real(3+4i)
				3
imag(3+4i)
				4
conj(3+4i)
				3-4j
angle(1+i)
				785.398m
sqrt(-1)
				1j
exp(i*pi)
				-1
log(-1)
				3.14159j
ln(-1)
				3.14159j
ceil(1.2+2.8i)
				2+3j
floor(1.2+2.8i)
				1+2j
round(1.2+2.8i)
				1+3j
sin(i)
				1.1752j
cos(i)
				1.54308
tan(i)
				761.594mj
asin(i)
				881.374mj
acos(i)
				1.5708-881.374mj
atan(i)
				infj
rad2deg(pi)
				180

// Function errors
sqrt()
				ERROR: invalid number of arguments for function 'sqrt'
sqrt(1, 2)
				ERROR: invalid number of arguments for function 'sqrt'
unknownFunction(1)
				ERROR: unknown function 'unknownFunction'
setDigits(6)
				ERROR: setDigits can only be called once per script
setTrailingZeros(0)
				ERROR: setTrailingZeros can only be called once per script
setAccounting(0)
				ERROR: setAccounting can only be called once per script

// Parser/runtime errors
unknownVariable
				ERROR: variable 'unknownVariable' has no value yet
1/0
				ERROR: division by zero
2^-3
				ERROR: unexpected symbol '-'
(1+2
				ERROR: expected ')' but received 'end of line'
1+)
				ERROR: unexpected symbol ')'

// Matlab initializer lists and first matrix operators
2.*3
				6
2./3
				666.667m
2.^3
				8
[1 2 3]
				[1  2  3]
[1, 2; 3, 4]
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
matrixA=[1 2; 3 4]
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
matrixB=[5; 6]
				⎡ 5 ⎤
				⎣ 6 ⎦
matrixA+1
				⎡ 2  3 ⎤
				⎣ 4  5 ⎦
matrixA+[10 20; 30 40]
				⎡ 11  22 ⎤
				⎣ 33  44 ⎦
matrixA*matrixB
				⎡ 17 ⎤
				⎣ 39 ⎦
matrixA.*matrixA
				⎡ 1   4 ⎤
				⎣ 9  16 ⎦
matrixA./2
				⎡ 500m  1 ⎤
				⎣  1.5  2 ⎦
2.^[1 2 3]
				[2  4  8]
[1 2 3].^2
				[1  4  9]
matrixA'
				⎡ 1  3 ⎤
				⎣ 2  4 ⎦
matrixA''
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
matrixC=[1 2 3; 4 5 6]
				⎡ 1  2  3 ⎤
				⎣ 4  5  6 ⎦
matrixD=[7 8; 9 10; 11 12]
				⎡  7   8 ⎤
				⎢  9  10 ⎥
				⎣ 11  12 ⎦
matrixC*matrixD
				⎡  58   64 ⎤
				⎣ 139  154 ⎦
matrixD*matrixC
				⎡ 39  54   69 ⎤
				⎢ 49  68   87 ⎥
				⎣ 59  82  105 ⎦
matrixC+10
				⎡ 11  12  13 ⎤
				⎣ 14  15  16 ⎦
10+matrixC
				⎡ 11  12  13 ⎤
				⎣ 14  15  16 ⎦
matrixC-1
				⎡ 0  1  2 ⎤
				⎣ 3  4  5 ⎦
10-matrixC
				⎡ 9  8  7 ⎤
				⎣ 6  5  4 ⎦
matrixC*2
				⎡ 2   4   6 ⎤
				⎣ 8  10  12 ⎦
2*matrixC
				⎡ 2   4   6 ⎤
				⎣ 8  10  12 ⎦
matrixC/2
				⎡ 500m    1  1.5 ⎤
				⎣    2  2.5    3 ⎦
matrixC.*2
				⎡ 2   4   6 ⎤
				⎣ 8  10  12 ⎦
2.*matrixC
				⎡ 2   4   6 ⎤
				⎣ 8  10  12 ⎦
matrixC./2
				⎡ 500m    1  1.5 ⎤
				⎣    2  2.5    3 ⎦
12./matrixC
				⎡ 12    6  4 ⎤
				⎣  3  2.4  2 ⎦
matrixC.^2
				⎡  1   4   9 ⎤
				⎣ 16  25  36 ⎦
2.^matrixC
				⎡  2   4   8 ⎤
				⎣ 16  32  64 ⎦
[1; 2; 3]'
				[1  2  3]
[1 2 3]'
				⎡ 1 ⎤
				⎢ 2 ⎥
				⎣ 3 ⎦
[1 2; 3 4]'*[10; 20]
				⎡  70 ⎤
				⎣ 100 ⎦
inv([1 2; 3 4])
				⎡  -2      1 ⎤
				⎣ 1.5  -500m ⎦
[1 2; 3 4]*inv([1 2; 3 4])
				⎡ 1  0 ⎤
				⎣ 0  1 ⎦
[1 2; 3 4]/[2 0; 0 2]
				⎡ 500m  1 ⎤
				⎣  1.5  2 ⎦
sqrt([1 4; 9 16])
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
exp([0 1])
				[1  2.71828]
log(exp([1 2; 3 4]))
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
ln(exp([1 2]))
				[1  2]
log10([1 10; 100 1000])
				⎡ 0  1 ⎤
				⎣ 2  3 ⎦
ceil([1.2 2.8; (-1.2) (-2.8)])
				⎡  2   3 ⎤
				⎣ -1  -2 ⎦
floor([1.2 2.8; (-1.2) (-2.8)])
				⎡  1   2 ⎤
				⎣ -2  -3 ⎦
round([1.2 1.5 1.8; (-1.2) (-1.5) (-1.8)])
				⎡  1   2   2 ⎤
				⎣ -1  -2  -2 ⎦
sin([0 pi/2])
				[0  1]
cos([0 pi])
				[1  -1]
tan([0 pi/4])
				[0  1]
asin([0 1])
				[0  1.5708]
acos([1 0])
				[0  1.5708]
atan([0 1])
				[0  785.398m]
atan2([1 1],[1 0])
				[785.398m  1.5708]
abs([-1 2; -3 4])
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
rad2deg([0 pi/2; pi 2*pi])
				⎡   0   90 ⎤
				⎣ 180  360 ⎦

// Complex numbers
i
				1j
j
				1j
3+4i
				3+4j
3+4j
				3+4j
2i
				2j
-2i
				-2j
i^2
				-1
1/i
				-1j
(3+4i)+(1-2i)
				4+2j
(3+4i)-(1-2i)
				2+6j
(3+4i)*(1-2i)
				11-2j
(3+4i)/(1-2i)
				-1+2j
(3+4i)^2
				-7+24j
(1+i)^3
				-2+2j
5k+3mj #V
				(5k+3mj)V
sqrt(-1)
				1j
abs(3+4i)
				5
real(3+4i)
				3
imag(3+4i)
				4
conj(3+4i)
				3-4j
angle(1+i)
				785.398m
exp(i*pi)
				-1
sin(i)
				1.1752j
complexA=[1 i; 2 3]
				⎡ 1  1j ⎤
				⎣ 2   3 ⎦
complexA'
				⎡   1  2 ⎤
				⎣ -1j  3 ⎦
inv(complexA)
				⎡  692.308m+461.538mj  153.846m-230.769mj ⎤
				⎣ -461.538m-307.692mj  230.769m+153.846mj ⎦
complexA*inv(complexA)
				⎡ 1  0 ⎤
				⎣ 0  1 ⎦
sin(complexA)
				⎡ 841.471m  1.1752j ⎤
				⎣ 909.297m  141.12m ⎦
abs(complexA)
				⎡ 1  1 ⎤
				⎣ 2  3 ⎦
real(complexA)
				⎡ 1  0 ⎤
				⎣ 2  3 ⎦
imag(complexA)
				⎡ 0  1 ⎤
				⎣ 0  0 ⎦
conj(complexA)
				⎡ 1  -1j ⎤
				⎣ 2    3 ⎦
complexB=[1+2i 3-4i; 5j 6]
				⎡ 1+2j  3-4j ⎤
				⎣   5j     6 ⎦
complexB+1
				⎡ 2+2j  4-4j ⎤
				⎣ 1+5j     7 ⎦
complexB+(1+i)
				⎡ 2+3j  4-3j ⎤
				⎣ 1+6j  7+1j ⎦
complexB.*complexB
				⎡ -3+4j  -7-24j ⎤
				⎣   -25      36 ⎦
complexB./(1+i)
				⎡ 1.5+500mj  -500m-3.5j ⎤
				⎣  2.5+2.5j        3-3j ⎦
complexB.^2
				⎡ -3+4j  -7-24j ⎤
				⎣   -25      36 ⎦
2.^[1 i; 2 3]
				⎡ 2  769.239m+638.961mj ⎤
				⎣ 4                   8 ⎦
complexB*complexB'
				⎡     30  28-29j ⎤
				⎣ 28+29j      61 ⎦
inv([1 i; 2 3])
				⎡  692.308m+461.538mj  153.846m-230.769mj ⎤
				⎣ -461.538m-307.692mj  230.769m+153.846mj ⎦
[1 i; 2 3]/[2 0; 0 2]
				⎡ 500m  500mj ⎤
				⎣    1    1.5 ⎦

1:5
				[1  2  3  4  5]
0:0.25:1
				[0  250m  500m  750m  1]
(-2):1:2
				[-2  -1  0  1  2]
1:2:9
				[1  3  5  7  9]
5:-2:1
				[5  3  1]
5:1
				[]
[1:3; 4:6]
				⎡ 1  2  3 ⎤
				⎣ 4  5  6 ⎦

// Matlab-compatible vector and matrix functions
sum(5)
				5
sum([1 2 3])
				6
sum(matrixC)
				[5  7  9]
sum(matrixC,2)
				⎡  6 ⎤
				⎣ 15 ⎦
cumsum([1 2 3])
				[1  3  6]
cumsum(matrixC,2)
				⎡ 1  3   6 ⎤
				⎣ 4  9  15 ⎦
prod([2 3 4])
				24
cumprod([2 3 4])
				[2  6  24]
min([3 1 2])
				1
max([3 1 2])
				3
mean([1 2 3])
				2
median([3 1 2 4])
				2.5
std([1 2 3])
				1
var([1 2 3])
				1
size(matrixC)
				[2  3]
size(matrixC,1)
				2
size(matrixC,2)
				3
length(matrixC)
				3
numel(matrixC)
				6
reshape(1:6,2,3)
				⎡ 1  3  5 ⎤
				⎣ 2  4  6 ⎦
reshape(matrixC,3,2)
				⎡ 1  5 ⎤
				⎢ 4  3 ⎥
				⎣ 2  6 ⎦
zeros(2,3)
				⎡ 0  0  0 ⎤
				⎣ 0  0  0 ⎦
ones(2,2)
				⎡ 1  1 ⎤
				⎣ 1  1 ⎦
eye(3)
				⎡ 1  0  0 ⎤
				⎢ 0  1  0 ⎥
				⎣ 0  0  1 ⎦
eye(2,3)
				⎡ 1  0  0 ⎤
				⎣ 0  1  0 ⎦
diag([1 2 3])
				⎡ 1  0  0 ⎤
				⎢ 0  2  0 ⎥
				⎣ 0  0  3 ⎦
diag(matrixA)
				⎡ 1 ⎤
				⎣ 4 ⎦
linspace(1,5,5)
				[1  2  3  4  5]
logspace(0,3,4)
				[1  10  100  1k]
det(matrixA)
				-2
trace(matrixA)
				5
rank(matrixA)
				2
norm([3 4])
				5
dot([1 2 3],[4 5 6])
				32
cross([1 0 0],[0 1 0])
				[0  0  1]
eig([1 0; 0 2])
				⎡ 1 ⎤
				⎣ 2 ⎦
diff([1 4 9 16])
				[3  5  7]
diff(matrixC,1,2)
				⎡ 1  1 ⎤
				⎣ 1  1 ⎦
gradient([1 4 9 16])
				[3  4  6  7]
sort([3 1 2])
				[1  2  3]
sort([3 1; 2 4],1)
				⎡ 2  1 ⎤
				⎣ 3  4 ⎦
unique([3 1 2 3 1])
				[1  2  3]
trapz([1 2 3])
				4
trapz(matrixC,2)
				⎡  4 ⎤
				⎣ 10 ⎦
unwrap([0, pi, (2*pi+0.1)])
				[0  3.14159  100m]
sum([1 i; 2 3])
				[3  3+1j]
mean([1+i 3+i])
				2+1j
det([1 i; 2 3])
				3-2j
dot([1 i],[2 3i])
				5

// Matrix error cases
[1 2; 3]
				ERROR: all matrix rows must have the same length
[1 2]+[1 2 3]
				ERROR: matrix dimensions must agree for '+'
[1 2]*[1 2]
				ERROR: matrix dimensions must agree for '*'
[1 2]^2
				ERROR: matrix power is not implemented; use '.^' for element-wise powers
100||[100 200]
				ERROR: parallel operator expects scalar values
1:0:5
				ERROR: range step must not be zero
inv([1 2; 2 4])
				ERROR: matrix is singular
inv([1 2 3; 4 5 6])
				ERROR: inv expects a square matrix
sum(matrixC,3)
				ERROR: sum dimension must be 1 or 2
reshape(1:5,2,3)
				ERROR: reshape dimensions must agree with number of elements
zeros(-1,2)
				ERROR: zeros dimensions must not be negative
det([1 2 3; 4 5 6])
				ERROR: det expects a square matrix
dot([1 2],[1 2 3])
				ERROR: matrix dimensions must agree for 'dot'
cross([1 2],[1 2])
				ERROR: cross expects 3-element vectors
atan2(1+i, 1)
				ERROR: atan2 expects real values
1:i:5
				ERROR: range operator expects real values
1 i
				ERROR: expected 'end of line' but received 'i'
[1 i; 2]
				ERROR: all matrix rows must have the same length
matrixA
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦

