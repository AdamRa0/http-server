source_directory := src
binary_directory := bin
data_structures_directory := DataStructures

executable := server

files := $(source_directory)/server.c $(source_directory)/http_req_parser.c $(source_directory)/$(data_structures_directory)/hash_table.c $(source_directory)/$(data_structures_directory)/linked_list.c

build:
	gcc $(files) -o $(binary_directory)/$(executable)

clean:
	rm $(binary_directory)/$(executable)