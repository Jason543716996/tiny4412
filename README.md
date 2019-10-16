# tiny4412
kernel:
	initialize kernel 
	adapter tiny4412
	adapter DM9621 "可配置网络文件系统"
	adapter SDcard
	adapter EMMC "配置从EMMC中启动系统"



busybox:
	add rootfs.tar.gz
