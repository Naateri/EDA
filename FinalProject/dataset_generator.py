from random import uniform
import csv

#modify this as you like
dimensions = 75
min_number = -1000
max_number = 1000
total_data = 1000000

file_name = "dataset_d" + str(dimensions) + "pts" + str(total_data) + ".csv"

csvData = list()
dimensions -= 2

#creation of dataset

cur_data = list()
cur_data.append("Latitude")
cur_data.append("Longitude")
for i in range(dimensions):
    cur_dimension = "Dimension" + str(i+1)
    cur_data.append(cur_dimension)

csvData.append(cur_data)

for i in range(total_data):
    cur_data = list()
    #first north-south coordinates, then east-west
    cur_data.append(uniform(-90, 90))
    cur_data.append(uniform(-180, 180))
    for dim in range(dimensions):
        cur_data.append(uniform(min_number, max_number))
    csvData.append(cur_data)

with open(file_name, 'w') as csvFile:
    writer = csv.writer(csvFile)
    writer.writerows(csvData)

csvFile.close()

print("Data created on " + file_name)
        
