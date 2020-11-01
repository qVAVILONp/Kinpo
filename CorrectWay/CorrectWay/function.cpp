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

TreeNode* convertAnExpressionToTree (std::vector<std::string> strs, const ExpressionNeededInfo& expressionNeededInfo, std::vector <ErrorInfo>& errorsInfo)
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
            bool isInDB = false;
            for (int j = 0; j < expressionNeededInfo.functionsInfo.size(); j++)
                if (strBuff.toStdString() == expressionNeededInfo.functionsInfo[j]->id)
                    isInDB = true;
            if(isInDB){
                //если в стеке осталось элементов меньше, чем количество требуемых операндов для текущего оператора
                if (stackT.size() < numArg)
                {
                    correct =false;
                std::cout << "MOOOORE VAR TO OP str[i]";
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

            } else {std::cout << "NO FUNC IN DB strBuff - имя переменной"; correct =false;}
            } else {std:: cout << "INCORRECT FUNC NAME strbuff"; correct =false;}
            } else {std::cout << "neverni vizov func strs[i]"; correct =false;}
        }
        else if (defOperator.find(strs[i]) != defOperator.cend()) //если подстрока является оператором
        {
            curr->type = oper;
            curr->op = defOperator[strs[i]];
            //если в стеке осталось элементов меньше, чем количество требуемых операндов для текущего оператора
            if (stackT.size() < curr->operandsCount()){
                std::cout << "MOOOORE VAR TO OP";
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
            if(!isCorrectVarName){ std::cout << "neopredelenaz substr strs[i]"; correct =false;}
            //проверить, находится ли название переменной в базе данных
            bool ok = false;
            for (int j = 0; j < expressionNeededInfo.variablesInfo.size(); j++)
                if (strs[i] == expressionNeededInfo.variablesInfo[j]->id)
                    ok = true;

            if (!ok)
            {
                correct =false;
                std::cout << "NO VAR IN DB strs[i]";
            }
            else
            {
                //считать, что подстрока является названием переменной
                curr->type = variable;
                curr->id = strs[i];
            }
        }
        if(!correct){
            std::cout << "errors list add";
        } else {
            stackT.push_back(curr);
            stackI.push_back(i);
        }
    }


    //если в стеке осталось более 1 элемента
    if (stackT.size() > 1)
    {
        //добавить индексы оставшихся элементов в список errorIndexes
        for (int i = 0; i < stackT.size(); i++)
            std::cout << stackI[i];

        //выбросить исключение "err_operatorsEnough"
        std::cout << "MOOORE OPs TO VAR";
    }

    //вернуть указатель на единственный оставшийся элемент в стеке
    return stackT[0];
}
