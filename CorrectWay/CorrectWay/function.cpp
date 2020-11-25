/*!
*\mainpage CorrectWay
*\brief Определение правильности пути к конечному элементу
*\author Печенов И.П. ПрИн-267
*/

/*!
*\file function.cpp
*/

#include "function.h"

void readFileContent (const std::string& filePath, std::string& fileContent, std::vector<ErrorInfo>& errorsInfo){

    std::ifstream file(filePath);                 // Попробовать открыть файл для чтения

    if(file.is_open()){                             // Если файл доступен
        fileContent.clear();                       // Очистить место для загрузки содержимого файла

        std::string temp = "";                    // Содержимое строки файла
        while(std::getline(file, temp)) {       // Пока не считали все строки файла
            fileContent += temp + "\n";      // Считать очередную строку содержимого файла
        }
        file.close();
    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE, {QString::fromStdString(filePath)}));}     // Иначе считать, что возникла ошибка при чтении файла

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
           }else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"var", "varibles", "name"}));}
           if ( varNode->Attribute("type") != nullptr ){
               varTypeStr = varNode->Attribute("type") ;          // Получить тип данных переменной в строковом представлении
           } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"var", "varibles", "type"}));}
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
        }else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"", "function", "name"}));}
        if ( funcNode->Attribute("returnValue") != nullptr ){
        funcTypeStr = funcNode->Attribute("returnValue") ;          // Получить тип возвращаемого значения функции в строковом представлении
        } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"", "function", "returnValue"}));}
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
            funcNode = funcNode->NextSiblingElement("function");          //Получить описание след. переменной
        }
        }
    if(!errorsInfo.empty()){
        funcInfo.clear();
    }
    return funcInfo;

    }

const std::vector <CustomDataInfo*> readCustomDataInfoFromXML(const tinyxml2::XMLDocument& docXML,  std::vector<ErrorInfo>&errorsInfo){
    QString fieldName;      //Имя поля
    QString nameCustom;  //Имя сложного типа данных
    DataType fieldType(DT_NONE, {}, NULL, ""); //Тип данных поля в реальном представлении
    std::string strTypeField;    //Тип данных поля в строковом представлении
    MainDataType typeCustom; //Строка содержащая тип данных (class, union, struct)
    QString methodName;  //Имя метода
    std::string returnTypeMethod; //Тип возвращаемого значения метода в строковом представлении
    DataType methodType(DT_NONE, {}, NULL, ""); //Тип возвращаемого значения метода в реальном представлении
    std::vector<VariableInfo*> fieldInfo; // Результирующий набор описаний полей
    std::vector<CustomDataInfo*> customInfo; // Результирующий набор описаний сложных типов данных
    std::vector<FunctionInfo*> methodInfo;// Результирующий набор описаний методов

    //Обработать дерево структуры
    tinyxml2::XMLElement* customNodeStruct = (tinyxml2::XMLElement*)docXML.FirstChildElement("struct");
    while(customNodeStruct != nullptr && errorsInfo.empty()){

        if(customNodeStruct->Attribute("name") != nullptr){
            nameCustom = customNodeStruct->Attribute("name");

            typeCustom = DT_STRUCT;
            tinyxml2::XMLElement*fieldNode = customNodeStruct->FirstChildElement("field");
            while(fieldNode != nullptr && errorsInfo.empty()){

                if(fieldNode->Attribute("name") != nullptr){
                    fieldName = fieldNode->Attribute("name");
                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"struct", "field", "name"}));}

                if(fieldNode->Attribute("typeF") != nullptr){
                    strTypeField = fieldNode->Attribute("typeF");

                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"struct", "field", "typeF"}));}

                if(!checkCorrectVarName(fieldName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, fieldName}));
                }
                if(isCppKeyword(fieldName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, fieldName}));
                }
                if(!isCorrectCppType(QString::fromStdString(strTypeField))){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(strTypeField)}));
                }

                if(!checkCorrectVarName(nameCustom)){
                    errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_NAME, {nameCustom}));
                }
                if(isCppKeyword(nameCustom)){
                    errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_CPP, {nameCustom}));
                }

                if(errorsInfo.empty()){
                    fieldType.convertStrDTToRealDT(QString::fromStdString(strTypeField));
                    fieldInfo.push_back(new VariableInfo{fieldName.toStdString(), fieldType, strTypeField});
                    fieldNode = fieldNode->NextSiblingElement("field");
                }
            }
            tinyxml2::XMLElement*methodNode = customNodeStruct->FirstChildElement("method");
            while(methodNode != nullptr && errorsInfo.empty()){

                if(methodNode->Attribute("name") != nullptr){
                    methodName = methodNode->Attribute("name");
                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"struct", "method", "name"}));}

                if(methodNode->Attribute("returnValue") != nullptr){
                    returnTypeMethod = methodNode->Attribute("returnValue");

                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"struct", "method", "returnValue"}));}

                if(!checkCorrectVarName(methodName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, methodName}));
                }

                if(isCppKeyword(methodName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, methodName}));
                }

                if(!isCorrectCppType(QString::fromStdString(returnTypeMethod))){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(returnTypeMethod)}));
                }

                if(errorsInfo.empty()){
                    methodType.convertStrDTToRealDT(QString::fromStdString(returnTypeMethod));
                    methodInfo.push_back(new FunctionInfo{methodName.toStdString(), methodType, returnTypeMethod});
                    methodNode = methodNode->NextSiblingElement("method");
                }
            }

            if(errorsInfo.empty()){
                customInfo.push_back(new CustomDataInfo{nameCustom.toStdString(), typeCustom, fieldInfo, methodInfo});
                fieldInfo.clear();
                methodInfo.clear();
                customNodeStruct = customNodeStruct->NextSiblingElement("struct");
            }
        } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"struct", "", "name"}));}
    }

    //Обработать дерева класса
    tinyxml2::XMLElement* customNodeClass = (tinyxml2::XMLElement*)docXML.FirstChildElement("class");
    while(customNodeClass != nullptr && errorsInfo.empty()){

        if(customNodeClass->Attribute("name") != nullptr){
            nameCustom = customNodeClass->Attribute("name");

            typeCustom = DT_CLASS;
            tinyxml2::XMLElement*fieldNode = customNodeClass->FirstChildElement("field");
            while(fieldNode != nullptr && errorsInfo.empty()){

                if(fieldNode->Attribute("name") != nullptr){
                    fieldName = fieldNode->Attribute("name");
                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"class", "field", "name"}));}

                if(fieldNode->Attribute("typeF") != nullptr){
                    strTypeField = fieldNode->Attribute("typeF");

                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"class", "field", "typeF"}));}

                if(!checkCorrectVarName(fieldName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, fieldName}));
                }
                if(isCppKeyword(fieldName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, fieldName}));
                }
                if(!isCorrectCppType(QString::fromStdString(strTypeField))){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(strTypeField)}));
                }
                if(!checkCorrectVarName(nameCustom)){
                    errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_NAME, {nameCustom}));
                }
                if(isCppKeyword(nameCustom)){
                    errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_CPP, {nameCustom}));
                }
                if(errorsInfo.empty()){
                    fieldType.convertStrDTToRealDT(QString::fromStdString(strTypeField));
                    fieldInfo.push_back(new VariableInfo{fieldName.toStdString(), fieldType, strTypeField});
                    fieldNode = fieldNode->NextSiblingElement("field");
                }
            }
            tinyxml2::XMLElement*methodNode = customNodeClass->FirstChildElement("method");
            while(methodNode != nullptr && errorsInfo.empty()){

                if(methodNode->Attribute("name") != nullptr){
                    methodName = methodNode->Attribute("name");
                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"class", "method", "name"}));}

                if(methodNode->Attribute("returnValue") != nullptr){
                    returnTypeMethod = methodNode->Attribute("returnValue");

                } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"class", "method", "returnValue"}));}

                if(!checkCorrectVarName(methodName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, methodName}));
                }

                if(isCppKeyword(methodName)){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, methodName}));
                }

                if(!isCorrectCppType(QString::fromStdString(returnTypeMethod))){
                    errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(returnTypeMethod)}));
                }

                if(errorsInfo.empty()){
                    methodType.convertStrDTToRealDT(QString::fromStdString(returnTypeMethod));
                    methodInfo.push_back(new FunctionInfo{methodName.toStdString(), methodType, returnTypeMethod});
                    methodNode = methodNode->NextSiblingElement("method");
                }
            }

            if(errorsInfo.empty()){
                customInfo.push_back(new CustomDataInfo{nameCustom.toStdString(), typeCustom, fieldInfo, methodInfo});
                fieldInfo.clear();
                methodInfo.clear();
                customNodeClass = customNodeClass->NextSiblingElement("class");
            }
        } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"class", "", "name"}));}
    }

         //Обработать дерева объединения
        tinyxml2::XMLElement* customNodeUnion = (tinyxml2::XMLElement*)docXML.FirstChildElement("union");
        while(customNodeUnion != nullptr && errorsInfo.empty()){

            if(customNodeUnion->Attribute("name") != nullptr){
                nameCustom = customNodeUnion->Attribute("name");

                typeCustom = DT_UNION;
                tinyxml2::XMLElement*fieldNode = customNodeUnion->FirstChildElement("field");
                while(fieldNode != nullptr && errorsInfo.empty()){

                    if(fieldNode->Attribute("name") != nullptr){
                        fieldName = fieldNode->Attribute("name");
                    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"union", "field", "name"}));}

                    if(fieldNode->Attribute("typeF") != nullptr){
                        strTypeField = fieldNode->Attribute("typeF");

                    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"union", "field", "typeF"}));}

                    if(!checkCorrectVarName(fieldName)){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, fieldName}));
                    }
                    if(isCppKeyword(fieldName)){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, fieldName}));
                    }
                    if(!isCorrectCppType(QString::fromStdString(strTypeField))){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(strTypeField)}));
                    }
                    if(!checkCorrectVarName(nameCustom)){
                        errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_NAME, {nameCustom}));
                    }
                    if(isCppKeyword(nameCustom)){
                        errorsInfo.push_back(ErrorInfo(ERROR_CUSTOMDATA_CPP, {nameCustom}));
                    }
                    if(errorsInfo.empty()){
                        fieldType.convertStrDTToRealDT(QString::fromStdString(strTypeField));
                        fieldInfo.push_back(new VariableInfo{fieldName.toStdString(), fieldType, strTypeField});
                        fieldNode = fieldNode->NextSiblingElement("field");
                    }
                }

                tinyxml2::XMLElement*methodNode = customNodeUnion->FirstChildElement("method");
                while(methodNode != nullptr && errorsInfo.empty()){

                    if(methodNode->Attribute("name") != nullptr){
                        methodName = methodNode->Attribute("name");
                    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"union", "method", "name"}));}

                    if(methodNode->Attribute("returnValue") != nullptr){
                        returnTypeMethod = methodNode->Attribute("returnValue");

                    } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"union", "method", "returnValue"}));}

                    if(!checkCorrectVarName(methodName)){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_NAME, {nameCustom, methodName}));
                    }

                    if(isCppKeyword(methodName)){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_CPP, {nameCustom, methodName}));
                    }

                    if(!isCorrectCppType(QString::fromStdString(returnTypeMethod))){
                        errorsInfo.push_back(ErrorInfo(ERROR_FIELD_TYPE, {nameCustom, QString::fromStdString(returnTypeMethod)}));
                    }

                    if(errorsInfo.empty()){
                        methodType.convertStrDTToRealDT(QString::fromStdString(returnTypeMethod));
                        methodInfo.push_back(new FunctionInfo{methodName.toStdString(), methodType, returnTypeMethod});
                        methodNode = methodNode->NextSiblingElement("method");
                    }
                }

                if(errorsInfo.empty()){
                    customInfo.push_back(new CustomDataInfo{nameCustom.toStdString(), typeCustom, fieldInfo, methodInfo});
                    fieldInfo.clear();
                    methodInfo.clear();
                    customNodeUnion = customNodeUnion->NextSiblingElement("union");
                }
            } else {errorsInfo.push_back(ErrorInfo(ERROR_FILE_XML_ATTRIBUTE, {"union", "", "name"}));}
        }

    return customInfo;
}

