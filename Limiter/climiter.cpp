#include "climiter.h"
#include <QDebug>

CLimiter::CLimiter()
{

}

void CLimiter::Init(const int Index, void *MainWindow) {
    m_Name=devicename;
    IDevice::Init(Index,MainWindow);
    AddJack("Out",IJack::Wave,IJack::Out,0);
    AddJack("In",IJack::Wave,IJack::In,0);
    AddParameter(ParameterType::dB,"Limit Vol","dB",0,200,0,"",100);
    AddParameter(ParameterType::dB,"Out Vol","dB",0,200,0,"",100);
    /* 80 Hz is the lowest frequency with which zero-crosses were
                 * observed to occur (this corresponds to 40 Hz signal frequency).
                 */
    // so lets make the buffer double size anyhoo :)
    buflen = m_Presets.SampleRate / 40;

    pos = 0;
    ready_num = 0;
    ringbuffer=new float[buflen];
    ZeroMemory(ringbuffer,buflen*sizeof(float));
    CalcParams();
}

float *CLimiter::GetNextA(const int ProcIndex) {
    float* InSignal=FetchA(jnIn);
    if (!InSignal) return NULL;
    unsigned int sample_index;
    unsigned int sample_count = m_BufferSize;
    unsigned int index_offs = 0;
    unsigned int i;
    float max_value = 0;
    float section_gain = 0;
    unsigned int run_length;
    unsigned int total_length = 0;
    float* output=AudioBuffers[ProcIndex]->Buffer;
    float* input=InSignal;

    while (total_length < sample_count)
    {
        run_length = buflen;
        if (total_length + run_length > sample_count)
            run_length = sample_count - total_length;

        while (ready_num < run_length)
        {
            //look for zero-crossings and detect a half cycle
            if (read_buffer(ringbuffer, buflen,pos, ready_num) >= 0)
            {
                index_offs = 0;
                while ((read_buffer(ringbuffer, buflen, pos, ready_num + index_offs) >= 0) &&
                       (ready_num + index_offs < run_length))
                {
                    index_offs++;
                }
            }
            else
            {
                index_offs = 0;
                while ((read_buffer(ringbuffer, buflen, pos, ready_num + index_offs) <= 0) &&
                       (ready_num + index_offs < run_length))
                {
                    index_offs++;
                }
            }

            /* search for max value in scanned halfcycle */
            max_value = 0;
            for (i = ready_num; i < ready_num + index_offs; i++)
            {
                if (fabs(read_buffer(ringbuffer, buflen, pos, i)) > max_value)
                {
                    max_value = fabs(read_buffer(ringbuffer, buflen, pos, i));
                }
            }
            if (max_value>0)
            {
                section_gain = limit_vol / max_value;
            }
            else
            {
                section_gain = 1.0;
            }
            if (max_value > limit_vol)
            {
                for (i = ready_num; i < ready_num + index_offs; i++)
                {
                    write_buffer(read_buffer(ringbuffer, buflen, pos, i) * section_gain, ringbuffer, buflen, pos, i);
                }
            }
            ready_num += index_offs;
        }

        /* push run_length values out of ringbuffer, feed with input */
        for (sample_index = 0; sample_index < run_length; sample_index++)
        {
            *(output++) = out_vol * push_buffer(*(input++), ringbuffer, buflen, &(pos));
        }
        ready_num -= run_length;
        total_length += run_length;
    }
    //*(latency) = buflen;
    return AudioBuffers[ProcIndex]->Buffer;
}

CLimiter::~CLimiter() {
    if (m_Initialized)
    {
        delete [] ringbuffer;
    }
}

void CLimiter::write_buffer(float insample, float *buffer, unsigned int buflen, unsigned int pos, unsigned int n) {
    while (n + pos >= buflen)
    {
        n -= buflen;
    }
    buffer[n + pos] = insample;
}

float CLimiter::push_buffer(float insample, float *buffer, unsigned int buflen, unsigned int *pos) {

    float outsample;

    outsample = buffer[*pos];
    buffer[(*pos)++] = insample;

    if (*pos >= buflen)
    {
        *pos = 0;
    }
    return outsample;
}

void CLimiter::CalcParams() {
    limit_vol=m_ParameterValues[pnLimitVol]*0.01;
    out_vol=m_ParameterValues[pnOutVol]*0.01;
}

float CLimiter::read_buffer(float *buffer, unsigned int buflen, unsigned int pos, unsigned int n) {
    while (n + pos >= buflen)
    {
        n -= buflen;
    }
    return buffer[n + pos];
}
