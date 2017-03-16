all:
	gcc -o htest htest.c
	gcc -fPIC -shared -o hook.so hook.c -ldl


run:
	LD_PRELOAD=./hook.so ./htest 123
