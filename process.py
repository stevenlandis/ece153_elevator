
floorData = []
for i in range(5):
	floorData.append([])

with open('out.txt') as dataFile:
	for line in dataFile:
		cleanData = list(map(float, line.split()))
		# print(cleanData)
		for i in range(5):
			# print(floorData[i])
			# print(cleanData[i])
			floorData[i].append(cleanData[i])
		# print(floorData)

offset = int(0.05/2*len(floorData[1]))
print(offset)

for l in floorData:
	l.sort()
	print('{} - {}'.format(l[offset], l[len(l)-offset]))
