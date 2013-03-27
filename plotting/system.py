#!/usr/bin/env python
# a line plot with errorbars
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import csv
import string
from matplotlib.ticker import MaxNLocator

def plot(csvFile, scriptFile):

	#defining default settings
	settings = {}
	settings['title'] = scriptFile[:scriptFile.find('.')]
	settings['xScale'] = 'linear'
	settings['xScaleBase'] = 10
	settings['xLabel'] = 'par_stride'
	settings['yLabel'] = 'CPU Cycles'
	settings['yDivider'] = 1
	settings['xDivider'] = 1
	settings['grid'] = 'none'
	settings['plotList'] = []
	settings['figureSize'] = (6.5, 6)
	settings['numberOfYTicks'] = False

	plots=list()

	# execfile(scriptFile)
	#executing the particular script so settings is filled with customization, errorcheck for crash prevention
	try:
		execfile(scriptFile)
	except IOError:
		print 'Couldn\'t open ' + scriptFile


	plotNumber = 1
	plotDir = csvFile[:csvFile.rfind('/') + 1]
	plotName = csvFile[csvFile.rfind('/'):]
	plotName = plotName[:plotName.find('.')]

	plotList = list()

	for settingsList in plots:
		csvReader = csv.reader(open(csvFile))
		line = csvReader.next()
		lineContents = line[0].split(' ')
		lineIndices = list()
		lineLabels = list()
		for entry in lineContents:
			for counter in settingsList['plotList']:
				if entry == counter:
					lineIndices.append(lineContents.index(entry))
					lineLabels.append(entry)

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


			mpl.rc('lines', linewidth=2)

			fig = plt.figure(1, figsize=settingsList['figureSize'])
			mpl.rcParams['axes.color_cycle'] = ['r', 'g', 'b', 'c']
			ax = fig.add_subplot(2,1,1)
			for i in range(0,len(lineIndices)):
				ax.plot(x, y[i], label=lineLabels[i])
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
