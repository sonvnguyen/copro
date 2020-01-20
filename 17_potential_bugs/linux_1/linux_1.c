#ifdef CONFIG_NODES_SPAN_OTHER_NODES
bool early_pfn_in_nid(unsigned long pfn, int node)
{
	int nid;

	nid = __early_pfn_to_nid(pfn);
	if (nid >= 0 && nid != node)
		return false;
	return true;
}
#endif
void memmap_init_zone(unsigned long size, int nid, unsigned long zone,
		unsigned long start_pfn, enum memmap_context context)
{
	struct page *page;
	unsigned long end_pfn = start_pfn + size;
	unsigned long pfn;
	struct zone *z;

	if (highest_memmap_pfn < end_pfn - 1)
		highest_memmap_pfn = end_pfn - 1;

	z = &NODE_DATA(nid)->node_zones[zone];
	for (pfn = start_pfn; pfn < end_pfn; pfn++) {
		/*
		 * There can be holes in boot-time mem_map[]s
		 * handed to this function.  They do not
		 * exist on hotplugged memory.
		 */
		if (context == MEMMAP_EARLY) {
			if (!early_pfn_valid(pfn))
				continue;
			if (!early_pfn_in_nid(pfn, nid))		// if !CONFIG_NODES_SPAN_OTHER_NODES --> an undeclared function: early_pfn_in_nid
				continue;
		}
		page = pfn_to_page(pfn);
		set_page_links(page, zone, nid, pfn);
		mminit_verify_page_links(page, zone, nid, pfn);
		init_page_count(page);
		reset_page_mapcount(page);
		SetPageReserved(page);
		if ((z->zone_start_pfn <= pfn)
		    && (pfn < z->zone_start_pfn + z->spanned_pages)
		    && !(pfn & (pageblock_nr_pages - 1)))
			set_pageblock_migratetype(page, MIGRATE_MOVABLE);

		INIT_LIST_HEAD(&page->lru);
#ifdef WANT_PAGE_VIRTUAL
		/* The shift won't overflow because ZONE_NORMAL is below 4G. */
		if (!is_highmem_idx(zone))
			set_page_address(page, __va(pfn << PAGE_SHIFT));
#endif
	}
}