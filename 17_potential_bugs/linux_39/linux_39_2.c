#ifdef CONFIG_IRQ_DOMAIN
struct device_node;
struct irq_domain;
struct irq_domain_ops {
	unsigned int (*to_irq)(struct irq_domain *d, unsigned long hwirq);

#ifdef CONFIG_OF
	int (*dt_translate)(struct irq_domain *d, struct device_node *node,
			    const u32 *intspec, unsigned int intsize,
			    unsigned long *out_hwirq, unsigned int *out_type);
#endif /* CONFIG_OF */
};

#define irq_domain_for_each_hwirq(d, hw) \
	for (hw = d->hwirq_base; hw < d->hwirq_base + d->nr_irq; hw++)

#define irq_domain_for_each_irq(d, hw, irq) \
	for (hw = d->hwirq_base, irq = irq_domain_to_irq(d, hw); \
	     hw < d->hwirq_base + d->nr_irq; \
	     hw++, irq = irq_domain_to_irq(d, hw))

extern void irq_domain_add(struct irq_domain *domain);
extern void irq_domain_del(struct irq_domain *domain);

extern struct irq_domain_ops irq_domain_simple_ops;
#endif /* CONFIG_IRQ_DOMAIN */

void irq_domain_add(struct irq_domain *domain)
{
	struct irq_data *d;
	int hwirq, irq;
	irq_domain_for_each_irq(domain, hwirq, irq) {					//if !CONFIG_IRQ_DOMAIN --> error
		d = irq_get_irq_data(irq);
		if (!d) {
			WARN(1, "error: assigning domain to non existant irq_desc");
			return;
		}
		if (d->domain) {
			/* things are broken; just report, don't clean up */
			WARN(1, "error: irq_desc already assigned to a domain");
			return;
		}
		d->domain = domain;
		d->hwirq = hwirq;
	}

	mutex_lock(&irq_domain_mutex);
	list_add(&domain->list, &irq_domain_list);
	mutex_unlock(&irq_domain_mutex);
}
void irq_domain_del(struct irq_domain *domain)
{
	struct irq_data *d;
	int hwirq, irq;

	mutex_lock(&irq_domain_mutex);
	list_del(&domain->list);
	mutex_unlock(&irq_domain_mutex);

	/* Clear the irq_domain assignments */
	irq_domain_for_each_irq(domain, hwirq, irq) {					//if !CONFIG_IRQ_DOMAIN --> error
		d = irq_get_irq_data(irq);
		d->domain = NULL;
	}
}