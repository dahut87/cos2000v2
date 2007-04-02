void setpic(void)
{
	/* MASTER */
	/* Initialisation de ICW1 */
	outbp(0x20,0x11);
	/* Initialisation de ICW2 - vecteur de depart = 32 */
	outbp(0x21,0x20);            
	/* Initialisation de ICW3 */
	outbp(0x21,0x04);
	/* Initialisation de ICW4 */
	outbp(0x21,0x01);
	/* masquage des interruptions */
	outbp(0x21,0xFF);
	/* SLAVE */
	/* Initialisation de ICW1 */
	outbp(0xA0,0x11);
	/* Initialisation de ICW2 - vecteur de depart = 96 */
	outbp(0xA1,0x70);      
	/* Initialisation de ICW3 */
	outbp(0xA1,0x02);
	/* Initialisation de ICW4 */
	outbp(0xA1,0x01);
	/* masquage des interruptions */
	outbp(0xA1,0xFF);
	/* Demasquage des irqs */
	outbp(0x21,0xFD);
}

void makeidtdesc(u32 offset, u16 select, u16 type, idtdesc* desc) 
{
	desc->offset0_15 = (offset & 0xffff);
	desc->select = select;
	desc->type = type;
	desc->offset16_31 = (offset & 0xffff0000) >> 16;
	return;
}
