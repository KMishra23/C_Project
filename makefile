CC=gcc 
all_sources=main.c readheader_2.c ReadData.c WriteBMP.c  	 
all_objs=$(all_sources:.c=.o)					 

my_project: $(all_objs) 					 
		$(CC) -o my_project $(all_objs) 
%.o: %.c 							 
	$(CC) -c $(all_sources) 

clean:								 
	rm my_project *.o