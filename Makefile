binary_directory := bin
data_structures_directory := DataStructures
executable := server
files := server.c http_req_parser.c $(data_structures_directory)/hash_table.c $(data_structures_directory)/linked_list.c
source_directory := src

build:
	gcc $(source_directory)/$(files) -o $(binary_directory)/$(executable)