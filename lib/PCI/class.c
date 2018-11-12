static pciclass old[] = {
	{ 0x0, "Pre-2.0 PCI Specification Device, Non-VGA" },
	{ 0x1, "Pre-2.0 PCI Specification Device, VGA Compatible" },
	{ 0, NULL }
};

static pciclass storage[] = {
	{ 0x00, "Mass Storage Controller, SCSI" },
	{ 0x01, "Mass Storage Controller, IDE" },
	{ 0x02, "Mass Storage Controller, Floppy" },
	{ 0x03, "Mass Storage Controller, IPI" },
	{ 0x04, "Mass Storage Controller, RAID" },
	{ 0x05, "Mass Storage Controller, ATA controller with single DMA" }, 
	{ 0x06, "Mass Storage Controller, SATA" },
	{ 0x07, "Mass Storage Controller, SAS" },
	{ 0x08, "Mass Storage Controller, NVM" },
	{ 0x80, "Mass Storage Controller, Other" },
	{ 0, NULL }
};

static pciclass network[] = {
	{ 0x00, "Network Controller, Ethernet" },
	{ 0x01, "Network Controller, Token Ring" },
	{ 0x02, "Network Controller, FDDI" },
	{ 0x03, "Network Controller, ATM" },
	{ 0x04, "Network Controller, ISDN" },
	{ 0x80, "Network Controller, Other" },
	{ 0, NULL }
};

static pciclass display[] = {
	{ 0x00, "Display Controller, VGA" },
	{ 0x01, "Display Controller, XGA" },
	{ 0x02, "Display Controller, 3D" },
	{ 0x80, "Display Controller, Other" },
	{ 0, NULL }
};

static pciclass multimedia[] = {
	{ 0x00, "Multimedia Device, Video" },
	{ 0x01, "Multimedia Device, Audio" },
	{ 0x02, "Multimedia Device, Telephony" },
	{ 0x03, "Multimedia Device, HDA" },
	{ 0x80, "Multimedia Device, Other" },
	{ 0, NULL }
};

static pciclass memory[] = {
	{ 0x00, "Memory Controller, RAM" },
	{ 0x01, "Memory Controller, Flash" },
	{ 0x80, "Memory Controller, Other" },
	{ 0, NULL }
};

static pciclass bridge[] = {
	{ 0x00, "Bridge Device, Host/PCI" },
	{ 0x01, "Bridge Device, PCI/ISA" },
	{ 0x02, "Bridge Device, PCI/EISA" },
	{ 0x03, "Bridge Device, PCI/Micro Channel" },
	{ 0x04, "Bridge Device, PCI/PCI" },
	{ 0x05, "Bridge Device, PCI/PCMCIA" },
	{ 0x06, "Bridge Device, PCI/NuBus" },
	{ 0x07, "Bridge Device, PCI/CardBus" },
	{ 0x08, "Bridge Device, PCI/RACEway" },
	{ 0x09, "Bridge Device, PCI/Transparent" },
	{ 0x0a, "Bridge Device, Infiniband" },
	{ 0x80, "Bridge Device, Other" },
	{ 0, NULL }
};

static pciclass simplecomm[] = {
	{ 0x00, "Simple Communications Controller, Serial" },
	{ 0x01, "Simple Communications Controller, Parallel" },
	{ 0x02, "Simple Communications Controller, Multiport" },
	{ 0x03, "Simple Communications Controller, Modem" },
	{ 0x04, "Simple Communications Controller, GPIB" },
	{ 0x05, "Simple Communications Controller, Smart Card" },
	{ 0x80, "Simple Communications Controller, Other" },
	{ 0, NULL }
};

