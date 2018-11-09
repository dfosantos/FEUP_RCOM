all:
	gcc appLayer.c dataLink.c utilities.c -o app -Wall
	cp app ../
	
clean:
	rm -rf app