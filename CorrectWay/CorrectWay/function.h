#include "dataStructure.h"

/*!
 * \brief Функция чтения содержимого файла
 * \param [in] filePath  - путь к файлу для чтения
 * \param [out] fileContent  - полученное содержимое файла
 * \param [out] errorsInfo - набор ошибок чтения файла
 */
void readFileContent (const std::string& filePath, std::string& fileContent, std::vector<ErrorInfo>&errorsInfo);

/*!
 * \brief Функция чтения XML дерева из файла
 * \param [in] xmlFilePath - путь к файлу XML для чтения
 * \param [out] xmlDoc - выходное XML дерево
 * \param [out] errorsInfo - набор ошибок чтения XML дерева из файла
 * \return Признак наличия содержимого в XML файле
 */
bool readXMLTreeFromFile(const std::string& xmlFilePath, tinyxml2::XMLDocument& xmlDoc,  std::vector<ErrorInfo>&errorsInfo);

/*!
 * \brief Функция выделения информации о переменных
 * \param [in] docXML - XML дерево с описанием необходимых данных о переменных, функциях, пользовательских типах данных
 * \param [out] errorsInfo - набор описаний ошибок при чтении файла
 * \return Набор информации о переменных
 */
const std::vector<VariableInfo*> readVarsInfoFromXML(const tinyxml2::XMLDocument& docXML,  std::vector<ErrorInfo>&errorsInfo);

/*!
 * \brief Функция выделения информации о функциях
 * \param [in] docXML - XML дерево с описанием необходимых данных о переменных, функциях, пользовательских типах данных
 * \param [out] errorsInfo - набор описаний ошибок при чтении файла
 * \return Набор информации о функциях
 */
const std::vector<FunctionInfo*> readFuncInfoFromXML(const tinyxml2::XMLDocument& docXML,  std::vector<ErrorInfo>&errorsInfo);

/*!
 * \brief Функция выделения информации о пользовательских типах данных
 * \param [in] docXML - XML дерево с описанием необходимых данных о переменных, функциях, пользовательских типах данных
 * \param [out] errorsInfo - набор описаний ошибок при чтении файла
 * \return Набор информации о пользовательских типах данных
 */
const std::vector <CustomDataInfo*> readCustomDataInfoFromXML(const tinyxml2::XMLDocument& docXML,  std::vector<ErrorInfo>&errorsInfo);

/*!
* \brief Функция проверка корректности типов данных
* \param [in] str - строковое представление типа данных
* \return Может ли строка являться корректным типом данных
 */
bool isCorrectCppType(const QString &str);

/*!
* \brief Проверка может ли строка являться именем переменной
* \param [in] str - строка с возможным именем переменной
* \return Может ли строка являться именем переменной
 */
bool checkCorrectVarName(const QString &str);

/*!
* \brief Функция для определения является ли поданная строка ключевым словом
* \param str - строка с предпологаемым ключевым словом
* \return Может ли строка являться  ключевым словом
*/
bool isCppKeyword(const QString &str);

