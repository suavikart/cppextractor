#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <iostream>
#include "cpp_find.h"

using namespace tlex::cppfind;


//#include "tparser.h"
//#include "cpp_find.h" // Подключаем наш экстрактор
//#include <cstdio>

using namespace tlex;
using namespace tlex::cppfind;

void example_find_specific_function()
{
   printf("\n── Пример поиска конкретной функции ──────────────────────────────\n");

   //QFile f("C:/work/neuro_command/ncommand/act/tebnf_act.cpp");

   QFile f(R"fn(C:\tmp\slot_report.cpp)fn");
   QString cpp_code;
   if (f.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QByteArray data = f.readAll();
      f.close();
      cpp_code = QString(data);
   }

#if 0
   // Имитируем фрагмент исходного кода, который мы хотим проанализировать
   const char* cpp_code = R"cpp(
        // Это комментарий перед функцией
        template<typename T>
        bool CppEntityExtractor::try_parse_class_struct(int prefix_idx) {
            if (!p.is_this_word("class") && !p.is_this_word("struct")) return false;
            std::string type = p.cur().v_s.to_std_string();
            p.next();
            
            if (!p.is_word()) return false;
            std::string name = p.cur().v_s.to_std_string();
            p.next();
            
            // ... какой-то код ...
            return true;
        }

        void some_other_function() {
            // Эта функция нас не интересует
        }
    )cpp";
#endif
   // 1. Инициализируем парсер
   TParser p;
   p.set_C_style(true); // Включаем поддержку C-стиля комментариев и строк
   p.assign(cpp_code);
   p.set_open_comment("/*");
   p.set_close_comment("*/");
   p.set_end_ofLine_comment("//");
   p.end_of_init();

   // 2. Создаем экстрактор и запускаем извлечение всех сущностей
   CppEntityExtractor extractor(p);
   extractor.extract_all();

   // 3. Фильтруем результаты и ищем нужную функцию
   bool found = false;
   for (const auto& entity : extractor.get_entities()) {
      // Ищем именно функцию с именем try_parse_class_struct
      if (entity.type == "func" && entity.name == "try_parse_class_struct") {
         found = true;
         printf("✅ НАЙДЕНО!\n");
         printf("Тип       : %s\n", entity.type.c_str());
         printf("Имя       : %s\n", entity.name.c_str());
         printf("Область   : %s\n", entity.full_scope.c_str());
         printf("Строки    : %d - %d\n", entity.start_line, entity.end_line);
         printf("─────────────────────────────────────────────────────────────\n");
         printf("%s\n", entity.raw_code.c_str());
         printf("─────────────────────────────────────────────────────────────\n");
         break; // Нашли, можно выходить
      }
   }

   if (!found) {
      printf("❌ Функция 'try_parse_class_struct' не найдена.\n");
   }
}

void print_help() {
    std::cout << "Usage: cppfind \"<arguments>\" <base_folder>\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  -r, --recursive       Search in subdirectories\n";
    std::cout << "  -l, --list            Show only signatures (no body)\n";
    std::cout << "  -S, --show            Show full code (default)\n";
    std::cout << "  --include=GLOB        File filter (e.g., *.cpp)\n";
    std::cout << "  --type=TYPE           Entity type: func, class, struct, enum, macro, alias, var, any\n";
    std::cout << "  --name=PATTERN        Name pattern (supports * wildcard, e.g., init*)\n";
    std::cout << "  --scope=PATTERN       Scope pattern (e.g., Core::Utils)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  cppfind \"-r --type=func --name=*init*\" /path/to/src\n";
    std::cout << "  cppfind \"-l --type=class --scope=MyNamespace\" /path/to/src\n";
}

int main(int argc, char *argv[]) {
   example_find_specific_function();

    QCoreApplication app(argc, argv);
    
    if (argc < 3) {
        print_help();
        return 1;
    }
    
    QString argsStr = QString::fromLocal8Bit(argv[1]);
    QString baseFolder = QString::fromLocal8Bit(argv[2]);
    
    CppFindArgs args;
    parse_find_args(argsStr, args);
    
    // Если паттерн имени не был задан флагом --name, возможно, он передан как последний аргумент
    // (упрощенная эвристика для совместимости с grep-подобным синтаксисом)
    if (args.name_pattern.empty() && argc > 3) {
        args.name_pattern = QString::fromLocal8Bit(argv[3]).toStdString();
    }
    
    QString result = CppFindEngine::execute(args, baseFolder);
    
    // Вывод в stdout (UTF-8)
    std::cout << result.toStdString() << std::endl;
    
    return 0;
}
