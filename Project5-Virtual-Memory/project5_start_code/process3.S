.text
.globl __start

.org 0x0
__start:
    j      page_1_A
    nop

.org 0x1100
page_1_A:
    j      page_2_A
    nop

.org 0x1200
page_1_B:
    j      page_2_B
    nop

.org 0x1300
page_1_C:
    j      page_2_C
    nop

.org 0x2100
page_2_A:
    j      page_3_A
    nop

.org 0x2200
page_2_B:
    j      page_5_A
    nop

.org 0x2300
page_2_C:
    j      page_3_B
    nop

.org 0x3100
page_3_A:
    j      page_4_A
    nop

.org 0x3200
page_3_B:
    j      page_4_B
    nop

.org 0x4100
page_4_A:
    j      page_1_B
    nop

.org 0x4200
page_4_B:
    j      page_5_B
    nop

.org 0x5100
page_5_A:
    j      page_1_C
    nop

.org 0x5200
page_5_B:
    j      __end
    nop

__end:
    j      __end
    nop
