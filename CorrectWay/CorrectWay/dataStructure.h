#include <algorithm>
#include <iostream>
#include <fstream>
#include<QString>
#include <QStringList>
#include <map>
#include "tinyxml2.h"

/*!
 * \brief Коды основных типов данных
 * \enum MainDataType
 */
enum MainDataType {

    DT_NONE,                                               ///< пустой тип данных

    DT_VOID, 												///< тип данных void
    DT_INT, 												    ///< целочисленный тип данных
    DT_FLOAT,												///< дробный тип

    DT_POINTER,										    ///< указательный тип данных
    DT_ARRAY,												///< тип данных массив

    DT_CLASS,                                               ///< пользовательский тип данных class
    DT_UNION,                                              ///< пользовательский тип данных union
    DT_STRUCT                                             ///< пользовательский тип данных struct
};

/*!
 * \brief Коды ошибок
 * \enum ErrorCode
 */
enum ErrorCode {
    // Основные коды ошибок
    ERR_CMD,                                                  ///< ошибка неверного числа аргументов

    ERR_FILE,                                                  ///< ошибка открытия файла для чтения+
    ERROR_NO_EXPRESSION,                         ///<ошибка отсутствия выражения в файле
    ERROR_NO_TEXT,                                     ///< ошибка отсутствия тестовой записи пути
    ERROR_WRITE,                                         ///< ошибка записи выходного файла

    ERROR_FILE_XML_OPEN,                      ///< ошибка открытия XML файла+
    ERROR_FILE_XML_ATTRIBUTE,             ///< ошибка отсутствия необходимого аттрибута узла+


    ERROR_VAR_NAME,                                 ///< ошибка задания некорректного имени переменной+
    ERROR_VAR_TYPE,                                  ///< ошибка задания некорректного типа переменной+
    ERROR_VAR_CPP,                                   ///< ошибка имя переменной принадлежит списку зарезервированных слов+


    ERROR_FUNC_NAME,                              ///< ошибка задания имени функции+
    ERROR_FUNC_RETURNTYPE,                 ///< ошибка задания некорректного типа возвр. значения функции+
    ERROR_FUNC_CPP,                                ///< ошибка имя функции принадлежит списку зарезервированных слов+

    ERROR_CUSTOMDATA_NAME,                ///< ошибка задания имя пользовательского типа данных
    ERROR_CUSTOMDATA_CPP,                  ///< ошибка имя пользовательского типа данных принадлежит списку зарезервированных слов

    ERROR_FIELD_NAME,                             ///< ошибка задания имени поля пользовательского типа данных
    ERROR_FIELD_TYPE,                              ///< ошибка задания типа поля пользовательского типа данных
    ERROR_FIELD_CPP,                               ///< ошибка имя поля принадлежит списку зарезервированных слов

    ERROR_METHOD_NAME,                       ///< ошибка задания имени метода
    ERROR_METOD_RETURNTYPE,             ///< ошибка задания некорретного типа возв. значения метода
    ERROR_METHOD_CPP,                         ///< ошибка имя метода принадлежит списку зарезервированных слов

    ERROR_COPY_FUNC_NAME,                ///< ошибка найденный две функции с одинаковым именем

    ERROR_COPY_VAR_NAME,                   ///< ошибка найденны две переменные с одинаковым именем

    ERROR_COPY_FIELD_NAME,               ///< ошибка найденны два поля одного пользовательского типа данных с одинаковым именем

    ERROR_COPY_CUSTOMDATA_NAME,  ///< ошибка два пользовательского типа данных с одинаковым именем

    ERROR_COPY_FUNC_VAR_NAME,       ///< ошибка найдены функция и переменная с одинаковыми именами

    ERROR_COPY_METHOD_NAME,          ///< ошибка найденны два метода одного пользовательского типа данных с одинаковым именем

    ERROR_ANALYZE_EXP_TRASH,                                    ///< ошибка обнаруженна последовательность символов, которую нельзя определить как корректный операнд или операцию
    ERROR_ANALYZE_EXP_MORE_VAR,                            ///< ошибка избыточное кол-во операндов
    ERROR_ANALYZE_EXP_MORE_OP,                              ///< ошибка оператор без операндов
    ERROR_ANALYZE_EXP_NO_VAR_IN_DB,                     ///< ошибка не найденно объявление переменной
    ERROR_ANALYZE_EXP_NO_FUNC_IN_DB,                  ///< ошибка не найдено объявление функции
    ERROR_ANALYZE_EXP_INCORRECT_FUNC_NAME,   ///< ошибка некорректное имя функции

    ERROR_OUT_OF_RANGE_ARRAY,                               ///< ошибка выхода за пределы массива
    ERROR_EXP_INT,                                                        ///< ошибка у операции доступа к члену массива отсутствует целочисленный операнд
    ERROR_EXP_ARRAY,                                                   ///< ошибка у операции доступа к члену массива отсутствует операнд массив

    ERROR_EXP_FIELD,                                                   ///< ошибка у операции доступа к полям сложного типа данных отсутствует операнд поле
    ERROR_EXP_CUSTOMDATA,                                      ///< ошибка у операции доступа к полям сложного типа данных отсутствует сложный тип даных
};

/*!
 * \class DataType
 * \brief Класс типов данных
 */
class DataType {
public:
        MainDataType mainDataType; 			    ///< основной тип данных
        std::vector<int> arraySize;						///< размер массива (используется, если тип данных массив)
        DataType* basicType;					        ///< базовый тип данных (используется, если тип данных является указателем или массивом)
        std::string id;                                         ///< идентификатор переменной
};

/*!
 * \class ErrorInfo
 * \brief Класс описаний ошибок в работе программы
 */
class ErrorInfo {
private:
   ErrorCode errorCode;
   std::vector<QString> errorContent;
};

/*!
 * \class VariableInfo
 * \brief Информация о переменной
 */
struct VariableInfo {
    std::string id;                                        ///< идентификатор переменной
    DataType dataType;                            ///< тип данных переменной
    std::string dataTypeStr;                        ///< тип данных переменной в строковом представлении
};

/*!
 * \class FunctionInfo
 * \brief Информация о функциях
 */
struct FunctionInfo {
    std::string id;                                ///< идентификатор функции
    DataType returnType;                ///< тип данных возвращаемого значения
    std::string returnTypeStr;              ///< тип данных возвращаемого значения в строковом представлении
};

/*!
 * \class CustomDataInfo
 * \brief Информация о пользовательском типе данных
 */
struct CustomDataInfo {
    std::string id;                                                     ///< идентификатор пользовательского типа данных
    MainDataType type;                                         ///< тип пользовательского типа данных (может быть равен только CLASS, UNION или STRUCT)
    std::vector<VariableInfo*> fields;                       ///< набор полей пользовательского типа данных
    std::vector<FunctionInfo*> methods;                 ///< набор методов пользовательского типа данных
};

/*!
 * \class ExpressionNeededInfo
 * \brief Класс вспомогательной информации для конвертации выражения
 */
struct ExpressionNeededInfo {
public:
    std::vector<VariableInfo*> variablesInfo;               ///< набор описаний переменных
    std::vector<FunctionInfo*> functionsInfo;             ///< набор описаний функций
    std::vector<CustomDataInfo*> customDataInfo;   ///< набор описаний пользовательских типов данных
};


