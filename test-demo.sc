% scicalc 2.1 demo
% Open this file in scicalc and refresh the document.

getVersion()
				2.1.0-012

// Accounting for line based continued calculations:
setAccounting(1)
				1.000000

3+5
				8.000000
+2
				10.00000


% Display setup for this demo
setDigits(6)
				6.00000
setTrailingZeros(0)
				0
setAccounting(0)
				ERROR: setAccounting can only be called once per script

% Numbers, prefixes and units
f=1.5k #Hz
				1.5kHz
period=1/f #s
				666.667us
resistance=470 #Ohm
				470Ω
current=12m #A
				12mA
voltage=resistance*current #V
				5.64V

% Constants and scalar functions
circle_area=pi*2^2 #m2
				12.5664m2
sin(30°)
				500m
rad2deg(pi/2)
				90
ceil(1.2)
				2
floor(1.8)
				1
round(1.5)
				2

% Complex numbers
3+4i
				3+4j
sqrt(-1)
				1j
5k+3mj #V
				(5k+3mj)V
abs(3+4i)
				5
conj(3+4i)
				3-4j

% Variables and previous result
a=3
				3
b=2*a+1
				7
b
				7
$
				7

% Parallel operator for resistor networks
R1=100 #Ohm
				100Ω
R2=220 #Ohm
				220Ω
R1||R2 #Ohm
				68.75Ω

% Row and column vectors
v=1:5
				[1  2  3  4  5]
w=1:2:9
				[1  3  5  7  9]
column=[1; 2; 3]
				⎡ 1 ⎤
				⎢ 2 ⎥
				⎣ 3 ⎦
column'
				[1  2  3]

% Matrices
A=[1 2; 3 4]
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦
B=[5; 6]
				⎡ 5 ⎤
				⎣ 6 ⎦
A*B
				⎡ 17 ⎤
				⎣ 39 ⎦
A'
				⎡ 1  3 ⎤
				⎣ 2  4 ⎦
A''
				⎡ 1  2 ⎤
				⎣ 3  4 ⎦

% Scalar broadcast and element-wise operators
A+10
				⎡ 11  12 ⎤
				⎣ 13  14 ⎦
10-A
				⎡ 9  8 ⎤
				⎣ 7  6 ⎦
A.*A
				⎡ 1   4 ⎤
				⎣ 9  16 ⎦
A./2
				⎡ 500m  1 ⎤
				⎣  1.5  2 ⎦
2.^[1 2 3]
				[2  4  8]
sin(A)
				⎡ 841.471m   909.297m ⎤
				⎣  141.12m  -756.802m ⎦
round(A./2)
				⎡ 1  1 ⎤
				⎣ 2  2 ⎦

% Larger matrix multiplication
C=[1 2 3; 4 5 6]
				⎡ 1  2  3 ⎤
				⎣ 4  5  6 ⎦
D=[7 8; 9 10; 11 12]
				⎡  7   8 ⎤
				⎢  9  10 ⎥
				⎣ 11  12 ⎦
C*D
				⎡  58   64 ⎤
				⎣ 139  154 ⎦
D*C
				⎡ 39  54   69 ⎤
				⎢ 49  68   87 ⎥
				⎣ 59  82  105 ⎦

% Inverse and right matrix division
inv(A)
				⎡  -2      1 ⎤
				⎣ 1.5  -500m ⎦
A*inv(A)
				⎡ 1  0 ⎤
				⎣ 0  1 ⎦
A/[2 0; 0 2]
				⎡ 500m  1 ⎤
				⎣  1.5  2 ⎦

% Ranges inside matrix literals
[1:3; 4:6]
				⎡ 1  2  3 ⎤
				⎣ 4  5  6 ⎦

% Complex matrices
Z=[1 i; 2 3]
				⎡ 1  1j ⎤
				⎣ 2   3 ⎦
Z*inv(Z)
				⎡ 1  0 ⎤
				⎣ 0  1 ⎦
sin(Z)
				⎡ 841.471m  1.1752j ⎤
				⎣ 909.297m  141.12m ⎦

% Comments and suppressed output
hidden=123;
% The line above deliberately has no output.
