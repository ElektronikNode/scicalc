% scicalc 2.0 demo
% Open this file in scicalc and refresh the document.

getVersion()

% Display setup for this demo
setDigits(6)
setTrailingZeros(0)
setAccounting(0)

% Numbers, prefixes and units
f=1.5k #Hz
period=1/f #s
resistance=470 #Ohm
current=12m #A
voltage=resistance*current #V

% Constants and scalar functions
circle_area=pi*2^2 #m2
sin(30°)
rad2deg(pi/2)
ceil(1.2)
floor(1.8)

% Variables and previous result
a=3
b=2*a+1
b
$

% Parallel operator for resistor networks
R1=100 #Ohm
R2=220 #Ohm
R1||R2 #Ohm

% Row and column vectors
v=1:5
w=1:2:9
column=[1; 2; 3]
column'

% Matrices
A=[1 2; 3 4]
B=[5; 6]
A*B
A'
A''

% Scalar broadcast and element-wise operators
A+10
10-A
A.*A
A./2
2.^[1 2 3]

% Larger matrix multiplication
C=[1 2 3; 4 5 6]
D=[7 8; 9 10; 11 12]
C*D
D*C

% Inverse and right matrix division
inv(A)
A*inv(A)
A/[2 0; 0 2]

% Ranges inside matrix literals
[1:3; 4:6]

% Comments and suppressed output
hidden=123;
% The line above deliberately has no output.
