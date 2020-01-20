#ifdef CONFIG_PAGEFLAGS_EXTENDED

static inline int PageCompound(struct page *page)
{
	return page->flags & ((1L << PG_head) | (1L << PG_tail));

}
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline void ClearPageCompound(struct page *page)
{
	BUG_ON(!PageHead(page));
	ClearPageHead(page);
}
#endif	/* TRANSPARENT_HUGEPAGE */
#else	/* PAGEFLAGS_EXTENDED */

#define PG_head_mask ((1L << PG_compound))
#define PG_head_tail_mask ((1L << PG_compound) | (1L << PG_reclaim))

static inline int PageHead(struct page *page)
{
	return ((page->flags & PG_head_tail_mask) == PG_head_mask);
}

static inline int PageTail(struct page *page)
{
	return ((page->flags & PG_head_tail_mask) == PG_head_tail_mask);
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline void ClearPageCompound(struct page *page)
{
	BUG_ON((page->flags & PG_head_tail_mask) != (1 << PG_compound));
	clear_bit(PG_compound, &page->flags);
}
#endif

#endif /* !PAGEFLAGS_EXTENDED */

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static inline int PageTransHuge(struct page *page)
{
	VM_BUG_ON(PageTail(page));						// if CONFIG_PAGEFLAGS_EXTENDED --> error
	return PageHead(page);							// if CONFIG_PAGEFLAGS_EXTENDED --> error
}
static inline int PageTransTail(struct page *page)
{	
	return PageTail(page);							// if CONFIG_PAGEFLAGS_EXTENDED --> error
}
#endif