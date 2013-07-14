//
// evalopcode.h - Header declarations for the Logo Operations
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


/////////////////////////////////////////////////////
//
//  Logo Virtual Machine
//
//  Written by Arnan (Roger) Sipitakiat
//
//  Logo running on the GoGo Board was created based
//  on the Cricket Logo.
//
//  Cricket Logo creators include:
//    Fred Martin
//    Brian Silverman
//    Mitchel Resnick
//    Robbie Berg
//
/////////////////////////////////////////////////////


int16 gblLoopAddress=0;   // Stores the start address of a Loop
int16 gblRepeatCount=0;  // Tracks the progress of the repeat command

int1 gblONFORNeedsToFinish=0;  // flags when onfor is launched
                        // it causes fetchNextOpcode() to return
                        // an Off command the next time it is called

void evalOpcode(unsigned char opcode);
#separate void evalOpcode2(unsigned char opcode);