bool isCorrectCppType(const QString &str){
    QStringList creatTypeName{"struct", "union", "class"};
    QStringList typeName{"int", "float"};
    bool result = false;
    QRegExp regular("", Qt::CaseSensitive, QRegExp::RegExp2);
    for(int i = 0; i < creatTypeName.count() && result == false; i++){
        regular.setPattern(QString("\\s*" + creatTypeName[i] + "\\s*[a-z|A-Z|_]\\w*\\s*(?:\\*|(?:[\\(]\\s*[\\*]\\s*[\\)]\\s*))?"));
        result = regular.exactMatch(str);
    }

    for(int i = 0; i < typeName.count() && result == false; i++){
        regular.setPattern(QString("\\s*" + typeName[i] + "\\s*(?:\\*|(?:[\\(]\\s*[\\*]\\s*[\\)]\\s*))?"));
        result = regular.exactMatch(str);
    }
    return result;
}

bool checkCorrectVarName(const QString &str){
    QRegExp regForVariable("\\s*[a-z|A-Z|_]\\w*\\s*", Qt::CaseSensitive, QRegExp::RegExp2);
    return regForVariable.exactMatch(str);
}

bool isCppKeyword(const QString &str) {
    //
    QStringList keywords{"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
                        "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor",
                        "bool", "break", "case", "catch", "char", "char8_t", "char16_t",
                        "char32_t", "class", "compl", "concept", "const", "consteval",
                        "constexpr", "constinit", "const_cast", "continue", "co_await",
                        "co_return", "co_yield", "decltype", "default", "delete", "do",
                        "double", "dynamic_cast", "else", "enum", "explicit", "export",
                        "extern", "false", "float", "for", "friend", "goto", "if", "inline",
                        "int", "long", "not", "reinterpret_cast", "requires", "return", "short",
                        "signed", "sizeof", "static", "static_assert", "static_cast", "struct",
                        "switch", "synchronized", "template", "this", "thread_local", "throw",
                        "true", "try", "typedef", "typeid", "typename", "union", "unsigned",
                        "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"};
    QRegExp keyReg("", Qt::CaseSensitive, QRegExp::RegExp2);
    bool key = false;
    for(int i = 0; i < keywords.count() && key == false; i++){
        keyReg.setPattern(QString("\\s*" + keywords[i] + "\\s*"));
        key = keyReg.exactMatch(str);
    }
    return key;
}

