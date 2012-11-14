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
## Setting up the method inspect
## 
## This method is used to display textual information of the disjunctive
## patterns found with the function disclosed (see disclosed.R)
## Patterns are formatted as a matrix; first column contains the list of items (features)
## second column contains the support of the pattern, and third column (if present) contains 
## the list of transactions (samples) where the pattern occur.

setMethod("inspect", signature(x = "disjunctive_itemsets"),
    function(x, ...) { 
	patterns=with(x,apply(x@items@data,2,
		FUN=function(y) {
			paste("{",paste(x@items@itemInfo$labels[which(y==TRUE)],collapse=','),"}",sep='')
			})
		)    	
	data.frame(items=patterns,support=x@quality$support,row.names=seq(length(x)))
})
