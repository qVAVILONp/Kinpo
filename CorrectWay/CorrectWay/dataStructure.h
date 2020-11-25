/*!
*\file dataStructure.h
*/
#include <QList>
#include <algorithm>
#include <iostream>
#include <fstream>
#include<QString>
#include <QStringList>
#include <map>
#include "tinyxml2.h"
#include <QDir>
#include <QTextStream>
#include <locale.h>
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
    ERROR_CMD,                                                  ///< ошибка неверного числа аргументов

    ERROR_FILE,                                                  ///< ошибка открытия файла для чтения
    ERROR_NO_EXPRESSION_AND_TEXT,     ///< ошибка

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
    ERROR_ANALYZE_EXP_INCORRECT_FUNC_NAME,   ///< ошибка некорректное имя функции

    ERROR_OUT_OF_RANGE_ARRAY,                               ///< ошибка выхода за пределы массива
    ERROR_EXP_INT,                                                        ///< ошибка у операции доступа к члену массива отсутствует целочисленный операнд
    ERROR_EXP_ARRAY,                                                   ///< ошибка у операции доступа к члену массива отсутствует операнд массив

    ERROR_EXP_CUSTOM,                                                ///< ошибка у операции доступа к сложному типу данных отсутствует операнд сложный тип данных
    ERROR_EXP_FIELD,                                                   ///< ошибка у операции доступа к сложному типу данных отсутствует операнд поле сложного типа данных
    ERROR_EXP_POINT_FIELD,                                        ///< ошибка у операции доступа к сложному типу данных операнд не является указателем на сложный тип данных
    ERROR_NO_CUSTOM_IN_DB,                                     ///< ошибка не удалось найти определение сложного типа данных

    ERROR_NO_TEMPLATE                                              ///< ошибка не удалось определить шаблон
};

/*!
 * \brief Представление операций шаблонов
 * \enum templates
 */
enum templates{
    oneArray_ = 1,          ///< шаблон одномерного массива
    moreArray_,          ///< шаблон многомерного массива
    pointer_,          ///< шаблон указателя
    field_,          ///< шаблон доступа к сложному типа данных
    pointerField_,          ///< шаблон доступа к сложному типа данных по указателю
    sum_,          ///< шаблон сумма
    sub_,          ///< шаблон вычитание
    mul_,          ///< шаблон умножение
    div_,          ///< шаблон деления
    funcCall_,          ///< шаблон вызова функции
    methodCall_,          ///< шаблон вызов метода
    methodPointCall_          ///< шаблон вызов метода по указателю
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

        /*!
         * \brief Конструктор по умолчанию
         */
        DataType (){};

        /*!
        * \brief Метод преобразования строкового представления размера массива в векторный вид
        * \param [in] strSize - строка, которая содержит размер массива в текстовой записи
        */
        void convertStrSizeToVectorSize(std::string strSize){
            std::vector<int> buff;
            QString buff2;
            QString strBuff = QString::fromStdString(strSize);
            QStringList substr = strBuff.split(",");
            QStringListIterator it(substr);

            while (it.hasNext()){
               buff2 = it.next();
               buff.push_back(buff2.toInt());
            }
            this->arraySize = buff;
        }

        /*!
         * \brief Конструктор заполняющий информацию
         */
        DataType(MainDataType mainDataType, std::vector<int> arraySize, DataType* basicType, std::string id){
            this->mainDataType = mainDataType;
            this->arraySize = arraySize;
            this->basicType = basicType;
            this->id = id;
        }

