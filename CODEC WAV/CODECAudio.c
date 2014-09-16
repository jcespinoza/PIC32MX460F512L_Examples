/*
** Module CODEC audio.c
**
** Provides Stereo output capabilities for the PIC32MX4 MMB 
** Use: SPI#1, I2C1
**
** Date     Rev Author Comments
** 03/19/09 1.0  Lucio Di Jasio mailto:lucio@dijasio.com
** 02/09/10 2.0  LDJ   Ported on PIC32MX4 MMB + batch buffer loading
** 02/12/10 3.0  LDJ   Ported for WM8731 CODEC output
*/

#include <plib.h>
#include "CODECAudio.h"
#include "HardwareProfile.h"


// local definitions
int SkipCount;              // oversampling counter
short BPtr;                 // pointer inside input buffer
short BCount;               // count input samples available (count down)
short OCount;               // counts samples moved into output buffer (count up)

unsigned short audioBuf[2][ OUTBUF_SIZE];

volatile AudioCfg ACfg;

typedef union {
      int   sample;
      
      struct{
        short left;
        short right; 
      };     
    } tsample;

// local prototypes
static int      Wm8731Init( int sampleRate);
static void		Wm8731DisableOutAmp(void);
static void		Wm8731EnableOutAmp(void);
static void     Wm8731Volume( int);
static int      WmMasterWrite(unsigned char regAdd, unsigned short cmd);

void setVolume( int v)
// input    v               volume setting    
{
    switch( v)
    {
      case VOL_MAX:     // find minimum divider
        ACfg.vol = 100;
        break;
        
      case VOL_MUTE:
        ACfg.vol = 0;
        break;
    
      case VOL_INC:
        if ( ACfg.vol < 100)
            ACfg.vol++;
        break;
        
      case VOL_DEC:
        if ( ACfg.vol > 0) 
            ACfg.vol--;
        break;
        
      case VOL_DEFAULT:
      default:      // medium setting
        ACfg.vol = 30;
        break;
    }    
    Wm8731Volume( ACfg.vol);
}// set volume
    
    
void initAudio( void)
// configures peripherals for CODEC Audio playback
// input    ACfg.period
{   
    int fail;
    
    // 1. init the WM8731 CODEC 
    fail = Wm8731Init( ACfg.rate);
    if (fail) 
        return;
        
    // 1. activate the SPI1 module for communication 
	SpiChnOpen( 1, SPI_CON_SLVEN|SPI_CON_CKP|SPI_CON_MODE32|
	                SPI_CON_ON|SPI_CON_FRMPOL|SPI_CON_FRMSYNC|
	                SPI_CON_FRMEN, 4);
	
//	SpiChnSetIntPriority( 1, 7, 3);
    IPC5bits.SPI1IP = 0b111;
    IPC5bits.SPI1IS = 0b11; 
	SpiChnTxIntEnable( 1);
	
	// force the 1st txfer
	SPI1BUF = 0;
    
    // init pointers/counters
    BPtr = 0;
    OCount = 0;
    BCount = 0;     // == OPtr    
    
} // initAudio

    
void startAudio( int b)
// makes b the input buffer and starts playback
// input    ACfg.skip
// modifies ACfg.cb
{     
    // 1. init pointers  and flags
    ACfg.cb = b;                    
    BPtr = 0;
    OCount = 0;
    SkipCount = ACfg.skip;
    
    // 2. init audio codec
    Wm8731EnableOutAmp();
		
    // 3. enable interrupts / run state machine
	SpiChnClrTxIntFlag( 1);
	SpiChnTxIntEnable( 1);
} // startAudio


void haltAudio( void)
{ // stops playback state machine
    BCount = 0;
    Wm8731DisableOutAmp();
	SpiChnClose( 1);	
} // halt audio


void __ISR( _SPI_1_VECTOR, ipl7) CODECInterrupt( void)
{
    int i;
    static tsample s;   // preserve last value (during skips)
    
    // 1. clear interrupt flag 
	SpiChnClrTxIntFlag( 1);
    i = SPI1BUF;                // empty receive buffer


    // 2. wait if current buffer is empty
    if (BCount != 0) 
    {
    
        // 3. check if skipping   
        if ( --SkipCount<0)
        {   // re-init skip counter
            SkipCount=ACfg.skip; 
            
          // 4. load the new samples from the current buffer
            s.left = audioBuf[ ACfg.cb][ BPtr++];
            BCount--;
            
            if ( ACfg.channels > 1) 
            {       // stereo      
                s.right = audioBuf[ ACfg.cb][ BPtr++];
                BCount--;
            }
            else    // mono
                s.right = s.left;
            
            // 6. check if current buffer emptied
            if ( BCount == 0)
            {                    
                // swap input/output buffers
                ACfg.cb = 1- ACfg.cb;               
                // restart counting input samples
                BCount = OCount;
                BPtr = 0;
                OCount = 0;
                
            } // if BCount==0
        } // skip         
        
        // skipping or not we need to service the SPI in slave mode
        SPI1BUF = s.sample;         // send new/old sample
    } // BCount           

    else {
    // even if we are idling we need to service the SPI in slave mode
        SPI1BUF = 0;            // send dummy value
    }
    
    // 7. check if a new output buffer ready to be copied
    if ( (ACfg.dsamples>0) && (OCount == 0))
    {   // move all samples available into the current RAM buffer
        //memcpy( (void*)audioBuf[1-ACfg.cb][OCount], 
        for( i=0; (ACfg.dsamples>0); i++, ACfg.dsamples--)
        {        
            audioBuf[1-ACfg.cb][ OCount] = ACfg.dbuf[ i];
            OCount++;               
        }    

        // 8. check if the output buffer was starved (start up)
        if ((BCount == 0)) 
        {
            // swap input/output buffers
            ACfg.cb = 1- ACfg.cb;
            
            // restart counting input samples
            BCount = OCount;
            BPtr = 0;
            OCount = 0;
        }
    }   // dsamples > 0
    

} // Interrupt Service Routine 


