\name{disjunctive_itemsets-class}
\Rdversion{1.1}
\docType{class}
\alias{disjunctive_itemsets-class}
\alias{inspect,disjunctive_itemsets-method}

\title{Class \code{"disjunctive_itemsets"}}
\description{
%%  ~~ A concise (1-5 lines) description of what the class is. ~~
}
\section{Objects from the Class}{
Objects can be created by calls of the form \code{new("disjunctive_itemsets", ...)}.
%%  ~~ describe objects here ~~ 
}
\section{Slots}{
  \describe{
    \item{\code{items}:}{Object of class \code{"itemMatrix"} ~~ }
    \item{\code{tidLists}:}{Object of class \code{"tidLists_or_NULL"} ~~ }
    \item{\code{quality}:}{Object of class \code{"data.frame"} ~~ }
    \item{\code{info}:}{Object of class \code{"list"} ~~ }
  }
}
\section{Extends}{
Class \code{"\linkS4class{itemsets}"}, directly.
Class \code{"\linkS4class{associations}"}, directly.
}
\section{Methods}{
  \describe{
    \item{inspect}{\code{signature(x = "disjunctive_itemsets")}: ... }
	 }
}
\references{
%%  ~~put references to the literature/web site here~~
}
\author{
%%  ~~who you are~~
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%%  ~~objects to See Also as \code{\link{~~fun~~}}, ~~~
%%  ~~or \code{\linkS4class{CLASSNAME}} for links to other classes ~~~
}
\examples{
showClass("disjunctive_itemsets")
}
\keyword{classes}
