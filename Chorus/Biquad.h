
#ifndef BiquadH

#define BiquadH

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

/* Please note that the majority of the definitions and helper
functions below have been derived from the source code of Steve
Harris's SWH plugins (particularly from the "biquad.h" file).  While I
give him credit for his excellent work, I reserve myself to be blamed
for any bugs or malfunction. */

#ifndef db2lin
#define db2lin(x) ((x) > -96.0f ? pow(10.0f, (x) * 0.05f) : 0.0f)
#endif

#ifndef lin2db
#define lin2db(x) ((x)>0 ? 20.0f*(log(x)/log(10)):-96)
#endif

#define ABS(x)  (x)>0.0f?(x):-1.0f*(x)


#define LN_2_2 0.34657359f
#define FLUSH_TO_ZERO(x) (((*(unsigned int*)&(x))&0x7f800000)==0)?0.0f:(x)
#define LIMIT(v,l,u) ((v)<(l)?(l):((v)>(u)?(u):(v)))

#define BIQUAD_TYPE float
typedef BIQUAD_TYPE bq_t;


/* Biquad filter (adapted from lisp code by Eli Brandt,
   http://www.cs.cmu.edu/~eli/) */

/* The prev. comment has been preserved from Steve Harris's biquad.h */

typedef struct {
	bq_t a1;
	bq_t a2;
	bq_t b0;
	bq_t b1;
	bq_t b2;
	bq_t x1;
	bq_t x2;
	bq_t y1;
	bq_t y2;
} biquad;


static inline void biquad_init(biquad *f) {

	f->x1 = 0.0f;
	f->x2 = 0.0f;
	f->y1 = 0.0f;
	f->y2 = 0.0f;
}


static inline
void
eq_set_params(biquad *f, bq_t fc, bq_t gain, bq_t bw, bq_t fs) {

	bq_t w = 2.0f * M_PI * LIMIT(fc, 1.0f, fs/2.0f) / fs;
	bq_t cw = cos(w);
	bq_t sw = sin(w);
	bq_t J = pow(10.0f, gain * 0.025f);
	bq_t g = sw * sin(LN_2_2 * LIMIT(bw, 0.0001f, 4.0f) * w / sw);
	bq_t a0r = 1.0f / (1.0f + (g / J));

	f->b0 = (1.0f + (g * J)) * a0r;
	f->b1 = (-2.0f * cw) * a0r;
	f->b2 = (1.0f - (g * J)) * a0r;
	f->a1 = -(f->b1);
	f->a2 = ((g / J) - 1.0f) * a0r;
}


static inline void lp_set_params(biquad *f, bq_t fc, bq_t bw, bq_t fs) {
	bq_t omega = 2.0 * M_PI * fc/fs;
	bq_t sn = sin(omega);
	bq_t cs = cos(omega);
	bq_t alpha = sn * sin(M_LN2 / 2.0 * bw * omega / sn);

        const float a0r = 1.0 / (1.0 + alpha);
#if 0
b0 = (1 - cs) /2;
b1 = 1 - cs;
b2 = (1 - cs) /2;
a0 = 1 + alpha;
a1 = -2 * cs;
a2 = 1 - alpha;
#endif
        f->b0 = a0r * (1.0 - cs) * 0.5;
	f->b1 = a0r * (1.0 - cs);
        f->b2 = a0r * (1.0 - cs) * 0.5;
        f->a1 = a0r * (2.0 * cs);
        f->a2 = a0r * (alpha - 1.0);
}


static inline
void
hp_set_params(biquad *f, bq_t fc, bq_t bw, bq_t fs)
{
	bq_t omega = 2.0 * M_PI * fc/fs;
	bq_t sn = sin(omega);
	bq_t cs = cos(omega);
	bq_t alpha = sn * sin(M_LN2 / 2.0 * bw * omega / sn);

        const float a0r = 1.0 / (1.0 + alpha);

#if 0
b0 = (1 + cs) /2;
b1 = -(1 + cs);
b2 = (1 + cs) /2;
a0 = 1 + alpha;
a1 = -2 * cs;
a2 = 1 - alpha;
#endif
        f->b0 = a0r * (1.0 + cs) * 0.5;
        f->b1 = a0r * -(1.0 + cs);
        f->b2 = a0r * (1.0 + cs) * 0.5;
        f->a1 = a0r * (2.0 * cs);
        f->a2 = a0r * (alpha - 1.0);
}


static inline
void
ls_set_params(biquad *f, bq_t fc, bq_t gain, bq_t slope, bq_t fs)
{

	bq_t w = 2.0f * M_PI * LIMIT(fc, 1.0, fs/2.0) / fs;
	bq_t cw = cos(w);
	bq_t sw = sin(w);
	bq_t A = pow(10.0f, gain * 0.025f);
	bq_t b = sqrt(((1.0f + A * A) / LIMIT(slope, 0.0001f, 1.0f)) - ((A -
					1.0f) * (A - 1.0)));
	bq_t apc = cw * (A + 1.0f);
	bq_t amc = cw * (A - 1.0f);
	bq_t bs = b * sw;
	bq_t a0r = 1.0f / (A + 1.0f + amc + bs);

	f->b0 = a0r * A * (A + 1.0f - amc + bs);
	f->b1 = a0r * 2.0f * A * (A - 1.0f - apc);
	f->b2 = a0r * A * (A + 1.0f - amc - bs);
	f->a1 = a0r * 2.0f * (A - 1.0f + apc);
	f->a2 = a0r * (-A - 1.0f - amc + bs);
}


static inline
void
hs_set_params(biquad *f, bq_t fc, bq_t gain, bq_t slope, bq_t fs) {

	bq_t w = 2.0f * M_PI * LIMIT(fc, 1.0, fs/2.0) / fs;
	bq_t cw = cos(w);
	bq_t sw = sin(w);
	bq_t A = pow(10.0f, gain * 0.025f);
	bq_t b = sqrt(((1.0f + A * A) / LIMIT(slope, 0.0001f, 1.0f)) - ((A -
					1.0f) * (A - 1.0f)));
	bq_t apc = cw * (A + 1.0f);
	bq_t amc = cw * (A - 1.0f);
	bq_t bs = b * sw;
	bq_t a0r = 1.0f / (A + 1.0f - amc + bs);

	f->b0 = a0r * A * (A + 1.0f + amc + bs);
	f->b1 = a0r * -2.0f * A * (A - 1.0f + apc);
	f->b2 = a0r * A * (A + 1.0f + amc - bs);
	f->a1 = a0r * -2.0f * (A - 1.0f - apc);
	f->a2 = a0r * (-A - 1.0f + amc + bs);
}


static inline
bq_t
biquad_run(biquad *f, bq_t x) {

	bq_t y;

	y = f->b0 * x + f->b1 * f->x1 + f->b2 * f->x2
		      + f->a1 * f->y1 + f->a2 * f->y2;
	y = FLUSH_TO_ZERO(y);
	f->x2 = f->x1;
	f->x1 = x;
	f->y2 = f->y1;
	f->y1 = y;

	return y;
}

#endif
