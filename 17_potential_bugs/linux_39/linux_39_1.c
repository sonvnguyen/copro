#ifdef CONFIG_OF_DEVICE
#if !defined(CONFIG_SPARC)
extern struct platform_device *of_platform_device_create(struct device_node *np,
						   const char *bus_id,
						   struct device *parent);

extern int of_platform_bus_probe(struct device_node *root,
				 const struct of_device_id *matches,
				 struct device *parent);
extern int of_platform_populate(struct device_node *root,
				const struct of_device_id *matches,
				const struct of_dev_auxdata *lookup,
				struct device *parent);
#endif /* !CONFIG_SPARC */

#endif /* CONFIG_OF_DEVICE */
				
				/* NOTE:  this driver only handles a single twl4030/tps659x0 chip */
static int twl_probe(struct i2c_client *client, const struct i2c_device_id *id){
#ifdef CONFIG_OF_DEVICE					
	if (node)
		status = of_platform_populate(node, NULL, NULL, &client->dev);			//if CONFIG_SPARC --> error
	else
#endif
		status = add_children(pdata, id->driver_data);
	
}