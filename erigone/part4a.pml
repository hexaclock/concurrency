/* 
 * dir values:
 * 0 = N
 * 1 = S
 * 2 = W
 * 3 = E
*/

byte cartsInISect;

proctype controller(byte dir) {
	byte cnsz = 2;
	byte cartnums[2];
	byte i = 0;
	byte cart = 0;
	
	/* init cartnums */
	do
	:: i >= cnsz -> i = 0;
					break
	:: else ->
			cartnums[i] = i;
			i = i + 1
	od;
	
	do
	:: i >= cnsz -> break
	:: else ->	
		cart = cartnums[i];
		
		/* monitor_arrive */
		atomic {
		/* wait until intersection is ready */
		do
		:: (cartsInISect == 0) -> break
		od;
		/* add 1 cart to the intersection */
		cartsInISect = cartsInISect + 1;
		/* assert that there is only 1 cart in intersection */
		assert(cartsInISect == 1)
		}
		printf("cart %d from dir %d has arrived\n", cart, dir);
		printf("cart %d from dir %d is on the launch pad\n", cart, dir);
		
		
		/* monitor_cross */
		printf("cart %d from dir %d must wait for intersection\n", cart, dir);
		atomic {
		/* wait until cartsInISect == 1 */
		do
		:: (cartsInISect == 1) -> break
		od;
		/* assert that there is still only one cart in intersection */
		assert(cartsInISect == 1)
		}
		
		printf("cart %d from dir %d enters intersection\n", cart, dir);
		/* there would be a sleep statement here in the actual program */
		printf("cart %d from dir %d crosses intersection\n", cart, dir);
		
		/* monitor_leave */
		atomic {
		/* assert that there is still only one cart in intersection */
		assert(cartsInISect == 1);
		/* remove cart from intersection */
		cartsInISect = cartsInISect - 1;
		/* should be 0 carts in intersection now */
		assert(cartsInISect == 0)
		}
		
		printf("cart %d from direction %d left intersection\n", cart, dir);
		
		/* go on to next cart */
		i = i + 1
	od		
}

init {
	atomic {
		/* spawn a process for each direction */
		run controller(0);
		run controller(1);
		run controller(2);
		run controller(3)
	}
	(_nr_pr == 1) -> 
				printf("at end of simulation carts in intersection = %d\n", cartsInISect);

	/* make sure no carts are left in intersection */
	assert(cartsInISect == 0);
	printf("simulation passed assertion!\n");
}