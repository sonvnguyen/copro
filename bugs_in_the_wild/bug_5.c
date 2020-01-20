//cursor.c (xterm)

/*
 * moves the cursor forward n, no wraparound
 */
void
CursorForward(XtermWidget xw, int n){
    TScreen *screen = TScreenOf(xw);
#if OPT_DEC_CHRSET
    LineData *ld = getLineData(screen, screen->cur_row);
#endif
    int next = screen->cur_col + n;
    int max;

    if (IsLeftRightMode(xw)) {
		max = screen->rgt_marg;
		if (screen->cur_col > max)
	   		max = screen->max_col;
    } else {
		max = LineMaxCol(screen, ld);
    }
    if (next > max)
		next = max;
    set_cur_col(screen, next);
    ResetWrap(screen);
}