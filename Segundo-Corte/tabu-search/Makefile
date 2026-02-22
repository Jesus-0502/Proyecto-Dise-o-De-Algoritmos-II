# Compilador
CXX = g++

# Archivos comunes
COMMON_SRCS = NEH.cpp TS.cpp

# Targets principales
TARGETS = pfsp pfsp2 pfsp3 pfsp4 pfsp5

# Regla por defecto: compilar TODOS cuando solo pones 'make'
default: all

# Regla para compilar todos
all: $(TARGETS)

# Reglas individuales
pfsp: main.cpp $(COMMON_SRCS)
	$(CXX) main.cpp $(COMMON_SRCS) -o pfsp

pfsp2: main2.cpp $(COMMON_SRCS)
	$(CXX) main2.cpp $(COMMON_SRCS) -o pfsp2

pfsp3: main3.cpp $(COMMON_SRCS)
	$(CXX) main3.cpp $(COMMON_SRCS) -o pfsp3

pfsp4: main4.cpp $(COMMON_SRCS)
	$(CXX) main4.cpp $(COMMON_SRCS) -o pfsp4

pfsp5: main5.cpp $(COMMON_SRCS)
	$(CXX) main5.cpp $(COMMON_SRCS) -o pfsp5

# Limpiar archivos compilados
clean:
	rm -f $(TARGETS)

# Ayuda
help:
	@echo "Opciones disponibles:"
	@echo "  make        - Compilar todos los ejecutables"
	@echo "  make pfsp   - Compilar solo pfsp"
	@echo "  make pfsp2  - Compilar solo pfsp2"
	@echo "  ..."
	@echo "  make clean  - Eliminar todos los ejecutables"
	@echo "  make help   - Mostrar esta ayuda"

.PHONY: default all clean help