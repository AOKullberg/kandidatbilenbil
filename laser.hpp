#ifndef laser_h
#define laser_h

/* Klass för att hantera i2c-kommunikation
 */
class i2cReadWrite {
	public:
	int file_i2c; //filehandle
	
	void setup(string filename);
	void readi2cData(unsigned char RegisterAddress, unsigned char * RxBuffer, int length);
	void writei2cData(unsigned char RegisterAddress, unsigned char * TxBuffer, int length);
};

/* Definierar funktione
 */
int ping_laser_distance(i2cReadWrite i2c );
void reset_i2c(i2cReadWrite i2c);



#endif
