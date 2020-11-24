#include "function.h"

int main(int argc, char * argv[]){

    std::vector <ErrorInfo> errorsInfo; //Список ошибок работы программы
    std::string pathExp;
    std::string pathXml;
    setlocale(LC_ALL, "Rus");


    // Проверка наличия необходимых аргументов командной строки
    if(argc < 3 || argc > 4){
        QString numArg;
        errorsInfo.push_back(ErrorInfo{ERROR_CMD, {numArg.setNum(argc)}});
    }

    pathExp = argv[1];

    pathXml =  argv[2];


    // Считывание вспомогательной информации для вычисления выражения...
    ExpressionNeededInfo exprNeedInfo;
    std::vector<CustomDataInfo*> customDataInfo;
    std::vector<FunctionInfo*> functionsInfo;
    std::vector<VariableInfo*> variablesInfo;

    if(errorsInfo.empty()){

        tinyxml2::XMLDocument xmlDoc; //Документ для запси xml дерева

        bool isNotEmpty = readXMLTreeFromFile(pathXml,  xmlDoc,  errorsInfo);

        if(errorsInfo.empty() && isNotEmpty ){
            variablesInfo = readVarsInfoFromXML(xmlDoc, errorsInfo);
        }
        if(errorsInfo.empty()){
            functionsInfo = readFuncInfoFromXML(xmlDoc, errorsInfo);
            if(errorsInfo.empty()){
                customDataInfo = readCustomDataInfoFromXML(xmlDoc, errorsInfo);
            }
        }
    }

    if(errorsInfo.empty()){
        exprNeedInfo.variablesInfo = variablesInfo;
        exprNeedInfo.functionsInfo = functionsInfo;
        exprNeedInfo.customDataInfo = customDataInfo;
    }

    checkENIOncopyInXML(exprNeedInfo, errorsInfo);

    std::string strExpAndText;
    std::string strExp;
    std::string strText;

    if(errorsInfo.empty()){
        readFileContent(pathExp, strExpAndText, errorsInfo);
        if(errorsInfo.empty()){
            int numStr;
            for(int i = 0; i<strExpAndText.size(); i++){
                if(strExpAndText[i] == '\n'){
                    numStr++;
                }
            }
            if(numStr == 2){
                QString buf = QString::fromStdString(strExpAndText);
                QStringList buf_substr = buf.split("\n");
                strExp = buf_substr[0].toStdString();
                strText = buf_substr[1].toStdString();
            } else{errorsInfo.push_back(ErrorInfo{ERROR_NO_EXPRESSION_AND_TEXT, {}});}
        }
    }

    TreeNode* expTree = nullptr;

    if(errorsInfo.empty()){

        std::vector<std::string> subStrExp;

        splitExprToSubstr(strExp, subStrExp);

        QString qStrExp = QString::fromStdString(strExp);

        expTree = convertAnExpressionToTree (subStrExp, errorsInfo);

        if(errorsInfo.empty()){

            conv_combineAddMulOperators(expTree);

            conv_sort(expTree);

            bringTreeToStandartForm(expTree, exprNeedInfo, strExp, errorsInfo);

        }
    }

    TreeNode* textTree = nullptr;

    if(errorsInfo.empty()){

        textTree = convertTextToTree (strText,  errorsInfo);

        if(errorsInfo.empty()){

            conv_combineAddMulOperators(textTree);

            conv_sort(textTree);
        }
    }


    bool oneNode = textTree->nodes.size() == 0 || expTree->nodes.size() == 0;

    QString errorMessage;
    std::string strTree;
    bool isEqualTrees; //Эквивалентны ли деревья

    if(errorsInfo.empty()){
        isEqualTrees = compareTextTreeAndExpressionTree(textTree, expTree, errorMessage, exprNeedInfo, oneNode);



        convertTreeToString(expTree, strTree);
    }
    std::string nameOutFile = argv[3];
    writeMessage(nameOutFile, errorMessage, errorsInfo, strTree);

    return 0;

}
