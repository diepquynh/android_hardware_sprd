#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc11b7ce6, "module_layout" },
	{ 0xadf42bd5, "__request_region" },
	{ 0xfad0732d, "cdev_del" },
	{ 0x55f36102, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xb784e3d6, "cdev_init" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0xab455eb0, "up_read" },
	{ 0x528c709d, "simple_read_from_buffer" },
	{ 0x5eb40890, "debugfs_create_dir" },
	{ 0x6ffa9772, "mem_map" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x788fe103, "iomem_resource" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0xbed60566, "sub_preempt_count" },
	{ 0xb0bb9c02, "down_interruptible" },
	{ 0xb580b6ba, "device_destroy" },
	{ 0x62b72b0d, "mutex_unlock" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x999e8297, "vfree" },
	{ 0xe3575015, "debugfs_create_file" },
	{ 0x58d6a09e, "debugfs_remove_recursive" },
	{ 0xc584f560, "__alloc_pages_nodemask" },
	{ 0x22e6a6f4, "_irq_restore" },
	{ 0xc6d533e1, "down_read" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0x48a0f939, "mutex_lock_interruptible" },
	{ 0xdc798d37, "__mutex_init" },
	{ 0x27e1a049, "printk" },
	{ 0xbf8ba54a, "vprintk" },
	{ 0xe16b893b, "mutex_lock" },
	{ 0x4c6ff041, "add_preempt_count" },
	{ 0xab676256, "device_create" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xadb3d81e, "up_write" },
	{ 0xb1b22911, "down_write" },
	{ 0x6e9dde02, "contig_page_data" },
	{ 0x1fb79d73, "cdev_add" },
	{ 0x2542fad1, "__free_pages" },
	{ 0x98079746, "_irq_save" },
	{ 0x5833f6e1, "___dma_page_cpu_to_dev" },
	{ 0x812a320f, "___dma_page_dev_to_cpu" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xc27487dd, "__bug" },
	{ 0x9bce482f, "__release_region" },
	{ 0xe9d7c658, "kmem_cache_alloc_trace" },
	{ 0x425456c9, "cpu_cache" },
	{ 0x37a0cba, "kfree" },
	{ 0x46727bf5, "remap_pfn_range" },
	{ 0x9d669763, "memcpy" },
	{ 0x8cf51d15, "up" },
	{ 0x77ee13e8, "class_destroy" },
	{ 0x701d0ebd, "snprintf" },
	{ 0xc3fe87c8, "param_ops_uint" },
	{ 0xa170bbdb, "outer_cache" },
	{ 0xa7497022, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x81799cee, "vscnprintf" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "54EDBF690AEBC910C03F1F5");
