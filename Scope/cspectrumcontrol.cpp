#include "cspectrumcontrol.h"
#include "ui_cspectrumcontrol.h"

#include <complex>
typedef std::complex<float> Complex;


class Fft
{
public:
    Fft (unsigned int points, unsigned int sampleRate);
    // Hard work
    ~Fft(){delete [] _aBitRev; delete [] _X; delete [] _W;}
    void    Transform (float* data);
    // Output: from i = 0 to points-1
    float inline  GetIntensity (unsigned int i) const
    {
        return abs (_X[i])*_sqrtPoints;
    }
    unsigned  Points () const { return _points; }
    // return frequency in Hz of a given point
    int GetFrequency (unsigned int point) const
    {
        //assert (point < _points);
        long x =_sampleRate * point;
        return x / _points;
    }
    int HzToPoint (int freq) const
    {
        return (int)_points * freq / _sampleRate;
    }
    int MaxFreq() const { return _sampleRate; }
    float _volume;

private:
    void DataIn (float* data);
    //    void inline PutAt (unsigned int i, int val)
    //    {
    //        _X [_aBitRev[i]] = Complex (val);
    //    }
private:
    unsigned int	_points;
    unsigned int	_sampleRate;
    unsigned int	_logPoints;		// binary log of _points
    float		_sqrtPoints;	// square root of _points

    unsigned int*			_aBitRev;	// bit reverse vector
    Complex*				_X;	// in-place fft array
    Complex*	_W;	// exponentials
    //int*				 _tape;	// recording tape
};

const float PI = 2.0 * asin (1.0);

Fft::Fft (unsigned int points, unsigned int sampleRate)
    : _points (points), _sampleRate (sampleRate), _volume (1)
{
    //_tape=new int[_points];
    _sqrtPoints = 1.0/sqrtf (_points);
    // calculate binary log
    _logPoints = 0;
    points--;
    while (points != 0)
    {
        points >>= 1;
        _logPoints++;
    }

    _aBitRev=new unsigned int[_points];
    _X=new Complex[_points];
    _W=new Complex[_logPoints*_points];
    // Precompute complex exponentials
    int _2_l = 2;
    for (unsigned int l = 1; l <= _logPoints; l++)
    {
        //_W[l].resize (_points);

        for (unsigned int i = 0; i != _points; i++ )
        {
            float re =  cos (2. * PI * i / _2_l);
            float im = -sin (2. * PI * i / _2_l);
            _W[((l-1)*_points)+i] = Complex (re, im);
        }
        _2_l *= 2;
    }

    // set up bit reverse mapping
    int rev = 0;
    unsigned int halfPoints = _points/2;
    for (unsigned int i = 0; i < _points - 1; i++)
    {
        _aBitRev [i] = rev;
        int mask = halfPoints;
        // add 1 backwards
        while (rev >= mask)
        {
            rev -= mask; // turn off this bit
            mask >>= 1;
        }
        rev += mask;
    }
    _aBitRev [_points-1] = _points-1;
}

void Fft::DataIn (float* data)
{
    /*
        if (Size  > _points)
                throw Exception ("Sample buffer larger than FFT capacity");
    // make space for samples at the end of tape
    // shifting previous samples towards the beginning
        // to           from
        // v-------------|
        // xxxxxxxxxxxxxxyyyyyyyyy
        // yyyyyyyyyoooooooooooooo
        // <- old -><- free tail->
        if (_points-Size>0)  memcpy(_tape,_tape+(Size*sizeof(int)),(_points-Size)*sizeof(int));
        if (data==NULL)
        {
            ZeroMemory(_tape + ((_points - Size )*sizeof(int)),Size*sizeof(int));
        }
        else
        {
            memcpy(_tape + ((_points - Size )*sizeof(int)),data,Size*sizeof(int));
        }

        //std::copy (_tape.begin () + data.size (), _tape.end (), _tape.begin ());
        //std::copy (data.begin (), data.end (), _tape.begin () + (_points - data.size ()));
 */
    // Initialize the FFT buffer
    for (unsigned int i = 0; i < _points; i++)
        //       PutAt (i, data[i]);
        _X [_aBitRev[i]] = Complex (data[i]*_volume);
}

