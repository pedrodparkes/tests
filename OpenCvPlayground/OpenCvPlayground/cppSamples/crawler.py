#!/usr/bin/env python3
import glob
import fileinput

def crawl():
	for fn in glob.glob('*.cpp'):
		fileName = fn.split('.')[0]
		functionToReplace = 'int main('
		newFunctionDef = 'int '+ fileName +'('
		headerFileName = fileName + '.h'
		repStatus = False
		# include header 
		with open(fn, 'r+') as file:
			lines = list(file)
			file.seek(0)
			content = file.read()
			file.seek(0)
			include = '#include '+'"'+headerFileName+'"'
			if not(include in content):
				file.write(include + '\n\n'+content)
		# insert namespace after last inluce or using
		max = -1 
		for n, line in enumerate(lines):
			if '#include' in line or 'using namespace' in line:
				max = n
		lines = [('\t'if n>max else '')+line for n, line in enumerate(lines)]	# intentation
#		lines = ['\t'if n>max else ''+line for n, line in enumerate(lines)]	# intentation
		lines.insert(max+1 if max!=0 else 0, 'namespace '+fileName+'\n{\n')
		lines.append('}')
		with open(fn, 'w+') as file:
			file.write(''.join(lines))
		# replace function definition
		res = None
		with open(fn, 'r+') as file:
			for line in file:
				line = line.rstrip()
				if functionToReplace in line:
					res = line.replace(functionToReplace, newFunctionDef).lstrip()
		print(res)
		with fileinput.FileInput(fn, inplace=True) as file:
			for line in file:
				if functionToReplace in line:
					repStatus = True
				print(line.replace(functionToReplace, newFunctionDef), end='')
#				print(line.strip())
#
#		print(result)
		# create header file
		if res:
			with open(headerFileName, 'w+') as headerFile:
				headerFile.write('namespace '+fileName+'\n{\n')
				headerFile.write('\t'+str(res)+';\n');
				headerFile.write('}\n')
		# return status
		print(fn + ' ' + str(repStatus))
