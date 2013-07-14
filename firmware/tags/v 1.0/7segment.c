
int Char2Segments(char inChar) {

   switch(inChar) {
      case 0:   //0       (used when displaying numbers through number routine)
      case 48:  //ascii 0 (used when displaying numbers through text routine)
         return(0b00111111);  //all segments turned on except for middle segment and decimal point
         break;
      case 1:   //1
      case 49:  //ascii 1... etc.
         return(0b00000110);
         break;
      case 2:
      case 50:
         return(0b01011011);
         break;
      case 3:
      case 51:
         return(0b01001111);
         break;
      case 4:
      case 52:
         return(0b01100110);
         break;
      case 5:
      case 53:
         return(0b01101101);
         break;
      case 6:
      case 54:
         return(0b01111101);
         break;
      case 7:
      case 55:
         return(0b00000111);
         break;
      case 8:
      case 56:
         return(0b01111111);
         break;
      case 9:
      case 57:
         return(0b01101111);
         break;
      case 10:
         return(0b00000000);  //nothing displayed (for non-ones digits), i.e. 0097 displays as 97
         break;
      case 65:   //A
      case 97:   //a
         return(0b01110111); //(A)
         break;
      case 66:   //B
      case 98:   //b
         return(0b01111100); //(b)
         break;
      case 67:   //C
      case 99:   //c
         return(0b00111001); //(c)
         break;
      case 68:   //D
      case 100:  //d
         return(0b01011110); //(d)
         break;
      case 69:   //E
      case 101:  //e
         return(0b01111001); //(E)
         break;
      case 70:   //F
      case 102:  //f
         return(0b01110001); //(F)
         break;
      case 71:   //G
      case 103:  //g
         return(0b00111101); //(G)
         break;
      case 72:   //H
      case 104:  //h
         return(0b01110110); //(h)
         break;
      case 73:   //I
      case 105:  //i
         return(0b00000100);
         break;
      case 74:   //J
      case 106:  //j
         return(0b00001110);
         break;
      case 76:   //L
      case 108:  //l
         return(0b00111000);
         break;
      case 78:   //N
      case 110:  //n
         return(0b01010100); //(n)
         break;
      case 79:   //O
      case 111:  //o
         return(0b01011100); // (o);
         break;
      case 80:   //P
      case 112:  //p
         return(0b01110011); //(P)
         break;
      case 81:   //Q
      case 113:  //q
         return(0b01100111); //(q)
         break;
      case 82:   //R
      case 114:  //r
         return(0b01010000); //(r)
         break;
      case 83:   //S
      case 115:  //s
         return(0b01101101); //(S)
         break;
      case 84:   //T
      case 116:  //t
         return(0b01111000); //(t)
         break;
      case 85:   //U
      case 117:  //u
         return(0b00111110); //(u)
         break;
      case 89:   //Y
      case 121:  //y
         return(0b01101110); //(y)
         break;
         
      // special characters
      case '|':   // | vertical bar -> shows as double vertical bar
         return(0b00110110);
         break;
         
      case 32:   //space*/
      case 23:   //i.e. no letter (blank)
      default:   //any characters not coded for will also be blank
         return(0b00000000);
         break;
      }

}

void cls_internal7Seg() {
   gblCharCodeToShow1=0;
   gblCharCodeToShow2=0;
   gblCharCodeToShow3=0;
   gblCharCodeToShow4=0;

}

void turnOffAllSegments() {
   output_low(SEG_A);
   output_low(SEG_B);
   output_low(SEG_C);
   output_low(SEG_D);
   output_low(SEG_E);
   output_low(SEG_F);
   output_low(SEG_G);
   output_low(SEG_DP);   
}

// show characters on the screen
void showOnBuiltIn7Segment(char ch1, char ch2, char ch3, char ch4) {
      gblCharCodeToShow1 = Char2Segments(ch1);
      gblCharCodeToShow2 = Char2Segments(ch2);
      gblCharCodeToShow3 = Char2Segments(ch3);
      gblCharCodeToShow4 = Char2Segments(ch4);
}

/// Shows the next digit on th 7-segment display
/// The code avoids using arrays so most of it is hard coded.
/// This is done in preference of execution speed over code compactness.

void showNextDigit() {
   char charToShow;
   int displayBits;
   int i;
   
   
   turnOffAllSegments();

   // enable the current digit
   (gblDisplayDigit == 0) ? output_low(EN_DIGIT1) : output_high(EN_DIGIT1);
   (gblDisplayDigit == 1) ? output_low(EN_DIGIT2) : output_high(EN_DIGIT2);
   (gblDisplayDigit == 2) ? output_low(EN_DIGIT3) : output_high(EN_DIGIT3);
   (gblDisplayDigit == 3) ? output_low(EN_DIGIT4) : output_high(EN_DIGIT4);
   
   // fetch the display char code
   if (gblDisplayDigit == 0) displayBits = gblCharCodeToShow1;
   if (gblDisplayDigit == 1) displayBits = gblCharCodeToShow2;
   if (gblDisplayDigit == 2) displayBits = gblCharCodeToShow3;
   if (gblDisplayDigit == 3) displayBits = gblCharCodeToShow4;

   // show the char code on the 7-segment display
   if (bit_test(displayBits,0)) output_high(SEG_A);
   if (bit_test(displayBits,1)) output_high(SEG_B);
   if (bit_test(displayBits,2)) output_high(SEG_C);
   if (bit_test(displayBits,3)) output_high(SEG_D);
   if (bit_test(displayBits,4)) output_high(SEG_E);
   if (bit_test(displayBits,5)) output_high(SEG_F);
   if (bit_test(displayBits,6)) output_high(SEG_G);
   if (bit_test(displayBits,7)) output_high(SEG_DP);

   // next digit
   gblDisplayDigit = (gblDisplayDigit+1) % 4;
   

}

// This is the original un-optimized code.
// It is short but consumes more computing cycles

//!void showNextDigit() {
//!   char charToShow;
//!   int displayBits;
//!   int i;
//!   
//!   
//!   // turn off all segments
//!   for (i=0;i<8;i++) {
//!      output_low(gblPINMap[i]);
//!   }
//!   
//!   // enable the current digit to be displayed
//!   for (i=0;i<4;i++) {
//!      if (i!=gblDisplayDigit) {
//!         output_high(gblDigitEn[i]);
//!      } else {
//!         output_low(gblDigitEn[i]);
//!      }
//!   }
//!
//!   // get the digit -> transform it to the 7-segment code
//!   charToShow = gblDisplayBuffer[gblDisplayDigit];
//!   displayBits = Char2Segments(charToShow);
//!   
//!   // show the segments
//!   for (i=0;i<8;i++) {
//!      output_bit(gblPINMap[i], bit_test(displayBits,i));
//!      //output_high(gblPINMap[i]);
//!   }
//!   gblDisplayDigit = (gblDisplayDigit+1) % 4;
//!   
//!
//!}
//!

