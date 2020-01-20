#ifdef CONFIG_USE_GENERIC_SMP_HELPERS
void generic_smp_call_function_single_interrupt(void);
void generic_smp_call_function_interrupt(void);
void ipi_call_lock(void);
void ipi_call_unlock(void);
void ipi_call_lock_irq(void);
void ipi_call_unlock_irq(void);
#endif

notrace static void __cpuinit start_secondary(void *unused)
{
	vmi_bringup();
	cpu_init();
	preempt_disable();
	smp_callin();
	barrier();
	check_tsc_sync_target();

	if (nmi_watchdog == NMI_IO_APIC) {
		legacy_pic->chip->mask(0);
		enable_NMI_through_LVT0();
		legacy_pic->chip->unmask(0);
	}

#ifdef CONFIG_X86_32
	while (low_mappings)
		cpu_relax();
	__flush_tlb_all();
#endif
	set_cpu_sibling_map(raw_smp_processor_id());
	wmb();
	ipi_call_lock();								// if !CONFIG_USE_GENERIC_SMP_HELPERS --> error
	lock_vector_lock();
	set_cpu_online(smp_processor_id(), true);
	unlock_vector_lock();
	ipi_call_unlock();
	per_cpu(cpu_state, smp_processor_id()) = CPU_ONLINE;
	x86_platform.nmi_init();
	local_irq_enable();
	boot_init_stack_canary();
	x86_cpuinit.setup_percpu_clockev();

	wmb();
	cpu_idle();
}