//----------------------------------------------------------------
//               0   1   2   3   4   5   6   7
//  level   1
//  step    1                                     0
//  increm  2                                   W
//  j = 0        <--->   <--->   <--->   <--->   1
//  level   2
//  step    2
//  increm  4                                     0
//  j = 0        <------->       <------->      W      1
//  j = 1            <------->       <------->   2   W
//  level   3                                         2
//  step    4
//  increm  8                                     0
//  j = 0        <--------------->              W      1
//  j = 1            <--------------->           3   W      2
//  j = 2                <--------------->            3   W      3
//  j = 3                    <--------------->             3   W
//                                                              3
//----------------------------------------------------------------

void Fft::Transform (float* data)
{
    DataIn (data);

    unsigned int step = 1;
    unsigned int increm,i,j;
    unsigned int BufferNum;
    unsigned int istep;
    Complex U,T;
    for (unsigned int level = 1; level <= _logPoints; level++)
    {
        BufferNum=(level-1)*_points;
        increm = step << 1;
        for (j = 0; j < step; j++)
        {
            U = _W [BufferNum+j];
            for (i = j; i < _points; i += increm)
            {
                istep=step+i;
                // butterfly
                T = U *_X [istep];
                _X [istep] = _X[i]-T;
                _X [i] += T;
            }
        }
        step=step << 1;
    }
}



//class Fft;
//class SampleIter;

class ViewFreq
{
public:
    ViewFreq (int Width,int Height,int ScaleWidth);
    ~ViewFreq (){}
    void Update (Fft const & fftTransformer);
    void Clear ();
    void Fake ();
    void Init (int points, int point1kHz);
    void PaintScale ();
    void SetBounds (int Width,int Height,int ScaleWidth);
    QImage img;
    QImage Scale;
    int Mode;
private:
    int _xRecord;
    int	_width;
    int	_height;
    int ImgHeight;
    int _points2; // Half the number of points = max freq
    int	_point1kHz;	// Point corresponding to 1kHz
    QImage BackBitmap;
    QRect R;
    void inline MoveTo(int X, int Y);
    void inline LineTo(int X, int Y,QPainter* p);
    QPoint CurrentPoint;
};

int ColorScale [] =
{
    0x800000,	//RGB(0, 0, 128), // dk blue
    0xFF0000,//RGB(0, 0, 255), // lt blue
    0xFF00,//RGB(0, 255, 0), // lt green
    0xFF00,//RGB(0, 255, 0),
    0xFFFF,//RGB(255, 255, 0), // lt yellow
    0xFFFF,//RGB(255, 255, 0),
    0xFFFF,//RGB(255, 255, 0),
    0xFFFF,//RGB(255, 255, 0),
    0x80FF,//RGB(255, 128, 0), // orange
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0x80FF,//RGB(255, 128, 0),
    0xFF//RGB(255, 0, 0) // lt red
};

int inline MapColor (unsigned int s)
{
    s /= 16;
    if (s >= sizeof (ColorScale))
        s = sizeof (ColorScale) - 1;
    return ColorScale [s];
}

// ViewFreq -------------

static int SCALE_WIDTH = 30;
static int NOTCH_1 = 5;
static int NOTCH_2 = 10;
static int NOTCH_3 = SCALE_WIDTH;

ViewFreq::ViewFreq (int Width,int Height,int ScaleWidth):
    _xRecord (1),
    _points2 (0),
    _point1kHz (0)
{
    Mode=0;
    SetBounds(Width,Height,ScaleWidth);
}

void ViewFreq::SetBounds (int Width,int Height,int ScaleWidth)
{
    img=QImage(Width,Height,QImage::Format_RGB32);
    Scale=QImage(ScaleWidth,Height,QImage::Format_RGB32);
    _width = Width;
    _height = Height;
    BackBitmap=QImage(1,_height,QImage::Format_RGB32);
    ImgHeight=_height;
}

void ViewFreq::Clear ()
{
    QPainter(&img).fillRect(img.rect(),Qt::black);
    QPainter(&BackBitmap).fillRect(BackBitmap.rect(),Qt::black);
}

