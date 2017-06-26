//getline needed #define _GNU_SOURCE
#define _GNU_SOURCE

#include "tuxwetter.h"
#include "gfx.h"

typedef struct { unsigned char width_lo; unsigned char width_hi; unsigned char height_lo; unsigned char height_hi; 	unsigned char transp; } IconHeader;

#ifdef MARTII
char circle[] =
{
	0,2,2,2,2,2,2,2,2,2,2,0,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	2,1,1,1,1,1,1,1,1,1,1,2,
	0,2,2,2,2,2,2,2,2,2,2,0
};
#else
char circle[] =
{
	0,0,0,0,0,1,1,0,0,0,0,0,
	0,0,0,1,1,1,1,1,1,0,0,0,
	0,0,1,1,1,1,1,1,1,1,0,0,
	0,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,0,
	0,1,1,1,1,1,1,1,1,1,1,0,
	0,0,1,1,1,1,1,1,1,1,0,0,
	0,0,0,1,1,1,1,1,1,0,0,0,
	0,0,0,0,0,1,1,0,0,0,0,0
};
#endif

void Center_Screen(int wx, int wy, int *csx, int *csy)
{
	*csx = ((ex-sx) - wx)/2;
	*csy = ((ey-sy) - wy)/2;
}
#ifdef MARTII
#if defined(HAVE_SPARK_HARDWARE) || defined(HAVE_DUCKBOX_HARDWARE)
void FillRect(int _sx, int _sy, int _dx, int _dy, uint32_t color)
{
	uint32_t *p1, *p2, *p3, *p4;
	sync_blitter = 1;

	STMFBIO_BLT_DATA bltData;
	memset(&bltData, 0, sizeof(STMFBIO_BLT_DATA));

	bltData.operation  = BLT_OP_FILL;
	bltData.dstOffset  = 1920 * 1080 * 4;
	bltData.dstPitch   = DEFAULT_XRES * 4;

	bltData.dst_left   = _sx;
	bltData.dst_top    = _sy;
	bltData.dst_right  = _sx + _dx - 1;
	bltData.dst_bottom = _sy + _dy - 1;

	bltData.dstFormat  = SURF_ARGB8888;
	bltData.srcFormat  = SURF_ARGB8888;
	bltData.dstMemBase = STMFBGP_FRAMEBUFFER;
	bltData.srcMemBase = STMFBGP_FRAMEBUFFER;
	bltData.colour     = color;

	if (ioctl(fb, STMFBIO_BLT, &bltData ) < 0)
		perror("RenderBox ioctl STMFBIO_BLT");
#if 0
	if(ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
		perror("blit ioctl STMFBIO_SYNC_BLITTER 2");
#else
	sync_blitter = 1;
#endif
}
# endif
#endif

/******************************************************************************
 * RenderBox
 ******************************************************************************/
void RenderBox(int rsx, int rsy, int rex, int rey, int rad, int col)
{
	int F,R=rad,ssx=sx+rsx,ssy=sy+rsy,dxx=rex,dyy=rey,rx,ry,wx,wy,count;

#ifdef MARTII
	uint32_t *pos = lbb + ssx + stride * ssy;
	uint32_t *pos0, *pos1, *pos2, *pos3, *i;
	uint32_t pix = bgra[col];
#else
	unsigned char *pos=(lbb+(ssx<<2)+fix_screeninfo.line_length*ssy);
	unsigned char *pos0, *pos1, *pos2, *pos3, *i;
	unsigned char pix[4]={bl[col],gn[col],rd[col],tr[col]};
#endif

	if (dxx<0)
	{
		printf("[shellexec] RenderBox called with dx < 0 (%d)\n", dxx);
		dxx=0;
	}

	if(R)
	{
#if defined(MARTII) && defined(HAVE_SPARK_HARDWARE) || defined(HAVE_DUCKBOX_HARDWARE)
		if(sync_blitter) {
			sync_blitter = 0;
			if (ioctl(fb, STMFBIO_SYNC_BLITTER) < 0)
				perror("RenderString ioctl STMFBIO_SYNC_BLITTER");
		}
#endif
		if(--dyy<=0)
		{
			dyy=1;
		}

		if(R==1 || R>(dxx/2) || R>(dyy/2))
		{
			R=dxx/10;
			F=dyy/10;
			if(R>F)
			{
				if(R>(dyy/3))
				{
					R=dyy/3;
				}
			}
			else
			{
				R=F;
				if(R>(dxx/3))
				{
					R=dxx/3;
				}
			}
		}
		ssx=0;
		ssy=R;
		F=1-R;

		rx=R-ssx;
		ry=R-ssy;

#ifdef MARTII
		pos0=pos+(dyy-ry)*stride;
		pos1=pos+ry*stride;
		pos2=pos+rx*stride;
		pos3=pos+(dyy-rx)*stride;
#else
		pos0=pos+((dyy-ry)*fix_screeninfo.line_length);
		pos1=pos+(ry*fix_screeninfo.line_length);
		pos2=pos+(rx*fix_screeninfo.line_length);
		pos3=pos+((dyy-rx)*fix_screeninfo.line_length);
#endif
		while (ssx <= ssy)
		{
			rx=R-ssx;
			ry=R-ssy;
			wx=rx<<1;
			wy=ry<<1;

#ifdef MARTII
			for(i=pos0+rx; i<pos0+rx+dxx-wx;i++)
				*i = pix;
			for(i=pos1+rx; i<pos1+rx+dxx-wx;i++)
				*i = pix;
			for(i=pos2+ry; i<pos2+ry+dxx-wy;i++)
				*i = pix;
			for(i=pos3+ry; i<pos3+ry+dxx-wy;i++)
				*i = pix;
#else
			for(i=pos0+(rx<<2); i<pos0+((rx+dxx-wx)<<2);i+=4)
				memcpy(i, pix, 4);
			for(i=pos1+(rx<<2); i<pos1+((rx+dxx-wx)<<2);i+=4)
				memcpy(i, pix, 4);
			for(i=pos2+(ry<<2); i<pos2+((ry+dxx-wy)<<2);i+=4)
				memcpy(i, pix, 4);
			for(i=pos3+(ry<<2); i<pos3+((ry+dxx-wy)<<2);i+=4)
				memcpy(i, pix, 4);
#endif

			ssx++;
#ifdef MARTII
			pos2-=stride;
			pos3+=stride;
#else
			pos2-=fix_screeninfo.line_length;
			pos3+=fix_screeninfo.line_length;
#endif
			if (F<0)
			{
				F+=(ssx<<1)-1;
			}
			else
			{
				F+=((ssx-ssy)<<1);
				ssy--;
#ifdef MARTII
				pos0-=stride;
				pos1+=stride;
#else
				pos0-=fix_screeninfo.line_length;
				pos1+=fix_screeninfo.line_length;
#endif
			}
		}
#ifdef MARTII
		pos+=R*stride;
#else
		pos+=R*fix_screeninfo.line_length;
#endif
	}

#if defined(MARTII) && defined(HAVE_SPARK_HARDWARE) || defined(HAVE_DUCKBOX_HARDWARE)
	FillRect(sx + rsx, sy + rsy + R, dxx + 1, dyy - 2 * R + 1, pix);
#else
	for (count=R; count<(dyy-R); count++)
	{
#ifdef MARTII
		for(i=pos; i<pos+dxx;i++)
			*i = pix;
		pos+=stride;
#else
		for(i=pos; i<pos+(dxx<<2);i+=4)
			memcpy(i, pix, 4);
		pos+=fix_screeninfo.line_length;
#endif
	}
#endif
}

/******************************************************************************
 * RenderCircle
 ******************************************************************************/

#ifdef MARTII
void RenderCircle(int sx, int sy, int col)
{
	int x, y;
	uint32_t pix = bgra[col];
	uint32_t *p = lbb + startx + sx;
	int s = stride * (starty + sy + y);

	for(y = 0; y < 12 * 12; y += 12, s += stride)
		for(x = 0; x < 12; x++)
			switch(circle[x + y]) {
				case 1: *(p + x + s) = pix; break;
				case 2: *(p + x + s) = 0xFFFFFFFF; break;
			}
}
#else
void RenderCircle(int sx, int sy, int col)
{
	int x, y;
#ifdef MARTII
	uint32_t pix = bgra[col];
#else
	unsigned char pix[4]={bl[col],gn[col],rd[col],tr[col]};
#endif
	//render

		for(y = 0; y < 12; y++)
		{
#ifdef MARTII
			for(x = 0; x < 12; x++) if(circle[x + y*12])
				*(lbb + startx + sx + x + stride*(starty + sy + y)) = pix;
#else
			for(x = 0; x < 12; x++) if(circle[x + y*12]) memcpy(lbb + (startx + sx + x)*4 + fix_screeninfo.line_length*(starty + sy + y), pix, 4);
#endif
		}
}
#endif

#if 0
/******************************************************************************
 * PaintIcon
 ******************************************************************************/
void PaintIcon(char *filename, int x, int y, unsigned char offset)
{
	IconHeader iheader;
	unsigned int  width, height,count,count2;
	unsigned char pixbuf[768],*pixpos,compressed,pix1,pix2;
	unsigned char * d = (lbb+(startx+x)+var_screeninfo.xres*(starty+y));
	unsigned char * d2;
	int fd;

	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		printf("shellexec <unable to load icon: %s>\n", filename);
		return;
	}

	read(fd, &iheader, sizeof(IconHeader));

	width  = (iheader.width_hi  << 8) | iheader.width_lo;
	height = (iheader.height_hi << 8) | iheader.height_lo;


	for (count=0; count<height; count ++ )
	{
		read(fd, &pixbuf, width >> 1 );
		pixpos = (unsigned char*) &pixbuf;
		d2 = d;
		for (count2=0; count2<width >> 1; count2 ++ )
		{
			compressed = *pixpos;
			pix1 = (compressed & 0xf0) >> 4;
			pix2 = (compressed & 0x0f);

			if (pix1 != iheader.transp)
			{
				*d2=pix1 + offset;
			}
			d2++;
			if (pix2 != iheader.transp)
			{
				*d2=pix2 + offset;
			}
			d2++;
			pixpos++;
		}
		d += var_screeninfo.xres;
	}
	close(fd);
	return;
}
#endif

