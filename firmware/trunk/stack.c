//
// stack.c - Provides stack objects for the Logo Virtual Machine
//
// Copyright (C) 2001-2007 Massachusetts Institute of Technology
// Contact   Arnan (Roger) Sipiatkiat [arnans@gmail.com]
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//





/////////////////////////////////////////////
//  Main Data Stack


void stkPush(unsigned int16 stackItem)
{
   if (gblStkPtr<STACK_SIZE) {
//      printf(ser_char, "data stack = %d\r\n", gblStkPtr);
      gblStack[gblStkPtr++] = stackItem;
   } else {
//      printf(ser_char,"Error: Stack overflow\r\n");

   /////////////////////
   //  Halt the board if stak overflows
   while (1) {
      output_low(RUN_LED); delay_ms(100);
      output_high(RUN_LED); delay_ms(1000);
   }

      gblErrFlag |= 0b00000001;  // set err bit 1 "stack overflow"
   }
}

unsigned int16 stkPop(void)
{
   if (gblStkPtr>0) {
      return(gblStack[--gblStkPtr]);
   } else {
      gblErrFlag |= 0b00000010;  // set error bit 2 "stack empty"
      /////////////////////
      //  Halt the board if stak overflows
      while (1) {
         output_low(RUN_LED); delay_ms(1000);
         output_high(RUN_LED); delay_ms(100);
      }
   
      
   }
}


/////////////////////////////////////////////
//  Procedure Input Stack


void inputPush(unsigned int16 stackItem)
{
   if (gblInputStkPtr<INPUT_STACK_SIZE) {
      //printf(ser_char, "input stack push = %lu\r\n", stackItem);
      gblInputStack[gblInputStkPtr++] = stackItem;
   } else {
     //printf(ser_char,"Error: Stack overflow\r\n");

   /////////////////////
   //  Halt the board if stak overflows
   while (1) {
      output_low(RUN_LED); delay_ms(500);
      output_high(RUN_LED); delay_ms(500);
   }



      gblErrFlag |= 0b00000100;  // set err bit 3 "stack overflow"
   }
}

unsigned int16 inputPop(void)
{
   if (gblInputStkPtr>0) {
      //printf(ser_char, "input stack pop = %lu\r\n", gblInputStack[gblInputStkPtr-1]);

      return(gblInputStack[--gblInputStkPtr]);
   } else {
      gblErrFlag |= 0b00001000;  // set error bit 4 "stack empty"
      //printf(ser_char,"Input stack empty!!!\r\n");
      /////////////////////
      //  Halt the board if stak overflows
      while (1) {
         output_low(RUN_LED); delay_ms(500);
         output_high(RUN_LED); delay_ms(500);
      }

   }
}



void clearStack() {
   gblStkPtr=gblInputStkPtr=0;
}



/*
void main()
{
   unsigned char i,inByte;

    byte InByte, buff_status;


   set_tris_c(PIC_TRIS_C);
   set_tris_d(PIC_TRIS_D);

   for (i=0;i<2;i++) {
      output_high(PIN_D0);
      delay_ms(200);
      output_low(PIN_D0);
      delay_ms(200);
   }

   // init the serial port
   ser_init();
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_RDA);

   gblStkPtr = 0;


   stkPush(0x0000);
   stkPush(0xffff);
   stkPush(0x00ff);
   stkPush(0xff00);
   stkPush(0x0f0f);
   stkPush(0xf0f0);
   stkPush(0x1111);
   stkPush(0x2222);
   stkPush(0x3333);
   stkPush(0x4444);
   stkPush(0x5555);
   stkPush(0x6666);
   stkPush(0x7777);
   stkPush(0x8888);
   stkPush(0x9999);
   stkPush(0xaaaa);
   stkPush(0xbbbb);
   stkPush(0xcccc);
   stkPush(0xdddd);
   stkPush(0xeeee);


   printf(ser_char,"poping stack data: \r\n");
   for (i=0;i<20;i++) {
      printf(ser_char, "%LX\r\n", stkPop());
   }
   printf(ser_char, "End of stack.\r\n");

}


*/

