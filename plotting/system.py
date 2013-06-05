#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import csv
import string
import sys
from optparse import OptionParser
from matplotlib.ticker import MaxNLocator


## Base class to plot with Python matplotlib.
#
#  Pyplot is the base class that people can derive from in your [benchmark].py,
#  to alter/add settings or plot methods to plot their data with python matplotlib.
class Pyplot:

	## @brief The constructor
	#
	#  Parses the given arguments to get csv file and benchmark name using OptionParser.
	#  Then initializes plotting by reading default settings and user given settings.
	#  Finalizes with plotting the data.
	def __init__(self):
		parser = self.getOptionParser()
		(self.options, args) = parser.parse_args(sys.argv, None)

		if self.options.file == None:
			parser.print_help()
			sys.exit(0)

		self.plots=list()
		self.settings = {}

		self.setDefaultSettings()
		self.setUp()
		self.plot(self.options.file)

	## Parses the given arguments
	#
	#  @return a parser that can be used to extract the arguments.
	def getOptionParser(self):
	    parser = OptionParser()
	    parser.add_option('-f', '--file', help="set csv file", type='string', dest='file')
	    parser.add_option('-n', '--name', help="set benchmarks name", type='string', dest='benchname')
	    return parser

	## Sets default settings that are needed by the script (but probably will be overwritten)
	def setDefaultSettings(self):

		#defining default settings
		self.settings['title'] = self.options.benchname
		self.settings['type'] ='line'
		self.settings['xScale'] = 'linear'
		self.settings['xScaleBase'] = 10
		self.settings['xLabel'] = ''
		self.settings['yLabel'] = ''
		self.settings['yDivider'] = 1
		self.settings['xDivider'] = 1
		self.settings['grid'] = 'none'
		self.settings['plotList'] = []
		self.settings['figureSize'] = (6.5, 6)
		self.settings['numberOfYTicks'] = False

		self.plots.append(dict(self.settings))

	## @brief Overwrite this to set default settings.
	#
	#  Function that allows to alter/add (in derived class) just few settings
	#  without having to overwrite the complete list of default settings.
	#  Will be automatically called in constructor after setDefaultSettings()
	def setUp(self):
		#overwrite in derived class
		return

	## @brief Invokes plotting.
	#
	#  Function that provides a default and general plotting method for normal plots
	#  as well as boxplots using custom settings that you specify in setUp().
	#  Should be overwritten for having completely different types of plots.
	#
	#  @param csvFile Path to the csv file that contains the data to be plotted.
	def plot(self, csvFile):

		plotNumber = 1
		plotDir = csvFile[:csvFile.rfind('/') + 1]
		plotName = csvFile[csvFile.rfind('/'):]
		plotName = plotName[:plotName.find('.')]

		plotList = list()

		for settingsList in self.plots:

			data = list()
			if settingsList['type'] == 'boxplot':
				data = self.getDataForBoxplot(csvFile, settingsList)
			else:
				data = self.getDataForNormalPlot(csvFile, settingsList)

			if data:

				mpl.rc('lines', linewidth=2)

				fig = plt.figure(1, figsize=settingsList['figureSize'])
				mpl.rcParams['axes.color_cycle'] = ['r', 'g', 'b', 'c']
				ax = fig.add_subplot(2,1,1)

				if settingsList['type'] == 'boxplot':
					for i in range(0,len(data[1])):
						ax.boxplot(data[1][i], positions=data[0])
				else:
					for i in range(0,len(data[1])):
						ax.plot(data[0], data[1][i], label=data[2][i])

				ax.set_title(settingsList['title'])
				ax.set_xlabel(settingsList['xLabel'])
				ax.set_ylabel(settingsList['yLabel'])
				ax.set_xscale(settingsList['xScale'], basex=settingsList['xScaleBase'])
				if settingsList['numberOfYTicks']:
					ax.yaxis.set_major_locator(MaxNLocator(settingsList['numberOfYTicks']))
				if settingsList['grid'] is 'yAxis':
					ax.yaxis.grid(True)

				# ax.annotate('Cache Linesize', (64, 500), xytext=None, xycoords='data', textcoords='data', arrowprops=None)
				
				ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.2), fancybox=True, shadow=True)

				resultPlot = plotDir + plotName + '_Py_' + str(plotNumber) + '.pdf'
				plotList.append(resultPlot)
				plt.savefig(resultPlot)
				plotNumber += 1
				plt.close()

		return plotList

	## The data grabbing function for normal plots.
	#
	#  @param csvFile Path to the csv file that contains the data.
	#  @param settingsList The actual used settings specified in setDefaultSetting() and/or setUp().
	#
	#  @return A list containing the ready-to-plot data.
	def getDataForNormalPlot(self, csvFile, settingsList):

		csvReader = csv.reader(open(csvFile))
		line = csvReader.next()
		lineContents = line[0].split(' ')
		lineIndices = list()
		lineLabels = list()
		for counter in settingsList['plotList']:
			for entry in lineContents:
				if type(counter) is tuple: 
					if entry == counter[0]:
						lineIndices.append(lineContents.index(entry))
						lineLabels.append(counter[1])
				else:
					if entry == counter:
						lineIndices.append(lineContents.index(entry))
						lineLabels.append(counter)

		if len(lineIndices) > 0:
			x = list()
			y = list()
			for index in lineIndices:
				z = list()
				y.append(z)

			for line in csvReader:
				lineContents = line[0].split(' ')
				x.append(float(lineContents[0]) / settingsList['xDivider'])
				for i in range(0,len(lineIndices)):
					y[i].append(float(lineContents[lineIndices[i]]) / settingsList['yDivider'])
			
			return [x, y, lineLabels]
		return False

	## The data grabbing function for boxplots.
	#
	#  @param csvFile Path to the csv file that contains the data.
	#  @param settingsList The actual used settings specified in setDefaultSetting() and/or setUp().
	#
	#  @return A list containing the ready-to-plot data.
	def getDataForBoxplot(self, csvFile, settingsList):

		csvReader = csv.reader(open(csvFile))
		line = csvReader.next()
		lineContents = line[0].split(' ')
		plotLabels = list()
		plotIndices = list()
		for counter in settingsList['plotList']:
			if type(counter) is tuple: 

				valueNumber = 0
				indices = list()
				while counter[0] + '_raw_' + str(valueNumber) in lineContents:
					indices.append(lineContents.index(counter[0] + '_raw_' + str(valueNumber)))
					valueNumber += 1
				if indices:
					plotIndices.append(indices)
					plotLabels.append(counter[1])

			else:
				valueNumber = 0
				indices = list()
				while counter + '_raw_' + str(valueNumber) in lineContents:
					indices.append(lineContents.index(counter + '_raw_' + str(valueNumber)))
					valueNumber += 1
				if indices:
					plotIndices.append(indices)
					plotLabels.append(counter)

		if len(plotIndices) > 0:
			x = list()
			y = list()
			for indexList in plotIndices:
				z = list()
				y.append(z)

			lineCount = 0
			for line in csvReader:
				lineContents = line[0].split(' ')
				x.append(float(lineContents[0]) / settingsList['xDivider'])
				for i in range(0,len(plotIndices)):
					y[i].append(list())
					for j in range(0,len(plotIndices[i])):
						y[i][lineCount].append(float(lineContents[plotIndices[i][j]]) / settingsList['yDivider'])
				lineCount += 1
				
			return [x, y, plotLabels]
		return False

	## @var plots
	#  List that will be filled with the settings for each plot.