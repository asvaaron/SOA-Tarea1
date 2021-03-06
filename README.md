# SOA-Tarea1




## Compilar Proyecto

```shell
make
```

## Limpiar ejecutables

```shell
make clean
```


### Distribución Probabilística


Para poder modelar los tiempos de espera para 
la creación de los autos en ambos carriles se 
utiliza una distribución exponencial. 
Este tipo de distribución es muy utilizadada
en el análisis del comportamiento de las líneas de 
espera, de forma que modela que el proceso de 
llegada de los clientes, en este caso automóviles, 
ocurra de manera aleatoria.

Se entiende por aleatorio que la ocurrencia de un
evento no se ve afectado por la ocurrencia de un 
evento anterior y así sucesivamente. 
Este tipo de distribución es continua y 
al igual que la distribución geométrica tiene 
la característica de que sufre perdida de 
memoria dado la aleatoriedad que presenta.

En este caso se utiliza la función cuantil, la cual se denota de la siguiente manera:

![eq1](https://latex.codecogs.com/svg.latex?\normalsize&space;F^{-1}(p,\lambda)=\frac{-\ln(1-p)}{\lambda})


siendo: ![eq1](https://latex.codecogs.com/svg.latex?\normalsize&space;0<p<1 ), un número aleatorio entre cero y uno.
El valor de lambda para el carril izquierdo es una costante igual a 1.002 mientras 
que que el lambda del carril derecho es 1.005.

## Ejecutar proyecto

El número de carros debe de ser pasado utilizando dos parámetros enteros.
El primer argumento corresponde a la cantidad de carros del carril oeste (izquierdo)
mientras que el segundo es la cantidad de carros en el carril este (derecho).
Para el siguiente ejemplo se utilizan dos carros en el carril oeste y tres
carros en el carril este.


```shell
./main 2 3
```


