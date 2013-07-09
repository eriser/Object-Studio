#include "cpitchtrackerclass.h"

// FFT Functions
void rdft(int n, int isgn, double *a)
{
    void bitrv2(int n, double *a);
    void cftfsub(int n, double *a);
    void cftbsub(int n, double *a);
    void rftfsub(int n, double *a);
    void rftbsub(int n, double *a);
    double xi;

    if (isgn >= 0) {
        if (n > 4) {
            bitrv2(n, a);
            cftfsub(n, a);
            rftfsub(n, a);
        } else if (n == 4) {
            cftfsub(n, a);
        }
        xi = a[0] - a[1];
        a[0] += a[1];
        a[1] = xi;
    } else {
        a[1] = 0.5 * (a[0] - a[1]);
        a[0] -= a[1];
        if (n > 4) {
            rftbsub(n, a);
            bitrv2(n, a);
            cftbsub(n, a);
        } else if (n == 4) {
            cftfsub(n, a);
        }
    }
}

#ifndef M_PI_2
#define M_PI_2      1.570796326794896619231321691639751442098584699687
#endif
#ifndef WR5000  /* cos(M_PI_2*0.5000) */
#define WR5000      0.707106781186547524400844362104849039284835937688
#endif
#ifndef WR2500  /* cos(M_PI_2*0.2500) */
#define WR2500      0.923879532511286756128183189396788286822416625863
#endif
#ifndef WI2500  /* sin(M_PI_2*0.2500) */
#define WI2500      0.382683432365089771728459984030398866761344562485
#endif


#ifndef RDFT_LOOP_DIV  /* control of the RDFT's speed & tolerance */
#define RDFT_LOOP_DIV 64
#endif

#ifndef DCST_LOOP_DIV  /* control of the DCT,DST's speed & tolerance */
#define DCST_LOOP_DIV 64
#endif


void bitrv2(int n, double *a)
{
    int j0, k0, j1, k1, l, m, i, j, k;
    double xr, xi, yr, yi;

    l = n >> 2;
    m = 2;
    while (m < l) {
        l >>= 1;
        m <<= 1;
    }
    if (m == l) {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 2) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + 2 * m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += 2 * m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1){};
            }
            j1 = j0 + k0 + m;
            k1 = j1 + m;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            for (i = n >> 1; i > (j0 ^= i); i >>= 1){};
        }
    } else {
        j0 = 0;
        for (k0 = 2; k0 < m; k0 += 2) {
            for (i = n >> 1; i > (j0 ^= i); i >>= 1){};
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1){};
            }
        }
    }
}

void cftfsub(int n, double *a)
{
    void cft1st(int n, double *a);
    void cftmdl(int n, int l, double *a);
    int j, j1, j2, j3, l;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    l = 2;
    if (n > 8) {
        cft1st(n, a);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i - x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i + x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i - x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = a[j + 1] - a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] += a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}


void cftbsub(int n, double *a)
{
    void cft1st(int n, double *a);
    void cftmdl(int n, int l, double *a);
    int j, j1, j2, j3, l;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    l = 2;
    if (n > 8) {
        cft1st(n, a);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = -a[j + 1] - a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = -a[j + 1] + a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i - x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i + x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i - x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i + x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = -a[j + 1] + a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] = -a[j + 1] - a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}


