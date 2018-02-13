# soundOfData

encode any file into frequencies

File -> Hex Code -> (each hex code) into a Frequency

usage:

./a.out fileToEncode (m or f) mul | arecord out.wav

./a.out fileToEncode (m or f) mul false

Arguments:

argv[1]		->	file to encode
"2		->	m or f / morse or frequency
"3		->	multipier for m is 1 and f is any number
"4		->	print hex data, default true (optional)
"5		->	in freq mode set time duration of note
"6		->	Enable sound, default is true (t/f)

m encode file->hex->morse
f encode file->hex->hex freq (each hex value is a different freq)


hex value	|	freq (Hz)
-------------------------------------
0		|	16.35
1		|	17.32
2		|	18.35
3		|	19.45
4		|	20.60
5		|	21.83
6		|	23.12
7		|	24.50
8		|	25.96
9		|	27.50
a		|	29.14
b		|	30.87
c		|	32.70
d		|	34.65
e		|	36.71
f		|	38.89