bool isConstant(std::string& str)
{
    for (int i = 0; i < str.length(); i++)
        if (!isdigit(str[i]) && str[i] != '.')
            return false;

    return true;
}

TreeNode* convertAnExpressionToTree (std::vector<std::string> strs, std::vector <ErrorInfo>& errorsInfo)
{
    std::vector<TreeNode*> stackT;              //стек вершин дерева
    std::vector<int> stackI;                             //стек индексов
    bool correct = true;
    std::map<std::string, Operator> defOperator //словарь определения оператора в обратной польской записи
    {
        {"[]", arrayItem},
        {"*", mul},
        {"->", arrow},
        {".", dot},
        {"/", dv},
        {"-", sub},
        {"+", add},
        {"#*", pointer}
    };

    //для всех подстрок...
    for (int i = 0; i < strs.size(); i++)
    {
        TreeNode* curr = new TreeNode(); //вершина, создаваемая из текущей строки
        curr->pos = i;
        //определить тип подстроки...
        if(strs[i][0] == '@'){
            if(strs[i].size() > 2 && isdigit(strs[i][1])){
            QString strBuff = QString::fromStdString(strs[i]);
            strBuff.remove("@");
            QRegExp reg("[0-9]*", Qt::CaseSensitive, QRegExp::RegExp2);
            reg.indexIn(strBuff);
            int numArg =  QString(reg.capturedTexts()[0]).toInt();
            strBuff.remove(QString::number(numArg));
            if(checkCorrectVarName(strBuff)){
                //если в стеке осталось элементов меньше, чем количество требуемых операндов для текущего оператора
                if (stackT.size() < numArg)
                {
                    correct =false;
                    errorsInfo.push_back(ErrorInfo(ERROR_ANALYZE_EXP_MORE_OP, {QString::fromStdString(strs[i])}));
                }
                for(int j = 0; j<numArg; j++){
                TreeNode* t = stackT.back();
                curr->nodes.insert(curr->nodes.begin(), t);
                stackT.pop_back();
                stackI.pop_back();
            }
                curr->op = func;
                curr->type = oper;
                curr->id = strBuff.toStdString();
            } else {errorsInfo.push_back(ErrorInfo(ERROR_ANALYZE_EXP_INCORRECT_FUNC_NAME, {strBuff})); correct = false;}
            }
        }
        else if (defOperator.find(strs[i]) != defOperator.cend()) //если подстрока является оператором
        {
            curr->type = oper;
            curr->op = defOperator[strs[i]];
            //если в стеке осталось элементов меньше, чем количество требуемых операндов для текущего оператора
            if (stackT.size() < curr->operandsCount()){
                 errorsInfo.push_back(ErrorInfo(ERROR_ANALYZE_EXP_MORE_OP, {QString::fromStdString(strs[i])}));
                correct =false;
            }

            //извлечь необходимое количество элементов из стека и сделать их операндами текущего оператора
            for (int j = 0; j < curr->operandsCount(); j++)
            {
                TreeNode* t = stackT.back();
                curr->nodes.insert(curr->nodes.begin(), t);
                stackT.pop_back();
                stackI.pop_back();
            }
        }
        else if (isConstant(strs[i])) //если подстрока является константой
                {
                    curr->type = strchr(strs[i].c_str(), '.') != NULL ? constant_float : constant_int;
                    curr->value = QString::fromStdString(strs[i]).toFloat();
                }
        else
        {
            bool isCorrectVarName = checkCorrectVarName(QString::fromStdString(strs[i]));
            if(!isCorrectVarName){errorsInfo.push_back(ErrorInfo(ERROR_ANALYZE_EXP_TRASH, {QString::fromStdString(strs[i])})); correct =false;}
            //считать, что подстрока является названием переменной
            curr->type = variable;
            curr->id = strs[i];
        }
        if(correct){
            stackT.push_back(curr);
            stackI.push_back(i);
        }

}

    //если в стеке осталось более 1 элемента
    if (stackT.size() > 1)
    {
        //выбросить исключение "err_operatorsEnough"
        errorsInfo.push_back(ErrorInfo(ERROR_ANALYZE_EXP_MORE_VAR, {QString::number(stackT.size())}));
    }

    //вернуть указатель на единственный оставшийся элемент в стеке
    return stackT[0];
}

void splitExprToSubstr (const std::string& expStr, std::vector<std::string>& strs)
{
    QString str = QString::fromStdString(expStr);

    QStringList substr = str.split(" ");

    for(int i = 0; i <substr.size(); i++){
        strs.push_back(substr[i].toStdString());
    }
}

bool is_template(const QString &str, templates &temp){
    bool result = false;
    QStringList reg_templates{
        "\\s*-th\\s+element\\s+of\\s*array\\s*",
        "(\\s*-th\\s+element\\s+of\\s*)+array\\s*",
        "\\s*value\\s+referenced\\s+by\\s*",
        "\\s*field\\s+of(\\s+struct|\\s+object|\\s+union|\\s*)",
        "\\s*field\\s+referenced\\s+by\\s*",
        "\\s*sum\\s+and\\s*",
        "\\s*subtract\\s*",
        "\\s*multiply\\s+with\\s*",
        "\\s*divide\\s+by\\s*",
        "\\s*return\\s+value\\s+of\\s+the\\s+function\\s+with\\s+parameters\\s*[\\s|\\,]+;",
        "\\s*return\\s+value\\s+of\\s+the\\s+method\\s+of\\s+with\\s+parameters\\s*[\\s|\\,]+;",
        "\\s*return\\s+value\\s+of\\s+the\\s+method\\s+of\\s+the\\s+object\\s+pointed\\s+to\\s+with\\s+parameters\\s*[\\s|\\,]+;"
    };
    QRegExp tempReg("", Qt::CaseSensitive, QRegExp::RegExp2);
    int i;
    for(i = 0; i < reg_templates.count() && result == false; i++){
        tempReg.setPattern(reg_templates[i]);
        result = tempReg.exactMatch(str);
    }
    temp = static_cast<templates>(i);
    return result;
}