        /*!
        * \brief Метод преобразования строкового представления типа данных в реальный тип данных
        * \param [in] strDT - строка которую необходимо преобразовать в реальный тип данных
        */
        void convertStrDTToRealDT(QString strDT){
            QString strBuff = strDT;
            if(!arraySize.empty()){
                this->mainDataType = DT_ARRAY;
                this->basicType = new DataType();
                if(strBuff.contains("*")){
                    this->basicType->mainDataType = DT_POINTER;
                    strBuff.remove("*");
                    this->basicType->basicType = new DataType();
                    this->basicType->basicType->mainDataType = convertStrDTToBasicDT(strBuff);
                    strBuff = deleteSpaces(strBuff.toStdString());
                    this->basicType->basicType->id = strBuff.toStdString();}
                else {
                    this->basicType->mainDataType = convertStrDTToBasicDT(strBuff);
                    strBuff = deleteSpaces(strBuff.toStdString());
                    this->basicType->id = strBuff.toStdString();
                }
            } else  if(strBuff.contains("*")){
                        this->mainDataType = DT_POINTER;
                        this->basicType = new DataType();
                        strBuff.remove("*");
                        this->basicType->mainDataType = convertStrDTToBasicDT(strBuff);
                        strBuff = deleteSpaces(strBuff.toStdString());
                         this->basicType->id = strBuff.toStdString();
            } else {
                this->mainDataType = convertStrDTToBasicDT(strBuff);
                strBuff = deleteSpaces(strBuff.toStdString());
                 this->id = strBuff.toStdString(); }
        }

        /*!
        * \brief Метод распознания типов данных в поданной строке
        * \param [in] strBasicDT - строка в которой необходимо определить тип данных
        * \return  Тип данных найденный в строке
        */
        MainDataType convertStrDTToBasicDT(QString& strBasicDT){
                if(strBasicDT.contains ("int")){  strBasicDT.remove("int"); return DT_INT;}
                else if(strBasicDT.contains ( "float")){strBasicDT.remove("float"); return DT_FLOAT;}
                else if(strBasicDT.contains ("class")){strBasicDT.remove("class"); return DT_CLASS;}
                else if(strBasicDT.contains ("struct")){strBasicDT.remove("struct"); return DT_STRUCT;}
                else if(strBasicDT.contains ("union")){strBasicDT.remove("union"); return DT_UNION;}
        }

        /*!
        * \brief Метод удаления пробелов из поданной строки
        * \param [in] str - строка в которой необходимо удалить пробелы
        * \return  Преобразованная строка
        */
        QString deleteSpaces(std::string str){
            QString strBuff = QString::fromStdString(str);
            for(int i = 0; i<str.length(); i++){
                strBuff.remove(" ");
            }
            return strBuff;
        }
};

/*!
 * \class ErrorInfo
 * \brief Класс описаний ошибок в работе программы
 */
class ErrorInfo {

public:
   ErrorCode errorCode;
   std::vector<QString> errorContent;


   /*!
    * \brief Конструктор по умолчанию
    */
   ErrorInfo();

   /*!
    * \brief Конструкор, заполняющий информацию об ошибке
    * \param [in] errorCode - код ошибки
    * \param [in] errorContent - дополнительная информация, поясняющая ошибку
    */
   ErrorInfo( ErrorCode errorCode, std::vector<QString> errorContent){
       this->errorCode = errorCode;
       this->errorContent = errorContent;
   }

