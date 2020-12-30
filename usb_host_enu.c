#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>

#define PCI_CONFIG_DATA 0x0CFC
#define PCI_CONFIG_ADDRESS 0x0CF8
#define PCI_VENDOR_OFFSET 0x00
#define PCI_REVISION_OFFSET 0x08

#define INVALID_DEVICE 0xFFFF

#define USB_CLASS_CODE 0x03
#define USB_SUBCLASS_CODE 0x0C
#define USB_UHCI 0x00
#define USB_OHCI 0x10
#define USB_EHCI 0X20
#define USB_XHCI 0x30

int read_usb_device(const unsigned char bus, const unsigned char device, const unsigned char fun, const char offset)
{
	int val32 = 0;

	val32 = 0x80000000 | (bus << 16) | (device << 11) | (fun << 8) | offset;
	outl(val32, PCI_CONFIG_ADDRESS);
	return inl(PCI_CONFIG_DATA);
}



static int __init  usb_host_init(void)
{
	unsigned char bus,dev,fun;
	int ret, vendorId, classCode, subClass,p_interface;
        char *str;

        pr_info("%s: Init if USB host enumuration\n",__func__);

	for(bus = 0; bus < 255; bus++)
	{
		for(dev = 0; dev < 32; dev++)
		{
			for(fun = 0; fun < 8; fun++)
			{
				ret = read_usb_device(bus,dev,fun,PCI_VENDOR_OFFSET);
        			vendorId = ret >> 16;
//				pr_info("%s: vendorID = %04x\n",__func__,vendorId);
				if(vendorId != INVALID_DEVICE)
				{
					ret  = read_usb_device(bus,dev,fun,PCI_REVISION_OFFSET);
					p_interface = (ret >> 8) & 0xFF;
					classCode = (ret >> 16) & 0xFF;
					subClass = (ret >> 24);
//					pr_info("%s: classcode = %02x, Subclass code = %02x\n",__func__, classCode, subClass);
					if((classCode == USB_CLASS_CODE) && (subClass == USB_SUBCLASS_CODE))
					{
						switch(p_interface)
						{
							case USB_UHCI:
								str = "UHCI Host Controller";
								break;
							case USB_OHCI:
								str = "OHCI Host Controller";
								break;
							case USB_EHCI:
								str = "EHCI Host Controller";
								break;
							case USB_XHCI:
								str = "xHCI Host Controller";
								break;
							default:
								str = "Unknown";
						}
						pr_info("%s: %02x:%02x:%02x USB controller, %s\n",__func__,bus,dev,fun,str);
					}
				}
			}
		}
	}
	return 0;
}

static void __exit usb_host_exit(void)
{
	pr_info("%s: Unloading module\n",__func__);
}


module_init(usb_host_init);
module_exit(usb_host_exit);

MODULE_AUTHOR("ASHISH VARA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("USB host enumuration on PCI bus");
