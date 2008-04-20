import sys


def stripBlock(block):
	while block[0].strip() == '':
		del block[0]
	while block[-1].strip() == '':
		del block[-1]
	return block


def splitLinesIntoBlocks(lines):
	blocks = []
	start = 0
	end = 0
	for i, line in enumerate( lines ):
		if line.strip() == '':
			# Reached the end of a block
			end = i
			if end > start:
				blocks.append( lines[start:end] )
			start = end
	end = len( lines )
	if end > start:
		blocks.append( lines[start:end] )
	blocks = [ stripBlock( block )   for block in blocks ]
	blocks = [ block   for block in blocks   if len( block ) > 0 ]
	return blocks


def sortBlocks(blocks):
	translatedBlocks = []
	untranslatedBlocks = []
	for block in blocks:
		bBlockTranslated = True
		bGettingMsgId = False
		bGettingMsgStr = False
		msgid = ''
		msgstr = ''
		for line in block:
			if line.startswith( 'msgid' ):
				line = line[5:]
				bGettingMsgId = True
				bGettingMsgStr = False
			elif line.startswith( 'msgstr' ):
				line = line[6:]
				bGettingMsgId = False
				bGettingMsgStr = True
			if bGettingMsgId:
				msgid += line.strip()[1:-1]
			if bGettingMsgStr:
				msgstr += line.strip()[1:-1]

		if msgid != ''  and msgstr == '':
			bBlockTranslated = False


		if bBlockTranslated:
			translatedBlocks.append( block )
		else:
			untranslatedBlocks.append( block )
	return translatedBlocks + untranslatedBlocks

def joinBlocks(blocks):
	lines = []
	for block in blocks:
		lines.extend( block )
		lines.append( '\n' )
	del lines[-1]
	return lines



if len( sys.argv ) != 3:
	print 'Usage:'
	print '\t%s <input_file> <output_file>'  %  ( sys.argv[0], )
else:
	inFilename = sys.argv[1]
	outFilename = sys.argv[2]
	lines = open( inFilename, 'r' ).readlines()
	blocks = splitLinesIntoBlocks( lines )
	sorted = sortBlocks( blocks )
	joined = joinBlocks( sorted )
	open( outFilename, 'w' ).writelines( joined )
