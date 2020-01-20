int dmesg_main(int argc, char **argv)
{
	char *buf;
	//....
	buf = xmalloc(bufsize);
	if ((n = klogctl(cmd, buf, bufsize)) < 0)
		goto die_the_death;

	lastc = '\n';
	for (i = 0; i < n; i++) {
		if (lastc == '\n' && buf[i] == '<') {
			i++;
			while (buf[i] >= '0' && buf[i] <= '9')
				i++;
			if (buf[i] == '>')
				i++;
		}
		lastc = buf[i];
		putchar(lastc);
	}
	if (lastc != '\n')
		putchar('\n');
all_done:
#ifdef CONFIG_FEATURE_CLEAN_UP	//if !CONFIG_FEATURE_CLEAN_UP --> memory leak
	if (buf) {
		free(buf);
	}
#endif
	return EXIT_SUCCESS;
die_the_death:
	bb_perror_nomsg_and_die();
}