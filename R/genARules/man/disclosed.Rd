\name{disclosed}
\alias{disclosed}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
%%  ~~function to do ... ~~
disclosed is a function to identify disjunctive frequent patterns
in high dimensional data
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
disclosed(mydata,minsup,maxsup)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{mydata}{
%%     ~~Describe \code{mydata} here~~
a data.frame containing the data
}
  \item{minsup}{minimum support}
  \item{mydata}{maximum support}

}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
A list of itemsets
}
\references{
%% ~put references to the literature/web site here ~
See Vimieiro, R. and Moscato, P. 
}
\author{
%%  ~~who you are~~
Renato Vimieiro
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
arules::apriori
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.
load("/home/renato/.RData")
disclosed(mydata,1,nrow(mydata))
## The function is currently defined as
function(mydata, minsup=1, maxsup=nrow(mydata)){
result <- .Call('__R_Call_disclosed',mydata,
			nobj=as.integer(nrow(mydata)),
			natts=as.integer(ncol(mydata)),
			ms=as.integer(minsup),
			mx=as.integer(maxsup))
return(result)			
}
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
%%\keyword{ ~kwd1 }
%%\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