void cft1st(int n, double *a)
{
    int j, kj, kr;
    double ew, wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    x0r = a[0] + a[2];
    x0i = a[1] + a[3];
    x1r = a[0] - a[2];
    x1i = a[1] - a[3];
    x2r = a[4] + a[6];
    x2i = a[5] + a[7];
    x3r = a[4] - a[6];
    x3i = a[5] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
    wn4r = WR5000;
    x0r = a[8] + a[10];
    x0i = a[9] + a[11];
    x1r = a[8] - a[10];
    x1i = a[9] - a[11];
    x2r = a[12] + a[14];
    x2i = a[13] + a[15];
    x3r = a[12] - a[14];
    x3i = a[13] - a[15];
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[12] = x2i - x0i;
    a[13] = x0r - x2r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[10] = wn4r * (x0r - x0i);
    a[11] = wn4r * (x0r + x0i);
    x0r = x3i + x1r;
    x0i = x3r - x1i;
    a[14] = wn4r * (x0i - x0r);
    a[15] = wn4r * (x0i + x0r);
    ew = M_PI_2 / n;
    kr = 0;
    for (j = 16; j < n; j += 16) {
        for (kj = n >> 2; kj > (kr ^= kj); kj >>= 1){};
        wk1r = cos(ew * kr);
        wk1i = sin(ew * kr);
        wk2r = 1 - 2 * wk1i * wk1i;
        wk2i = 2 * wk1i * wk1r;
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        x0r = a[j] + a[j + 2];
        x0i = a[j + 1] + a[j + 3];
        x1r = a[j] - a[j + 2];
        x1i = a[j + 1] - a[j + 3];
        x2r = a[j + 4] + a[j + 6];
        x2i = a[j + 5] + a[j + 7];
        x3r = a[j + 4] - a[j + 6];
        x3i = a[j + 5] - a[j + 7];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 4] = wk2r * x0r - wk2i * x0i;
        a[j + 5] = wk2r * x0i + wk2i * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 2] = wk1r * x0r - wk1i * x0i;
        a[j + 3] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 6] = wk3r * x0r - wk3i * x0i;
        a[j + 7] = wk3r * x0i + wk3i * x0r;
        x0r = wn4r * (wk1r - wk1i);
        wk1i = wn4r * (wk1r + wk1i);
        wk1r = x0r;
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        x0r = a[j + 8] + a[j + 10];
        x0i = a[j + 9] + a[j + 11];
        x1r = a[j + 8] - a[j + 10];
        x1i = a[j + 9] - a[j + 11];
        x2r = a[j + 12] + a[j + 14];
        x2i = a[j + 13] + a[j + 15];
        x3r = a[j + 12] - a[j + 14];
        x3i = a[j + 13] - a[j + 15];
        a[j + 8] = x0r + x2r;
        a[j + 9] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 12] = -wk2i * x0r - wk2r * x0i;
        a[j + 13] = -wk2i * x0i + wk2r * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 10] = wk1r * x0r - wk1i * x0i;
        a[j + 11] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 14] = wk3r * x0r - wk3i * x0i;
        a[j + 15] = wk3r * x0i + wk3i * x0r;
    }
}