   /*!
    * \brief Метод создания ошибки
    */
   QString createErrorMessage(){
       QString error;

       switch (this->errorCode) {

       case ERROR_CMD:                                                  ///< ошибка неверного числа аргументов
           error = "Ошибка: ожидаемое кол-во аргументов командной строки: 3-4, действительное: " + this->errorContent[0];
           break;

       case ERROR_FILE:                                                  ///< ошибка открытия файла для чтения
           error = "Ошибка: не удалось открыть файл находящийся по пути: " + this->errorContent[0];
           break;
       case ERROR_NO_EXPRESSION_AND_TEXT:     ///<ошибка отсутствия полного комплекта данных txt файле
           error = "Ошибка: в txt файле отсутствует(-ют) необходимые данные, путь к файлу: " + this->errorContent[0];
           break;

       case ERROR_FILE_XML_OPEN:                      ///< ошибка открытия XML файла
           error = "Ошибка: при открытие xml файла находящегося по пути: " + this->errorContent[0] + " возникла ошибка: "+ this->errorContent[1];
           break;
       case ERROR_FILE_XML_ATTRIBUTE:             ///< ошибка отсутствия необходимого аттрибута узла
            error = "Ошибка: у дочернего узла " + this->errorContent[0] + this->errorContent[1] + " отсутствует необходимый аттрибут: " + this->errorContent[2];
           break;


       case ERROR_VAR_NAME:                                 ///< ошибка задания некорректного имени переменной
            error = "Ошибка: задано некорректное имя переменной:" + this->errorContent[0];
           break;
       case ERROR_VAR_TYPE:                                  ///< ошибка задания некорректного типа переменной
           error = "Ошибка: задан некорректный тип " + this->errorContent[1] + " у переменной: " + this->errorContent[0];
           break;
       case ERROR_VAR_CPP:                                   ///< ошибка имя переменной принадлежит списку зарезервированных слов
           error = "Ошибка: заданое имя переменной " + this->errorContent[0] + ", является ключевым словом";
           break;


       case ERROR_FUNC_NAME:                              ///< ошибка задания имени функции
           error = "Ошибка: задано некорректное имя функции:" + this->errorContent[0];
           break;
       case ERROR_FUNC_RETURNTYPE:                 ///< ошибка задания некорректного типа возврcase значения функции
            error = "Ошибка: задан некорректный тип " + this->errorContent[1] + " у возвращаемого значения функции: " + this->errorContent[0];
           break;
       case ERROR_FUNC_CPP:                                ///< ошибка имя функции принадлежит списку зарезервированных слов
            error = "Ошибка: заданое имя функции " + this->errorContent[0] + ", является ключевым словом";
           break;

       case ERROR_CUSTOMDATA_NAME:                ///< ошибка задания имя пользовательского типа данных
           error = "Ошибка: задано некорректное имя пользовательского типа данных:" + this->errorContent[0];
           break;
       case ERROR_CUSTOMDATA_CPP:                  ///< ошибка имя пользовательского типа данных принадлежит списку зарезервированных слов
           error = "Ошибка: заданое имя пользовательского типа данных: " + this->errorContent[0] + ", является ключевым словом";
           break;

       case ERROR_FIELD_NAME:                             ///< ошибка задания имени поля пользовательского типа данных
           error = "Ошибка:у пользовательского типа данных "+ this->errorContent[0] +  " задано некорректное имя поля:" + this->errorContent[1];
           break;
       case ERROR_FIELD_TYPE:                              ///< ошибка задания типа поля пользовательского типа данных
            error = "Ошибка: у пользовательского типа данных " + this->errorContent[0] + " задан некорректный тип данных поля " + this->errorContent[1];
           break;
       case ERROR_FIELD_CPP:                               ///< ошибка имя поля принадлежит списку зарезервированных слов
           error = "Ошибка: у пользовательского типа данных " + this->errorContent[0] + " имя поля является ключевым словом " + this->errorContent[1];
           break;

       case ERROR_METHOD_NAME:                       ///< ошибка задания имени метода
          error = "Ошибка:у пользовательского типа данных "+ this->errorContent[0] +  " задано некорректное имя метода:" + this->errorContent[1];
           break;
       case ERROR_METOD_RETURNTYPE:             ///< ошибка задания некорретного типа возвcase значения метода
           error = "Ошибка: у пользовательского типа данных " + this->errorContent[0] + " задан некорректный тип возвращаемого значения метода " + this->errorContent[1];
           break;
       case ERROR_METHOD_CPP:                         ///< ошибка имя метода принадлежит списку зарезервированных слов
          error = "Ошибка: у пользовательского типа данных " + this->errorContent[0] + " имя метода является ключевым словом " + this->errorContent[1];
           break;

       case ERROR_COPY_FUNC_NAME:                ///< ошибка найденный две функции с одинаковым именем
           error = "Ошибка: найдено несколько объявлений функций с именем:" + this->errorContent[0];
           break;

       case ERROR_COPY_VAR_NAME:                   ///< ошибка найденны две переменные с одинаковым именем
            error = "Ошибка: найдено несколько объявлений переменных с именем:" + this->errorContent[0];
           break;

       case ERROR_COPY_FIELD_NAME:               ///< ошибка найденны два поля одного пользовательского типа данных с одинаковым именем
           error ="Ошибка: у пользовательского типа данных " + this->errorContent[0] + " найдено несколько полей с одинаковым именем  " + this->errorContent[1];
           break;

       case ERROR_COPY_CUSTOMDATA_NAME:  ///< ошибка два пользовательского типа данных с одинаковым именем
           error ="Ошибка: найдено несколько пользовательских типов данных с именем " + this->errorContent[0];
           break;

       case ERROR_COPY_FUNC_VAR_NAME:       ///< ошибка найдены функция и переменная с одинаковыми именами
            error = "Ошибка: найдены имя переменной и функции с одинаковым именем:" + this->errorContent[0];
           break;

       case ERROR_COPY_METHOD_NAME:          ///< ошибка найденны два метода одного пользовательского типа данных с одинаковым именем
           error ="Ошибка: у пользовательского типа данных " + this->errorContent[0] + " найдено несколько методов с одинаковым именем  " + this->errorContent[1];
           break;

       case ERROR_ANALYZE_EXP_TRASH:                                    ///< ошибка обнаруженна последовательность символов: которую нельзя определить как корректный операнд или операцию
           error ="Ошибка: обнаруженна последовательность символов: которую нельзя определить как корректный операнд или операцию: " + this->errorContent[0];
           break;
       case ERROR_ANALYZE_EXP_MORE_VAR:                            ///< ошибка избыточное кол-во операндов
           error ="Ошибка: при обработке операции осталось избыточное кол-во операндов в кол-ве: " + this->errorContent[0];
           break;
       case ERROR_ANALYZE_EXP_MORE_OP:                              ///< ошибка оператор без операндов
           error ="Ошибка: у операции: " + this->errorContent[0] +  " не хватает операндов";
           break;

       case ERROR_ANALYZE_EXP_NO_VAR_IN_DB:                     ///< ошибка не найденно объявление переменной
           error ="Ошибка: не удалось найти объявление переменой с именем" + this->errorContent[0];
           break;
       case ERROR_ANALYZE_EXP_INCORRECT_FUNC_NAME:   ///< ошибка некорректное имя функции
            error ="Ошибка: вызов функции с некорректным именем: " + this->errorContent[0];
           break;

       case ERROR_OUT_OF_RANGE_ARRAY:                               ///< ошибка выхода за пределы массива
           error ="Ошибка: у массива с именем " + this->errorContent[1] + " осуществляется выход за пределы массива со значением" +  this->errorContent[0];
           break;
       case ERROR_EXP_INT:                                                        ///< ошибка у операции доступа к члену массива отсутствует целочисленный операнд
           error ="Ошибка: среди операндов " + this->errorContent[2] +", "+ this->errorContent[3] + " в " + this->errorContent[0]
                   + " в позиции операции доступа к элементу массива " + this->errorContent[1] + " отсутствует операнд целочисленного типа";
           break;
       case ERROR_EXP_ARRAY:                                                   ///< ошибка у операции доступа к члену массива отсутствует операнд массив
           error ="Ошибка: среди операндов " + this->errorContent[2] +", "+ this->errorContent[3] + " в " + this->errorContent[0]
                   + " в позиции операции доступа к элементу массива " + this->errorContent[1] + " отсутствует операнд массив";
           break;
       case ERROR_EXP_CUSTOM:
           error ="Ошибка: " + this->errorContent[0] + " не является членом " + this->errorContent[1] + " в " + this->errorContent[2] + " в позиции " + this->errorContent[3];
           break;
       case ERROR_EXP_FIELD:
           error ="Ошибка: " + this->errorContent[0] + " не является ни классом, ни структурой, ни объединением в " + this->errorContent[1] + " в позиции " + this->errorContent[2];
           break;
       case ERROR_EXP_POINT_FIELD:
           error ="Ошибка: " + this->errorContent[0] + " не является указателем ни на класс, ни на структуру, ни на объединение в " + this->errorContent[1] + " в позиции " + this->errorContent[2];
           break;
       case ERROR_NO_TEMPLATE:
           error ="Ошибка: не удалось определить шаблон как корректный: " + this->errorContent[0];
           break;
       case ERROR_NO_CUSTOM_IN_DB:
           error = "Ошибка: не удалось найти объявление" + this->errorContent[0];
           break;
       }
       return error;
   }
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
    std::vector<VariableInfo*> variablesInfo;               ///< набор описаний переменных
    std::vector<FunctionInfo*> functionsInfo;             ///< набор описаний функций
    std::vector<CustomDataInfo*> customDataInfo;   ///< набор описаний пользовательских типов данных
};