TreeNode* convertTextToTree(std::string textPath, std::vector <ErrorInfo>& errorsInfo) {
    //Вытащить строковые представления из текстовой записи, сохранить их и удалить из исходной текстовой записи
    int bracesDepth = 0;
    std::vector<TreeNode*> childs;
    std::vector<int> starts;
    std::vector<int> ends;
    for (int i = 0; i < textPath.size(); i++) //анализ поступившей строки с возможным шаблоном
    {
        if (textPath[i] == '{')
        {
            if (bracesDepth == 0)
                starts.push_back(i);
            bracesDepth++;
        }
        else if (textPath[i] == '}')
        {
            if (bracesDepth == 1)
                ends.push_back(i + 1);
            bracesDepth--;
        }
    }

    //получение дочерних вершин дерева по массиву строковых представлений аргументов и их извлечение из textPath
    for (int i = starts.size() - 1; i >= 0; i--)
    {
        std::string arg = textPath.substr(starts[i] + 1, ends[i] - starts[i] - 2);
        childs.insert(childs.begin(), convertTextToTree(arg, errorsInfo));
        textPath.erase(textPath.begin() + starts[i], textPath.begin() + ends[i]);
    }

    TreeNode* tree = new TreeNode();
    templates temp;
    if (is_template(QString::fromStdString(textPath), temp))
    {
        tree->type = oper;
        switch (temp)
        {
        case oneArray_:
            tree = new TreeNode(arrayItem, std::vector<TreeNode*> { childs[1], childs[0] });
            break;
         case moreArray_:{
            tree = new TreeNode(arrayItem, std::vector<TreeNode*>{});
            TreeNode* curr = tree;
            for (int i = 0; i < childs.size() - 2; i++)
            {
                curr->nodes.push_back(childs[i]);
                childs.erase(childs.begin());
                TreeNode* leftChild = new TreeNode(arrayItem, std::vector<TreeNode*>{});
                curr->nodes.insert(curr->nodes.begin(), leftChild);
                curr = leftChild;
            }
            curr->nodes = std::vector<TreeNode*>{childs[1], childs[0]};
            break;
        }
        case pointer_:
            tree = new TreeNode(pointer, std::vector<TreeNode*>{});
            tree->nodes.push_back(childs[0]);
            break;

        case field_:
            tree = new TreeNode(dot, std::vector<TreeNode*>{});
            tree->nodes.push_back(childs[1]);
            tree->nodes.push_back(childs[0]);
            break;

        case pointerField_:
            tree = new TreeNode(arrow, std::vector<TreeNode*>{});
            tree->nodes.push_back(childs[1]);
            tree->nodes.push_back(childs[0]);
            break;

        case sum_:
            tree = new TreeNode(add, std::vector<TreeNode*>{});
            tree->nodes = childs;
            break;

        case sub_:
            tree = new TreeNode(sub, std::vector<TreeNode*>{});
            tree->nodes = childs;
            break;

        case mul_:
            tree = new TreeNode(mul, std::vector<TreeNode*>{});
            tree->nodes = childs;
            break;

        case div_:{
            tree = new TreeNode(dv, std::vector<TreeNode*>{});
            tree->nodes = childs;
            break;
        }
        case funcCall_:{
            tree = new TreeNode(func, std::vector<TreeNode*>{});
            tree->nodes = childs;
            break;
        }
        case methodCall_:{
            tree = new TreeNode(dot, std::vector<TreeNode*>{});
            tree->nodes.push_back(childs[childs.size() - 1]);
            childs.erase(childs.end() - 1);
            TreeNode* function = new TreeNode(func, std::vector<TreeNode*>{});
            function->id = childs[0]->id;
            childs.erase(childs.begin());
            function->nodes = childs;
            tree->nodes.push_back(function);
            break;
        }
        case methodPointCall_:{
            tree = new TreeNode(arrow, std::vector<TreeNode*>{});
            tree->nodes.push_back(childs[childs.size() - 1]);
            childs.erase(childs.end() - 1);
            TreeNode* function = new TreeNode(func, std::vector<TreeNode*>{});
            function->id = childs[0]->id;
            childs.erase(childs.begin());
            function->nodes = childs;
            tree->nodes.push_back(function);
            break;
        }
        }
    }
    else if (isConstant(textPath))
    {
        tree = new TreeNode(QString::fromStdString(textPath).toFloat());
        tree->type = strchr(textPath.c_str(), '.') != NULL ? constant_float : constant_int;
    }
    else
    {
        //проверка на переменную
        if (checkCorrectVarName(QString::fromStdString(textPath))){
            tree = new TreeNode(textPath);
    }
        else
            errorsInfo.push_back(ErrorInfo{ERROR_NO_TEMPLATE, {QString::fromStdString(textPath)}});
    }

    return tree;
}

bool compareTrees(TreeNode* tree1, TreeNode* tree2, std::string& pass)
{
    char buffer[25]; //буфер для преобразования из чисел в строки

    //запомнить длину строки
    int length = pass.size();

    //приклеить к pass стрелку
    pass.append(" -> ");

    //проверить равенство типов вершин
    if (tree1->type != tree2->type)
    {
        pass.append("different node types: exp ");
        pass.append(_itoa((int)tree1->type, buffer, 10));
        pass.append(", act: ");
        pass.append(_itoa((int)tree2->type, buffer, 10));
        return false;
    }

    if (tree1->type == constant_int || tree1->type == constant_float) //если эти вершины являются константами
    {
        //проверить равенство значений констант
        if (tree1->value != tree2->value)
        {
            //приклеить к pass представление константы
            pass.append(_itoa(tree2->value, buffer, 10));
            return false;
        }
    }
    else if (tree1->type == variable) //если эти вершины являются переменными
    {
        //проверить равенство идентификаторов переменных
        if (tree1->id != tree2->id)
        {
            //приклеить к pass представление переменной
            pass.append(tree2->id);
            return false;
        }
    }
    else //эти вершины являются операторами
    {
        //приклеить к pass представление оператора
        pass.append(tree2->operatorString());

        //проверить равенство операторов
        if (tree1->op != tree2->op)
            return false;

        //проверить равенство количеств дочерних вершин
        if (tree1->nodes.size() != tree2->nodes.size())
            return false;

        //проверить попарное равенство всех дочерних вершин
        for (int i = 0; i < tree1->nodes.size(); i++)
            if (!compareTrees(tree1->nodes[i], tree2->nodes[i], pass))
                return false;
    }

    //удалить из строки новое содержимое
    pass.erase(pass.begin() + length, pass.end());

    return true;
}

void getNearDiffOperands(TreeNode* tree, std::vector<TreeNode*>& operands)
{
    //для всех дочерних вершин
    for (int i = 0; i < tree->nodes.size(); i++)
    {
        //если дочерняя вершина является равным исходному оператором
        if (tree->nodes[i]->type == oper && tree->nodes[i]->op == tree->op)
            getNearDiffOperands(tree->nodes[i], operands); //рекурсивный вызов
        else //иначе добавить вершину в список отличающихся операндов
            operands.push_back(tree->nodes[i]);
    }
}

void conv_combineAddMulOperators(TreeNode* tree)
{
    //если текущая вершина является оператором сложения или умножения
    if (tree->op == add || tree->op == mul)
    {
        //получить список операндов, которые отличаются от искомого оператора
        std::vector<TreeNode*> operands;
        getNearDiffOperands(tree, operands);

        //сделать полученные операнды операндами текущей вершины
        tree->nodes = operands;
    }

    //проделать то же самое для всех дочерних вершин
    for (int i = 0; i < tree->nodes.size(); i++)
        conv_combineAddMulOperators(tree->nodes[i]);
}

