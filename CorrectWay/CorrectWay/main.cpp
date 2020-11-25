#include "function.h"

int main(int argc, char * argv[]){

    std::vector <ErrorInfo> errorsInfo; //Список ошибок работы программы
    std::string pathExp; //Путь к файлу с выражением и текстовой записью
    std::string pathXml; //Путь к файлу с необходимой информацией для обработки выражения
    setlocale(LC_ALL, "Rus"); //Необходимо для вывода русского языка в консоль и файл

    int numArg = argc; //Кол-во аргументов командной строки

    // Проверка наличия необходимых аргументов командной строки
    if(numArg < 3 || numArg > 4){

        errorsInfo.push_back(ErrorInfo{ERROR_CMD, {QString::number(numArg)}});// Вывести ошибку отсутствия необходимого кол-ва аргументов
    } else {

        //Записать пути к файлам
        pathExp = argv[1];

        pathXml =  argv[2];
    }

    // Считывание вспомогательной информации для вычисления выражения...
    ExpressionNeededInfo exprNeedInfo;
    std::vector<CustomDataInfo*> customDataInfo;
    std::vector<FunctionInfo*> functionsInfo;
    std::vector<VariableInfo*> variablesInfo;

    if(errorsInfo.empty()){

        tinyxml2::XMLDocument xmlDoc; //Документ для запси xml дерева

        bool isNotEmpty = readXMLTreeFromFile(pathXml,  xmlDoc,  errorsInfo); //Считать xml дерево

        if(errorsInfo.empty() && isNotEmpty ){
            variablesInfo = readVarsInfoFromXML(xmlDoc, errorsInfo); //Считывание информации о переменных
        }
        if(errorsInfo.empty()){
            functionsInfo = readFuncInfoFromXML(xmlDoc, errorsInfo);//Считывание информации о функциях
            if(errorsInfo.empty()){
                customDataInfo = readCustomDataInfoFromXML(xmlDoc, errorsInfo);// Считывание информации о пользовательских типах данных
            }
        }
    }

    //Если не возникло ошибок чтения xml дерева
    if(errorsInfo.empty()){
        exprNeedInfo.variablesInfo = variablesInfo;
        exprNeedInfo.functionsInfo = functionsInfo;
        exprNeedInfo.customDataInfo = customDataInfo;
    }

    //Проверить полученные данные
    checkENIOncopyInXML(exprNeedInfo, errorsInfo);

    std::string strExpAndText; //Строка с выражением и текстовой записью пути
    std::string strExp;; //Строка с выражением
    std::string strText;//Строка с текстовой записью пути

    int numStr = 0; //Кол-во строк в txt файле

    if(errorsInfo.empty()){
        readFileContent(pathExp, strExpAndText, errorsInfo); //Считать информацию из txt файла
        //Определить кол-во строк в txt файле
        if(errorsInfo.empty()){
            for(int i = 0; i<strExpAndText.size(); i++){
                if(strExpAndText[i] == '\n'){
                    numStr++;
                }
            }
            if(numStr == 2){
                //Разделение информации из файла на строку с выражением и строку с текстовой записью
                QString buf = QString::fromStdString(strExpAndText);
                QStringList buf_substr = buf.split("\n");
                strExp = buf_substr[0].toStdString();
                strText = buf_substr[1].toStdString();
            } else{errorsInfo.push_back(ErrorInfo{ERROR_NO_EXPRESSION_AND_TEXT, {}});} //Считать, что возникла ошибка при чтение информации из txt файла
        }
    }

    TreeNode* expTree = nullptr; //Дерево выражения

    if(errorsInfo.empty()){

        std::vector<std::string> subStrExp;

        splitExprToSubstr(strExp, subStrExp); //Разделить выражение в обратной польской записи на подстроки разделеные пробелами

        QString qStrExp = QString::fromStdString(strExp);

        expTree = convertAnExpressionToTree (subStrExp, errorsInfo); //Сборка дерева выражения

        if(errorsInfo.empty()){

            conv_combineAddMulOperators(expTree); //Схлопнуть коммутативные операторы

            conv_sort(expTree); //Отсортировать дерево

            bringTreeToStandartForm(expTree, exprNeedInfo, strExp, errorsInfo); //Привести дерево к эквивалентному виду
        }
    }

    TreeNode* textTree = nullptr; //Дерево по текстовой записи пути

    if(errorsInfo.empty()){

        textTree = convertTextToTree (strText,  errorsInfo); //Сборка дерево по текстовой записи пути

        if(errorsInfo.empty()){

            conv_combineAddMulOperators(textTree);

            conv_sort(textTree);
        }
    }

    QString errorMessage; //Сообщение об ошибке в прохождении пути
    std::string strTree = ""; //Строковое представление выражения
    bool isEqualTrees = false; //Эквивалентны ли деревья
    bool oneNode; //Является ли дерево - узлом

    if(errorsInfo.empty()){
        oneNode = textTree->nodes.size() == 0 || expTree->nodes.size() == 0;

        if(errorsInfo.empty()){
            isEqualTrees = compareTextTreeAndExpressionTree(textTree, expTree, errorMessage, exprNeedInfo, oneNode); //Сравнение деревьев
            convertTreeToString(expTree, strTree);
        }
    }

    std::string nameOutFile;//Имя выходного файла
    //Определить задал ли пользователь имя выходного файла
    if(argc == 4)
        nameOutFile = argv[3];

    //Записать сообщение о результате работы программы
    writeMessage(nameOutFile, errorMessage, errorsInfo, strTree, argv[0]);

    return 0;

}
