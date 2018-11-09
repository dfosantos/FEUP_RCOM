all:
	gcc appLayer.c dataLink.c utilities.c -o app
	
clean:
	rm -rf app