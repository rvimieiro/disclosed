#################################################################################
# genARules - Mining Disjunctive Frequent Itemsets
# Copyrigth (C) 2012 Renato Vimieiro
#			
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


##
## Function disclosed
##
## Call the Disclosed algorithm
## Please refer to Vimieiro, R. & Moscato, P. (2012)
## _Mining disjunctive closed itemsets in high dimensional biomedical data_
## The University of Newcastle, NSW, AUSTRALIA

disclosed <- function(mydata, minsup, maxsup)
{
result <- .Call('__R_Call_disclosed',mydata,
			nobj=as.integer(nrow(mydata)),
			natts=as.integer(ncol(mydata)),
			ms=as.integer(minsup),
			mx=as.integer(maxsup), 
			PACKAGE="genARules")
#itemsets <- new("disjunctive_itemsets")
#itemsets@items <- result
#itemsets@itemInfo <- data.frame(labels=colnames(mydata))

return(result)			
}

