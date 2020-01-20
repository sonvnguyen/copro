#ifndef APACHE_SCOREBOARD_H
ap_generation_t volatile ap_my_generation;
#endif

static int status_handler(request_rec *r){
    //...
#ifdef HAVE_TIMES
#ifdef _SC_CLK_TCK
    float tick = sysconf(_SC_CLK_TCK);
#else
    float tick = HZ;
#endif
#endif
    for (i = 0; i < server_limit; ++i) {
        for (j = 0; j < thread_limit; ++j) {
            if (!ps_record.quiescing
                && ps_record.generation == ap_my_generation
                && ps_record.pid) {
					//...
            }
            if (ap_extended_status) {
                lres = ws_record.access_count;
                bytes = ws_record.bytes_served;

                if (lres != 0 || (res != SERVER_READY && res != SERVER_DEAD)) {
#ifdef HAVE_TIMES
                    //...
#endif /* HAVE_TIMES */
					//...
                }
            }
        }
        pid_buffer[i] = ps_record.pid;
    }
    if (!short_report) {
        ap_rprintf(r, "s", (int)ap_my_generation);
        ap_rputs("<dt>Server uptime: ", r);
        show_time(r, up_time);
        ap_rputs("</dt>\n", r);
    }
	//...
    return 0;
}