/*****************************************************************************
 * @(#) Ndless hook (i.e run user programs thanks to the TI documents browser)
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Geoffrey ANNEHEIM
 * <geoffrey.anneheim@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Olivier ARMAND <olivier.calc@gmail.com>
 ****************************************************************************/

  #include <os.h>
  
  #ifdef CAS
    .set  OS_OFFSET_HACKED,             0x10005E40
    .set  OS_SHADOWED_CALL,             0x10005758
  #elif NON_CAS
    .set  OS_OFFSET_HACKED,             0x10005E58
    .set  OS_SHADOWED_CALL,             0x10005770
  #endif
  
  .set  PRG_SIGNATURE_BYTES_SIZE,       4
    

main: .global main

  .end