void conv_sort(TreeNode* tree)
{
    std::vector<std::string> buf;

    //отсортировать все дочерние вершины
    for (int i = 0; i < tree->nodes.size(); i++)
        conv_sort(tree->nodes[i]);

    //если текущий оператор является коммутативным
    if (tree->op == add || tree->op == mul)
    {
        //получить строковые представления всех дочерних вершин
        std::vector<std::string> strs;
        for (int i = 0; i < tree->nodes.size(); i++)
        {
            std::string str; //строковое представление очередной вершины
            convertTreeToString(tree->nodes[i], str);
            strs.push_back(str);
        }
        //применить пузырьковую сортировку для списка полученных строк, параллельно с этим сортируя дочерние вершины
        for (int j = 0; j < strs.size(); j++) //повторить столько раз, сколько строк в списке
            for (int i = 0; i < strs.size() - 1; i++) //для всех пар соседних строк
                if (strs[i] > strs[i + 1]) //если строки нарушают порядок сортировки
                {
                    std::swap(strs[i], strs[i + 1]); //поменять строки местами
                    std::swap(tree->nodes[i], tree->nodes[i + 1]); //поменять соответствующие им дочерние вершины местами
                }
    }
}

void checkENIOncopyInXML(ExpressionNeededInfo& eni, std::vector<ErrorInfo>& errorList)
{
    //проверка совпадений по переменным
    std::vector<std::string> examples;
    for (int i = 0; i < eni.variablesInfo.size(); i++)
    {
        //проверить наличие текущего id в списке examples
        for (int j = 0; j < examples.size(); j++)
            if (eni.variablesInfo[i]->id == examples[j])
            {
                //ОШИБКА совпадения имён переменных
                errorList.push_back(ErrorInfo(ERROR_COPY_VAR_NAME, { QString::fromStdString(examples[j]) }));
            }

        //добавить новый example
        examples.push_back(eni.variablesInfo[i]->id);
    }
    examples.clear();

    //проверка совпадений по функциям
    for (int i = 0; i < eni.functionsInfo.size(); i++)
    {
        //проверить наличие текущего id в списке examples
        for (int j = 0; j < examples.size(); j++)
            if (eni.functionsInfo[i]->id == examples[j])
            {
                //ОШИБКА совпадения имён функций
                errorList.push_back( ErrorInfo(ERROR_COPY_FUNC_NAME, { QString::fromStdString(examples[j]) }));
            }

        //добавить новый example
        examples.push_back(eni.functionsInfo[i]->id);
    }
    examples.clear();

    //проверка совпадений по функциям
    for (int i = 0; i < eni.functionsInfo.size(); i++)
        for (int j = 0; j < eni.variablesInfo.size(); j++)
            if (eni.functionsInfo[i]->id == eni.variablesInfo[i]->id)
            {
                //ОШИБКА совпадения имени переменной и имени функции
                errorList.push_back(ErrorInfo(ERROR_COPY_FUNC_VAR_NAME, { QString::fromStdString(eni.functionsInfo[i]->id)}));
            }

    //проверить customData
    for (int i = 0; i < eni.customDataInfo.size(); i++)
    {
        //проверить наличие текущего id в списке examples
        for (int j = 0; j < examples.size(); j++)
            if (eni.customDataInfo[i]->id == examples[j])
            {
                //ОШИБКА совпадения имён пользовательского типа данных
                errorList.push_back(ErrorInfo(ERROR_COPY_CUSTOMDATA_NAME, { QString::fromStdString(examples[j]) }));
            }

        //добавить новый example
        examples.push_back(eni.customDataInfo[i]->id);
    }
    examples.clear();

    //проверить поля в customData
    for (int i = 0; i < eni.customDataInfo.size(); i++)
    {
        for (int k = 0; k < eni.customDataInfo[i]->fields.size(); k++)
        {
            //проверить наличие текущего id в списке examples
            for (int j = 0; j < examples.size(); j++)
                if (eni.customDataInfo[i]->fields[k]->id == examples[j])
                {
                    //ОШИБКА совпадения имён пользовательского типа данных
                    errorList.push_back(ErrorInfo(ERROR_COPY_FIELD_NAME, { QString::fromStdString(eni.customDataInfo[i]->id), QString::fromStdString(examples[j]) }));
                }

            //добавить новый example
            examples.push_back(eni.customDataInfo[i]->fields[k]->id);
        }
        examples.clear();
    }

    //проверить методы в customData
    for (int i = 0; i < eni.customDataInfo.size(); i++)
    {
        for (int k = 0; k < eni.customDataInfo[i]->fields.size(); k++)
        {
            //проверить наличие текущего id в списке examples
            for (int j = 0; j < examples.size(); j++)
                if (eni.customDataInfo[i]->fields[k]->id == examples[j])
                {
                    //ОШИБКА совпадения имён пользовательского типа данных
                    errorList.push_back(ErrorInfo(ERROR_COPY_METHOD_NAME, { QString::fromStdString(eni.customDataInfo[i]->id), QString::fromStdString(examples[j]) }));
                }

            //добавить новый example
            examples.push_back(eni.customDataInfo[i]->fields[k]->id);
        }
        examples.clear();
    }
}

