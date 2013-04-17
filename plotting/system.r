# If not already, install ggplot2 library and load required packages
if (!require("ggplot2"))
{
	install.packages("ggplot2", repos='http://star-www.st-andrews.ac.uk/cran/')
	library("ggplot2")
}
require("reshape2")
require("scales")


args=(commandArgs(TRUE))
if(length(args)==0)
{
	print("No arguments supplied.")
}

csvFile <- args[1]
scriptFile <- args[2]


source(scriptFile)
