#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

enum {
	Dwidth = 3,
	Dheight = 5
};

char Dfont[10][Dwidth*Dheight] = {
	{ 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1 }, /* 0 */
	{ 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 }, /* 1 */
	{ 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1 }, /* 2 */
	{ 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 }, /* 3 */
	{ 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1 }, /* 4 */
	{ 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1 }, /* 5 */
	{ 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1 }, /* 6 */
	{ 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 }, /* 7 */
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 }, /* 8 */
	{ 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1 }  /* 9 */
};

char* menustr[] = {
	"exit",
	0
};

Menu menu = {
	menustr
};

int pixelsize = 10;
int spacing = 10;
Image *bg;
Image *fg;
Point origin;

void
drawpixel(int x0, int y0, int x, int y)
{
	Point p[4], o;

	o = addpt(Pt(x0, y0), origin);
	p[0] = addpt(o, mulpt(Pt(x,   y),   pixelsize));
	p[1] = addpt(o, mulpt(Pt(x+1, y),   pixelsize));
	p[2] = addpt(o, mulpt(Pt(x+1, y+1), pixelsize));
	p[3] = addpt(o, mulpt(Pt(x  , y+1), pixelsize));
	fillpoly(screen, p, 4, 1, fg, ZP);
}

void
drawdigit(int digit, int x0, int y0)
{
	int i;

	for(i = 0; i < Dwidth*Dheight; i++){
		if(Dfont[digit][i])
			drawpixel(x0, y0, i%Dwidth, i/Dwidth);
	}
}

void
drawcolon(int x0)
{
	drawpixel(x0, spacing, 0, 1);
	drawpixel(x0, spacing, 0, 3);
}

int
nextcharpos(int x0, int width)
{
	return x0 + width*pixelsize + spacing;
}

void
showtime(void)
{
	Tm* tm;
	int x;

	tm = localtime(time(0));
	draw(screen, screen->r, bg, nil, ZP);
	x = nextcharpos(0, 0);
	drawdigit(tm->hour/10, x, spacing);
	x = nextcharpos(x, Dwidth);
	drawdigit(tm->hour%10, x, spacing);
	x = nextcharpos(x, Dwidth);
	drawcolon(x);
	x = nextcharpos(x, 1);
	drawdigit(tm->min/10, x, spacing);
	x = nextcharpos(x, Dwidth);
	drawdigit(tm->min%10, x, spacing);
}

void
eresized(int new)
{
	int w, h;

	if(new && getwindow(display, Refnone) < 0)
		sysfatal("cannot reattach to window: %r");
	w = (4*Dwidth+1)*pixelsize+6*spacing;
	h = Dheight*pixelsize+2*spacing;
	origin = screen->r.min;
	origin.x += (Dx(screen->r)-w)/2;
	origin.y += (Dy(screen->r)-h)/2;
	showtime();
}

void
usage(void)
{
	fprint(2, "Usage: %s [-sspacing] [-ppixelsize]\n", argv0);
	exits("usage");
}

void
main(int argc, char** argv)
{
	Event ev;
	int e, timer;

	ARGBEGIN {
	case 's':
		spacing = atoi(EARGF(usage()));
		break;
	case 'p':
		pixelsize = atoi(EARGF(usage()));
		break;
	default:
		usage();
	} ARGEND
	if(initdraw(nil, nil, "clock") < 0)
		sysfatal("initdraw: %r");
	fg = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, 0x55AAAAFF);
	bg = allocimagemix(display, DPalebluegreen, DWhite);
	einit(Emouse);
	timer = etimer(0, 15*1000);
	eresized(0);
	for(;;){
		e = event(&ev);
		if(e == Emouse){
			if((ev.mouse.buttons == 4)
			&& (emenuhit(3, &ev.mouse, &menu) == 0))
				exits(nil);
		}else if(e == timer){
			showtime();
		}
	}
}
