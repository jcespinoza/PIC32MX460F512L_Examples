/*
** Module audio.h
**
** Provides Stereo output capabilities for the PIC32MX MMB 
**
** Date     Rev Author Comments
** 03/19/09 1.0  LDJ   First draft
** 02/09/10 2.0  LDJ   Ported on PIC32MX4 MMB
*/

#define OUTBUF_SIZE     64

// profiling  
extern volatile unsigned freeTime;

// audio configuration 
typedef struct {
    char cb;                // current buffer (0/1)
    char vol;
    char ef;                // empty flag
    short *dbuf;            // pointer to decoder output buffer
    unsigned short dsamples;         // number of samples in decoder output buffer
    char channels;          // stereo/mono
    char skip;              // oversampling 
    long period;            // sampling timer period 
    long rate;              // sample rate (20kHz< x <48kHz)
} AudioCfg;

extern volatile AudioCfg ACfg;

#define VOL_MAX     -1      // maximum amplitude
#define VOL_MUTE    0       // mute output (continue playback)
#define VOL_DEFAULT 1       // medium initial value
#define VOL_INC     2       // increase volume (<MAX)
#define VOL_DEC     3       // decrease volume (>MUTE)


// functions prototypes
void initAudio( void);
void startAudio( int);
void haltAudio( void);
void setVolume( int);
