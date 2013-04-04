args=(commandArgs(TRUE))
if(length(args)==0)
{
	print("No arguments supplied.")
}

csvFile <- args[1]
scriptFile <- args[2]

resultFile <- "./results/Stride/Stride.pdf" # 

# If not already, install ggplot2 library
if (!require("ggplot2"))
{
	install.packages("ggplot2")
	library("ggplot2")
}
require("reshape2")
require("scales")

preDefinedValues <- c("x","par_jumps","par_stride")
measuredVariables <- c("random_PAPI_TOT_CYC_y", "sequential_forwards_PAPI_TOT_CYC_y")

# typeOfPlot <- geom_line() + geom_point()
xAxisName <- "Stride in Bytes"
yAxisName <- "CPU Cycles per Element"
yDivider <- 4096
fontTheme <- "Times"
fontSize <- 18

figureWidth <- 8
figureHeight <- 6

# Convert data into a readable table for ggplot (in "long format")
data <- melt(read.table(csvFile, header=TRUE),
	id.vars=preDefinedValues,
	measure.vars=measuredVariables,
	variable.name="measuring",
	value.name="value")

# Rename variables in column "measuring"
levels(data$measuring)[levels(data$measuring)=="random_PAPI_TOT_CYC_y"] <- "Random"
levels(data$measuring)[levels(data$measuring)=="sequential_forwards_PAPI_TOT_CYC_y"] <- "Sequential"

# Just show the actual table on the console
print(data)

# The actual plot command with several options
myplot = (ggplot(data=data, aes(x=x, y=value, group=measuring, colour=measuring))
+ geom_line() + geom_point()
+ scale_x_continuous(xAxisName,
	trans="log2",
	breaks=trans_breaks("log2", function(x) 2^x)(2^0:2^20),
	labels=trans_format("log2", math_format(2^.x)))
+ scale_y_continuous(yAxisName,
	breaks=seq(0*yDivider,600*yDivider,100*yDivider),
	labels=function(x)x/yDivider)
+ theme(text=element_text(family=fontTheme, size=fontSize, face="plain"),
	legend.title=element_text(colour="White"), # To make the legend title invisible
	legend.direction="vertical",
	legend.position="bottom"))

ggsave(filename=resultFile, plot=myplot, width=figureWidth, height=figureHeight)