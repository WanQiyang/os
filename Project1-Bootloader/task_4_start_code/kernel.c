
#define PORT3f8 0xbfe48000
#define PORT3fd 0xbfe48006

void delay(unsigned long n)
{
    while (n > 0) {
        n--;
    }
}

void printstr(char *s)
{
   unsigned long port = PORT3f8; 
   delay(10000);
   while (*s) {
     /* while (((*(volatile unsigned char*)PORT3fd) & 0x20)==0);  */
     	*(unsigned char*)port = *s;
    	s++;
   }
}

int main(){
	printstr("It's kernel!\r\n");
    printstr("Test #1!\r\n");
    printstr("Test #2!\r\n");
    printstr("Test #3!\r\n");
    printstr("Test #4!\r\n");
    printstr("Test #5!\r\n");
    printstr("Test #6!\r\n");
    printstr("Test #7!\r\n");
    printstr("Test #8!\r\n");
    printstr("Test #9!\r\n");
    printstr("Test #10!\r\n");
    printstr("Test #11!\r\n");
    printstr("Test #12!\r\n");
    printstr("Test #13!\r\n");
    printstr("Test #14!\r\n");
    printstr("Test #15!\r\n");
    printstr("Test #16!\r\n");
    printstr("Test #17!\r\n");
    printstr("Test #18!\r\n");
    printstr("Test #19!\r\n");
    printstr("Test #20!\r\n");
    printstr("Test #21!\r\n");
    printstr("Test #22!\r\n");
    printstr("Test #23!\r\n");
    printstr("Test #24!\r\n");
    printstr("Test #25!\r\n");
    printstr("Test #26!\r\n");
    printstr("Test #27!\r\n");
    printstr("Test #28!\r\n");
    printstr("Test #29!\r\n");
    printstr("Test #30!\r\n");
    printstr("Test #31!\r\n");
    printstr("Test #32!\r\n");
    printstr("Test #33!\r\n");
    printstr("Test #34!\r\n");
    printstr("Test #35!\r\n");
    printstr("Test #36!\r\n");
    printstr("Test #37!\r\n");
    printstr("Test #38!\r\n");
    printstr("Test #39!\r\n");
    printstr("Test #40!\r\n");
    printstr("Test #41!\r\n");
    printstr("Test #42!\r\n");
    printstr("Test #43!\r\n");
    printstr("Test #44!\r\n");
    printstr("Test #45!\r\n");
    printstr("Test #46!\r\n");
    printstr("Test #47!\r\n");
    printstr("Test #48!\r\n");
    printstr("Test #49!\r\n");
    printstr("Test #50!\r\n");
    printstr("Test #51!\r\n");
    printstr("Test #52!\r\n");
    printstr("Test #53!\r\n");
    printstr("Test #54!\r\n");
    printstr("Test #55!\r\n");
    printstr("Test #56!\r\n");
    printstr("Test #57!\r\n");
    printstr("Test #58!\r\n");
    printstr("Test #59!\r\n");
    printstr("Test #60!\r\n");
    printstr("Test #61!\r\n");
    printstr("Test #62!\r\n");
    printstr("Test #63!\r\n");
    printstr("Test #64!\r\n");
    printstr("Test #65!\r\n");
    printstr("Test #66!\r\n");
    printstr("Test #67!\r\n");
    printstr("Test #68!\r\n");
    printstr("Test #69!\r\n");
    printstr("Test #70!\r\n");
    printstr("Test #71!\r\n");
    printstr("Test #72!\r\n");
    printstr("Test #73!\r\n");
    printstr("Test #74!\r\n");
    printstr("Test #75!\r\n");
    printstr("Test #76!\r\n");
    printstr("Test #77!\r\n");
    printstr("Test #78!\r\n");
    printstr("Test #79!\r\n");
    printstr("Test #80!\r\n");
    printstr("Test #81!\r\n");
    printstr("Test #82!\r\n");
    printstr("Test #83!\r\n");
    printstr("Test #84!\r\n");
    printstr("Test #85!\r\n");
    printstr("Test #86!\r\n");
    printstr("Test #87!\r\n");
    printstr("Test #88!\r\n");
    printstr("Test #89!\r\n");
    printstr("Test #90!\r\n");
    printstr("Test #91!\r\n");
    printstr("Test #92!\r\n");
    printstr("Test #93!\r\n");
    printstr("Test #94!\r\n");
    printstr("Test #95!\r\n");
    printstr("Test #96!\r\n");
    printstr("Test #97!\r\n");
    printstr("Test #98!\r\n");
    printstr("Test #99!\r\n");
    printstr("Test #100!\r\n");
    printstr("Test #101!\r\n");
    printstr("Test #102!\r\n");
    printstr("Test #103!\r\n");
    printstr("Test #104!\r\n");
    printstr("Test #105!\r\n");
    printstr("Test #106!\r\n");
    printstr("Test #107!\r\n");
    printstr("Test #108!\r\n");
    printstr("Test #109!\r\n");
    printstr("Test #110!\r\n");
    printstr("Test #111!\r\n");
    printstr("Test #112!\r\n");
    printstr("Test #113!\r\n");
    printstr("Test #114!\r\n");
    printstr("Test #115!\r\n");
    printstr("Test #116!\r\n");
    printstr("Test #117!\r\n");
    printstr("Test #118!\r\n");
    printstr("Test #119!\r\n");
    printstr("Test #120!\r\n");
    printstr("Test #121!\r\n");
    printstr("Test #122!\r\n");
    printstr("Test #123!\r\n");
    printstr("Test #124!\r\n");
    printstr("Test #125!\r\n");
    printstr("Test #126!\r\n");
    printstr("Test #127!\r\n");
    printstr("Test #128!\r\n");
	return(0);
}
