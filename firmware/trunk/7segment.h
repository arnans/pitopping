#define  EN_DIGIT1   PIN_F7
#define  EN_DIGIT2   PIN_E3
#define  EN_DIGIT3   PIN_E4
#define  EN_DIGIT4   PIN_E0

#define  SEG_A       PIN_E1
#define  SEG_B       PIN_E2
#define  SEG_C       PIN_G3
#define  SEG_D       PIN_G1
#define  SEG_E       PIN_G0
#define  SEG_F       PIN_E5
#define  SEG_G       PIN_G4
#define  SEG_DP      PIN_G2

void showNextDigit();
void turnOffAllSegments();
int Char2Segments(char inChar);
void cls_internal7Seg();
void showOnBuiltIn7Segment(char ch1, char ch2, char ch3, char ch4);

int gblDisplayDigit=0;

//!int gblCharCodeToShow1 = 0b00111101;  // 'G'
//!int gblCharCodeToShow2 = 0b01011100;  // 'o'
//!int gblCharCodeToShow3 = 0b00111101;  // 'G'
//!int gblCharCodeToShow4 = 0b01011100;  // 'o'

int gblCharCodeToShow1 = 0;  // 'G'
int gblCharCodeToShow2 = 0;  // 'o'
int gblCharCodeToShow3 = 0;  // 'G'
int gblCharCodeToShow4 = 0;  // 'o'
