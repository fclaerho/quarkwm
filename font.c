// Copyright (c) 2007 FC - Licenced under the GPLv3.

static XFontStruct* gpMediumFont=0;
static XFontStruct* gpSmallFont=0;
static XFontStruct* gpBigFont=0;

I loadMediumFont(void) {
	gpMediumFont=XLoadQueryFont(gpDisplay,MEDIUMFONT);
	if(!gpMediumFont) log("Cannot load medium font (" MEDIUMFONT ")");
	return gpMediumFont!=0;
}

I loadSmallFont(void) {
	gpSmallFont=XLoadQueryFont(gpDisplay,SMALLFONT);
	if(!gpSmallFont) log("Cannot load small font (" SMALLFONT ")");
	return gpSmallFont!=0;
}

I loadBigFont(void) {
	gpBigFont=XLoadQueryFont(gpDisplay,BIGFONT);
	if(!gpBigFont) log("Cannot load big font (" BIGFONT ")");
	return gpBigFont!=0;
}

I loadFonts(void) {
	return loadMediumFont() && loadSmallFont() && loadBigFont();
}

I mediumFontHeight(void) {
	return gpMediumFont?gpMediumFont->ascent+gpMediumFont->descent:0;
}

I mediumTextWidth(S s,I length) {
	return gpMediumFont?XTextWidth(gpMediumFont,s,length):0;
}

void drawMediumText(S s,I length,XID wid,GC gc,I x,I y) {
	if(!gpMediumFont) return;
	XSetFont(gpDisplay,gc,gpMediumFont->fid);
	XDrawString(gpDisplay,wid,gc,x,y+gpMediumFont->ascent,s,length);
}

I smallFontHeight(void) {
	return gpSmallFont?gpSmallFont->ascent+gpSmallFont->descent:0;
}

I smallTextWidth(S s,I length) {
	return gpSmallFont?XTextWidth(gpSmallFont,s,length):0;
}

void drawSmallText(S s,I length,XID wid,GC gc,I x,I y) {
	if(!gpSmallFont) return;
	XSetFont(gpDisplay,gc,gpSmallFont->fid);
	XDrawString(gpDisplay,wid,gc,x,y+gpSmallFont->ascent,s,length);
}

I bigFontHeight(void) {
	return gpBigFont?gpBigFont->ascent+gpBigFont->descent:0;
}

I bigTextWidth(S s,I length) {
	return gpBigFont?XTextWidth(gpBigFont,s,length):0;
}

void drawBigText(S s,I length,XID wid,GC gc,I x,I y) {
	if(!gpBigFont) return;
	XSetFont(gpDisplay,gc,gpBigFont->fid);
	XDrawString(gpDisplay,wid,gc,x,y+gpBigFont->ascent,s,length);
}
