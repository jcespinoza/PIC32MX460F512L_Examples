using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace GIF_Decoder
{
    public partial class GIFDecoder : Form
    {
        // global logic image structure
        int GWidth, GHeight, Background, Aspect, Flags, GCTsize;
        // local logic image structure
        int ILeft, ITop, IWidth, IHeight, IFlags, LCTsize;
        // Transparent Color
        int TColor = -1;

        // drawing coordinates
        int x, y;

        // dictionary string
        public int[] S = new int[16384];
        int Ssize;

        // color tables
        public struct COLORentry
        {
            public int R;
            public int G;
            public int B;
        }
        public COLORentry[] GCT = new COLORentry[256];
        public COLORentry[] LCT = new COLORentry[256];

        // dictionary table
        public struct TABLEentry
        { 
            public int ix;  // index in string
            public int len;  // length
        } 
        public TABLEentry[] TABLE = new TABLEentry[4096];
        public TABLEentry W, E;

        // data block and bit bucket 
        int BLOCKsize;
        int bitcount, bucket;

        // GIF special codes
        int CLEARCODE, EOI;


        private int getCODE(int CODEsize, BinaryReader r)
        {
            // fill the bucket adding bytes as necessary
            while(CODEsize > bitcount)
            { // need a new byte
                if (BLOCKsize == 0)
                {
                    BLOCKsize = r.ReadByte();   // start a new block
                    if (BLOCKsize == 0)
                        return -1;              // end of stream
                }
                // add byte to the bucket
                bucket += (r.ReadByte() << bitcount);
                bitcount += 8;
                // count the bytes used
                BLOCKsize--;
            }

            // extract CODEs from the bucket
            int ret = bucket & ((1 << CODEsize) - 1);
            bitcount -= CODEsize;
            bucket >>= CODEsize;
            return ret;
        }

        private void Plot(int code)
        {
            Graphics objG = Graphics.FromImage(mDS);
            SolidBrush myBrush;
            Color myColor;
//            if (code != TColor)   // ignore Transparent Color
            {
                myColor = Color.FromArgb(255, GCT[code].R, GCT[code].G, GCT[code].B);
                myBrush = new SolidBrush(myColor);
                objG.FillRectangle(myBrush, (ILeft + x) * 2, (ITop + y) * 2, 2, 2);
                myBrush.Dispose();
            }
                if (++x >= IWidth)
            {
                x = 0; y++;
            }

            objG.Dispose();
        }

        private void CopyS( int dest, int source, int len)
        {
            for( int i=0; i<len; i++)
                S[dest++] = S[source++];
        }

        private int InitTABLE(int CODEsize)
        {   // init the root table
            int size;

            Ssize = 0;
            size = 1 << CODEsize;
            for (int i = 0; i < size; i++)
            {
                S[i] = i;
                TABLE[i].ix = i;
                TABLE[i].len = 1;
            }
            Ssize = size;       // initially only one code per entry
            CLEARCODE = size;
            EOI = size + 1;
            size += 2;          // make room for CC and EOI

            W.ix = Ssize;       // W empty
            W.len = 0;
            E.ix = W.ix;
            E.len = 0;

            return size;
        }

        private void LZW( BinaryReader r)
        {
            int TABLEsize, CODEsize, INITsize, k;

            // 1. get the initial CODEsize and init the TABLE
            INITsize = r.ReadByte();                    // the code size byte 
            TABLEsize = InitTABLE(INITsize);            // init table and define CC and EOI symbols
            CODEsize= INITsize+1;                       // must increase already the code size
            BLOCKsize = 0;                              // init the bit slicer
            bucket = 0;
            bitcount = 0;

            // 2. get the first code (should always be CLEARCODE in a GIF stream)
            k = getCODE(CODEsize, r);
            if (k == CLEARCODE)
            {
                TABLEsize = InitTABLE(INITsize);    // re-init the table and redefine CC and EOI
                CODEsize = INITsize + 1;            // re-init code size
                k = getCODE(CODEsize, r);           // get the first real LZW code
            }

            // 3. translate immediately the first code
            Plot(S[k]);

            // 4. put it in W
            S[Ssize] = k;   // put code on top of the string
            W.ix = Ssize;   // W is on top of S
            W.len = 1;
            Ssize++;        // extend S

            // 5. read one code at a time
            while ((k = getCODE(CODEsize, r)) >= 0)      // read next code
            {
                if (k < TABLEsize)
                {  // entry = dictionary (k)
                    if (k == CLEARCODE)
                    {
                        TABLEsize = InitTABLE(INITsize);    // re-init the table and redefine CC and EOI
                        CODEsize = INITsize + 1;            // re-init code size
                        continue;
                    }
                    else if (k == EOI)
                        goto EndOfImage;
                    else
                    {   // entry = TABLE[k]
                        E.ix = W.ix + W.len;                        // E follows W
                        CopyS(E.ix, TABLE[k].ix, TABLE[k].len);     // E = TABLE[k]
                        E.len = TABLE[k].len;                       // E is last on S
                        Ssize += E.len;                             // check Ssize?
                    }
                }
                else if (k == TABLEsize)
                { // entry = W + W[0]
                    E.ix = W.ix + W.len;                // E follows W
                    CopyS(E.ix, W.ix, W.len);           // E = W 
                    S[E.ix + W.len] = S[W.ix];          // + W[0]
                    E.len = W.len + 1;                  // E is last on S
                    Ssize += E.len;                     // check Ssize?
                }
                else
                    goto FormatError;

                // 6. output Entry
                for (int i = 0; i < E.len; i++)         // plot all points in E
                    Plot(S[E.ix + i]);

                // 7. add W + Entry[0] into TABLE 
                TABLE[TABLEsize].ix = W.ix;             // make new TABLE[] = W + E[0]
                TABLE[TABLEsize].len = W.len + 1;       // have its lenght
                TABLEsize++;                            // point to next empty element in TABLE
                // check if code size needs to be increased
                if (TABLEsize == (1 << CODEsize))
                    CODEsize++;
                if (CODEsize > 12)
                    goto FormatError;

                // 8. make W = E
                W.ix = E.ix;
                W.len = E.len;
            }

        EndOfImage:
            // check termination block
            if ((k = r.ReadByte())!= 0)
                goto FormatError;
            return;

        FormatError:
            return;

        } // LZW

        private Bitmap mDS;     // my drawing surface



        private int EXT(BinaryReader r)
        {
            int ex, i;
            int GTime, GFlag;

            // for now skip all extension blocks
            ex = r.ReadByte();                   // Extension code

            switch (ex)
            {
                case 0xF9:                      // Graphic Control Extension
                    if ((i = r.ReadByte()) != 4)      // lenght should be 4
                        return -1;                  
                                        
                    GFlag = r.ReadByte();       // get packed flags
/*
 * 3 bit reserved
 * 3 bit disposal method
 * 1 bit user Input 
 * 1 bit Transparent Color 
 */
                    GTime = r.ReadByte();       // get delay time
                    GTime += r.ReadByte()*256;
                    TColor = r.ReadByte();      // get transparent color
                    
                    if ((i = r.ReadByte())>0)
                       return -1;               // should be 0 termination 

                    if (GTime > 0)
                    {
                        // start the timer
                        Timer1.Interval = GTime*10; // convert to ms
                        IDLE=1;
                    }
                        

                    break;
                default:
                    while ((i = r.ReadByte()) > 0)      // skip all data blocks
                    {  // consume the data
                        while (i-- > 0) r.ReadByte();
                    }
                    break;
            }
            return 1;
        } // EXT


        private void btnOpen_Click(object sender, EventArgs e)
        {

/*
 *          string sFilename, sLine;
            saveFileDialog1.InitialDirectory = @"C:\";
            saveFileDialog1.Title = "Output File Name";
            saveFileDialog1.FileName = OutputFile.Text;
            if (saveFileDialog1.ShowDialog() == DialogResult.Cancel)
                return;
            sFilename = saveFileDialog1.FileName;
            OutputFile.Text = sFilename;
            StreamWriter dstFile = new StreamWriter(sFilename);
*/
            FileStream fs = new FileStream(@"c:/C32/Dev/GIF/reel3.gif", FileMode.Open, FileAccess.Read);
            BinaryReader r = new System.IO.BinaryReader(fs);
            byte c, v; 

            // 1. File signature 
            if ((c = r.ReadByte())!= 'G') 
                goto FormatError;
            if ((c = r.ReadByte())!= 'I') 
                goto FormatError;
            if ((c = r.ReadByte())!= 'F') 
                goto FormatError;
            if ((c = r.ReadByte())!= '8') 
                goto FormatError;
            v = r.ReadByte();              // version number '7'=87, '9'=89
            if ((r.ReadByte())!= 'a') 
                goto FormatError;

            // 2. Logical Image descriptor
            GWidth = r.ReadByte();
            GWidth += r.ReadByte() * 256;
            GHeight = r.ReadByte();
            GHeight += r.ReadByte() * 256;

            mDS = new Bitmap(GWidth * 2, GHeight* 2, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
          
            
            Flags = r.ReadByte();
            Background = r.ReadByte();
            Aspect = r.ReadByte();
          /*    Flags 
           *    Global Color Table Flag       1 Bit
           *    Color Resolution              3 Bits
           *    Sort Flag                     1 Bit
           *    Size of Global Color Table    3 Bits
          */
            
            // 3. Global Color Table GCT (optional)
            if (Flags > 128) 
            {
                // determine size of the GCT
                GCTsize = 1 << ((Flags & 0x7) + 1);

                // load the GCT
                for (int i = 0; i < GCTsize; i++)
                {
                    GCT[i].R = r.ReadByte();    // RED
                    GCT[i].G = r.ReadByte();   // GREEN
                    GCT[i].B = r.ReadByte();   // BLUE
                }

            }

            // 4. <Data>
            do
            {
                int i, ex;
                c = r.ReadByte();
                switch (c)
                {
                    case 0x2c:  // Image

                        // 4.1 Image descriptor
                        ILeft = r.ReadByte();
                        ILeft += r.ReadByte() * 256; //Console.WriteLine(ILeft);
                        ITop = r.ReadByte();
                        ITop += r.ReadByte() * 256;

                        IWidth = r.ReadByte();
                        IWidth += r.ReadByte() * 256;
                        IHeight = r.ReadByte();
                        IHeight += r.ReadByte() * 256;
                        x = 0; y = 0;

                        IFlags = r.ReadByte();
                        /*  IFlags                         
                         *      Local Color Table Flag        1 Bit
                         *      Interlace Flag                1 Bit
                         *      Sort Flag                     1 Bit
                         *      Reserved                      2 Bits
                         *      Size of Local Color Table     3 Bits
                        */

                        // 4.2 Local Color Table LCT (optional)
                        if (IFlags > 128)
                        {
                            // determine size of the GCT
                            LCTsize = 1 << ((IFlags & 0x7) + 1);

                            // load the LCT
                            for ( i = 0; i < LCTsize; i++)
                            {
                                LCT[i].R = r.ReadByte();    // RED
                                LCT[i].G = r.ReadByte();    // GREEN
                                LCT[i].B = r.ReadByte();    // BLUE
                            }

                        }

                        // decompress image
                        LZW( r);
                        this.Refresh();
                        
                        if (IDLE>0)       // if a graphic control block was found
                        {
                            Timer1.Start();
                            // wait for a flag to be rised by triggered event
                            while (IDLE > 0)
                                Application.DoEvents();
                        }

                        break; // Image

                    case 0x01:  // Text
                    case 0x21:  // Extension
                        EXT( r);
                        break;  // Extensions

                    case 0x3b:  // Terminator
                        break;

                    default:
                        goto FormatError;
                }
            } while (c != 0x3b);

            this.Refresh();

    FormatError:
            r.Close();
            fs.Close();

        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.DrawImage(mDS, 0, 0, mDS.Width, mDS.Height);
            e.Graphics.Dispose();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            mDS = new Bitmap(256 * 2, 200 * 2, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
/*            Graphics objG = Graphics.FromImage(mDS);
            objG.Clear(Color.Black);
            objG.Dispose();
*/
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public GIFDecoder()
        {
            InitializeComponent();
        }

        int IDLE = 0;
        private void Timer1_Tick(object sender, EventArgs e)
        {
            IDLE = 0; Timer1.Stop();
        }
  
    }
}
