% tests
% send output back

// Temporary display settings
getVersion()
setDigits(6)
setTrailingZeros(0)
setAccounting(0)

// Numeric literals and SI prefixes
0
1
1.5
1,5
1e3
2.5e-3
3k
4M
5G
6m
7u
8n
9p
10f
11a
sin(30°)

// Basic operators and precedence
2+3
7-5
4*6
8/2
2^3
2^3^2
(2+3)*4
2+3*5^2
2^(-3)
10/4
5-8

// Parallel operator
100||100
100||200
10+100||100
(10+100)||100

// Variables, constants and previous result
a=3
b=2*a+1
b
$
pi
_g
_G
_mu0
_eps0
_c0
_e
_kB
_h

// Units and stored unit display for autocomplete
force=3 #N
resistance=470 #Ohm
hidden_unit=12; #V
resistance

// Comments and suppressed output
// This line should have no output
% This line should also have no output
123;
456; #A

// Built-in functions
sqrt(9)
exp(1)
log(exp(1))
ln(exp(1))
log10(1000)
ceil(1.2)
floor(1.8)
round(1.5)
sin(pi/2)
cos(0)
tan(pi/4)
asin(1)
acos(1)
atan(1)
atan2((1),(1))
abs(-5)
rad2deg(pi)

// Function errors
sqrt()
sqrt(1, 2)
unknownFunction(1)
setDigits(6)
setTrailingZeros(0)
setAccounting(0)

// Parser/runtime errors
unknownVariable
1/0
2^-3
(1+2
1+)

// Matlab initializer lists and first matrix operators
2.*3
2./3
2.^3
[1 2 3]
[1, 2; 3, 4]
matrixA=[1 2; 3 4]
matrixB=[5; 6]
matrixA+1
matrixA+[10 20; 30 40]
matrixA*matrixB
matrixA.*matrixA
matrixA./2
2.^[1 2 3]
[1 2 3].^2
matrixA'
matrixA''
matrixC=[1 2 3; 4 5 6]
matrixD=[7 8; 9 10; 11 12]
matrixC*matrixD
matrixD*matrixC
matrixC+10
10+matrixC
matrixC-1
10-matrixC
matrixC*2
2*matrixC
matrixC/2
matrixC.*2
2.*matrixC
matrixC./2
12./matrixC
matrixC.^2
2.^matrixC
[1; 2; 3]'
[1 2 3]'
[1 2; 3 4]'*[10; 20]
inv([1 2; 3 4])
[1 2; 3 4]*inv([1 2; 3 4])
[1 2; 3 4]/[2 0; 0 2]
sqrt([1 4; 9 16])
exp([0 1])
log(exp([1 2; 3 4]))
ln(exp([1 2]))
log10([1 10; 100 1000])
ceil([1.2 2.8; -1.2 -2.8])
floor([1.2 2.8; -1.2 -2.8])
round([1.2 1.5 1.8; -1.2 -1.5 -1.8])
sin([0 pi/2])
cos([0 pi])
tan([0 pi/4])
asin([0 1])
acos([1 0])
atan([0 1])
atan2([1 1],[1 0])
abs([-1 2; -3 4])
rad2deg([0 pi/2; pi 2*pi])
1:5
1:2:9
5:-2:1
5:1
[1:3; 4:6]

// Matrix error cases
[1 2; 3]
[1 2]+[1 2 3]
[1 2]*[1 2]
[1 2]^2
sqrt([1 2])
100||[100 200]
1:0:5
inv([1 2; 2 4])
inv([1 2 3; 4 5 6])
