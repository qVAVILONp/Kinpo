/*!
*\file function.h
*/

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

/*!
* \brief Функция перевода обратной польской записи в дерево
* \param [in] strs - исходные подстроки выражения
* \param [in] expressionNeededInfo - необходимая информация для обработки выражения
* \param [out] errorsInfo - набор ошибок при выполнении функции
* \return  Указатель на вершину полученного дерева
*/
TreeNode* convertAnExpressionToTree (std::vector<std::string> strs, std::vector <ErrorInfo>& errorsInfo);

/*!
* \brief Функция получения строкового представления по дереву
* \param [in] tree - дерево для преобразования
* \param [out] path - пошаговый путь
*/
void convertTreeToString(TreeNode* tree, std::string& strout);

/*!
* \brief Функция для определения является ли поданная строка константой
* \param [in] str - строка с предпологаемой константой
* \return  Может ли строка являться константой
*/
bool isConstant(std::string& str);

/*!
* \brief Функция разбиения поданной строки на подстроки разделенные пробелами
* \param [in] expStr - строка из которой необходимо выделить подстроки
* \param [out] strs - вектор подстрок
*/
void splitExprToSubstr(const std::string& expStr, std::vector<std::string>& strs);

/*!
* \brief Функция приведения дерева к эквивалентному виду
* \param [in|out] tree - указатель на вершину приводимого дерева
* \param [in] expressionNeededInfo - необходимая информация для обработки дерева
* \param [in] polsk - строка с обратной польской записью
*/
void bringTreeToStandartForm(TreeNode* tree, ExpressionNeededInfo& expressionNeededInfo, std::string& polsk, std::vector<ErrorInfo>& errorList);

/*!
* \brief Функция перевода текстовой записи пути в дерево
* \param [in] textPath - исходная строка с текстовой записью
* \param [out] errorsInfo - набор ошибок при выполнении функции
* \return  Указатель на вершину полученного дерева
*/
TreeNode* convertTextToTree (std::string textPath,  std::vector <ErrorInfo>& errorsInfo);

/*!
* \brief Функция сравнения деревьев
* \param [in] tree1 - указатель на вершину первого дерева
* \param [in] tree2 - указатель на вершину второго дерева
* \return  Являются ли деревья равными
*/
bool compareTrees(TreeNode* tree1, TreeNode* tree2);

/*!
* \brief Функция определения цепочки отличных от указанного узла
* \param [in] tree - указатель на цепочный узел исследуемого дерева
* \param [in] operands - цепочка из операндов не являющимися изначальным узлом
*/
void getNearDiffOperands(TreeNode* tree, std::vector<TreeNode*>& operands);

/*!
* \brief Функция схлопывания цепочек коммутативных операторов
* \param [in|out] tree - указатель на исследуемое дерево
*/
void conv_combineAddMulOperators(TreeNode* tree);

/*!
* \brief Функция сортировки дерева
* \param [in|out] tree - указатель на узел исследуемого дерева
*/
void conv_sort(TreeNode* tree);

/*!
* \brief Функция определения является ли подданая строка шаблоном и определение операции данного шаблона
* \param [in] str - строка с предпологаемым шаблоном
* \param [out] temp - определенная операция
* \return Является ли строка шаблоном
*/
bool is_template(const QString &str, templates &temp);

/*!
* \brief Функция сравнения полученных деревьев из тестовой записи и из выражения
* \param [in] tree1 - дерево по текстовой записи
* \param [in] tree2 - дерево по выражению
* \param [out] errorString - строка с ошибкой
* \param [in] eni - необходимая информация
* \param [in] oneNode - признак того, что дерево состоит из одной вершины
* \return Могут ли деревья быть эквивалентными
*/
bool compareTextTreeAndExpressionTree(TreeNode* tree1, TreeNode* tree2, QString& errorString, ExpressionNeededInfo& eni, const bool oneNode);

/*!
* \brief Функция проверки необходимой информации
* \param [in] eni - необходимая информация
* \param [out] errorList - список ошибок при обработке необходимой информации
*/
void checkENIOncopyInXML(ExpressionNeededInfo& eni, std::vector<ErrorInfo>& errorList);


/*!
* \brief Функция записи итогового сообщения
* \param [in] nameOutFile - имя выходного файла
* \param [in] errorMessage - сообщение об ошибке пользователя
* \param [in] errorsInfo - сообщение об ошибке во входных данных
* \param [in] strout - текстовое представление пути
*/
void writeMessage(std::string nameOutFile, QString errorMessage, std::vector <ErrorInfo>& errorsInfo, std::string strout);
