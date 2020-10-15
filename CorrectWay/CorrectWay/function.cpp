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