void ViewFreq::Init (int points, int point1kHz)
{
    _points2 = points / 2;
    _point1kHz = point1kHz;
    _height=_points2;
    BackBitmap=QImage(1,_height,QImage::Format_RGB32);
}

void inline ViewFreq::MoveTo(int X, int Y)
{
    CurrentPoint=QPoint(X,Y);
}

void inline ViewFreq::LineTo(int X, int Y, QPainter* p)
{
    p->drawLine(CurrentPoint.x(),CurrentPoint.y(),X,Y);
    MoveTo(X,Y);
}

void ViewFreq::PaintScale ()
{
    if (_points2 == 0)
    {
        return;
    }
    QRect r(Scale.rect());
    QPainter p(&Scale);
    p.fillRect(r,Qt::black);
    int x0 = r.right();
    int y0 = r.height();
    r.setHeight(_height);
    p.setPen(Qt::lightGray);
    //QFontMetrics fm(QFont());
    int charY;//, charX;
    charY=0;//fm.height();
    MoveTo(x0, 0);
    LineTo(x0, ImgHeight,&p);
    // draw a notch every 100 Hz
    float s1000 = (float)ImgHeight/(float)(CPresets::Presets.SampleRate/2000);
    int s100 = s1000 / 10;
    int count = 0;
    //int xx = x0;
    int bigNotchCount = 0;
    if (s100 < 3)
    {
        //for (int s = 0; s < maxS; s += s1000)
        forever
        {
            int y= y0 -(count*s1000);
            //int y = y0 - ((float)s*ScaleY);
            if (y <= 0)
                break;
            if (count % 10 == 0) // very big notch
            {
                MoveTo(x0 - NOTCH_3,y);
                LineTo(x0, y, &p);
                if (bigNotchCount == 1)
                {
                    p.drawText(0,(y)-(charY/2),"10k");
                    //canvas.Text ("10k", 0, y - charY/2);
                }
                ++bigNotchCount;
            }
            else
            {
                MoveTo(x0 - NOTCH_2, y);
                LineTo(x0, y, &p);
            }
            count++;
        }
    }
    else
    {
        forever
        {
            int y= y0 -(count*s100);
            if ( y <= 0 )
                break;
            if (count % 10 == 0) // big notch
            {
                MoveTo(x0 - NOTCH_2, y);
                LineTo(x0, y, &p);
                if (bigNotchCount == 1)
                {
                    p.drawText(0,(y)-(charY/2),"1k");
                }
                ++bigNotchCount;
            }
            else
            {
                MoveTo(x0 - NOTCH_1, y);
                LineTo(x0, y, &p);
            }
            count++;
        }
    }
    Clear();
}

void ViewFreq::Update (Fft const &fftTransformer)
{
    if (Mode==2)
    {
        img.fill(0);
        QPainter p(&img);
        int s;
        int xwidth=(_width*2)/3;
        p.setPen(Qt::darkGray);
        p.drawLine(xwidth,0,xwidth,ImgHeight);
        float step = (float)ImgHeight/(float)(_height);
        for (int i = 0; i < _height; i++ )
        {
            int y=ImgHeight-(step*i);
            s = fftTransformer.GetIntensity(i)* xwidth/10.0;
            if (s>xwidth)
            {
                p.fillRect(0,y,xwidth,1,Qt::green);
                p.fillRect(xwidth,y,s-xwidth,1,Qt::red);
            }
            else
            {
                p.fillRect(0,y,s,1,Qt::green);
            }
        }
        _xRecord=0;
    }
    else
    {
        QPainter p(&BackBitmap);
        //p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(BackBitmap.rect(),Qt::black);
        int s;
        int y=_height;//+pts;
        QColor color;
        for (int i = 0; i < _height; i++ )
        {
            y--;
            s = fftTransformer.GetIntensity(i)* 128;
            if (s > 8)
            {
                if (s>170)
                {
                    s=(s-170)*3;
                    if (s>255) s=255;
                    color=QColor(s,s,0);
                }
                else if (s>85)
                {
                    color=QColor((s-85)*3,0,0);
                }
                else
                {
                    color=QColor(0,0,s*3);
                }
                p.setPen(color);
                p.drawPoint(0,y);
            }
        }

        if (Mode==0)
        {
            QPainter imgPainter(&img);
            imgPainter.setRenderHint(QPainter::SmoothPixmapTransform);
            imgPainter.drawImage(QRect(_xRecord,0,1,ImgHeight),BackBitmap,BackBitmap.rect());
        }
        else if (Mode==1)
        {
            img=img.copy(1,0,_width,ImgHeight);
            QPainter imgPainter(&img);
            imgPainter.setRenderHint(QPainter::SmoothPixmapTransform);
            imgPainter.drawImage(QRect(_width-1,0,1,ImgHeight),BackBitmap,BackBitmap.rect());
            _xRecord=0;
        }
    }
    _xRecord++;
    /*
    img->Canvas->CopyRect(TRect(_xRecord,0,_xRecord+1,ImgHeight),BackBitmap->Canvas,R);
    _xRecord++;
    img->Canvas->CopyRect(TRect(_xRecord,0,_xRecord+1,ImgHeight),BackBitmap->Canvas,R);
    _xRecord++;
    */
    if (_xRecord >= _width)
    {
        _xRecord = 0;
    }
}

