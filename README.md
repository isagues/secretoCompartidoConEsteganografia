# Shared Secret

ss (Shared Secret) es una programa implementado en C para la distribucion y recuperacion de un secreto en multiples imagenes portadoras basado en el algoritmo propuesto en el paper: [Sistema de Imagen Secreta Compartida con Optimización de la Carga Útil](https://scielo.conicyt.cl/pdf/infotec/v25n4/art21.pdf).

## Autores

- [Brandy, Tobias](https://github.com/tobiasbrandy)
- [Pannunzio, Faustino](https://github.com/Fpannunzio)
- [Sagues, Ignacio](https://github.com/isagues)

## Dependencias

Para el testing del programa se utiliza la libreria check. [Instrucciones para intalarla](https://libcheck.github.io/check/web/install.html). Puede ser necesario instalar "libcheck-dev" ademas de check.

## Compilacion

Para la compilacion del proyecto basta con correr `make all`. Se generar el binario `ss` en la raiz del proyecto.

## Ejecucion

El programa permite tanto distribuir un secreto como recuperar un secreto.

### Distribucion

```sh
ss d <path del secreto> <k> <path de las imagenes portadoras> [OPCIONES]
```

- `d` 
  - Indica que se esta realizando una distribucion.
- `<path del secreto>`
  - Es el path a la imagen que se desea esconder.
- `<k>`
  - Es el valor k utilizado por el algoritmo.
  - Indica la cantidad minima de sombras que hay que juntar para recuperar el secreto.
  - Debe ser mayor a 2 y menor o igual a la cantidad de imagenes portadoras.
  - El tamaño de la imagen debe ser divisible por k.
  - Mas informacion puede encontrarse en el paper.
- `<path de las imagenes portadoras>`
  - El path del directorio que contiene las imagenes que seran convertidas en sombras.
  - Las sombras seran dejadas en este mismo directorio.

#### Opciones extra para la distribucion

- `-o <path de las sombras>`
  - Permite cambiar el path del directorio donde se dejaran las imagenes.
  - Tanto los directorios intermedios como el directorio final deberan estar creados de antemano.
- `-p`
  - Habilita el uso de padding para el secreto. Util si el k que se desea usar no es divisor del secreto.
- `-g <polinomio generador>`
  - Permite utilizar un polinomio generador a eleccion.
  - El polinomio default es: `x^8+x^6+x^5+x^1+1` o, en notacion numerica `355`.

### Recuperacion

```sh
ss r <path del secreto> <k> <path de las sombras> [OPCIONES]
```

- `r` 
  - Indica que se esta realizando una recuperacion.
- `<path del secreto>`
  - Es el path donde se dejara el secreto recuperado.
  - Tanto los directorios intermedios como el directorio final deberan estar creados de antemano.
- `<k>`
  - Es el valor k utilziado por el algoritmo.
  - Debe ser el valor de k que se utilizo cuando se distribuyo el secreto.
  - Mas informacion puede encontrarse en el paper.
- `<path de las sombras>`
  - El path del directorio que contiene las sombras que seran utilizadas para recuperar el secreto.

#### Opciones extra para la distribucion

- `-p`
  - Habilita el uso de padding para el secreto. Debe ser usado si la distribucion fue con padding.
- `-g <polinomio generador>`
  - Permite utilizar un polinomio generador a eleccion.
  - El polinomio default es: `x^8+x^6+x^5+x^1+1` o, en notacion numerica `355`.
  - El polinomio que se use debe ser el mismo que se uso en la distribucion.

### Opciones genericas de ejecucion

- `-h`
  - Si la opcion esta presente, se imprime informacion de ayuda y se termina inmediatamente.
- `--version`
  - Si la opcion esta presente, se imprime informacion de la version y el programa y se termina inmediatamente.
- `-v`
  - Habilita el logeo por `stderr` de mensajes de tipo INFO, WARN, ERROR y FATAL.
  - Por default solo se muestran los errores de tipo FATAL, ya que estos conllevan a la finalizacion inmediata.
- `-d`
  - Habilita el logeo por `stderr` de mensajes de tipo DEBUG, INFO, WARN, ERROR y FATAL.
  - A los mensajes se le agrega la hora, archivo y linea donde se produjo la alerta.

## Creditos

- Libreria de log: [log.c](https://github.com/rxi/log.c).