bool compareTextTreeAndExpressionTree(TreeNode* tree1, TreeNode* tree2, QString& errorString, ExpressionNeededInfo& eni, const bool oneNode)
{
    char buffer[25]; //буфер для преобразования из чисел в строки

    //проверить равенство типов вершин
    if (tree1->type != tree2->type)
    {
        switch (tree1->type)
        {
        case constant_int:
            errorString = "В указанной позиции неверный тип операнда. Должен быть: тип целочисленная константа";
             break;
        case constant_float:
            errorString = "В указанной позиции неверный тип операнда. Должен быть: тип вещественная константа";
             break;
        case oper:
            errorString = "В указанной позиции неверный тип операнда. Должен быть: тип оператор";
            break;
        case variable:
            errorString =  "В указанной позиции неверный тип операнда. Должен быть: тип переменная";
             break;
        }

        tree2->id.insert(tree2->id.begin(), '#');
        return false;
    }

    if (tree1->type == constant_int || tree1->type == constant_float) //если эти вершины являются константами
    {
        //проверить равенство значений констант
        if (tree1->value != tree2->value)
        {
            if (oneNode)
            {
                tree2->id.insert(tree2->id.begin(), '#');
                errorString = "В указанной позиции константы отличаются";
            }
            return false;
        }
    }
    else if (tree1->type == variable) //если эти вершины являются переменными
    {
        //проверить равенство идентификаторов переменных
        if (tree1->id != tree2->id)
        {
            if (oneNode)
            {
                tree2->id.insert(tree2->id.begin(), '#');
                errorString = "В указанной позиции переменные отличаются";
            }
            return false;
        }
    }
    else //эти вершины являются операторами
    {
        //проверить равенство операторов
        if (tree1->op != tree2->op)
        {
            tree2->id.insert(tree2->id.begin(), '#');
            errorString = "Указанный оператор выбран неправильно. Должен быть " + QString::fromStdString(tree1->operatorStringText());
            return false;
        }

        //проверить равенство количеств дочерних вершин
        if (tree1->nodes.size() != tree2->nodes.size())
        {
            tree2->id.insert(tree2->id.begin(), '#');
            errorString = "Количество операндов у указанного оператора неправильно. Должен(-но) быть " + QString::number(tree1->nodes.size()) + "операнд(-ов)";
            return false;
        }

        //получить признаки равенства для каждой дочерней вершины
        std::vector<bool> childsEquals;
        //проверить попарное равенство всех дочерних вершин
        for (int i = 0; i < tree1->nodes.size(); i++)
            childsEquals.push_back(compareTextTreeAndExpressionTree(tree1->nodes[i], tree2->nodes[i], errorString, eni, oneNode));

        //проверить и определить ошибку
        bool allCorrect = true; //изначально считать, что всё совпало
        for (int i = 0; i < tree1->nodes.size(); i++){
            if(!childsEquals[i])
                allCorrect = false;
        }


        if (!allCorrect)
        {
            switch (tree2->op)
            {
            case arrayItem:
                if (!childsEquals[0])
                    errorString = "В указанном месте идёт обращение к иному массиву. Должен быть массив " + QString::fromStdString(tree1->nodes[0]->id);
                else if (!childsEquals[1])
                    errorString = "В указанном месте идёт обращение к иной ячейке массива. Должно быть обращение к ячейке " + QString::number(tree1->nodes[1]->value);
                break;
            case mul:
                if (!childsEquals[0])
                    errorString = "Левый множитель неправильный. Должен быть" + QString::number(tree1->nodes[0]->value);
                else if (!childsEquals[1])
                    errorString = "Правый множитель неправильный. Должен быть" + QString::number(tree1->nodes[1]->value);
                break;
            case arrow:
                if (!childsEquals[0])
                    errorString = "В указанной операции идет обращение к иной пользовательской структуре данных. Имя необходимой структуры данных: " + QString::fromStdString(tree1->nodes[0]->id);
                else if (!childsEquals[1])
                {
                    //найти сложный тип данных и определить тип
                    int classPosition = 0;
                    for (int i = 0; i < eni.customDataInfo.size(); i++)
                        if (tree1->id == eni.customDataInfo[i]->id)
                            classPosition = i;

                    //пробить по базе данных имя поля (метода) и сказать, какое поле (метод) должно быть указано справа от точки
                    bool isMethod = false;
                    for (int i = 0; i < eni.customDataInfo[classPosition]->methods.size(); i++)
                        if (eni.customDataInfo[classPosition]->methods[i]->id == tree1->id)
                            isMethod = true;

                    errorString = "У указанной операции " + QString::fromStdString(isMethod ? "вызывается иной метод. Необходим вызов метода:": "идет обращение к иному полю. Необходимое поле:")  + QString::fromStdString(tree1->nodes[1]->id);
                }
                break;
            case dot:
                if (!childsEquals[0])
                    errorString = "В указанной операции идет обращение к иной пользовательской структуре данных. Имя необходимой структуры данных:" + QString::fromStdString(tree1->nodes[0]->id);
                else if (!childsEquals[1])
                {
                    //найти сложный тип данных и определить тип
                    int classPosition = 0;
                    for (int i = 0; i < eni.customDataInfo.size(); i++)
                        if (tree1->id == eni.customDataInfo[i]->id)
                            classPosition = i;

                    //пробить по базе данных имя поля (метода) и сказать, какое поле (метод) должно быть указано справа от точки
                    bool isMethod = false;
                    for (int i = 0; i < eni.customDataInfo[classPosition]->methods.size(); i++)
                        if (eni.customDataInfo[classPosition]->methods[i]->id == tree1->id)
                            isMethod = true;

                    errorString = "" + QString::fromStdString(tree1->nodes[1]->id);

                }
                break;
            case dv:
                if (!childsEquals[0])
                    errorString = "Делимое неправильное. Должно быть" + QString::number(tree1->nodes[0]->value);
                else if (!childsEquals[1])
                    errorString = "Делитель неправильный. Должен быть" + QString::number(tree1->nodes[1]->value);
                break;
            case sub:
                if (!childsEquals[0])
                    errorString = "Уменьшаемое неправильное. Должно быть" + QString::number(tree1->nodes[0]->value);
                else if (!childsEquals[1])
                    errorString = "Вычитаемое неправильное. Должно быть" + QString::number(tree1->nodes[1]->value);
                break;
            case add:
                if (!childsEquals[0])
                    errorString = "Левое слагаемое неправильное. Должно быть" + QString::number(tree1->nodes[0]->value);
                else if (!childsEquals[1])
                    errorString = "Правое слагаемое неправильное. Должно быть" + QString::number(tree1->nodes[1]->value);
                break;
            case func:
                if (!childsEquals[0])
                    errorString = "Указанная функция должна быть иной: " + QString::fromStdString(tree1->nodes[0]->id);
            case pointer:
                if (!childsEquals[0])
                    errorString = "Выражение под (*) не является указателем" + QString::fromStdString(tree1->nodes[0]->id);
                break;
            }
            return false;
        }
    }
    return true;
}