//------------------------------------------------------------------------
// Low level WM8731 CODEC interface
//
#define	WM8731_ADDRESS	            0x1a

#define	WM8731_REG_LLINE_IN		    0x0
#define	WM8731_REG_RLINE_IN		    0x1
#define	WM8731_REG_LHPHONE_OUT		0x2
#define	WM8731_REG_RHPHONE_OUT		0x3
#define	WM8731_REG_ANALOG_PATH		0x4
#define	WM8731_REG_DIGITAL_PATH		0x5
#define	WM8731_REG_PDOWN_CTRL		0x6
#define	WM8731_REG_DIGITAL_IF		0x7
#define	WM8731_REG_SAMPLING_CTRL	0x8
#define	WM8731_REG_ACTIVE_CTRL		0x9
#define	WM8731_REG_RESET		    0xf

#define Fsck		100000	    // standard I2C bus frequency


#define	I2C_PLIB


static int Wm8731Init( int sampleRate)
{
	int fail=0;
    int delay;
    
    CloseI2C1();
	OpenI2C1(I2C_EN | I2C_IDLE_CON | I2C_7BIT_ADD | I2C_STR_EN,
	     (GetPeripheralClock() / (2 * Fsck)-2));
    IdleI2C1();		// Wait to complete
    // approx 1ms delay
    for( delay = 0; delay < (GetSystemClock()/5/80000ul); delay++);

	fail|=WmMasterWrite(WM8731_REG_RESET, 0x00);	// reset device

	fail|=WmMasterWrite(WM8731_REG_LLINE_IN, 0x180);	// LRINBOTH|LINMUTE|(-34.5dB); use just the left input channel
	fail|=WmMasterWrite(WM8731_REG_LHPHONE_OUT, 0x16F);	// LRHPBOTH|!LZCEN|+6dB; use the left headphone commands
	fail|=WmMasterWrite(WM8731_REG_ANALOG_PATH, 0xd0);	// (SIDEATT=-15dB)|!SIDETONE|DACSEL|!BYPASS;

	fail|=WmMasterWrite(WM8731_REG_DIGITAL_PATH, 0x06);	// DACMU|(DEEMP=44.1KHz);

	fail|=WmMasterWrite(WM8731_REG_PDOWN_CTRL, 0x07);	// ADCPD|MICPD|LINEINPD
	fail|=WmMasterWrite(WM8731_REG_DIGITAL_IF, 0x53);	// MS|LRP|DSP_MODE

//	fail|=WmMasterWrite(WM8731_REG_SAMPLING_CTRL, 0x2f);	// SR(1011)|BOSR|USB;
//	fail|=WmMasterWrite(WM8731_REG_SAMPLING_CTRL, 0x0d);	// SR(0011)|!BOSR|USB; 8KHz exactly, but no filtering!
	fail|=WmMasterWrite(WM8731_REG_SAMPLING_CTRL, 0x23);	// SR(1000)|BOSR|USB; 44.1KHz
		
	return fail;	
}

static void Wm8731DisableOutAmp(void)
{
	WmMasterWrite(WM8731_REG_ACTIVE_CTRL, 0x00);	// ACTIVE; turn off
}

static void Wm8731EnableOutAmp(void)
{
	WmMasterWrite(WM8731_REG_ACTIVE_CTRL, 0x01);	// ACTIVE; turn on
}

static void Wm8731Volume( int level)
{ // input level 0-100
    WmMasterWrite(WM8731_REG_LHPHONE_OUT, 0x12F + (level*80/100));	// LRHPBOTH|LZCEN| 00-2F = Mute, 7F max
}


// returns 0 for success!
int WmMasterWrite(unsigned char regAdd, unsigned short cmd)
{
	unsigned char	buff[4];	// store the words here
	unsigned short	wmWord;
	int		fail=0;


	wmWord=((unsigned short)regAdd<<9)|(cmd&0x1ff);	// reg address is 7 bits, command is 9 bits
	buff[0]=(WM8731_ADDRESS<<1)|0;	// issue a write command with WM8731 address
	buff[1]=(unsigned char)(wmWord>>8);
	buff[2]=(unsigned char)(wmWord);

	// now transmit
	{
		int		ix;
	    	
		StartI2C1();		// Send the Start Bit
		IdleI2C1();		// Wait to complete
	
		for(ix=0; ix<3; ix++)
		{
			if(MasterWriteI2C1(buff[ix])!=0)
			{
				fail=1;	// failed
				break;
			}
		    IdleI2C1();		// Wait to complete

	    if ( I2C1STATbits.BCL)
	        I2C1STATbits.BCL = 0;
	        //while(1);
		}


		StopI2C1();	// Send the Stop condition
		IdleI2C1();	// Wait to complete
	}

	return fail;
}



