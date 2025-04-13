pwmodoro: main.c
	bash -c "gcc -o pwmodoro main.c `pkg-config --cflags --libs gtk+-3.0`"