void cftmdl(int n, int l, double *a)
{
    int j, j1, j2, j3, k, kj, kr, m, m2;
    double ew, wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    m = l << 2;
    for (j = 0; j < l; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x0r - x2r;
        a[j2 + 1] = x0i - x2i;
        a[j1] = x1r - x3i;
        a[j1 + 1] = x1i + x3r;
        a[j3] = x1r + x3i;
        a[j3 + 1] = x1i - x3r;
    }
    wn4r = WR5000;
    for (j = m; j < l + m; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x2i - x0i;
        a[j2 + 1] = x0r - x2r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j1] = wn4r * (x0r - x0i);
        a[j1 + 1] = wn4r * (x0r + x0i);
        x0r = x3i + x1r;
        x0i = x3r - x1i;
        a[j3] = wn4r * (x0i - x0r);
        a[j3 + 1] = wn4r * (x0i + x0r);
    }
    ew = M_PI_2 / n;
    kr = 0;
    m2 = 2 * m;
    for (k = m2; k < n; k += m2) {
        for (kj = n >> 2; kj > (kr ^= kj); kj >>= 1){};
        wk1r = cos(ew * kr);
        wk1i = sin(ew * kr);
        wk2r = 1 - 2 * wk1i * wk1i;
        wk2i = 2 * wk1i * wk1r;
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        for (j = k; j < l + k; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = wk2r * x0r - wk2i * x0i;
            a[j2 + 1] = wk2r * x0i + wk2i * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
        x0r = wn4r * (wk1r - wk1i);
        wk1i = wn4r * (wk1r + wk1i);
        wk1r = x0r;
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        for (j = k + m; j < l + (k + m); j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = -wk2i * x0r - wk2r * x0i;
            a[j2 + 1] = -wk2i * x0i + wk2r * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
    }
}


void rftfsub(int n, double *a)
{
    int i, i0, j, k;
    double ec, w1r, w1i, wkr, wki, wdr, wdi, ss, xr, xi, yr, yi;

    ec = 2 * M_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = cos(ec);
    wdr = sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 4) {
            i0 = 4;
        }
        for (j = i - 4; j >= i0; j -= 4) {
            k = n - j;
            xr = a[j + 2] - a[k - 2];
            xi = a[j + 3] + a[k - 1];
            yr = wdr * xr - wdi * xi;
            yi = wdr * xi + wdi * xr;
            a[j + 2] -= yr;
            a[j + 3] -= yi;
            a[k - 2] += yr;
            a[k - 1] -= yi;
            wkr += ss * wdi;
            wki += ss * (0.5 - wdr);
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr - wki * xi;
            yi = wkr * xi + wki * xr;
            a[j] -= yr;
            a[j + 1] -= yi;
            a[k] += yr;
            a[k + 1] -= yi;
            wdr += ss * wki;
            wdi += ss * (0.5 - wkr);
        }
        if (i0 == 4) {
            break;
        }
        wkr = 0.5 * sin(ec * i0);
        wki = 0.5 * cos(ec * i0);
        wdr = 0.5 - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = 0.5 - wkr;
        i = i0;
    }
    xr = a[2] - a[n - 2];
    xi = a[3] + a[n - 1];
    yr = wdr * xr - wdi * xi;
    yi = wdr * xi + wdi * xr;
    a[2] -= yr;
    a[3] -= yi;
    a[n - 2] += yr;
    a[n - 1] -= yi;
}


void rftbsub(int n, double *a)
{
    int i, i0, j, k;
    double ec, w1r, w1i, wkr, wki, wdr, wdi, ss, xr, xi, yr, yi;

    ec = 2 * M_PI_2 / n;
    wkr = 0;
    wki = 0;
    wdi = cos(ec);
    wdr = sin(ec);
    wdi *= wdr;
    wdr *= wdr;
    w1r = 1 - 2 * wdr;
    w1i = 2 * wdi;
    ss = 2 * w1i;
    i = n >> 1;
    a[i + 1] = -a[i + 1];
    for (;;) {
        i0 = i - 4 * RDFT_LOOP_DIV;
        if (i0 < 4) {
            i0 = 4;
        }
        for (j = i - 4; j >= i0; j -= 4) {
            k = n - j;
            xr = a[j + 2] - a[k - 2];
            xi = a[j + 3] + a[k - 1];
            yr = wdr * xr + wdi * xi;
            yi = wdr * xi - wdi * xr;
            a[j + 2] -= yr;
            a[j + 3] = yi - a[j + 3];
            a[k - 2] += yr;
            a[k - 1] = yi - a[k - 1];
            wkr += ss * wdi;
            wki += ss * (0.5 - wdr);
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = wkr * xr + wki * xi;
            yi = wkr * xi - wki * xr;
            a[j] -= yr;
            a[j + 1] = yi - a[j + 1];
            a[k] += yr;
            a[k + 1] = yi - a[k + 1];
            wdr += ss * wki;
            wdi += ss * (0.5 - wkr);
        }
        if (i0 == 4) {
            break;
        }
        wkr = 0.5 * sin(ec * i0);
        wki = 0.5 * cos(ec * i0);
        wdr = 0.5 - (wkr * w1r - wki * w1i);
        wdi = wkr * w1i + wki * w1r;
        wkr = 0.5 - wkr;
        i = i0;
    }
    xr = a[2] - a[n - 2];
    xi = a[3] + a[n - 1];
    yr = wdr * xr + wdi * xi;
    yi = wdr * xi - wdi * xr;
    a[2] -= yr;
    a[3] = yi - a[3];
    a[n - 2] += yr;
    a[n - 1] = yi - a[n - 1];
    a[1] = -a[1];
}