void ViewFreq::Fake ()
{
    if (Mode==2)
    {
        img.fill(0);
        QPainter p(&img);
        int xwidth=(_width*2)/3;
        p.setPen(Qt::darkGray);
        p.drawLine(xwidth,0,xwidth,ImgHeight);
        _xRecord=0;
    }
    else if (Mode==1)
    {
        img=img.copy(1,0,_width,ImgHeight);
        _xRecord=0;
    }
    else if (Mode==0)
    {
        QPainter(&img).fillRect(_xRecord,0,_xRecord+1,ImgHeight,Qt::black);
        //img->Canvas->FillRect(TRect(_xRecord,0,_xRecord+2,_height));
    }
    _xRecord ++;
    if (_xRecord >= _width)
    {
        _xRecord =  0;
    }
}

//---------------------------------------------------------------------------

CSpectrumControl::CSpectrumControl(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CSpectrumControl)
{
    ui->setupUi(this);
    m_ViewFreq=new ViewFreq(width()-24,height(),24);
    m_Fft=new Fft(CPresets::Presets.ModulationRate,CPresets::Presets.SampleRate);
    ViewFreq* VF=(ViewFreq*)m_ViewFreq;
    Fft* F=(Fft*)m_Fft;
    VF->Init(F->Points (), F->HzToPoint (1000));
}

CSpectrumControl::~CSpectrumControl()
{
    delete (ViewFreq*)m_ViewFreq;
    delete (Fft*)m_Fft;

    delete ui;
}

void CSpectrumControl::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    ViewFreq* VF=(ViewFreq*)m_ViewFreq;
    Fft* F=(Fft*)m_Fft;
    VF->SetBounds(width()-24,height(),24);
    VF->Init(F->Points (), F->HzToPoint (1000));
    VF->PaintScale();
    VF->Clear();
}

void CSpectrumControl::paintEvent(QPaintEvent* /*event*/)
{
    ViewFreq* VF=(ViewFreq*)m_ViewFreq;
    QPainter p(this);
    p.drawImage(VF->img.rect().translated(24,0),VF->img);
    p.drawImage(VF->Scale.rect(),VF->Scale);
}

void CSpectrumControl::Process(float* Buffer)
{
    ViewFreq* VF=(ViewFreq*)m_ViewFreq;
    Fft* F=(Fft*)m_Fft;
    if (Buffer==NULL)
    {
        VF->Fake();
    }
    else
    {
        F->Transform(Buffer);
        VF->Update(*F);
    }
}

void CSpectrumControl::SetVol(int Vol)
{
    Fft* F=(Fft*)m_Fft;
    F->_volume=(float)Vol*0.01;
}

void CSpectrumControl::SetMode(int Mode)
{
    ViewFreq* VF=(ViewFreq*)m_ViewFreq;
    VF->Mode=Mode;
}