static pciclass baseperiph[] = {
	{ 0x00, "Base Systems Peripheral, Interrupt Controller" },
	{ 0x01, "Base Systems Peripheral, DMA" },
	{ 0x02, "Base Systems Peripheral, System Timer" },
	{ 0x03, "Base Systems Peripheral, Real Time Clock" },
	{ 0x04, "Base Systems Peripheral, PCI Hot-plug" },
	{ 0x05, "Base Systems Peripheral, SD Host Controller" },
	{ 0x06, "Base Systems Peripheral, IOMMU" },
	{ 0x80, "Base Systems Peripheral, Other" },
	{ 0, NULL }
};

static pciclass input[] = {
	{ 0x00, "Input Device, Keyboard" },
	{ 0x01, "Input Device, Digitizer" },
	{ 0x02, "Input Device, Mouse" },
	{ 0x03, "Input Device, Scanner" },
	{ 0x04, "Input Device, Game Port" },
	{ 0x80, "Input Device, Other" },
	{ 0, NULL }
};

static pciclass docking[] = {
	{ 0x00, "Docking Station, Generic" },
	{ 0x80, "Docking Station, Other" },
	{ 0, NULL }
};

static pciclass processor[] = {
	{ 0x00, "Processor, i386" },
	{ 0x01, "Processor, i486" },
	{ 0x02, "Processor, Pentium" },
	{ 0x10, "Processor, Alpha" },
	{ 0x20, "Processor, Power PC" },
	{ 0x80, "Processor, Co-processor" },
	{ 0, NULL }
};

static pciclass serial[] = {
	{ 0x00, "Serial Bus Controller, Firewire" },
	{ 0x01, "Serial Bus Controller, ACCESS.bus" },
	{ 0x02, "Serial Bus Controller, SSA" },
	{ 0x03, "Serial Bus Controller, USB" },
	{ 0x04, "Serial Bus Controller, Fibre Channel" },
	{ 0x05, "Serial Bus Controller, SMBus" },
	{ 0x06, "Serial Bus Controller, Inifiniband" },
	{ 0x07, "Serial Bus Controller, IPMI" },
	{ 0, NULL }
};

static pciclass wireless[] = {
	{ 0x00, "Wireless Controller, iRDA" },
	{ 0x01, "Wireless Controller, IR" },
	{ 0x10, "Wireless Controller, RF" },
	{ 0x11, "Wireless Controller, Bluetooth" },
	{ 0x12, "Wireless Controller, Broadband" },
	{ 0x20, "Wireless Controller, 802.11a" },
	{ 0x21, "Wireless Controller, 802.11b" },
	{ 0x80, "Wireless Controller, Other" },
	{ 0, NULL }
};

static pciclass intelliio[] = {
	{ 0x00, "Intelligent IO Controller, I2O" },
	{ 0, NULL }
};

static pciclass satcomm[] = {
	{ 0x00, "Satellite Communication Controller, TV" },
	{ 0x01, "Satellite Communication Controller, Audio" },
	{ 0x02, "Satellite Communication Controller, Voice" },
	{ 0x03, "Satellite Communication Controller, Data" },
	{ 0, NULL }
};

static pciclass crypto[] = {
	{ 0x00, "Encryption/Decryption Controller, Network/computer" },
	{ 0x01, "Encryption/Decryption Controller, Entertainment" },
	{ 0x80, "Encryption/Decryption Controller, Other" },
	{ 0, NULL }
};

static pciclass dasp[] = {
	{ 0x00, "Data Acquisition and Signal Processing Controller, DPIO" },
	{ 0x01, "Data Acquisition and Signal Processing Controller, Performance Counter" },
	{ 0x10, "Data Acquisition and Signal Processing Controller, Communications Synchronization" },
	{ 0x20, "Data Acquisition and Signal Processing Controller, Management Card" },
	{ 0x80, "Data Acquisition and Signal Processing Controller, Other" },
	{ 0, NULL }
};

static pciclass *classcodes[] = {
	old,
	storage,
	network,
	display,
	multimedia,
	memory,
	bridge,
	simplecomm,
	baseperiph,
	input,
	docking,
	processor,
	serial,
	wireless,
	intelliio,
	satcomm,
	crypto,
	dasp,
	[255] = NULL
};

