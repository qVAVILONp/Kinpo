#include "function.h"

void readFileContent (const std::string& filePath, std::string& fileContent, std::vector<ErrorInfo>& errorsInfo){

    std::ifstream file(filePath);                 // Попробовать открыть файл для чтения

    if(file.is_open()){                             // Если файл доступен
        fileContent.clear();                       // Очистить место для загрузки содержимого файла

        std::string temp = "";                    // Содержимое строки файла
        while(std::getline(file, temp)){       // Пока не считали все строки файла
            fileContent += temp + "\n";      // Считать очередную строку содержимого файла
        }
    } else {errorsInfo.push_back(ErrorInfo(ERR_FILE, {QString::fromStdString(filePath)}));}        // Иначе считать, что возникла ошибка при чтении файла
}

bool readXMLTreeFromFile(const std::string& xmlFilePath, tinyxml2::XMLDocument& xmlDoc, std::vector<ErrorInfo>& errorsInfo){
    bool isFileNotEmpty = true;                    // Признак наличия содержимого в файле
    std::string xmlFileContent;                       // Содержимое XML файла
    std::vector<ErrorInfo> currErrorsInfo;    // Набор ошибок чтения XML дерева из файла

    readFileContent(xmlFilePath, xmlFileContent, currErrorsInfo);  // Чтение содержимого XML файла
    isFileNotEmpty = (xmlFileContent != "");

    if(!xmlFileContent.empty() && currErrorsInfo.empty()){ // Если не было ошибок при чтении XML файла и XML файл не пустой
        xmlDoc.Parse(xmlFileContent.c_str());                      // Получение XML дерева из содержимого XML файла

        if(xmlDoc.Error() ){// Считать, что возникла ошибка, если были ошибки при получении XML дерева
         currErrorsInfo.push_back( ErrorInfo( ERROR_FILE_XML_OPEN, { { QString::fromStdString(xmlFilePath) }, { QString::fromStdString(xmlDoc.ErrorStr()) } }  ) );
    }
        errorsInfo.insert(errorsInfo.end(), currErrorsInfo.begin(), currErrorsInfo.end());      // Добавить полученные ошибки в общий набор ошибок работы программы
    }
    return isFileNotEmpty;
}

const std::vector<VariableInfo*> readVarsInfoFromXML(const tinyxml2::XMLDocument& docXML, std::vector<ErrorInfo>&errorsInfo ){
   std::vector<VariableInfo*> varsInfo; // Результирующий набор описаний переменных

    tinyxml2::XMLElement* variablesNode = (tinyxml2::XMLElement*)docXML.FirstChildElement("variables"); //Перейти в тело родительского узла
    if(variablesNode != nullptr){
    tinyxml2::XMLElement* varNode = variablesNode->FirstChildElement("var");  // Получить описание первой переменной
    while (varNode != nullptr && errorsInfo.empty()){ // Пока не просмотрели все переменные
    QString varName;      //Имя переменной
    std::string varTypeStr;   //Тип данных переменной в строковом представлении
    DataType varType(DT_NONE, {}, NULL, "");       //Тип данных переменной
    std::string varSizeStr;         //Размер массива в строковом представлении

    if ( varNode->Attribute("name") != nullptr ){
    varName = varNode->Attribute("name");             // Получить имя переменной
    }else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"varibles", "name"}));}
    if ( varNode->Attribute("type") != nullptr ){
    varTypeStr = varNode->Attribute("type") ;          // Получить тип данных переменной в строковом представлении
    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"varibles", "type"}));}
    if ( varNode->Attribute("size") != nullptr ){          // Если в данном родительском узле есть элемент отвечающий за размер массива
     varSizeStr =  varNode->Attribute("size");  // Получить размер массива
     varType.convertStrSizeToVectorSize(varSizeStr);
    }
    //Проверить корректность переменной
    if(!checkCorrectVarName(varName)){    //Провеверить корректность имени переменной
        errorsInfo.push_back(ErrorInfo(ERROR_VAR_NAME, {varName}));
    }
    if(isCppKeyword(varName)){               //Провеверить не является ли переменная ключевым словом
        errorsInfo.push_back(ErrorInfo(ERROR_VAR_CPP, {varName}));
    }
    if(!isCorrectCppType(QString::fromStdString(varTypeStr))){       //Проверить корректность типа данных переменной
        errorsInfo.push_back(ErrorInfo(ERROR_VAR_TYPE, {varName, QString::fromStdString(varTypeStr)}));
    }
    if(errorsInfo.empty()){
        varType.convertStrDTToRealDT(QString::fromStdString(varTypeStr));
        varsInfo.push_back(new VariableInfo{varName.toStdString(), varType, varTypeStr});
        varNode = varNode->NextSiblingElement();                            //Получить описание след. переменной
    }
    }
    if(!errorsInfo.empty()){
        varsInfo.clear();
    }
    }
    return varsInfo;
}

const std::vector<FunctionInfo*> readFuncInfoFromXML(const tinyxml2::XMLDocument& docXML, std::vector<ErrorInfo>&errorsInfo){
    std::vector<FunctionInfo*> funcInfo; // Результирующий набор описаний функций
    tinyxml2::XMLElement* funcNode = (tinyxml2::XMLElement*)docXML.FirstChildElement("function");
    while (funcNode != nullptr && errorsInfo.empty()){ // Пока не просмотрели все переменные
        QString funcName;                                               //Имя функции
        std::string funcTypeStr;                                            //Тип возвращаемого значения функции в строковом представлении
        DataType funcType(DT_NONE, {}, NULL, "");       //Тип возвращаемого значения функции
        if ( funcNode->Attribute("name") != nullptr ){
        funcName = funcNode->Attribute("name");             // Получить имя функции
        }else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"function", "name"}));}
        if ( funcNode->Attribute("returnValue") != nullptr ){
        funcTypeStr = funcNode->Attribute("returnValue") ;          // Получить тип возвращаемого значения функции в строковом представлении
        } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"function", "returnValue"}));}
        //Проверить корректность функции
        if(!checkCorrectVarName(funcName)){    //Провеверить корректность имени функции
            errorsInfo.push_back(ErrorInfo(ERROR_FUNC_NAME, {funcName}));
        }
        if(isCppKeyword(funcName)){               //Провеверить не является ли имя функции ключевым словом
            errorsInfo.push_back(ErrorInfo(ERROR_FUNC_CPP, {funcName}));
        }
        if(!isCorrectCppType(QString::fromStdString(funcTypeStr))){       //Проверить корректность типа данных возвращаемого значения функции
            errorsInfo.push_back(ErrorInfo(ERROR_FUNC_RETURNTYPE, {funcName, QString::fromStdString(funcTypeStr)}));
        }
        if(errorsInfo.empty()){
            funcType.convertStrDTToRealDT(QString::fromStdString(funcTypeStr));
            funcInfo.push_back(new FunctionInfo{funcName.toStdString(), funcType, funcTypeStr});
            funcNode = funcNode->NextSiblingElement();          //Получить описание след. переменной
        }
        }
    if(!errorsInfo.empty()){
        funcInfo.clear();
    }
    return funcInfo;

    }

const std::vector <CustomDataInfo*> readCustomDataInfoFromXML(const tinyxml2::XMLDocument& docXML,  std::vector<ErrorInfo>&errorsInfo){
    std::vector<CustomDataInfo*> customDataInfo; // Результирующий набор описаний функций
     tinyxml2::XMLElement* funcNode = (tinyxml2::XMLElement*)docXML.FirstChildElement("struct");

}
