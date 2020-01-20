#ifdef CONFIG_FEATURE_LS_RECURSIVE
static void dfree(struct dnode **dnp){
	//... free each element
	free(dnp);	/* free the array holding the dnode pointers */
}
#endif
void showdirs(struct dnode **dn, int ndirs){
	int i, nfiles;
	struct dnode **subdnp;
#ifdef BB_FEATURE_LS_SORTFILES
	int dndirs;
	struct dnode **dnd;
#endif
	if (dn==NULL || ndirs < 1) return;
	for (i=0; i<ndirs; i++) {
		//...
		subdnp= list_dir(dn[i]->fullname);
		nfiles= countfiles(subdnp);
		if (nfiles > 0) {
#ifdef BB_FEATURE_LS_SORTFILES
			shellsort(subdnp, nfiles);
#endif
			showfiles(subdnp, nfiles);
#ifdef BB_FEATURE_LS_RECURSIVE
			if (disp_opts & DISP_RECURSIVE) {
				dnd= splitdnarray(subdnp, nfiles, SPLIT_DIR);
				dndirs= countdirs(subdnp, nfiles);				//if !BB_FEATURE_LS_RECURSIVE &&  BB_FEATURE_LS_SORTFILES --> unused dndirs
				if (dndirs > 0) {
					//...
					free(dnd);  /* free the array of dnode pointers to the dirs */
				}
			}
			dfree(subdnp);  /* free the dnodes and the fullname mem */
#endif
		}
	}
}