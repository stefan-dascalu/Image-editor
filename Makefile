build: clean
	gcc -Wall image_editor.c -o image_editor -g -Wextra -lm
clean:
	rm -rf image_editor