// Average Energy Function
float Energy(double a[],int BufferSize)
{
        float sum=0;
        for(int i=1; i<BufferSize; ++i)
        {
                //sum += (a[i]-129) * (a[i]-129);
                sum+=fabs(a[i]);
        }

        return sum / (BufferSize/2);
}

CPitchTrackerClass::~CPitchTrackerClass()
    {
        delete[] coeffs;
        delete[] coeffs1;
        delete[] coeffs2;
        delete[] coeffs3;
        delete[] Product;
        //free(coeffs);
        //free(coeffs1);
        //free(coeffs2);
                //free(coeffs3);
        //free(Product);
    }

        CPitchTrackerClass::CPitchTrackerClass(int BufferSz,int SampleRate)
        {
        m_SampleRate=SampleRate;
        BufferSize=BufferSz;
        CurrentVel=0;
        CurrentDiff=0;
        CurrentNote=0;
        CurrentFreq=0;
        CurrentMIDIFreq=0;
        Threshold=0;
                InTune=440.0;
                OutTune=440.0;
        coeffs=new double[BufferSize];//(double*)calloc(BufferSize,sizeof(double));
        coeffs1=new double[BufferSize/2];//(double*)calloc(BufferSize/2,sizeof(double));
        coeffs2=new double[BufferSize/2];//(double*)calloc(BufferSize/2,sizeof(double));
        coeffs3=new double[BufferSize/2];//(double*)calloc(BufferSize/2,sizeof(double));
        Product=new double[BufferSize/2];//(double*)calloc(BufferSize/2,sizeof(double));
    }

    void CPitchTrackerClass::Process()
    {
        float freq;
        float freq1;
        float freq3;
        CurrentVel=0;
        CurrentNote=0;

        float Average=Energy(coeffs,BufferSize);
                if (Average<Threshold)
        {
            return;
                }
                CurrentVel=Average*127;
                if (CurrentVel>127)
                {
                        CurrentVel=127;
                }

        // Calculate FFT
        rdft(BufferSize, 1, coeffs);
        // Get Absolute value of spectrum
        for (int i=0; i<BufferSize/2; i++)
        {
                coeffs1[i] = fabs(coeffs[i]);
        }

         // Downsampling
        // Set initial array values
        for (int i=0; i<BufferSize/2; i++)
        {
                coeffs2[i] = 1;
                coeffs3[i] = 1;
        }
        for (int i=0; i<BufferSize/4; i++)
        {
                coeffs2[i] = (coeffs1[2*i]+coeffs1[(2*i)+1])/2;
                if (i<BufferSize/6)
                        coeffs3[i] = (coeffs1[3*i]+coeffs1[(3*i)+1]+coeffs1[(3*i)+2])/3;
        }

// Calculate Product
        for (int i=1; i<BufferSize/2; i++)
        {
                Product[i] = coeffs1[i]*coeffs2[i]*coeffs3[i];
        }

// Find frequency for 1 Harmonics
        // Find position of maximum peak
        double largest1 = 0;
        int position1 = 0;
        for(int i=10; i<BufferSize/2; ++i) // 20 corresponds to 55Hz. ie. don't search below 55Hz
        {
                if (coeffs1[i]>largest1)
                {
                        largest1 = coeffs1[i];
                        position1 = i;
                }
        }

        // Post processing for octave errors
        double largest1_lwr = 0;
        int position1_lwr = 0;
        for(int i = 10; i < position1*0.8; ++i)
        {
                if (coeffs1[i]>largest1_lwr)
                {
                        largest1_lwr = coeffs1[i];
                        position1_lwr = i;
                }
        }

        for(int i = (int)position1*1.2; i < BufferSize/2; ++i)
        {
                if (coeffs1[i]>largest1_lwr)
                {
                        largest1_lwr = coeffs1[i];
                        position1_lwr = i;
                }
        }

        float ratio1 = 0;
        if (coeffs1[position1]!=0)
        {
                ratio1=coeffs1[position1_lwr]/coeffs1[position1];
        }

        if (position1_lwr > position1 * 0.4 && position1_lwr < position1 * 0.6 && ratio1 > 0.1)
                position1 = position1_lwr;

        if(BufferSize != 0)
                freq1 = ((float)m_SampleRate/4)*(float)position1/(float)BufferSize;

        // Find corresponding note number

        int note_num1  = FreqtoMIDI(freq1,InTune);


// Find frequency for 3 Harmonics
        // Find position of maximum peak
        double largest3 = 0;
        int position3 = 0;
        for(int i=10; i<BufferSize/2; ++i) // 20 corresponds to 55Hz. ie. don't search below 55Hz
        {
                if (Product[i]>largest3)
                {
                        largest3 = Product[i];
                        position3 = i;
                }
        }

        // Post processing for octave errors
        double largest3_lwr = 0;
        int position3_lwr = 0;
        for(int i = 10; i < position3*0.8; ++i)
        {
                if (Product[i]>largest3_lwr)
                {
                        largest3_lwr = Product[i];
                        position3_lwr = i;
                }
        }

        for(int i = (int)position3*1.2; i < BufferSize/2; ++i)
        {
                if (Product[i]>largest3_lwr)
                {
                        largest3_lwr = Product[i];
                        position3_lwr = i;
                }
        }


        float ratio3 = 0;
        if (Product[position3]!=0)
        {
                ratio3=Product[position3_lwr]/Product[position3];
        }

        if (position3_lwr > position3 * 0.4 && position3_lwr < position3 * 0.6 && ratio3 > 0.3)
                position3 = position3_lwr;

        if(BufferSize != 0)
                freq3 = ((float)m_SampleRate/4)*(float)position3/(float)BufferSize;

        note_num3[4] = note_num3[3];
        note_num3[3] = note_num3[2];
        note_num3[2] = note_num3[1];
        note_num3[1] = note_num3[0];

        // Find corresponding note number
           note_num3[0]  = FreqtoMIDI(freq3,InTune);


// Decide between freq1 and freq3

        note_num[4] = note_num[3];
        note_num[3] = note_num[2];
        note_num[2] = note_num[1];
        note_num[1] = note_num[0];

        // If both estimations agree
        if (note_num1 == note_num3[0])
        {
                note_num[0] = note_num1;
                freq = freq1;
        }
        else
        {
/*		if (freq1 > 440 && note_num3[0] != (note_num3[1]+note_num3[2]+note_num3[3]+note_num3[4])/4)
                        note_num[0] = note_num1;
                else
                        note_num[0] = note_num3[0];
*/
                if (ratio1 < ratio3)
                {
                        note_num[0] = note_num1;
                        freq = freq1;
                }
                if (ratio1 > ratio3)
                {
                        note_num[0] = note_num3[0];
                        freq = freq3;
                }

        }
                if (largest1>1)
                {
                        float CurrentFrequency=MIDItoFreq(note_num[0],OutTune);
                        CurrentDiff=((float)(CurrentFrequency-freq)/CurrentFrequency);
        //int TuneVar=((float)fabs(FreqDiff*10000.0)/10000.0)+0.5;
//    if (FreqDiff < 0)
//    {
//        TuneVar=-TuneVar;
//    }
        //if (TuneVar>0)
        //{
                //TuneVar=TuneVar*2;
        //}

                        /*
                        float Upper=CurrentFrequency*2;
                        float Lower=CurrentFrequency/2;
                        float Diff=(CurrentFrequency-(float)freq);
                        CurrentDiff=0;
                        if (Diff>0)
                        {
                                CurrentDiff=(Diff*100000)/(Upper-CurrentFrequency);
                        }
            if (Diff<0)
            {
                CurrentDiff=(Diff*100000)/(CurrentFrequency-Lower);
            }
            */
                        CurrentFreq=freq*FreqResolution;
            CurrentNote=note_num[0];
            CurrentMIDIFreq=CurrentFrequency*FreqResolution;
        }
    }


