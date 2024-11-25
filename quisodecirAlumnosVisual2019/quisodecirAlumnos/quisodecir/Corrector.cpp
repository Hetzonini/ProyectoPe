/*****************************************************************************************************************
    UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
    FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

    Computadoras y programacion.
    Hetzon Israel Cruz Guerrero 425017027

    Quiso decir: Programa principal de la aplicacion de la distancia de Levenstein.

******************************************************************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "corrector.h"
#define ABCEDARIO "abcdefghijklmnñopqrstuvwxyzáéíóú"

//Funciones publicas del proyecto
void LimpiarPalabra(char* palabra) {
    int j = 0;
    for (int i = 0; palabra[i] != '\0'; i++) {
        if (isalnum((unsigned char)palabra[i]) || isspace((unsigned char)palabra[i])) {
            palabra[j++] = tolower((unsigned char)palabra[i]);
        }
    }
    palabra[j] = '\0';
}
/*****************************************************************************************************************
    Diccionario: Procesa un archivo para crear el diccionario de palabras y sus estadísticas de frecuencia.
    char *	szNombre                : Nombre del archivo de entrada.
    char	szPalabras[][TAMTOKEN] : Arreglo para almacenar las palabras únicas del diccionario.
    int		iEstadisticas[]         : Arreglo para almacenar la frecuencia de cada palabra.
    int &	iNumElementos           : Número total de palabras únicas procesadas.
******************************************************************************************************************/
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos) {
    FILE* fp = nullptr;
    char palabralimpia[NUMPALABRAS];
    iNumElementos = 0;

    // Intentar abrir el archivo en modo lectura
    errno_t err = fopen_s(&fp, szNombre, "r");
    if (err != 0 || fp == nullptr) {
        printf("No se pudo abrir el archivo %s.\n", szNombre);
        return;
    }

    // Leer línea por línea el archivo
    while (fgets(palabralimpia, sizeof(palabralimpia), fp) != nullptr) {
        LimpiarPalabra(palabralimpia); // Limpiar la línea para remover caracteres no deseados

        char* context = nullptr;
        char* palabra = strtok_s(palabralimpia, " \n\r", &context);

        // Procesar cada palabra encontrada en la línea
        while (palabra != nullptr) {
            if (iNumElementos >= NUMPALABRAS) {
                break;  // Evitar desbordamiento del arreglo
            }

            char palabraLimpia[TAMTOKEN];
            strcpy_s(palabraLimpia, TAMTOKEN, palabra);

            // Verificar si la palabra ya está en el arreglo
            int encontrada = -1;
            for (int i = 0; i < iNumElementos; i++) {
                if (strcmp(szPalabras[i], palabraLimpia) == 0) {
                    encontrada = i;
                    break;
                }
            }

            if (encontrada != -1) {
                // Si la palabra ya existe, incrementar su frecuencia
                iEstadisticas[encontrada]++;
            }
            else {
                // Si no existe, agregarla al arreglo
                strcpy_s(szPalabras[iNumElementos], TAMTOKEN, palabraLimpia);
                iEstadisticas[iNumElementos] = 1;
                iNumElementos++;
            }

            palabra = strtok_s(nullptr, " \n\r", &context);
        }
    }

    fclose(fp); // Cerrar el archivo
}

/*****************************************************************************************************************
    ListaCandidatas: Esta funcion recupera desde el diccionario las palabras validas y su peso
    Regresa las palabras ordenadas por su peso
    char	szPalabrasSugeridas[][TAMTOKEN],	//Lista de palabras clonadas
    int		iNumSugeridas,						//Lista de palabras clonadas
    char	szPalabras[][TAMTOKEN],				//Lista de palabras del diccionario
    int		iEstadisticas[],					//Lista de las frecuencias de las palabras
    int		iNumElementos,						//Numero de elementos en el diccionario
    char	szListaFinal[][TAMTOKEN],			//Lista final de palabras a sugerir
    int		iPeso[],							//Peso de las palabras en la lista final
    int &	iNumLista)							//Numero de elementos en la szListaFinal
******************************************************************************************************************/

