all:
	gcc appLayer.c dataLink.c utilities.c -o app 
	cp app ../
	
clean:
	rm -rf app