void bringTreeToStandartForm(TreeNode* tree, ExpressionNeededInfo& expressionNeededInfo, std::string& polsk, std::vector<ErrorInfo>& errorList)
{
    if(tree->type == oper){
    //эквивалентность A[B]
    if (tree->op == arrayItem)
    {

        bool zeroIsInt = false; //нулевой из операндов является INT
        bool zeroIsArray = false; //нулевой из операндов является ARRAY
        bool oneIsInt = false; //первый из операндов является INT
        bool oneIsArray = false; //первый из операндов является ARRAY
        int arraySize = 0; //размерность массива
        int index = 0; //номер адресуемой ячейки в массиве
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < expressionNeededInfo.functionsInfo.size(); j++) {
                if (tree->nodes[i]->id == expressionNeededInfo.functionsInfo[j]->id &&
                    expressionNeededInfo.functionsInfo[j]->returnType.mainDataType == DT_INT)
                {
                    (i == 0 ? zeroIsInt : oneIsInt) = true;
                    index = tree->nodes[i]->value;
                }
            }for (int j = 0; j < expressionNeededInfo.variablesInfo.size(); j++) {
                //проверка на целое число
                if ((tree->nodes[i]->id == expressionNeededInfo.variablesInfo[j]->id && //1) переменная типа DT_INT
                    expressionNeededInfo.variablesInfo[j]->dataType.mainDataType == DT_INT) ||
                    tree->nodes[i]->type == constant_int) //2) константа с целым значением
                {
                    (i == 0 ? zeroIsInt : oneIsInt) = true;
                    index = tree->nodes[i]->value;
                }
                //проверка на тип DT_ARRAY
                if (tree->nodes[i]->id == expressionNeededInfo.variablesInfo[j]->id && //переменная типа DT_ARRAY
                    expressionNeededInfo.variablesInfo[j]->dataType.mainDataType == DT_ARRAY)
                {
                    (i == 0 ? zeroIsArray : oneIsArray) = true;
                    arraySize = expressionNeededInfo.variablesInfo[j]->dataType.arraySize[0];
                }
            }
        }

        if (!zeroIsInt && !oneIsInt) //интов не нашлось
            errorList.push_back(ErrorInfo(ERROR_EXP_INT, {QString::fromStdString(polsk), QString::number(tree->pos), QString::fromStdString(tree->nodes[0]->id), QString::fromStdString(tree->nodes[1]->id)}));
        else if (!zeroIsArray && !oneIsArray) //массивов не нашлось
            errorList.push_back(ErrorInfo(ERROR_EXP_ARRAY, {QString::fromStdString(polsk), QString::number(tree->pos), QString::fromStdString(tree->nodes[0]->id), QString::fromStdString(tree->nodes[1]->id)}));
        else if (zeroIsInt) //или oneIsArray
        {
            //проверить границы массива
            if (index < 0 || index > arraySize - 1)
                errorList.push_back(ErrorInfo(ERROR_OUT_OF_RANGE_ARRAY, {QString::number(index), QString::fromStdString(tree->nodes[0]->id)}));
            else std::swap(tree->nodes[0], tree->nodes[1]); //преобразовать
        }
    }

    //эквивалентность *(A+B)
    if (tree->op == pointer && (tree->nodes[0]->op == add || tree->nodes[0]->op == sub))
    {

        bool zeroIsInt = false;      //нулевой из операндов является INT
        bool zeroIsArray = false; //нулевой из операндов является ARRAY
        bool oneIsInt = false;       //первый из операндов является INT
        bool oneIsArray = false; //первый из операндов является ARRAY
        int arraySize = 0;            //размерность массива
        int index = 0;                 //номер адресуемой ячейки в массиве
        //проверить типы слагаемых
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < expressionNeededInfo.functionsInfo.size(); j++) {
                if (tree->nodes[0]->nodes[i]->id == expressionNeededInfo.functionsInfo[j]->id &&
                    expressionNeededInfo.functionsInfo[j]->returnType.mainDataType == DT_INT)
                {
                    (i == 0 ? zeroIsInt : oneIsInt) = true;
                    index = tree->nodes[0]->nodes[i]->value;
                }
            }for (int j = 0; j < expressionNeededInfo.variablesInfo.size(); j++) {
                //проверка на целое число
                if ((tree->nodes[0]->nodes[i]->id == expressionNeededInfo.variablesInfo[j]->id && //1) переменная типа DT_INT
                    expressionNeededInfo.variablesInfo[j]->dataType.mainDataType == DT_INT) ||
                    tree->nodes[0]->nodes[i]->type == constant_int) { //2) константа с целым значением

                    (i == 0 ? zeroIsInt : oneIsInt) = true;
                    index = tree->nodes[0]->nodes[i]->value;
                }
                //проверка на тип DT_ARRAY
                if (tree->nodes[0]->nodes[i]->id == expressionNeededInfo.variablesInfo[j]->id && //переменная типа DT_ARRAY
                    expressionNeededInfo.variablesInfo[j]->dataType.mainDataType == DT_ARRAY)
                {
                    (i == 0 ? zeroIsArray : oneIsArray) = true;
                    arraySize = expressionNeededInfo.variablesInfo[j]->dataType.arraySize[0];
                }
            }
        }

        if (!zeroIsInt && !oneIsInt) //интов не нашлось
            errorList.push_back(ErrorInfo(ERROR_EXP_INT, {QString::fromStdString(polsk), QString::number(tree->pos), QString::fromStdString(tree->nodes[0]->nodes[0]->id), QString::fromStdString(tree->nodes[0]->nodes[1]->id)}));
        else if (!zeroIsArray && !oneIsArray) //массивов не нашлось
             errorList.push_back(ErrorInfo(ERROR_EXP_ARRAY, {QString::fromStdString(polsk), QString::number(tree->pos), QString::fromStdString(tree->nodes[0]->nodes[0]->id), QString::fromStdString(tree->nodes[0]->nodes[1]->id)}));
        else {
            if (index < 0 || index > arraySize - 1) //проверить границы массива
                errorList.push_back(ErrorInfo(ERROR_OUT_OF_RANGE_ARRAY, {QString::number(index), QString::fromStdString(tree->nodes[0]->nodes[0]->id)}));
            else
            {
                //преобразовать
                tree->op = arrayItem;
                tree->nodes = tree->nodes[0]->nodes;
                if (zeroIsInt)
                    std::swap(tree->nodes[0], tree->nodes[1]);
            }
        }
    }

    //эквивалентность *(A.B) -- A->B
    if (tree->op == pointer && tree->nodes[0]->op == dot)
    {

        tree->op = arrow;
        tree->nodes = tree->nodes[0]->nodes;
    }

    //проверка типов в A.B
    if (tree->op == dot)
    {

        bool AisVariable = false;
        bool BisField = false;
        bool BisMethod = false;
        int classPosition;
        std::string customType;
        //проверка A - класс, а B - поле этого класса

        //найти название класса
        for (int i = 0; i < expressionNeededInfo.variablesInfo.size(); i++)
            if (tree->nodes[0]->id == expressionNeededInfo.variablesInfo[i]->id)
            {
                AisVariable = true;

                bool classFound = false;
                for (int j = 0; j < expressionNeededInfo.customDataInfo.size(); j++)
                    if (expressionNeededInfo.variablesInfo[i]->dataType.id == expressionNeededInfo.customDataInfo[j]->id)
                    {
                        classFound = true;
                        classPosition = j;
                    }

                if (!classFound)
                    errorList.push_back(ErrorInfo(ERROR_NO_CUSTOM_IN_DB, {QString::fromStdString(expressionNeededInfo.variablesInfo[i]->id)}));

                switch (expressionNeededInfo.customDataInfo[i]->type)
                {
                case DT_CLASS:
                    customType = "класса";
                    break;
                case DT_UNION:
                    customType = "объединения";
                    break;
                case DT_STRUCT:
                    customType = "структуры";
                    break;
                }
            }

        if (AisVariable)
        {
            //найти поле B в этом классе
            for (int i = 0; i < expressionNeededInfo.customDataInfo[classPosition]->fields.size(); i++)
                if (tree->nodes[1]->id == expressionNeededInfo.customDataInfo[classPosition]->fields[i]->id)
                    BisField = true;

            //найти метод B в этом классе
            for (int i = 0; i < expressionNeededInfo.customDataInfo[classPosition]->methods.size(); i++)
                if (tree->nodes[1]->id == expressionNeededInfo.customDataInfo[classPosition]->methods[i]->id)
                    BisMethod = true;

            if (!BisMethod && !BisField)
                errorList.push_back(ErrorInfo(ERROR_EXP_CUSTOM, {QString::fromStdString(tree->nodes[1]->id), QString::fromStdString(customType), QString::fromStdString(polsk),  QString::number(tree->pos)}));

        }
        else
            errorList.push_back(ErrorInfo(ERROR_EXP_FIELD, {QString::fromStdString(tree->nodes[0]->id), QString::fromStdString(polsk), QString::number(tree->pos)}));
    }

    //проверка типов в A->B
    if (tree->op == arrow)
    {

        bool AisVariable = false;
        bool BisField = false;
        bool BisMethod = false;
        int classPosition;
        std::string customType;
        //проверка A - указатель на класс, а B - поле этого класса

        //найти название класса
        for (int i = 0; i < expressionNeededInfo.variablesInfo.size(); i++)
            if (DT_POINTER == expressionNeededInfo.variablesInfo[i]->dataType.mainDataType &&
                tree->nodes[0]->id == expressionNeededInfo.variablesInfo[i]->id)
            {
                AisVariable = true;

                bool classFound = false;
                for (int j = 0; j < expressionNeededInfo.customDataInfo.size(); j++)
                    if (expressionNeededInfo.variablesInfo[i]->dataType.basicType->id == expressionNeededInfo.customDataInfo[j]->id)
                    {
                        classFound = true;
                        classPosition = j;
                    }

                if (!classFound)
                    errorList.push_back({ ERROR_ANALYZE_EXP_NO_VAR_IN_DB, { QString::fromStdString(expressionNeededInfo.variablesInfo[i]->id) } });

                switch (expressionNeededInfo.customDataInfo[classPosition]->type)
                {
                case DT_CLASS:
                    customType = "класса";
                    break;
                case DT_UNION:
                    customType = "объединения";
                    break;
                case DT_STRUCT:
                    customType = "структуры";
                    break;
                }
            }

        if (AisVariable)
        {
            //найти поле B в этом классе
            for (int i = 0; i < expressionNeededInfo.customDataInfo[classPosition]->fields.size(); i++)
                if (tree->nodes[1]->id == expressionNeededInfo.customDataInfo[classPosition]->fields[i]->id)
                    BisField = true;

            //найти метод B в этом классе
            for (int i = 0; i < expressionNeededInfo.customDataInfo[classPosition]->methods.size(); i++)
                if (tree->nodes[1]->id == expressionNeededInfo.customDataInfo[classPosition]->methods[i]->id)
                    BisMethod = true;

            if (!BisMethod && !BisField)
                errorList.push_back(ErrorInfo(ERROR_EXP_CUSTOM, {QString::fromStdString(tree->nodes[1]->id), QString::fromStdString(customType), QString::fromStdString(polsk),  QString::number(tree->pos)}));
        }
        else
             errorList.push_back(ErrorInfo(ERROR_EXP_POINT_FIELD, {QString::fromStdString(tree->nodes[0]->id), QString::fromStdString(polsk), QString::number(tree->pos)}));
    }

    //проверка типов переменных у арифметических операторов
    if (tree->op == add || tree->op == sub || tree->op == mul || tree->op == dv)
    {

        for (int i = 0; i < 2; i++)
        {
            if (tree->nodes[i]->type == variable)
            {
                bool ok = false;
                for (int i = 0; i < expressionNeededInfo.variablesInfo.size(); i++)
                    if (tree->nodes[0]->id == expressionNeededInfo.variablesInfo[i]->id ||
                        tree->nodes[1]->id == expressionNeededInfo.variablesInfo[i]->id)
                        ok = true;

                if (!ok)
                {
                    errorList.push_back({ ERROR_ANALYZE_EXP_NO_VAR_IN_DB, { QString::fromStdString(tree->nodes[0]->id) } });
                }
            }
        }
    }

    //проверка типа под указателем *
    if (tree->op == pointer)
    {

        bool varFound = false;
        for (int i = 0; i < expressionNeededInfo.variablesInfo.size(); i++)
            if (tree->nodes[0]->id == expressionNeededInfo.variablesInfo[i]->id)
                varFound = true;

        if (!varFound)
            errorList.push_back({ ERROR_ANALYZE_EXP_NO_VAR_IN_DB, { QString::fromStdString(tree->nodes[0]->id) } });
    }
}

    //рекурсивные вызовы для дочерних вершин
    for (int i = 0; i < tree->nodes.size(); i++)
        bringTreeToStandartForm(tree->nodes[i], expressionNeededInfo, polsk, errorList);

}

