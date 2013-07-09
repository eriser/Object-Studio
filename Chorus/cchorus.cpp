#include "cchorus.h"

CChorus::CChorus()
{
}

void CChorus::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::Numeric,"Frequency","Hz",0,500,100,"",400);
    AddParameter(ParameterType::Numeric,"Phase","Centigrades",0,180,0,"",100);
    AddParameter(ParameterType::Numeric,"Depth","%",0,100,0,"",50);
    AddParameter(ParameterType::Numeric,"Delay","%",0,100,0,"",50);
    AddParameter(ParameterType::Numeric,"Contour","Hz",20,20000,0,"",100);
    AddParameter(ParameterType::dB,"Dry Level","dB",0,200,0,"",100);
    AddParameter(ParameterType::dB,"Wet Level","dB",0,200,0,"",100);
    for (int i = 0; i < COS_TABLE_SIZE; i++) cos_table[i] = cos(i * 2.0f * M_PI / COS_TABLE_SIZE);
    ring=new float[(DEPTH_BUFLEN + DELAY_BUFLEN) * m_Presets.SampleRate / 192000];
    buflen = (DEPTH_BUFLEN + DELAY_BUFLEN) * m_Presets.SampleRate / 192000;
    pos = 0;
    cm_phase = 0.0f;
    dm_phase = 0.0f;
    ZeroMemory(ring,((DEPTH_BUFLEN + DELAY_BUFLEN) * m_Presets.SampleRate / 192000)*sizeof(float));
    biquad_init(&highpass);
    CalcParams();
}

CChorus::~CChorus() {
    if (m_Initialized) delete [] ring;
}

float *CChorus::GetNextA(const int ProcIndex) {
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    float* input=InSignal;
    float* output=AudioBuffers[ProcIndex]->Buffer;
    unsigned int sample_index;
    unsigned int sample_count = m_BufferSize;
    float in = 0;
    float d = 0.0f;
    float f = 0.0f;
    float out = 0.0f;
    float phase = 0.0f;
    float fpos = 0.0f;
    float n = 0.0f;
    float rem = 0.0f;
    float s_a, s_b;
    float d_pos = 0.0f;
    for (sample_index = 0; sample_index < sample_count; sample_index++)
    {
        in = *(input++);
        push_buffer(in, ring, buflen, &pos);
        cm_phase += Frequency / m_Presets.SampleRate * COS_TABLE_SIZE;
        while (cm_phase >= COS_TABLE_SIZE) cm_phase -= COS_TABLE_SIZE;
        dm_phase = Phase * COS_TABLE_SIZE / 2.0f;
        phase = cm_phase;
        d_pos = Delay * m_Presets.SampleRate / 1000.0f;
        fpos = d_pos + Depth * (0.5f + 0.5f * cos_table[(unsigned int)phase]);
        n = floor(fpos);
        rem = fpos - n;
        s_a = read_buffer(ring, buflen, pos, (unsigned int) n);
        s_b = read_buffer(ring, buflen, pos, (unsigned int) n + 1);
        d = ((1 - rem) * s_a + rem * s_b);
        f = biquad_run(&highpass, d);
        out = (DryLevel * in) + (WetLevel * f);
        *(output++) = out;
    }

    return AudioBuffers[ProcIndex]->Buffer;
}

float CChorus::read_buffer(float *buffer, unsigned int buflen, unsigned int pos, unsigned int n) {
    while (n + pos >= buflen)  n -= buflen;
    return buffer[n + pos];
}

void CChorus::write_buffer(float insample, float *buffer, unsigned int buflen, unsigned int pos, unsigned int n) {
    while (n + pos >= buflen) n -= buflen;
    buffer[n + pos] = insample;
}

float CChorus::push_buffer(float insample, float *buffer, unsigned int buflen, unsigned int *pos) {

    float outsample;
    outsample = buffer[*pos];
    buffer[(*pos)++] = insample;
    if (*pos >= buflen) *pos = 0;
    return outsample;
}

void CChorus::CalcParams() {
    Frequency=m_ParameterValues[pnFrequency]*0.01;
    Phase=m_ParameterValues[pnPhase];
    Depth=100.0f * m_Presets.SampleRate / 44100.0f * m_ParameterValues[pnDepth] / 100.0f;
    Delay=m_ParameterValues[pnDelay];
    if (Delay < 1.0f) Delay = 1.0f;
    Delay = 100.0f - Delay;
    Contour=m_ParameterValues[pnContour];
    DryLevel=(float)m_ParameterValues[pnDryLevel]*0.01;
    WetLevel=(float)m_ParameterValues[pnWetLevel]*0.01;
    hp_set_params(&highpass, Contour, HP_BW, m_Presets.SampleRate);
}
