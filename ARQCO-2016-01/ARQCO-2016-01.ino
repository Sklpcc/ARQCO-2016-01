#define shift 5
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
}
