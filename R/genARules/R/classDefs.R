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
## Definition of the class disjunctive itemset.
## This class is redefinition of the class itemsets
## in the package aRules.
## I defined this class to implement a new inspect method (see inspect.R)
## to display patterns, while I maintained the original method aRules::inspect intact.
## 

setClass("disjunctive_itemsets",    
    contains=c("itemsets","associations")
)
