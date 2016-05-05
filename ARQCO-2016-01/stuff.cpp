/*#define shift 5
#define storage 6
#define dataRed 2
#define dataGreen 3
#define dataBlue 4
int datas[] = {2,3,4};
int filas[] = {22,23,24,25,26,27,28,29};

byte letra[] = {0x05,0x42,0x81,0x91,0x91,0x6E, 0x00, 0x00, 0x00};
byte colors[3] = {B11111111};//[0] = r, [1] = g, [2] = b; 255 = off, 0 = on
int actualData = 0;

void led(int l, int r, int g, int b)
{
r = constrain(r, 0, 255);
g = constrain(r, 0, 255);
b = constrain(r, 0, 255);
}

void setup()
{
for(int i = 0; i < 3; ++i)
{
pinMode(datas[i], OUTPUT);
digitalWrite(datas[i], HIGH);
}
pinMode(shift,OUTPUT);
pinMode(storage,OUTPUT);
digitalWrite(shift, LOW);
digitalWrite(storage, LOW);
for(int i = 0; i < 8; ++i)
{
pinMode(filas[i], OUTPUT);
digitalWrite(filas[i], LOW);
}
}

void loop()
{
for(int x = 0; x < 150; x++)
{
for(int c = 0; c < 8; ++c)
{
for(int f = 0; f < 8; ++f)
digitalWrite(filas[f], LOW);
if(c == 0)
digitalWrite(datas[actualData], LOW);
else
digitalWrite(datas[actualData], HIGH);
digitalWrite(shift, HIGH);
digitalWrite(shift, LOW);
digitalWrite(storage, HIGH);
digitalWrite(storage, LOW);
for(int f = 0; f < 8; ++f)
digitalWrite(filas[f], letra[c+1] & (1<<f));
delayMicroseconds(500);
}
}
delay(100);
actualData = (actualData+1)%3;
}*/
/*
struct rgb
{
byte r, g, b;
};

#define spi_latch     41//storage 12
#define output_enable 40//g 13
#define spi_mosi      51//data 14
#define spi_sck       52//shift 11
#define _START_TIME 1

#define COLUMNS 8//columns
#define ROWS 8

rgb global = {255,0,0};

byte letra[] = {0x18,0x66,0x81,0x00,0x00,0x00,0x00,0x00}; // (


byte bcm;

byte columna, fila;
byte red, green, blue, rows;

void setup()
{
//Serial.begin(9600);
pinMode(spi_latch, OUTPUT);
pinMode(output_enable, OUTPUT);
pinMode(spi_mosi, OUTPUT);
pinMode(spi_sck, OUTPUT);
digitalWrite(output_enable, LOW);
delay(10);
columna = 0;
bcm = 1;
fila = 1;
blue = green = red = 0xFF;
rows = 0x00;

//cli();
setupSPI();
//setupISR(false);
TCNT1 = 0x0000;
//sei();
}

void clear()
{
digitalWrite(spi_latch, LOW);
shiftOut(spi_mosi, spi_sck, LSBFIRST, B00000000);
shiftOut(spi_mosi, spi_sck, LSBFIRST, B11111111);
shiftOut(spi_mosi, spi_sck, LSBFIRST, B11111111);
shiftOut(spi_mosi, spi_sck, LSBFIRST, B11111111);
digitalWrite(spi_latch, HIGH);
}

void show()
{
digitalWrite(spi_latch, LOW);
shiftOut(spi_mosi, spi_sck, LSBFIRST, rows);
shiftOut(spi_mosi, spi_sck, LSBFIRST, blue);
shiftOut(spi_mosi, spi_sck, LSBFIRST, green);
shiftOut(spi_mosi, spi_sck, LSBFIRST, red);
digitalWrite(spi_latch, HIGH);
}

void loop()
{
for(int j = 0; j < 8; ++j)
{
for(int i = 0; i < 8; ++i)
{
red = ~(1<<i);
rows = (1<<j);
show();
delay(10000);
}
}
}

ISR(TIMER1_COMPA_vect)
{
//Serial.println("**");
//Serial.println(OCR1A);
//BCM implementation
digitalWrite(output_enable, HIGH);
byte data = 0;
//Serial.println("1");
if(bcm == 0)
{
OCR1A = _START_TIME;
bcm = 1;
fila <<= 1;
if(fila == 0)
{
fila = 1;
++columna;
columna &= 7;
}
//Serial.print("columna: ");Serial.println(columna);
}
//End BCM implementation
//Serial.println("2");
digitalWrite(spi_latch, LOW);
//Serial.print("Columna: ");Serial.println(columna);
//Serial.print("Fila: ");Serial.println(fila);
//Serial.print("bcm: ");Serial.println(bcm);
//SPDR = 0;

//rows
data = letra[columna] & fila;
shiftOut(spi_mosi, spi_sck, LSBFIRST, data);
//Serial.print("rows: ");Serial.println(byte(data));
//blue
data = (global.b & (bcm));
shiftOut(spi_mosi, spi_sck, LSBFIRST, data);
//Serial.print("blue: ");Serial.println(byte(data));
//green
data = (global.g & (bcm));
shiftOut(spi_mosi, spi_sck, LSBFIRST, data);
//Serial.print("green: ");Serial.println(byte(data));
//red
data = (global.r & (bcm));
shiftOut(spi_mosi, spi_sck, LSBFIRST, data);
//Serial.print("red: ");Serial.println(byte(data));
digitalWrite(spi_latch, HIGH);

digitalWrite(output_enable, LOW);
//Serial.println("**");
bcm <<= 1;
OCR1A <<= 1;
OCR1A <<= 1;
bcm <<=1;
if(bcm == 0)
{
OCR1A = _START_TIME;
bcm = 1;
}
Serial.println(OCR1A);
}

void setupISR(bool OV)
{
TCCR1A = 0;
TCCR1B = 0;
if(OV)
{
TCCR1B |= ((1 << CS12)|(1<<CS10));
TCNT1 = 0;
TIMSK1 |= (1 << TOIE1);
}
else
{
OCR1A = _START_TIME;
TCCR1B |= (1<<WGM12);
TCCR1B |= ((1<<CS12));
TIMSK1 |= (1<<OCIE1A);
}
}

void setupSPI()
{
SPCR = 0;
SPCR = ((1<<SPE)|(1<<MSTR)|(1<<DORD));
SPSR |= (1<<SPI2X);
}

byte spi_transfer(byte data)
{
SPDR = data;
while(!(SPSR & (1<<SPIF)));
return SPDR;
}

// SECOND

#define spi_latch     41//storage 12
#define output_enable 40//g 13
#define spi_miso	  50//not used
#define spi_mosi      51//data 14
#define spi_sck       52//shift 11
#define start_time	  1

volatile byte b;
volatile byte counter;

void setup()
{
	//Serial.begin(38400);
	pinMode(spi_latch, OUTPUT);
	//digitalWrite(spi_latch, LOW);//desactiva el storage
	pinMode(output_enable, OUTPUT);
	pinMode(spi_mosi, OUTPUT);
	pinMode(spi_sck, OUTPUT);
	pinMode(spi_miso, INPUT);
	digitalWrite(spi_miso, HIGH);
	digitalWrite(output_enable, LOW);
	counter = 0;
	b = 128;
	delay(10);
	cli();
	TCNT1 = 0x0000;
	setupSPI();
	setupISR(false);
	sei();
}

volatile byte brg = 0;

void loop()
{
	//Serial.println(counter);
}

ISR(TIMER1_COMPA_vect)
{
	//digitalWrite(output_enable, HIGH);
	//Serial.println(F("Fired"));
	digitalWrite(spi_latch, LOW);
	b <<= 1;
	counter++;
	OCR1A <<= 1;
	if (b == 0)
	{
		b = 1;
		counter = 2;
		OCR1A = start_time;
	}
	//SPDR = B11111111;//blue
	//asm volatile("nop");
	//while(!(SPSR & (1 << SPIF))); // transfer
	//shiftOut(spi_mosi, spi_sck, LSBFIRST, 0xFF);
	//SPDR = B11111111;//green
	//asm volatile("nop");
	//while(!(SPSR & (1 << SPIF))); // transfer
	//shiftOut(spi_mosi, spi_sck, LSBFIRST, 0xFF);
	//SPDR = ~((brg>>counter)&1);//red
	SPDR = B11111101;
	//SPDR = 0xFF;
	//asm volatile("nop");
	while (!(SPSR & (1 << SPIF))); // transfer
								   //shiftOut(spi_mosi, spi_sck, LSBFIRST, 0xFF);
	digitalWrite(spi_latch, HIGH);
	//digitalWrite(output_enable, LOW);
}


void setupISR(bool OV)
{
	TCCR1A = 0;
	TCCR1B = 0;
	if(OV)
	{
	TCCR1B |= ((1 << CS12)|(1<<CS10));
	TCNT1 = 0;
	TIMSK1 |= (1 << TOIE1);
	}
	else
	{
	OCR1A = start_time;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= ((1 << CS12));
	TIMSK1 |= (1 << OCIE1A);
	}
}

void setupSPI()
{
	SPCR = 0;
	SPCR = ((1 << SPE) | (1 << MSTR) | (1 << DORD));
	SPSR |= (1 << SPI2X);
}

void spi_tranfer(char cData)
{
	SPDR = cData;
	while (!(SPSR & (1 << SPIF)));
}


*/