/*!
*\brief Типы вершин графа
*\enum NodeType
*/
enum NodeType
{
    constant_int,                       ///<константа целочисленная
    constant_float,                    ///<константа вещественная
    variable,                            ///< переменная
    oper,                                  ///< оператор
};

/*!
*\enum Operator
*\brief Список операторов
*/
enum Operator
{
    arrayItem,                         ///< "[]"
    mul,                                 ///< "*"
    arrow,                              ///< "->"
    dot,                                 ///< "."
    dv,                                  ///< "/"
    sub,                                 ///< "-"
    add,                                ///< "+"
    func,                               ///< вызов функции(метода)
    pointer                          ///< непрямое обращение
};

/*!
*\class TreeNode
*\brief Дерево
*/
class TreeNode
{
public:
    std::vector<TreeNode*> nodes;   ///< Список указателей на дочерние вершины
    NodeType type;                          ///< Тип вершины
    Operator op;                               ///< Оператор в вершине
    std::string id;                              ///< Идентификатор переменной
    double value;                                ///< Значение константы
    int pos;                                      ///< Позиция в обратной польской записи

    /*!
    *\brief Конструктор по умолчанию
    */
    TreeNode() { }

    /*!
    *\brief Создать оператор
    */
    TreeNode(Operator o, std::vector<TreeNode*> n)
    {
        type = oper;
        op = o;
        nodes = n;
    }

    /*!
    *\brief Создать переменную
    */
    TreeNode(std::string varName)
    {
        type = variable;
        id = varName;
    }

    /*!
        *\brief Создать константу
        */
    TreeNode(double v)
        {
            value = v;
        }

    /*!
        * \brief Получить текстовое представление оператора на русском языке
        * \return - текстовое представление
        */
        std::string operatorStringText()
        {
            //массив определения строкового представления оператора
            std::string strs[] = {
                "доступ к члену массива",  "multable", "arrow", "dot", "divide", "subtract", "addiction", "function call", "pointer"
            };

            return strs[op];
        }

    /*!
    * \brief Получить строковое представление оператора
    * \return - строковое представление
    */
    std::string operatorString(){
    //массив определения строкового представления оператора
    std::string strs[] = {"____",  "*", "->", ".", "/", "-", "+", "____", "*"};
    return strs[op];
    }

    /*!
    * \brief Получить количество операндов, необходимых для оператора
    * \return - количество необходимых операторов
    */
    int operandsCount(){
    //массив определения количества операндов у оператора
    int defOperandsCount[] = { 2, 2, 2, 2, 2, 2, 2, -1, 1 };
    return defOperandsCount[op];
    }

};