void convertTreeToString(TreeNode* tree, std::string& strout)
{
    std::string str; //строковое представление текущей вершины

    if (tree->type == constant_int || tree->type == constant_float) //если данная вершина является константой
    {
        char buffer[25] = { 0 }; //буфер для перевода константы
        str.append(tree->id);
        str.append(QString::number(tree->value).toStdString()); //строка с константой
    }
    else if (tree->type == variable) //если данна вершина является переменной
    {
        str = tree->id;
    }
    else //данная вершина является оператором
    {
        std::vector<std::string> args; //список строковых представлений дочерних вершин

        //для каждой дочерней вершины
        for (int i = 0; i < tree->nodes.size(); i++)
        {
            //определить строковое представление дочерней вершины
            std::string cur;
            convertTreeToString(tree->nodes[i], cur);
            args.push_back(cur);
        }

        //слепить в строковое представление текущей вершины...
        //определить особый оператор в текущей вершине
        if (tree->op == pointer) { //в данной вершине указатель
            str.append(tree->id);
            str.append("*(");
            str.append(args[0]); //операция унарная
            str.append(")");
        }
        else if (tree->op == arrayItem) { //в данной вершине операция []
            str.append(args[0]); //операция бинарная
            str.append(tree->id);
            str.append("[");
            str.append(args[1]);
            str.append("]");
        }
        else if (tree->op == func) { //в данной вершине операция вызова функции
            str.append(tree->id);
            str.append("(");
            if (args.size() > 0) //аргументы есть?
            {
                str.append(args[0]);
                for (int i = 1; i < args.size(); i++)
                {
                    str.append(", ");
                    str.append(args[i]);
                }
            }
            str.append(")");
        }
        else //иначе стандартный оператор (+ - * / ...)
        {
            str.append(args[0]); //стандартный оператор имеет минимум 2 операнда
            for (int i = 1; i < args.size(); i++)
            {
                str.append(tree->id);
                str.append(tree->operatorString());
                str.append(args[i]);
            }
        }
    }

    strout.append(str);
}

void writeMessage(std::string nameOutFile, QString errorMessage, std::vector <ErrorInfo>& errorsInfo, std::string strout, QString exepath){



    int lastIndex = exepath.lastIndexOf(QChar('\\'));

    QString path = exepath.mid(0, lastIndex);

    QTextStream abc(stdout);



    if(!nameOutFile.empty()){
        path.append("\\").append(QString::fromStdString(nameOutFile)).append(".txt");
    } else path.append("\\").append("output").append(".txt");

    QStringList message;
    QStringList finalErrorMessage;
    QString strOut = QString::fromStdString(strout);
    QFile file(path);
    // Если файл был успешно открыт в режиме записи
    if(file.open(QIODevice::WriteOnly)){
        QTextStream out(&file);
        if(errorMessage.isEmpty()){
            for(int i = 0; i<errorsInfo.size(); i++)
                message.append(errorsInfo[i].createErrorMessage());
            // записываю содержимое листа в файл
            for(int i = 0; i < message.count(); i++){
                out << message[i] << Qt::endl;
            }
        } else{
            int pos =strOut.indexOf("#");
            strOut.remove("#");
            finalErrorMessage.append(strOut + ":" + errorMessage);
            QString bufStr;
            for(int i = 0; i<pos-1; i++){
                bufStr.prepend(" ");
            }
            bufStr.append("^");
            finalErrorMessage.append(bufStr);
            for(int i = 0; i < finalErrorMessage.count(); i++){
                out << finalErrorMessage[i] << Qt::endl;
            }
        }

        if(errorMessage == "" && errorsInfo.empty()){

        QString correctMessage = "Путь построен верно!";
        out << correctMessage;
        }
    } else {std::cout << "Can't write file";};



    file.close();

}
