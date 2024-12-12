# Decodificador Morse con Predicciones de Palabras por Modelo de IA

## Descripción

Este proyecto de IoT tiene como objetivo traducir el código Morse obtenido a través de un equipo físico. Utiliza un circuito con un controlador ESP32, programado en Arduino, que se conecta a un endpoint en un equipo de cómputo donde se ejecuta un modelo de procesamiento de lenguaje natural (NLP) en Python. El sistema permite realizar predicciones de palabras que el usuario puede aceptar a través del equipo físico.

## Tabla de Contenidos

- [Componentes del Proyecto](#componentes-del-proyecto)
  - [Equipo Físico](#equipo-físico)
  - [Modelo de NLP](#modelo-de-nlp)
  - [Servidor](#servidor)
  - [Nube y BDD](#nube-y-bdd)
  - [FrontEnd](#frontend)

## Componentes del Proyecto

### Equipo Físico

El equipo físico consiste en un circuito que permite el envío de código Morse mediante tres botones con las siguientes funcionalidades:

- **Enviar Código Morse**: Detecta puntos y líneas al pasar un umbral de 500 ms.
- **Enviar Señal de Aceptar**: Acepta la recomendación generada por el modelo de NLP.
- **Enviar Señal Borrar**: Borra el último carácter enviado al servidor.

El ESP32 se conecta a Internet y envía el carácter después de traducirlo de puntos y rayas.

### Modelo de NLP

Un modelo de IA que realiza predicciones de autocompletado basadas en la información obtenida a través del código Morse.

### Servidor

Se implementa un servidor en Python utilizando Flask, que maneja las peticiones de las diferentes partes del proyecto. Incluye métodos para:

- Recibir información del equipo físico (recibir Morse y procesar la predicción del modelo).
- Controlar la base de datos (enviar y recuperar información de Firestore).
- Proveer el backend para la página web.

### Nube y BDD

Se utiliza Firestore de Firebase como base de datos en la nube para almacenar información relevante del proyecto, como:

- Longitud de la palabra.
- Tiempos entre envíos de información.
- Palabra enviada.
- Cantidad de toques cortos que componen palabra
- Cantidad de toques largos que componen la palabra.

### FrontEnd

Se desarrolla una página web utilizando React y Vite que permite acceder a los datos recopilados y mostrar métricas. Además, muestra la palabra que se está escribiendo, la oración actual y la predicción resultante del modelo de IA.
