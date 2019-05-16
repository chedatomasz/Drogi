//
// Created by chedat on 16.05.19.
//

#ifndef DROGI_INPUT_H
#define DROGI_INPUT_H

/**
 * @brief Wczytuje kolejną linię standardowego wejścia.
 * W przypadku problemów z alokacją pamięci zamyka program z kodem wyjścia 0.
 * @return C-style string jeśli udało się wczytać, NULL jeśli linia ma niepoprawny format.
 */
char *readInNextLine();

/**
 * @brief Czyta bez zapisywania znaki do końca linii lub pliku - zależnie, które nastąpi pierwsze.
 */
void readToLineEnd();

/**
 * @brief Podgląda następny znak na wejściu standardowym nie usuwając go
 * @return Podejrzany znak
 */
int peek();

/**
 * @brief Zwraca początek kolejnego tokena, analogicznie do strtok
 * Napisana z powodu złego zachowania strtok przy wielu delimiterach,
 * i wielu innych upośledzeń tamtej funkcji.
 * @param line double pointer na przeszukiwany string. Jest modyfikowany.
 * @param delimiter znak, którym rozdzielamy wyrazy.
 * @return Początek kolejnego tokena
 */
char* tokenize(char** line, char delimiter);

#endif //DROGI_INPUT_H
