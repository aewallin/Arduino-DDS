import numpy
#from bigfloat import * # https://pypi.python.org/pypi/bigfloat/0.3.0a2
import math 

#fs = BigFloat.exact( '660e6', precision=400)
fs = 660e6

varnames= ['100 MHz','10 MHz ','1 MHz','100 kHz','10 kHz ','1 kHz','100 Hz','10 Hz ','1 Hz','100 mHz','10 mHz ','1 mHz','100 uHz','10 uHz ','1 uHz','100 nHz','10 nHz','1 nHz']
#print b

# FTW = int( f/fs * 2**48 )
Nmax = 18;
print "const int F660[18][14]  = \t {"
for (div,variable) in zip(range(Nmax),varnames):
	#f = BigFloat.exact( '100e6', precision=400)/BigFloat.exact(10**div)
	f = 100e6 / 10**div

	#print f
	a = f/fs
	bits = 48
	ebits = 64
	#b=  int( math.ceil( a*BigFloat.exact(2**(bits+ebits)) ) )
	b=  int( math.ceil( a*2**(bits+ebits) ) )
	comma = ","
	if div==Nmax-1:
		comma=""
	comment = "// {0} ".format(variable)
	print " \t { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x }%s %s" % ( 0x0000000000FF & (b >> -64+ebits),
	0x0000000000FF & (b >> -56+ebits),
	0x0000000000FF & (b >> -48+ebits),
	0x0000000000FF & (b >> -40+ebits),
	0x0000000000FF & (b >> -32+ebits),
	0x0000000000FF & (b >> -24+ebits),
	0x0000000000FF & (b >> -16+ebits), 
	0x0000000000FF & (b >> -8+ebits), 
	0x0000000000FF & (b>>(0+ebits)),
	0x0000000000FF & (b>>(8+ebits)),
	0x0000000000FF & (b>>(16+ebits)),
	0x0000000000FF & (b>>(24+ebits)),
	0x0000000000FF & (b>>(32+ebits)),
	0x0000000000FF & (b>>(40+ebits)), comma, comment)
	#print "%012.6f" % (BigFloat(b)/2**48 * fs)
print "};"

def digit(nr,pos):
	if pos == 0:
	    d = int(nr%10) 
	elif pos > 0:
		d = int( int(nr)%pow(10,pos) / pow(10,pos-1) ) 
	else:
		d = (int(nr*pow(10,-pos))% 10)
	return d
	
# fDDS = FTW/2^48  Fs
print "const int FTW660[6][18]  = \t {"
for n in range(6):
    #b = BigFloat(2**(n*8))
    #f = ( b / BigFloat(2**48) )*fs
    b = (2**(n*8))
    f = ( float(b) / (2**48) )*fs
    #print f
    print "{",
    for d in [-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8,9]:
		print digit(f,d),
		if d!=9:
			print ",",
		else :
			print " }",
			if n != 5:
			    print ","
			else:
			    print " };"