/******************************************************************************
 * RenderLine
 ******************************************************************************/

void RenderLine( int xa, int ya, int xb, int yb, unsigned char col )
{
	int dx;
	int	dy;
	int	x;
	int	y;
	int	End;
	int	step;
#ifdef MARTII
	uint32_t pix = bgra[col];
# ifdef HAVE_SPARK_HARDWARE_NOTYET // the blitter doesn't seem to like drawing lines. wtf...
	if (xa == xb || ya == yb) {
		FillRect(xa, ya, xb - xa + 1, yb - ya + 1, pix);
		return;
	}
# endif
#else
	unsigned char pix[4]={bl[col],gn[col],rd[col],tr[col]};
#endif

	dx = abs (xa - xb);
	dy = abs (ya - yb);
	
	if ( dx > dy )
	{
		int	p = 2 * dy - dx;
		int	twoDy = 2 * dy;
		int	twoDyDx = 2 * (dy-dx);

		if ( xa > xb )
		{
			x = xb;
			y = yb;
			End = xa;
			step = ya < yb ? -1 : 1;
		}
		else
		{
			x = xa;
			y = ya;
			End = xb;
			step = yb < ya ? -1 : 1;
		}

#ifdef MARTII
		*(lbb + startx+x + stride*(y+starty)) = pix;
#else
		memcpy(lbb + (startx+x)*4 + fix_screeninfo.line_length*(y+starty), pix, sizeof(pix));
#endif

		while( x < End )
		{
			x++;
			if ( p < 0 )
				p += twoDy;
			else
			{
				y += step;
				p += twoDyDx;
			}
#ifdef MARTII
			*(lbb + startx+x + stride*(y+starty)) = pix;
#else
			memcpy(lbb + (startx+x)*4 + fix_screeninfo.line_length*(y+starty), pix, sizeof(pix));
#endif
		}
	}
	else
	{
		int	p = 2 * dx - dy;
		int	twoDx = 2 * dx;
		int	twoDxDy = 2 * (dx-dy);

		if ( ya > yb )
		{
			x = xb;
			y = yb;
			End = ya;
			step = xa < xb ? -1 : 1;
		}
		else
		{
			x = xa;
			y = ya;
			End = yb;
			step = xb < xa ? -1 : 1;
		}

#ifdef MARTII
		*(lbb + startx+x + stride*(y+starty)) = pix;
#else
		memcpy(lbb + (startx+x)*4 + fix_screeninfo.line_length*(y+starty), pix, sizeof(pix));
#endif

		while( y < End )
		{
			y++;
			if ( p < 0 )
				p += twoDx;
			else
			{
				x += step;
				p += twoDxDy;
			}
#ifdef MARTII
			*(lbb + startx+x + stride*(y+starty)) = pix;
#else
			memcpy(lbb + (startx+x)*4 + fix_screeninfo.line_length*(y+starty), pix, sizeof(pix));
#endif
		}
	}
}


