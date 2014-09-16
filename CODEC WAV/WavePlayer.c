/*
**   WavePlayer.c
**
**	07/08/06 v1.0 LDJ
**  07/14/07 v1.1 LDJ PIC32 porting
**  11/23/07 v1.2 LDJ LCD interface
**  23/02/10 v2.0 LDJ MikroE MMB WM8731 codec support
*/
// configuration bit settings, Fcy=80MHz, Fpb=80MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL 
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_20, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF

#include <plib.h>
#include <MMB.h>

#include <MDD File System/FSIO.h>
#include <lcd.h>
#include "CODECAudio.h"

#define  RIFF_DWORD  0x46464952UL
#define  WAVE_DWORD  0x45564157UL
#define  DATA_DWORD  0x61746164UL
#define  FMT_DWORD   0x20746d66UL     
#define  WAV_DWORD   0x00564157UL

typedef struct {
    int ckID;
    int ckSize;
    int ckType;
} chunk;

typedef struct {
    // format chunk 
    unsigned short  subtype;    // compression code  
    unsigned short  channels;   // # of channels
                                //  (1= mono,2= stereo)
    unsigned int    srate;      // sample rate in Hz
    unsigned int    bps;        // bytes per second

    unsigned short  bpsample;   // bytes per sample
                                //    (4=16bit stereo)
    unsigned short  bitpsample; // bit per sample
    unsigned short  extra;      // extra format bytes
} WAVE_fmt; 


main( void)
{
    SearchRec sr;
    unsigned char sa = ATTR_READ_ONLY | ATTR_ARCHIVE;

    MMBInit();
    initLCD();
    putsLCD( "   WAV Player v1.0\n\n");
    putsLCD( " for MikroE PIC32MX4\n\n");
    putsLCD( "   2/20/2010 LDJ\n\n");
    MMBFadeIn( 500);
    
    while( 1)
    {
        putsLCD( "Insert card...");
        while ( !FSInit())
            DelayMs( 100);
        
        if ( FindFirst( "*.WAV", sa, &sr))
        {
            clrLCD();
            putsLCD("File not found");
        }
        else do
        {
            clrLCD();
            putsLCD("Playing:");
            putsLCD(sr.filename);
            playWAV( sr.filename);
    
            if ( MMBReadKey())
                MMBGetKey();
            // repeatedly play all songs
        }
        while ( !FindNext(&sr));

        // ask to replace the card     
        clrLCD();
        putsLCD("Remove Card!");

        // wait for SD card to be removed
        while( FSInit())
            DelayMs( 100); // !mount() )
        clrLCD();

    } // while
} // main 



/*-------------------------------------------------------------
** Wave
** 
** 	Wave File Player 
**	Uses 2 x 8 bit PWM channels at 44kHz
**
** revision:
** 05/02/03 v1.00 LDJ PIC18
** 07/08/06 v2.00 LDJ porting to PIC24/C30
** 07/14/07 v3.00 LDJ PIC32 porting
*/
int playWAV( char *name)
{
    chunk    ck;
    WAVE_fmt wav;
    FSFILE   *fp;
    short buffer[ OUTBUF_SIZE];
    
    unsigned int lc, r, bpchan;    
    char s[16];
    
    r = FALSE;
        
    // 1. open the file           
    if ( (fp = FSfopen( name, "r")) == NULL)
    {   // failed to open 
        return r;
    }
    
    // 2. verify it is a RIFF - WAVE formatted file
    FSfread( (void*)&ck, sizeof(chunk), 1, fp);
       
    // check that file type is correct
    if (( ck.ckID != RIFF_DWORD) || ( ck.ckType != WAVE_DWORD))
        goto Exit;
    
    // 3. look for the chunk containing the wave format data
    FSfread( (void*)&ck, 1, 8, fp);
    if ( ck.ckID != FMT_DWORD)
        goto Exit;

    // 4. get the WAVE_FMT struct
    FSfread( (void*)&wav, 1, sizeof(WAVE_fmt), fp);
    ACfg.channels = wav.channels;
    bpchan = wav.bpsample/wav.channels;
        
    // 4.1 print some additional info
    sprintf( s, "\nRate: %d", wav.srate); LCDPuts( s);
    sprintf( s, "\nChan: %d", wav.channels); LCDPuts( s);
    sprintf( s, "\nBits: %d", wav.bitpsample); LCDPuts( s);
    
    // 5. skip extra format bytes
    FSfseek( fp, ck.ckSize - sizeof(WAVE_fmt), SEEK_CUR);
    
    // 6. search for the "data" chunk
    while( 1)
    {   // read next chunk 
        if ( FSfread( (void*)&ck, 1, 8, fp) != 8)
            goto Exit;

        if ( ck.ckID != DATA_DWORD)
            FSfseek( fp, ck.ckSize, SEEK_CUR);
        else
            break;
    } 

    // 7. find the data size 
    lc = ck.ckSize;
    
    // 8. reduce sample rate to sub of 44100Hz
    ACfg.rate = wav.srate; 
    ACfg.skip = 1;              
    while ( ACfg.rate < 44100)
    {
        ACfg.rate <<= 1;           // multiply sample rate by two
        ACfg.skip <<= 1;           // multiply skip by two
    }
    ACfg.skip--;    // 0, 1, 3, 7...

    // 9. define a buffer to transfer data to CODEC
    ACfg.dbuf = buffer;
    
    // 10. configure codec/Player state machine and start
    initAudio( );
    startAudio( 0); 

        
    // 11. keep feeding the buffers in the playing loop
    //     as long as entire buffers can be filled
    while (lc > 0)            
    {   // 11.1 check user input to stop playback
        if ( MMBReadKey())          // if any button pressed
        {                           
            lc = 0;                 // playback completed
            break;
        }
        
        // 11.2 refill buffer
        r = FSfread( (void*)buffer, 1, OUTBUF_SIZE*bpchan, fp); 
        lc -= r;                 // decrement byte count
        if ( bpchan == 1)    
        {   // extend samples to 16 bit signed 
            unsigned char *p = (unsigned char*)&buffer[r/2-1]+1;
            int i;
            for( i=r-1; i>=0; i--)
                buffer[ i] = ((*p--)<<8)^0x8000; 
                
        }    
        ACfg.dsamples = r/bpchan;

        while ( ACfg.dsamples >0);          // wait for data to be consumed
        // <<put here additional tasks>>
        
        
    } // while wav data available
    
    // 16. stop playback 
    haltAudio();

    // return with success
    r = TRUE;

Exit:
    // 17. close the file 
    FSfclose( fp);

    // 18. return
    return r;

} // playWAV
         


