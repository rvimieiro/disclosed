/*
 * errorHandling.c
 *
 *  Created on: 13/08/2012
 *      Author: renato
 *  
 * CENTRE FOR BIOINFORMATICS, BIOMARKER-DISCOVERY & INFORMATION-BASED MEDICINE
 * THE UNIVERSITY UNIVERSITY OF NEWCASTLE <http://www.newcastle.edu.au/research-centre/cibm/>
 * HUNTER MEDICAL RESEARCH INSTITUTE <http://hmri.com.au/>
 * Kookaburra Circuit 1, New Lambton Heights, NSW 2305, AUSTRALIA
 */

#include "errorHandling.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <R.h>

void SIG_HANDLER(int sig){
	char errorMsg[20];
	sprintf(errorMsg,"Disclosed signal handling (%d): %s\n",sig,strsignal(sig));
	error(errorMsg);
}
