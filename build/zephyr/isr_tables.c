
/* AUTO-GENERATED by gen_isr_tables.py, do not edit! */

#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>
#include <zephyr/sw_isr_table.h>
#include <zephyr/arch/cpu.h>

typedef void (* ISR)(const void *);
uintptr_t __irq_vector_table _irq_vector_table[39] = {
	((uintptr_t)&_isr_wrapper),
	120929,
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
	((uintptr_t)&_isr_wrapper),
};
struct _isr_table_entry __sw_isr_table _sw_isr_table[39] = {
	{(const void *)0x28d99, (ISR)0x3b127}, /* 0 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 1 */
	{(const void *)0x3c7d0, (ISR)0x36bb3}, /* 2 */
	{(const void *)0x29f69, (ISR)0x3b127}, /* 3 */
	{(const void *)0x29f7d, (ISR)0x3b127}, /* 4 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 5 */
	{(const void *)0x293c9, (ISR)0x3b127}, /* 6 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 7 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 8 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 9 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 10 */
	{(const void *)0x0, (ISR)0x1d87d}, /* 11 */
	{(const void *)0x3c8ac, (ISR)0x23fdd}, /* 12 */
	{(const void *)0x20008e90, (ISR)0x214e9}, /* 13 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 14 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 15 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 16 */
	{(const void *)0x0, (ISR)0x24525}, /* 17 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 18 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 19 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 20 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 21 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 22 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 23 */
	{(const void *)0x0, (ISR)0x1d8ad}, /* 24 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 25 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 26 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 27 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 28 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 29 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 30 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 31 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 32 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 33 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 34 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 35 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 36 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 37 */
	{(const void *)0x0, (ISR)z_irq_spurious}, /* 38 */
};