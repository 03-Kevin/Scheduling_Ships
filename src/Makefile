# Definir el compilador
CC = gcc

# Definir las banderas del compilador
CFLAGS = -Wall -g

# Definir los archivos fuente
SRCS = ready_queue.c main.c monitor.c mutex.c

# Definir los archivos objeto
OBJS = exes/ready_queue.o exes/main.o exes/monitor.o exes/mutex.o

# Definir los ejecutables
EXECUTABLES = exes/main exes/monitor

# Regla por defecto
all: $(EXECUTABLES)

# Crear el directorio de ejecutables si no existe
exes:
	mkdir -p exes

# Regla para compilar ready_queue.o
exes/ready_queue.o: ready_queue.c ready_queue.h mutex.h | exes
	$(CC) $(CFLAGS) -c ready_queue.c -o exes/ready_queue.o

# Regla para compilar main.o
exes/main.o: main.c ready_queue.h mutex.h | exes
	$(CC) $(CFLAGS) -c main.c -o exes/main.o

# Regla para compilar monitor.o
exes/monitor.o: monitor.c ready_queue.h mutex.h | exes
	$(CC) $(CFLAGS) -c monitor.c -o exes/monitor.o

# Regla para compilar mutex.o
exes/mutex.o: mutex.c mutex.h | exes
	$(CC) $(CFLAGS) -c mutex.c -o exes/mutex.o

# Regla para enlazar el ejecutable main
exes/main: exes/main.o exes/ready_queue.o exes/mutex.o | exes
	$(CC) $(CFLAGS) -o exes/main exes/main.o exes/ready_queue.o exes/mutex.o

# Regla para enlazar el ejecutable monitor
exes/monitor: exes/monitor.o exes/ready_queue.o exes/mutex.o | exes
	$(CC) $(CFLAGS) -o exes/monitor exes/monitor.o exes/ready_queue.o exes/mutex.o

# Regla para limpiar los archivos objeto y ejecutables
clean:
	rm -f $(OBJS) $(EXECUTABLES)
	rm -rf exes

# Regla para mostrar ayuda
help:
	@echo "Comandos disponibles:"
	@echo "  make          Compila todos los ejecutables."
	@echo "  make exes     Crea el directorio de ejecutables si no existe."
	@echo "  make clean    Limpia los archivos objeto y ejecutables."
	@echo "  make help     Muestra esta ayuda."
