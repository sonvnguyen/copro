#ifndef NAJOR
#define MAJOR(dev) (((dev)>>8)&0xff)
#define MINOR(dev) ((dev)&0xff)
#endif
#ifdef BB_FEATURE_LS_SORTFILES
/* how will the files be sorted */
#define SORT_FORWARD    0		/* sort in reverse order */
#define SORT_REVERSE    1		/* sort in reverse order */
#define SORT_NAME		2		/* sort by file name */
#define SORT_SIZE		3		/* sort by file size */
#define SORT_ATIME		4		/* sort by last access time */
#define SORT_CTIME		5		/* sort by last change time */
#define SORT_MTIME		6		/* sort by last modification time */
#define SORT_VERSION	7		/* sort by version */
#define SORT_EXT		8		/* sort by file name extension */
#define SORT_DIR		9		/* sort by file or directory */
static unsigned int sort_opts=	SORT_FORWARD;
static unsigned int sort_order=	SORT_FORWARD;
#endif


int list_single(struct dnode *dn)
{
	int i, len;
	char scratch[BUFSIZ + 1];
#ifdef BB_FEATURE_LS_TIMESTAMPS
	char *filetime;
	time_t ttime, age;
#endif
#ifdef BB_FEATURE_LS_FILETYPES
	struct stat info;
	char append;
#endif

	if (dn==NULL || dn->fullname==NULL) return(0);

#ifdef BB_FEATURE_LS_TIMESTAMPS
	ttime= dn->dstat.st_mtime;      /* the default time */
	if (time_fmt & TIME_ACCESS) ttime= dn->dstat.st_atime;
	if (time_fmt & TIME_CHANGE) ttime= dn->dstat.st_ctime;
	filetime= ctime(&ttime);
#endif
#ifdef BB_FEATURE_LS_FILETYPES
	append = append_char(dn->dstat.st_mode);
#endif

	for (i=0; i<=31; i++) {
		switch (list_fmt & (1<<i)) {
			case LIST_INO:
				fprintf(stdout, "%7ld ", dn->dstat.st_ino);
				column += 8;
				break;
			case LIST_BLOCKS:
#if _FILE_OFFSET_BITS == 64
				fprintf(stdout, "%4lld ", dn->dstat.st_blocks>>1);
#else
				fprintf(stdout, "%4ld ", dn->dstat.st_blocks>>1);
#endif
				column += 5;
				break;
			case LIST_MODEBITS:
				fprintf(stdout, "%10s", (char *)modeString(dn->dstat.st_mode));
				column += 10;
				break;
			case LIST_NLINKS:
				fprintf(stdout, "%4d ", dn->dstat.st_nlink);
				column += 10;
				break;
			case LIST_ID_NAME:
#ifdef BB_FEATURE_LS_USERNAME
				{
					memset(&info, 0, sizeof(struct stat));		//if !BB_FEATURE_LS_FILETYPES --> error
					memset(scratch, 0, sizeof(scratch));
					if (!stat(dn->fullname, &info)) {
						my_getpwuid(scratch, info.st_uid);
					}
					if (*scratch) {
						fprintf(stdout, "%-8.8s ", scratch);
					} else {
						fprintf(stdout, "%-8d ", dn->dstat.st_uid);
					}
					memset(scratch, 0, sizeof(scratch));
					if (info.st_ctime != 0) {
						my_getgrgid(scratch, info.st_gid);
					}
					if (*scratch) {
						fprintf(stdout, "%-8.8s", scratch);
					} else {
						fprintf(stdout, "%-8d", dn->dstat.st_gid);
					}
				column += 17;
				}
				break;
#endif
			case LIST_ID_NUMERIC:
				fprintf(stdout, "%-8d %-8d", dn->dstat.st_uid, dn->dstat.st_gid);
				column += 17;
				break;
			case LIST_SIZE:
			case LIST_DEV:
				if (S_ISBLK(dn->dstat.st_mode) || S_ISCHR(dn->dstat.st_mode)) {
					fprintf(stdout, "%4d, %3d ", (int)MAJOR(dn->dstat.st_rdev), (int)MINOR(dn->dstat.st_rdev));	//if NAJOR --> error
				} else {
#if _FILE_OFFSET_BITS == 64
					fprintf(stdout, "%9lld ", dn->dstat.st_size);
#else
					fprintf(stdout, "%9ld ", dn->dstat.st_size);
#endif
				}
				column += 10;
				break;
#ifdef BB_FEATURE_LS_TIMESTAMPS
			case LIST_FULLTIME:
			case LIST_DATE_TIME:
				if (list_fmt & LIST_FULLTIME) {
					fprintf(stdout, "%24.24s ", filetime);
					column += 25;
					break;
				}
				age = time(NULL) - ttime;
				fprintf(stdout, "%6.6s ", filetime+4);
				if (age < 3600L * 24 * 365 / 2 && age > -15 * 60) {
					/* hh:mm if less than 6 months old */
					fprintf(stdout, "%5.5s ", filetime+11);
				} else {
					fprintf(stdout, " %4.4s ", filetime+20);
				}
				column += 13;
				break;
#endif
			case LIST_FILENAME:
				fprintf(stdout, "%s", dn->name);
				column += strlen(dn->name);
				break;
			case LIST_SYMLINK:
				if (S_ISLNK(dn->dstat.st_mode)) {
					len= readlink(dn->fullname, scratch, (sizeof scratch)-1);
					if (len > 0) {
						scratch[len]= '\0';
						fprintf(stdout, " -> %s", scratch);
#ifdef BB_FEATURE_LS_FILETYPES
						if (!stat(dn->fullname, &info)) {
							append = append_char(info.st_mode);
						}
#endif
						column += len+4;
					}
				}
				break;
#ifdef BB_FEATURE_LS_FILETYPES
			case LIST_FILETYPE:
				if (append != '\0') {
					fprintf(stdout, "%1c", append);
					column++;
				}
				break;
#endif
		}
	}

	return(0);
}