void ListaCandidatas(
    char szPalabrasSugeridas[][TAMTOKEN],  // Lista de palabras candidatas
    int iNumSugeridas,                    // Número de palabras sugeridas
    char szPalabras[][TAMTOKEN],          // Diccionario de palabras
    int iEstadisticas[],                  // Frecuencia de las palabras en el diccionario
    int iNumElementos,                    // Número de palabras en el diccionario
    char szListaFinal[][TAMTOKEN],        // Lista final de palabras a sugerir
    int iPeso[],                          // Peso (frecuencia) de las palabras en la lista final
    int& iNumLista)                       // Número de elementos en la lista final
{
    iNumLista = 0; // Inicializar contador de la lista final


    for (int i = 0; i < iNumSugeridas; i++) {
        char palabraLimpia[TAMTOKEN];
        strcpy_s(palabraLimpia, TAMTOKEN, szPalabrasSugeridas[i]);
        LimpiarPalabra(palabraLimpia);

        if (strlen(palabraLimpia) == 0) {
            continue;
        }

        int encontrada = -1;
        for (int j = 0; j < iNumLista; j++) {
            if (strcmp(palabraLimpia, szListaFinal[j]) == 0) {
                encontrada = j;
            }
        }

        if (encontrada != -1) {
            iPeso[encontrada]++;
        }
        else {
            if (iNumLista >= NUMPALABRAS) {
                return;
            }

            for (int j = 0; j < iNumElementos; j++) {
                if (strcmp(palabraLimpia, szPalabras[j]) == 0) {
                    strcpy_s(szListaFinal[iNumLista], TAMTOKEN, palabraLimpia);
                    iPeso[iNumLista] = iEstadisticas[j];
                    iNumLista++;
                }
            }
        }
    }

    for (int i = 0; i < iNumLista - 1; i++) {
        for (int j = i + 1; j < iNumLista; j++) {
            if (iPeso[i] < iPeso[j]) {
                char tempPalabra[TAMTOKEN];
                strcpy_s(tempPalabra, TAMTOKEN, szListaFinal[i]);
                strcpy_s(szListaFinal[i], TAMTOKEN, szListaFinal[j]);
                strcpy_s(szListaFinal[j], TAMTOKEN, tempPalabra);

                int tempPeso = iPeso[i];
                iPeso[i] = iPeso[j];
                iPeso[j] = tempPeso;
            }
        }
    }
}
/*****************************************************************************************************************
    ClonaPalabras: Genera las combinaciones y permutaciones requeridas para el método.
    char *	szPalabraLeida            : Palabra base a partir de la cual se generan combinaciones.
    char	szPalabrasSugeridas[][TAMTOKEN]: Lista donde se almacenan las combinaciones generadas.
    int &	iNumSugeridas             : Número de combinaciones generadas.
******************************************************************************************************************/
void ClonaPalabras(char* szPalabraLeida, char szPalabrasSugeridas[][TAMTOKEN], int& iNumSugeridas) {
    iNumSugeridas = 0;

    // Longitud de la palabra base
    int longitud = strlen(szPalabraLeida);
    const char* alfabeto = ABCEDARIO;
    int alfabetolen = strlen(alfabeto);

    // Limitar el número máximo de combinaciones sugeridas para evitar sobrecarga
    const int MAX_SUGERENCIAS = 1000;

    // Generar palabras eliminando un carácter en cada posición
    for (int i = 0; i < longitud && iNumSugeridas < MAX_SUGERENCIAS; i++) {
        char palabraModificada[TAMTOKEN];
        strcpy_s(palabraModificada, TAMTOKEN, szPalabraLeida);
        memmove(&palabraModificada[i], &palabraModificada[i + 1], strlen(szPalabraLeida) - i);
        strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, palabraModificada);
    }

    // Generar palabras reemplazando cada carácter por cada letra del alfabeto
    for (int i = 0; i < longitud && iNumSugeridas < MAX_SUGERENCIAS; i++) {
        for (int j = 0; j < alfabetolen && iNumSugeridas < MAX_SUGERENCIAS; j++) {
            char palabraModificada[TAMTOKEN];
            strcpy_s(palabraModificada, TAMTOKEN, szPalabraLeida);
            palabraModificada[i] = alfabeto[j];
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, palabraModificada);
        }
    }

    // Generar palabras intercambiando caracteres adyacentes
    for (int i = 0; i < longitud - 1 && iNumSugeridas < MAX_SUGERENCIAS; i++) {
        char palabraModificada[TAMTOKEN];
        strcpy_s(palabraModificada, TAMTOKEN, szPalabraLeida);
        char temp = palabraModificada[i];
        palabraModificada[i] = palabraModificada[i + 1];
        palabraModificada[i + 1] = temp;
        strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, palabraModificada);
    }

    // Generar palabras insertando cada letra del alfabeto en cada posición
    for (int i = 0; i <= longitud && iNumSugeridas < MAX_SUGERENCIAS; i++) {
        for (int j = 0; j < alfabetolen && iNumSugeridas < MAX_SUGERENCIAS; j++) {
            char palabraModificada[TAMTOKEN];
            strncpy_s(palabraModificada, szPalabraLeida, i); // Copiar hasta la posición i
            palabraModificada[i] = alfabeto[j];             // Insertar una letra del alfabeto
            strcpy_s(&palabraModificada[i + 1], TAMTOKEN - i - 1, &szPalabraLeida[i]); // Copiar el resto
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, palabraModificada);
        }
    }
}