extern int ls_main(int argc, char **argv)
{
	struct dnode **dnf, **dnd;
	int dnfiles, dndirs;
	struct dnode *dn, *cur, **dnp;
	int i, nfiles;
	int opt;
	int oi, ac;
	char **av;

	disp_opts= DISP_NORMAL;
	style_fmt= STYLE_AUTO;
	list_fmt=  LIST_SHORT;
#ifdef BB_FEATURE_LS_SORTFILES
	sort_opts= SORT_NAME;
#endif
#ifdef BB_FEATURE_LS_TIMESTAMPS
	time_fmt= TIME_MOD;
#endif
	nfiles=0;

	applet_name= argv[0];
	/* process options */
	while ((opt = getopt(argc, argv, "1AaCdgilnsx"
#ifdef BB_FEATURE_AUTOWIDTH
"T:w:"
#endif
#ifdef BB_FEATURE_LS_FILETYPES
"Fp"
#endif
#ifdef BB_FEATURE_LS_RECURSIVE
"R"
#endif
#ifdef BB_FEATURE_LS_SORTFILES
"rSvX"
#endif
#ifdef BB_FEATURE_LS_TIMESTAMPS
"cetu"
#endif
#ifdef BB_FEATURE_LS_FOLLOWLINKS
"L"
#endif
	)) > 0) {
		switch (opt) {
			case '1': style_fmt = STYLE_SINGLE; break;
			case 'A': disp_opts |= DISP_HIDDEN; break;
			case 'a': disp_opts |= DISP_HIDDEN | DISP_DOT; break;
			case 'C': style_fmt = STYLE_COLUMNS; break;
			case 'd': disp_opts |= DISP_NOLIST; break;
			case 'e': list_fmt |= LIST_FULLTIME; break;
			case 'g': /* ignore -- for ftp servers */ break;
			case 'i': list_fmt |= LIST_INO; break;
			case 'l': style_fmt = STYLE_LONG; list_fmt |= LIST_LONG; break;
			case 'n': list_fmt |= LIST_ID_NUMERIC; break;
			case 's': list_fmt |= LIST_BLOCKS; break;
			case 'x': disp_opts = DISP_ROWS; break;
#ifdef BB_FEATURE_LS_FILETYPES
			case 'F': list_fmt |= LIST_FILETYPE | LIST_EXEC; break;
			case 'p': list_fmt |= LIST_FILETYPE; break;
#endif
#ifdef BB_FEATURE_LS_RECURSIVE
			case 'R': disp_opts |= DISP_RECURSIVE; break;
#endif
#ifdef BB_FEATURE_LS_SORTFILES
			case 'r': sort_order |= SORT_REVERSE; break;
			case 'S': sort_opts= SORT_SIZE; break;
			case 'v': sort_opts= SORT_VERSION; break;
			case 'X': sort_opts= SORT_EXT; break;
#endif
#ifdef BB_FEATURE_LS_TIMESTAMPS
			case 'c': time_fmt = TIME_CHANGE; sort_opts= SORT_CTIME; break;	//if !BB_FEATURE_LS_SORTFILES --> SORT_CTIME is undeclared
			case 't': sort_opts= SORT_MTIME; break;							//if !BB_FEATURE_LS_SORTFILES --> SORT_MTIME is undeclared
			case 'u': time_fmt = TIME_ACCESS; sort_opts= SORT_ATIME; break; //if !BB_FEATURE_LS_SORTFILES --> SORT_ATIME is undeclared
#endif
#ifdef BB_FEATURE_LS_FOLLOWLINKS
			case 'L': follow_links= TRUE; break;
#endif
#ifdef BB_FEATURE_AUTOWIDTH
			case 'T': tabstops= atoi(optarg); break;
			case 'w': terminal_width= atoi(optarg); break;
#endif
			default:
				goto print_usage_message;
		}
	}

	/* sort out which command line options take precedence */
#ifdef BB_FEATURE_LS_RECURSIVE
	if (disp_opts & DISP_NOLIST)
		disp_opts &= ~DISP_RECURSIVE;   /* no recurse if listing only dir */
#endif
#ifdef BB_FEATURE_LS_TIMESTAMPS
	if (time_fmt & TIME_CHANGE) sort_opts= SORT_CTIME;						//if !BB_FEATURE_LS_SORTFILES --> sort_opts is undeclared
	if (time_fmt & TIME_ACCESS) sort_opts= SORT_ATIME;
#endif
	if (style_fmt != STYLE_LONG)
			list_fmt &= ~LIST_ID_NUMERIC;  /* numeric uid only for long list */
#ifdef BB_FEATURE_LS_USERNAME
	if (style_fmt == STYLE_LONG && (list_fmt & LIST_ID_NUMERIC))
			list_fmt &= ~LIST_ID_NAME;  /* don't list names if numeric uid */
#endif

	/* choose a display format */
	if (style_fmt == STYLE_AUTO)
		style_fmt = isatty(fileno(stdout)) ? STYLE_COLUMNS : STYLE_SINGLE;

	/*
	 * when there are no cmd line args we have to supply a default "." arg.
	 * we will create a second argv array, "av" that will hold either
	 * our created "." arg, or the real cmd line args.  The av array
	 * just holds the pointers- we don't move the date the pointers
	 * point to.
	 */
	ac= argc - optind;   /* how many cmd line args are left */
	if (ac < 1) {
		av= (char **)xcalloc((size_t)1, (size_t)(sizeof(char *)));
		av[0]= xstrdup(".");
		ac=1;
	} else {
		av= (char **)xcalloc((size_t)ac, (size_t)(sizeof(char *)));
		for (oi=0 ; oi < ac; oi++) {
			av[oi]= argv[optind++];  /* copy pointer to real cmd line arg */
		}
	}

	/* now, everything is in the av array */
	if (ac > 1)
		disp_opts |= DISP_DIRNAME;   /* 2 or more items? label directories */

	/* stuff the command line file names into an dnode array */
	dn=NULL;
	for (oi=0 ; oi < ac; oi++) {
		cur= (struct dnode *)xmalloc(sizeof(struct dnode));
		cur->fullname= xstrdup(av[oi]);
		cur->name= cur->fullname;
		if (my_stat(cur))
			continue;
		cur->next= dn;
		dn= cur;
		nfiles++;
	}

	/* now that we know how many files there are
	** allocate memory for an array to hold dnode pointers
	*/
	dnp= dnalloc(nfiles);
	for (i=0, cur=dn; i<nfiles; i++) {
		dnp[i]= cur;   /* save pointer to node in array */
		cur= cur->next;
	}


	if (disp_opts & DISP_NOLIST) {
#ifdef BB_FEATURE_LS_SORTFILES
		shellsort(dnp, nfiles);
#endif
		if (nfiles > 0) showfiles(dnp, nfiles);
	} else {
		dnd= splitdnarray(dnp, nfiles, SPLIT_DIR);
		dnf= splitdnarray(dnp, nfiles, SPLIT_FILE);
		dndirs= countdirs(dnp, nfiles);
		dnfiles= nfiles - dndirs;
		if (dnfiles > 0) {
#ifdef BB_FEATURE_LS_SORTFILES
			shellsort(dnf, dnfiles);
#endif
			showfiles(dnf, dnfiles);
		}
		if (dndirs > 0) {
#ifdef BB_FEATURE_LS_SORTFILES
			shellsort(dnd, dndirs);
#endif
			showdirs(dnd, dndirs);
		}
	}

	return(status);

  print_usage_message:
	usage(ls_usage);
	return(FALSE);
}